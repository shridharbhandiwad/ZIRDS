//#include "ctracklayer.h"
//#include <qgsmaptopixel.h>
//#include <QtMath>
//#include <QPainter>
//#include  "../cdatawarehouse.h"
//#include "globalmacros.h"

//int nAnimFrame = 0;

///**
// * @brief CTrackLayer constructor
// * @param pCanvas Pointer to the QgsMapCanvas
// */

//CTrackLayer::CTrackLayer(QgsMapCanvas *canvas)
//    : QgsMapCanvasItem(canvas), m_canvas(canvas)
//{
//    setZValue(101); // Ensure drawing order: above base map, below UI overlays
//    QObject::connect(&m_timer, &QTimer::timeout, this, &CTrackLayer::_UpdateAnimation);
//    m_timer.start(150); // ~10 FPS for smooth animation
//}

///**
// * @brief Returns bounding rectangle for the layer — covers the whole canvas
// * @return QRectF bounding rectangle
// */
//QRectF CTrackLayer::boundingRect() const
//{
//    return m_canvas->rect();
//}

//void CTrackLayer::_UpdateAnimation()
//{
//    nAnimFrame = (nAnimFrame + 2) % 20; // Loop from 0–19
//    update(); // Redraw
//}

///**
// * @brief Paints the tracks on the canvas
// * @param pPainter QPainter instance used for drawing
// */
//void CTrackLayer::paint(QPainter *pPainter)
//{
//    if (!pPainter) return;

//    pPainter->setRenderHint(QPainter::Antialiasing, true);

//    // Transform geographic positions to screen coordinates
//    const QgsMapToPixel &mapToPixel = m_canvas->mapSettings().mapToPixel();

//    QList<stTrackDisplayInfo> listTracks = CDataWarehouse::getInstance()->getTrackList();

//    for (const stTrackDisplayInfo &track : listTracks) {
//        QPointF ptScreen = mapToPixel.transform(QgsPointXY(track.lon, track.lat)).toQPointF();
//        double pixelPerDegree = 1.0 / m_canvas->mapUnitsPerPixel();
//        QColor  clr = Qt::cyan;
//        switch (track.nTrackIden)  {
//        case TRACK_IDENTITY_UNKNOWN : clr = Qt::yellow;
//                break;
//        case TRACK_IDENTITY_FRIEND :  clr  = Qt::green;
//           break;
//        case TRACK_IDENTITY_HOSTILE : clr =  Qt::red;;
//           break;

//        }
//        if ( pixelPerDegree > PPI_VISIBLE_THRESHOLD ) {
//            // Core dot
//            pPainter->setPen(clr);
//            pPainter->setBrush(clr);
//            pPainter->drawEllipse(ptScreen, 4, 4);

//            // Heading line
//            double dHeadingRad = qDegreesToRadians(track.heading);
//            QPointF ptTip(
//                ptScreen.x() + std::cos(dHeadingRad) * 10,
//                ptScreen.y() - std::sin(dHeadingRad) * 10
//            );
//            pPainter->drawLine(ptScreen, ptTip);

//            if ( pixelPerDegree > TEXT_VISIBLE_THRESHOLD ) {
//                pPainter->setFont(QFont("century",11,80,true));
//                // Label
//                pPainter->setPen(Qt::white);
//                pPainter->drawText(ptScreen + QPointF(6, -6), QString::number(track.nTrkId));
//            }
//        }

//        // Glowing blip animation
//        int nMaxRadius = 20;
//        int nCurrentRadius = 4 + (nAnimFrame * (nMaxRadius - 4) / 20);
//        int nAlpha = 255 - (nAnimFrame * 255 / 20); // Fades out

//        // Gradient blip: transparent center → bright edge
//        QRadialGradient gradient(ptScreen, nCurrentRadius);
//        QColor clrEdge(clr.red(), clr.green(), clr.blue(), nAlpha);    // Bright neon green at edge
//        //QColor clrCenter(0, 255, 255, 0);       // Fully transparent center

//        gradient.setColorAt(0, Qt::cyan);
//        gradient.setColorAt(0.5, clr);   // Center of the blip
//        gradient.setColorAt(1.0, clrEdge);     // Edge of the blip

//        pPainter->setPen(Qt::NoPen);
//        pPainter->setBrush(gradient);
//        pPainter->drawEllipse(ptScreen, nCurrentRadius, nCurrentRadius);
//    }
//}


#include "ctracklayer.h"
#include <qgsmaptopixel.h>
#include <QtMath>
#include <QPainter>
#include "../cdatawarehouse.h"
#include "globalmacros.h"
#include <QDateTime>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <cmath>
#include "../cdrone.h"
#include <QPixmap>
#include <QTransform>
#include <QPainterPath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int nAnimFrame = 0;

/**
 * @brief CTrackLayer constructor
 * @param pCanvas Pointer to the QgsMapCanvas
 */
CTrackLayer::CTrackLayer(QgsMapCanvas *canvas)
    : QgsMapCanvasItem(canvas), m_canvas(canvas), m_hoveredTrackId(-1), m_rightClickedTrackId(-1), 
      m_contextMenu(nullptr), m_focusedTrackId(-1), m_hasPendingMouseMove(false)
{
    setZValue(101); // Ensure drawing order: above base map, below UI overlays
    QObject::connect(&m_timer, &QTimer::timeout, this, &CTrackLayer::_UpdateAnimation);
    m_timer.start(200); // Reduced from 150ms to 200ms for better performance

    // Enable mouse tracking on the canvas
    m_canvas->viewport()->setMouseTracking(true);
    m_canvas->viewport()->installEventFilter(this);
    
    // Setup mouse move throttling - only update tooltips every 100ms
    m_mouseMoveThrottle.setSingleShot(true);
    m_mouseMoveThrottle.setInterval(100);
    QObject::connect(&m_mouseMoveThrottle, &QTimer::timeout, this, [this]() {
        if (m_hasPendingMouseMove) {
            int trackId = getTrackAtPosition(m_pendingMousePos);
            if (trackId != m_hoveredTrackId) {
                m_hoveredTrackId = trackId;
                update();
            }
            m_hasPendingMouseMove = false;
        }
    });
    
    // Create context menu
    createContextMenu();
    
    // Load default drone icon
    m_droneIcon = QPixmap(":/images/resources/drone_icon.png");
    if (m_droneIcon.isNull()) {
        qWarning() << "Failed to load default drone icon from resources";
    } else {
        qDebug() << "Loaded drone icon:" << m_droneIcon.size();
    }
}

CTrackLayer::~CTrackLayer()
{
    if (m_canvas && m_canvas->viewport()) {
        m_canvas->viewport()->removeEventFilter(this);
    }
}

/**
 * @brief Returns bounding rectangle for the layer — covers the whole canvas
 * @return QRectF bounding rectangle
 */
QRectF CTrackLayer::boundingRect() const
{
    return m_canvas->rect();
}

void CTrackLayer::_UpdateAnimation()
{
    nAnimFrame = (nAnimFrame + 2) % 20; // Loop from 0–19
    update(); // Redraw
}

/**
 * @brief Event filter to capture mouse events
 */
bool CTrackLayer::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_canvas->viewport()) {
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            m_mousePos = mouseEvent->pos();
            m_pendingMousePos = m_mousePos;
            m_hasPendingMouseMove = true;
            
            // Throttle mouse move processing to reduce redraws
            if (!m_mouseMoveThrottle.isActive()) {
                m_mouseMoveThrottle.start();
            }
            
            // Still update cursor immediately for better responsiveness
            int trackId = getTrackAtPosition(m_mousePos);
            if (trackId != -1) {
                m_canvas->setCursor(Qt::PointingHandCursor);
            } else {
                m_canvas->unsetCursor();
            }
        } else if (event->type() == QEvent::Leave) {
            if (m_hoveredTrackId != -1) {
                m_hoveredTrackId = -1;
                m_canvas->unsetCursor();
                update();
            }
        } else if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::RightButton) {
                m_mousePos = mouseEvent->pos();
                int trackId = getTrackAtPosition(m_mousePos);
                
                if (trackId != -1) {
                    m_rightClickedTrackId = trackId;
                    QPoint globalPos = m_canvas->viewport()->mapToGlobal(mouseEvent->pos());
                    m_contextMenu->exec(globalPos);
                    return true; // Consume the event
                }
            }
        }
    }

    return QObject::eventFilter(obj, event);
}

/**
 * @brief Creates the context menu for tracks
 */
void CTrackLayer::createContextMenu()
{
    m_contextMenu = new QMenu();
    
    QAction *focusAction = m_contextMenu->addAction("🎯 Focus Track");
    connect(focusAction, &QAction::triggered, this, &CTrackLayer::onFocusTrack);
    
    m_contextMenu->addSeparator();
    
    int historyLimit = CDataWarehouse::getInstance()->getHistoryLimit();
    QAction *historyAction = m_contextMenu->addAction(QString("📍 Toggle History (Max %1)").arg(historyLimit));
    connect(historyAction, &QAction::triggered, this, &CTrackLayer::onToggleTrackHistory);
    
    QAction *highlightAction = m_contextMenu->addAction("✨ Toggle Highlight");
    connect(highlightAction, &QAction::triggered, this, &CTrackLayer::onHighlightTrack);
    
    m_contextMenu->addSeparator();
    
    QAction *imageAction = m_contextMenu->addAction("🖼️ Load Track Image");
    connect(imageAction, &QAction::triggered, this, &CTrackLayer::onLoadTrackImage);
    
    m_contextMenu->addSeparator();
    
    QAction *deleteAction = m_contextMenu->addAction("🗑️ Delete Track");
    connect(deleteAction, &QAction::triggered, this, &CTrackLayer::onDeleteTrack);
    
    // Style the context menu
    m_contextMenu->setStyleSheet(
        "QMenu {"
        "   background-color: #ffffff;"
        "   color: #1e293b;"
        "   border: 2px solid #e2e8f0;"
        "   border-radius: 8px;"
        "   padding: 6px;"
        "}"
        "QMenu::item {"
        "   padding: 8px 24px;"
        "   border-radius: 4px;"
        "   font-weight: 500;"
        "}"
        "QMenu::item:selected {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3b82f6, stop:1 #2563eb);"
        "   color: white;"
        "}"
        "QMenu::separator {"
        "   height: 1px;"
        "   background-color: #e2e8f0;"
        "   margin: 4px 16px;"
        "}"
    );
}

/**
 * @brief Context menu action slots
 */
void CTrackLayer::onFocusTrack()
{
    if (m_rightClickedTrackId == -1) return;
    
    // Toggle focus state
    if (m_focusedTrackId == m_rightClickedTrackId) {
        m_focusedTrackId = -1;
        qDebug() << "Unfocused track" << m_rightClickedTrackId;
    } else {
        m_focusedTrackId = m_rightClickedTrackId;
        qDebug() << "Focused on track" << m_rightClickedTrackId;
        
        // Center the canvas on the focused track
        QList<stTrackDisplayInfo> tracks = CDataWarehouse::getInstance()->getTrackList();
        for (const stTrackDisplayInfo &track : tracks) {
            if (track.nTrkId == m_rightClickedTrackId) {
                QgsPointXY centerPoint(track.lon, track.lat);
                m_canvas->setCenter(centerPoint);
                m_canvas->refresh();
                break;
            }
        }
    }
    
    update(); // Redraw to show focus changes
}

void CTrackLayer::onDeleteTrack()
{
    if (m_rightClickedTrackId == -1) return;
    
    // Remove from highlight and focus sets if present
    m_highlightedTracks.remove(m_rightClickedTrackId);
    if (m_focusedTrackId == m_rightClickedTrackId) {
        m_focusedTrackId = -1;
    }
    
    // Delete track from data warehouse
    CDataWarehouse::getInstance()->deleteTrack(m_rightClickedTrackId);
    qDebug() << "Track" << m_rightClickedTrackId << "deleted";
    
    update(); // Redraw to reflect changes
}

void CTrackLayer::onLoadTrackImage()
{
    if (m_rightClickedTrackId == -1) return;
    
    QString filter = "Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.svg)";
    QString imagePath = QFileDialog::getOpenFileName(
        nullptr,
        QString("Load Image for Track #%1").arg(m_rightClickedTrackId),
        QString(),
        filter
    );
    
    if (!imagePath.isEmpty()) {
        qDebug() << "Load image for track" << m_rightClickedTrackId << ":" << imagePath;
        // Cache pixmap
        QPixmap pix(imagePath);
        if (!pix.isNull()) {
            m_trackPixmaps.insert(m_rightClickedTrackId, pix);
            // Persist path in data warehouse so it survives refresh
            CDataWarehouse::getInstance()->setTrackImagePath(m_rightClickedTrackId, imagePath);
            update();
        } else {
            QMessageBox::warning(nullptr, "Invalid Image", "Failed to load the selected image.");
        QPixmap customImage(imagePath);
        
        if (customImage.isNull()) {
            QMessageBox::warning(nullptr, "Load Image", 
                QString("Failed to load image from: %1").arg(imagePath));
            qWarning() << "Failed to load image for track" << m_rightClickedTrackId << ":" << imagePath;
        } else {
            // Store the custom image for this track
            m_trackImages[m_rightClickedTrackId] = customImage;
            qDebug() << "Loaded custom image for track" << m_rightClickedTrackId 
                    << "Size:" << customImage.size() << "Path:" << imagePath;
            
            QMessageBox::information(nullptr, "Load Image", 
                QString("Successfully loaded image for Track #%1\nSize: %2x%3")
                .arg(m_rightClickedTrackId)
                .arg(customImage.width())
                .arg(customImage.height()));
            
            // Force redraw to show the new image
            update();
        }
    }
}
}

void CTrackLayer::onToggleTrackHistory()
{
    if (m_rightClickedTrackId == -1) return;
    
    CDataWarehouse::getInstance()->toggleTrackHistory(m_rightClickedTrackId);
    qDebug() << "Toggle history for track" << m_rightClickedTrackId;
    
    // Force redraw to show/hide history
    update();
}

void CTrackLayer::onHighlightTrack()
{
    if (m_rightClickedTrackId == -1) return;
    
    if (m_highlightedTracks.contains(m_rightClickedTrackId)) {
        m_highlightedTracks.remove(m_rightClickedTrackId);
        qDebug() << "Removed highlight from track" << m_rightClickedTrackId;
    } else {
        m_highlightedTracks.insert(m_rightClickedTrackId);
        qDebug() << "Highlighted track" << m_rightClickedTrackId;
    }
    
    update(); // Redraw to show highlight changes
}

/**
 * @brief Detects if mouse is hovering over a track
 * @param pos Mouse position in screen coordinates
 * @return Track ID if hovering over a track, -1 otherwise
 */
int CTrackLayer::getTrackAtPosition(const QPointF &pos)
{
    const QgsMapToPixel &mapToPixel = m_canvas->mapSettings().mapToPixel();
    QList<stTrackDisplayInfo> listTracks = CDataWarehouse::getInstance()->getTrackList();

    // Detection radius in pixels
    const double detectionRadius = 20.0;
    const double detectionRadiusSq = detectionRadius * detectionRadius;  // Avoid sqrt

    // Find closest track within detection radius
    int closestTrackId = -1;
    double closestDistanceSq = detectionRadiusSq;

    for (const stTrackDisplayInfo &track : listTracks) {
        QPointF ptScreen = mapToPixel.transform(QgsPointXY(track.lon, track.lat)).toQPointF();

        // Calculate squared distance (faster than sqrt)
        double dx = pos.x() - ptScreen.x();
        double dy = pos.y() - ptScreen.y();
        double distanceSq = dx * dx + dy * dy;

        if (distanceSq < closestDistanceSq) {
            closestDistanceSq = distanceSq;
            closestTrackId = track.nTrkId;
        }
    }

    return closestTrackId;
}

/**
 * @brief Draws the tooltip for the hovered track
 * @param pPainter QPainter instance
 * @param trackInfo Track information to display
 * @param screenPos Position on screen where to draw tooltip
 */
void CTrackLayer::drawTooltip(QPainter *pPainter, const stTrackDisplayInfo &trackInfo, const QPointF &screenPos)
{
    // Determine identity display
    QString identityStr;
    QString identityIcon;
    QColor identityColor;

    switch(trackInfo.nTrackIden) {
        case TRACK_IDENTITY_FRIEND:
            identityStr = "FRIENDLY";
            identityIcon = "✓";
            identityColor = QColor(46, 204, 113); // Modern green
            break;
        case TRACK_IDENTITY_HOSTILE:
            identityStr = "HOSTILE";
            identityIcon = "✖";
            identityColor = QColor(231, 76, 60); // Modern red
            break;
        case TRACK_IDENTITY_UNKNOWN:
            identityStr = "UNKNOWN";
            identityIcon = "?";
            identityColor = QColor(241, 196, 15); // Modern yellow
            break;
        default:
            identityStr = "UNDEFINED";
            identityIcon = "•";
            identityColor = QColor(149, 165, 166);
            break;
    }

    // Minimal tooltip content
    QStringList lines;
    lines << QString("ID %1").arg(trackInfo.nTrkId);
    lines << QString("%1 %2").arg(identityIcon).arg(identityStr);
    lines << "";
    lines << QString("📍 %1°, %2°").arg(trackInfo.lat, 0, 'f', 4).arg(trackInfo.lon, 0, 'f', 4);
    lines << QString("📏 %1 km").arg(trackInfo.range / 1000.0, 0, 'f', 1);
    lines << QString("🧭 %1°").arg(trackInfo.heading, 0, 'f', 0);
    lines << QString("⬆ %1 m").arg(trackInfo.alt, 0, 'f', 0);

    // Calculate dimensions
    QFont tooltipFont("Segoe UI", 10);
    pPainter->setFont(tooltipFont);
    QFontMetrics fm(tooltipFont);

    int maxWidth = 0;
    for (const QString &line : lines) {
        int lineWidth = fm.horizontalAdvance(line);
        if (lineWidth > maxWidth) {
            maxWidth = lineWidth;
        }
    }

    int padding = 15;
    int tooltipWidth = maxWidth + padding * 2;
    int lineHeight = fm.height() + 3;
    int tooltipHeight = lines.count() * lineHeight + padding * 2;

    // Position tooltip
    QPointF tooltipPos = screenPos + QPointF(25, -tooltipHeight / 2);

    // Keep within bounds
    if (tooltipPos.x() + tooltipWidth > m_canvas->width()) {
        tooltipPos.setX(screenPos.x() - tooltipWidth - 25);
    }
    if (tooltipPos.y() < 5) {
        tooltipPos.setY(5);
    }
    if (tooltipPos.y() + tooltipHeight > m_canvas->height()) {
        tooltipPos.setY(m_canvas->height() - tooltipHeight - 5);
    }

    QRectF tooltipRect(tooltipPos, QSizeF(tooltipWidth, tooltipHeight));

    // Simple background - no gradients for performance
    pPainter->setPen(QPen(identityColor, 2));
    pPainter->setBrush(QColor(30, 30, 40, 200));
    pPainter->drawRoundedRect(tooltipRect, 8, 8);

    // Accent line on left side
    QRectF accentRect(tooltipPos.x() + 2, tooltipPos.y() + 5, 3, tooltipHeight - 10);
    pPainter->setPen(Qt::NoPen);
    pPainter->setBrush(identityColor);
    pPainter->drawRoundedRect(accentRect, 2, 2);

    // Draw text with subtle shadow
    int yOffset = padding + fm.ascent();

    for (int i = 0; i < lines.count(); i++) {
        const QString &line = lines[i];

        if (line.isEmpty()) {
            yOffset += lineHeight / 2;
            continue;
        }

        // Main text with colors (removed text shadow for performance)
        if (i == 0) {
            // Track ID - bright cyan
            pPainter->setFont(QFont("Segoe UI", 11, QFont::Bold));
            pPainter->setPen(QColor(100, 200, 255));
        } else if (i == 1) {
            // Identity - use identity color
            pPainter->setFont(QFont("Segoe UI", 10, QFont::Bold));
            pPainter->setPen(identityColor);
        } else {
            // Regular info - bright for visibility on transparent bg
            pPainter->setFont(QFont("Segoe UI", 9));
            pPainter->setPen(QColor(240, 240, 240));
        }

        pPainter->drawText(QPointF(tooltipPos.x() + padding + 10, tooltipPos.y() + yOffset), line);
        yOffset += lineHeight;
    }
}

/**
 * @brief Draws speed vector for a track
 * @param pPainter QPainter instance
 * @param trackInfo Track information
 * @param screenPos Screen position of track
 * @param trackColor Color of the track
 */
void CTrackLayer::drawSpeedVector(QPainter *pPainter, const stTrackDisplayInfo &trackInfo, 
                                 const QPointF &screenPos, const QColor &trackColor)
{
    if (trackInfo.velocity <= 0) return; // No vector for stationary targets
    
    // Calculate speed vector length (proportional to speed)
    // Scale factor: 1 m/s = 2 pixels, max length = 50 pixels
    double speedScale = 2.0;
    double maxVectorLength = 50.0;
    double vectorLength = qMin(trackInfo.velocity * speedScale, maxVectorLength);
    
    // Calculate speed vector direction (same as heading)
    double speedHeadingRad = qDegreesToRadians(trackInfo.heading);
    QPointF vectorEnd(
        screenPos.x() + std::cos(speedHeadingRad) * vectorLength,
        screenPos.y() - std::sin(speedHeadingRad) * vectorLength
    );
    
    // Draw speed vector line
    QColor vectorColor = trackColor;
    vectorColor.setAlpha(200);
    pPainter->setPen(QPen(vectorColor, 2, Qt::SolidLine));
    pPainter->drawLine(screenPos, vectorEnd);
    
    // Draw arrowhead at the end
    double arrowSize = 6.0;
    double arrowAngle = M_PI / 6; // 30 degrees
    
    QPointF arrowP1(
        vectorEnd.x() - arrowSize * std::cos(speedHeadingRad - arrowAngle),
        vectorEnd.y() + arrowSize * std::sin(speedHeadingRad - arrowAngle)
    );
    QPointF arrowP2(
        vectorEnd.x() - arrowSize * std::cos(speedHeadingRad + arrowAngle),
        vectorEnd.y() + arrowSize * std::sin(speedHeadingRad + arrowAngle)
    );
    
    pPainter->setPen(QPen(vectorColor, 2, Qt::SolidLine));
    pPainter->drawLine(vectorEnd, arrowP1);
    pPainter->drawLine(vectorEnd, arrowP2);
    
    // Optional: Draw speed text near the vector
    if (vectorLength > 20) { // Only show text if vector is long enough
        pPainter->setFont(QFont("Arial", 8));
        pPainter->setPen(Qt::white);
        QString speedText = QString("%1 m/s").arg(trackInfo.velocity, 0, 'f', 1);
        QPointF textPos = screenPos + QPointF(vectorLength * 0.6 * std::cos(speedHeadingRad), 
                                             -vectorLength * 0.6 * std::sin(speedHeadingRad));
        pPainter->drawText(textPos, speedText);
    }
}

/**
 * @brief Draws focused track datatip that follows the track
 * @param pPainter QPainter instance
 * @param trackInfo Track information
 * @param screenPos Screen position of track
 */
void CTrackLayer::drawFocusedTrackDatatip(QPainter *pPainter, const stTrackDisplayInfo &trackInfo, 
                                         const QPointF &screenPos)
{
    // Use a modified version of the tooltip for focused tracks
    // This will always be visible and follow the track
    
    // Determine identity display
    QString identityStr;
    QString identityIcon;
    QColor identityColor;

    switch(trackInfo.nTrackIden) {
        case TRACK_IDENTITY_FRIEND:
            identityStr = "FRIENDLY";
            identityIcon = "✓";
            identityColor = QColor(46, 204, 113);
            break;
        case TRACK_IDENTITY_HOSTILE:
            identityStr = "HOSTILE";
            identityIcon = "✖";
            identityColor = QColor(231, 76, 60);
            break;
        case TRACK_IDENTITY_UNKNOWN:
            identityStr = "UNKNOWN";
            identityIcon = "?";
            identityColor = QColor(241, 196, 15);
            break;
        default:
            identityStr = "UNDEFINED";
            identityIcon = "•";
            identityColor = QColor(149, 165, 166);
            break;
    }

    // Focused track tooltip content (more compact)
    QStringList lines;
    lines << QString("🎯 FOCUSED: ID %1").arg(trackInfo.nTrkId);
    lines << QString("%1 %2").arg(identityIcon).arg(identityStr);
    lines << QString("🚀 %1 m/s").arg(trackInfo.velocity, 0, 'f', 1);
    lines << QString("🧭 %1°").arg(trackInfo.heading, 0, 'f', 0);

    // Calculate dimensions
    QFont tooltipFont("Segoe UI", 9, QFont::Bold);
    pPainter->setFont(tooltipFont);
    QFontMetrics fm(tooltipFont);

    int maxWidth = 0;
    for (const QString &line : lines) {
        int lineWidth = fm.horizontalAdvance(line);
        if (lineWidth > maxWidth) {
            maxWidth = lineWidth;
        }
    }

    int padding = 10;
    int tooltipWidth = maxWidth + padding * 2;
    int lineHeight = fm.height() + 2;
    int tooltipHeight = lines.count() * lineHeight + padding * 2;

    // Position tooltip above and to the right of track
    QPointF tooltipPos = screenPos + QPointF(15, -tooltipHeight - 15);

    // Keep within bounds
    if (tooltipPos.x() + tooltipWidth > m_canvas->width()) {
        tooltipPos.setX(screenPos.x() - tooltipWidth - 15);
    }
    if (tooltipPos.y() < 5) {
        tooltipPos.setY(screenPos.y() + 15);
    }

    QRectF tooltipRect(tooltipPos, QSizeF(tooltipWidth, tooltipHeight));

    // Draw focused tooltip with stronger background
    QLinearGradient bgGradient(tooltipRect.topLeft(), tooltipRect.bottomLeft());
    bgGradient.setColorAt(0, QColor(30, 30, 40, 200));
    bgGradient.setColorAt(1, QColor(20, 20, 30, 220));

    pPainter->setBrush(bgGradient);
    pPainter->setPen(QPen(identityColor, 2));
    pPainter->drawRoundedRect(tooltipRect, 8, 8);

    // Draw text
    int yOffset = padding + fm.ascent();
    for (int i = 0; i < lines.count(); i++) {
        const QString &line = lines[i];

        if (i == 0) {
            // Focused header - bright cyan
            pPainter->setPen(QColor(100, 200, 255));
        } else if (i == 1) {
            // Identity - use identity color
            pPainter->setPen(identityColor);
        } else {
            // Regular info - bright white
            pPainter->setPen(QColor(255, 255, 255));
        }

        pPainter->drawText(QPointF(tooltipPos.x() + padding, tooltipPos.y() + yOffset), line);
        yOffset += lineHeight;
    }
}

/**
 * @brief Draws drone internal details panel next to track
 * @param pPainter QPainter instance
 * @param trackInfo Track information with drone pointer
 * @param screenPos Screen position of track
 */
void CTrackLayer::drawDroneInternalDetails(QPainter *pPainter, const stTrackDisplayInfo &trackInfo, 
                                          const QPointF &screenPos)
{
    if (!trackInfo.pDrone) return;
    
    stDroneInternalState droneState = trackInfo.pDrone->getInternalState();
    
    // Determine health color
    QColor healthColor = trackInfo.pDrone->getHealthStatusColor();
    
    // Build info lines for display
    QStringList lines;
    lines << QString("🚁 DRONE #%1").arg(trackInfo.nTrkId);
    lines << QString("━━━━━━━━━━━━━━");
    lines << QString("MODE: %1").arg(trackInfo.pDrone->getFlightModeString());
    lines << QString("STATUS: %1").arg(droneState.statusMessage);
    lines << "";
    lines << QString("⚡ POWER");
    lines << QString("  Battery: %1%").arg(droneState.batteryLevel, 0, 'f', 1);
    lines << QString("  Voltage: %1V").arg(droneState.batteryVoltage, 0, 'f', 2);
    lines << QString("  Flight Time: %1m").arg(droneState.estimatedFlightTime, 0, 'f', 1);
    lines << "";
    lines << QString("🎯 DYNAMICS");
    lines << QString("  Speed: %1 m/s").arg(droneState.groundSpeed, 0, 'f', 1);
    lines << QString("  V-Speed: %1 m/s").arg(droneState.verticalSpeed, 0, 'f', 1);
    lines << QString("  Accel: %1 m/s²").arg(droneState.acceleration, 0, 'f', 2);
    lines << QString("  Heading: %1°").arg(droneState.yaw, 0, 'f', 1);
    lines << "";
    lines << QString("🎚️ ATTITUDE");
    lines << QString("  Pitch: %1°").arg(droneState.pitch, 0, 'f', 1);
    lines << QString("  Roll: %1°").arg(droneState.roll, 0, 'f', 1);
    lines << QString("  Yaw: %1°").arg(droneState.yaw, 0, 'f', 1);
    lines << "";
    lines << QString("📡 SENSORS");
    lines << QString("  GPS: %1%2").arg(droneState.sensors.gpsActive ? "✓" : "✗").arg(QString(" %1%").arg(droneState.sensors.gpsQuality));
    lines << QString("  Link: %1%").arg(droneState.sensors.linkQuality);
    lines << QString("  IMU: %1").arg(droneState.sensors.imuActive ? "✓" : "✗");
    lines << "";
    lines << QString("📍 MISSION");
    lines << QString("  ID: %1").arg(droneState.missionId);
    lines << QString("  WP: %1/%2").arg(droneState.waypointIndex).arg(droneState.totalWaypoints);
    lines << QString("  Progress: %1%").arg(droneState.missionProgress, 0, 'f', 0);
    
    // Calculate dimensions
    QFont detailFont("Consolas", 8);
    pPainter->setFont(detailFont);
    QFontMetrics fm(detailFont);
    
    int maxWidth = 0;
    for (const QString &line : lines) {
        int lineWidth = fm.horizontalAdvance(line);
        if (lineWidth > maxWidth) {
            maxWidth = lineWidth;
        }
    }
    
    int padding = 12;
    int panelWidth = maxWidth + padding * 2;
    int lineHeight = fm.height() + 1;
    int panelHeight = lines.count() * lineHeight + padding * 2;
    
    // Position panel to the right of track
    QPointF panelPos = screenPos + QPointF(40, -panelHeight / 2);
    
    // Keep within bounds
    if (panelPos.x() + panelWidth > m_canvas->width() - 10) {
        panelPos.setX(screenPos.x() - panelWidth - 40);
    }
    if (panelPos.y() < 10) {
        panelPos.setY(10);
    }
    if (panelPos.y() + panelHeight > m_canvas->height() - 10) {
        panelPos.setY(m_canvas->height() - panelHeight - 10);
    }
    
    QRectF panelRect(panelPos, QSizeF(panelWidth, panelHeight));
    
    // Draw panel shadow
    pPainter->setPen(Qt::NoPen);
    pPainter->setBrush(QColor(0, 0, 0, 60));
    pPainter->drawRoundedRect(panelRect.adjusted(-3, -3, 3, 3), 12, 12);
    
    // Main background - dark semi-transparent
    QLinearGradient bgGradient(panelRect.topLeft(), panelRect.bottomLeft());
    bgGradient.setColorAt(0, QColor(20, 25, 35, 240));
    bgGradient.setColorAt(1, QColor(15, 20, 30, 250));
    
    pPainter->setBrush(bgGradient);
    pPainter->setPen(QPen(healthColor, 2));
    pPainter->drawRoundedRect(panelRect, 10, 10);
    
    // Health status indicator bar on left
    QRectF healthBar(panelPos.x() + 5, panelPos.y() + 5, 4, panelHeight - 10);
    pPainter->setPen(Qt::NoPen);
    pPainter->setBrush(healthColor);
    pPainter->drawRoundedRect(healthBar, 2, 2);
    
    // Battery level indicator (visual bar)
    if (droneState.batteryLevel > 0) {
        QRectF batteryBg(panelPos.x() + padding, panelPos.y() + panelHeight - padding - 8, 
                         panelWidth - padding * 2, 6);
        pPainter->setPen(Qt::NoPen);
        pPainter->setBrush(QColor(60, 60, 70));
        pPainter->drawRoundedRect(batteryBg, 3, 3);
        
        // Battery fill color based on level
        QColor batteryColor;
        if (droneState.batteryLevel > 50) {
            batteryColor = QColor(46, 204, 113); // Green
        } else if (droneState.batteryLevel > 25) {
            batteryColor = QColor(241, 196, 15); // Yellow
        } else {
            batteryColor = QColor(231, 76, 60); // Red
        }
        
        QRectF batteryFill(batteryBg.x(), batteryBg.y(), 
                          batteryBg.width() * (droneState.batteryLevel / 100.0), batteryBg.height());
        pPainter->setBrush(batteryColor);
        pPainter->drawRoundedRect(batteryFill, 3, 3);
    }
    
    // Draw text lines
    int yOffset = padding + fm.ascent();
    
    for (int i = 0; i < lines.count(); i++) {
        const QString &line = lines[i];
        
        if (line.isEmpty() || line.startsWith("━")) {
            yOffset += lineHeight / 2;
            
            // Draw separator line
            if (line.startsWith("━")) {
                pPainter->setPen(QPen(QColor(100, 100, 120, 100), 1));
                pPainter->drawLine(panelPos.x() + padding + 15, panelPos.y() + yOffset - fm.ascent()/2,
                                  panelPos.x() + panelWidth - padding - 15, panelPos.y() + yOffset - fm.ascent()/2);
            }
            continue;
        }
        
        // Color coding for different line types
        if (line.startsWith("🚁")) {
            // Title - bright cyan
            pPainter->setFont(QFont("Consolas", 9, QFont::Bold));
            pPainter->setPen(QColor(100, 200, 255));
        } else if (line.startsWith("MODE:") || line.startsWith("STATUS:")) {
            // Status lines
            pPainter->setFont(QFont("Consolas", 8, QFont::Bold));
            if (line.contains("EMERGENCY") || line.contains("CRITICAL")) {
                pPainter->setPen(QColor(231, 76, 60)); // Red
            } else if (line.contains("WARNING") || line.contains("RTB")) {
                pPainter->setPen(QColor(241, 196, 15)); // Yellow
            } else {
                pPainter->setPen(healthColor);
            }
        } else if (line.startsWith("⚡") || line.startsWith("🎯") || 
                   line.startsWith("🎚️") || line.startsWith("📡") || line.startsWith("📍")) {
            // Section headers
            pPainter->setFont(QFont("Consolas", 8, QFont::Bold));
            pPainter->setPen(QColor(150, 180, 255));
        } else if (line.startsWith("  ")) {
            // Data lines (indented)
            pPainter->setFont(QFont("Consolas", 8));
            pPainter->setPen(QColor(220, 220, 230));
        } else {
            // Default
            pPainter->setFont(detailFont);
            pPainter->setPen(QColor(200, 200, 210));
        }
        
        pPainter->drawText(QPointF(panelPos.x() + padding + 15, panelPos.y() + yOffset), line);
        yOffset += lineHeight;
    }
    
    // Draw connection line from panel to track
    QPainterPath connectorPath;
    if (panelPos.x() > screenPos.x()) {
        // Panel on right
        connectorPath.moveTo(panelPos.x(), panelPos.y() + panelHeight / 2);
        connectorPath.lineTo(screenPos.x() + 15, screenPos.y());
    } else {
        // Panel on left
        connectorPath.moveTo(panelPos.x() + panelWidth, panelPos.y() + panelHeight / 2);
        connectorPath.lineTo(screenPos.x() - 15, screenPos.y());
    }
    
    pPainter->setPen(QPen(healthColor, 2, Qt::DotLine));
    pPainter->setBrush(Qt::NoBrush);
    pPainter->drawPath(connectorPath);
}

/**
 * @brief Draws a rotated drone image based on heading
 * @param pPainter QPainter instance
 * @param image Drone image to draw
 * @param screenPos Screen position of track
 * @param heading Heading angle in degrees (0 = North, clockwise)
 * @param scale Scale factor for the image
 */
void CTrackLayer::drawRotatedDroneImage(QPainter *pPainter, const QPixmap &image, 
                                       const QPointF &screenPos, double heading, double scale)
{
    if (image.isNull()) return;
    
    // Save the current painter state
    pPainter->save();
    
    // Move to the track position
    pPainter->translate(screenPos);
    
    // Rotate based on heading
    // Note: Heading is in degrees (0 = North, clockwise)
    // Qt rotation is counter-clockwise from the positive x-axis
    // So we need to adjust: Qt angle = 90 - heading
    pPainter->rotate(90 - heading);
    
    // Apply scale
    if (scale != 1.0) {
        pPainter->scale(scale, scale);
    }
    
    // Calculate the center point to draw from
    QPointF imageCenter(-image.width() / 2.0, -image.height() / 2.0);
    
    // Draw the image centered at the origin (which is now at screenPos)
    pPainter->drawPixmap(imageCenter, image);
    
    // Restore the painter state
    pPainter->restore();
}

/**
 * @brief Paints the tracks on the canvas
 * @param pPainter QPainter instance used for drawing
 */
void CTrackLayer::paint(QPainter *pPainter)
{
    if (!pPainter) return;

    pPainter->setRenderHint(QPainter::Antialiasing, true);

    // Transform geographic positions to screen coordinates
    const QgsMapToPixel &mapToPixel = m_canvas->mapSettings().mapToPixel();

    QList<stTrackDisplayInfo> listTracks = CDataWarehouse::getInstance()->getTrackList();

    stTrackDisplayInfo hoveredTrack;
    bool hasHoveredTrack = false;

    for (const stTrackDisplayInfo &track : listTracks) {
        QPointF ptScreen = mapToPixel.transform(QgsPointXY(track.lon, track.lat)).toQPointF();
        double pixelPerDegree = 1.0 / m_canvas->mapUnitsPerPixel();
        QColor clr = Qt::cyan;

        // Check track states
        bool isHovered = (track.nTrkId == m_hoveredTrackId);
        bool isHighlighted = m_highlightedTracks.contains(track.nTrkId);
        bool isFocused = (track.nTrkId == m_focusedTrackId);
        
        if (isHovered) {
            hoveredTrack = track;
            hasHoveredTrack = true;
        }

        switch (track.nTrackIden) {
        case TRACK_IDENTITY_UNKNOWN:
            clr = Qt::yellow;
            break;
        case TRACK_IDENTITY_FRIEND:
            clr = Qt::green;
            break;
        case TRACK_IDENTITY_HOSTILE:
            clr = Qt::red;
            break;
        }

        if (pixelPerDegree > PPI_VISIBLE_THRESHOLD) {
            // Determine track size based on state
            double trackSize = 4;
            if (isHighlighted) trackSize = 8;  // Larger for highlighted tracks
            if (isFocused) trackSize = 10;     // Even larger for focused tracks
            
            // Highlight effects
            if (isHighlighted || isFocused) {
                // Outer highlight ring
                pPainter->setPen(QPen(Qt::white, 3));
                pPainter->setBrush(Qt::NoBrush);
                pPainter->drawEllipse(ptScreen, trackSize + 6, trackSize + 6);

                // Inner glow effect
                pPainter->setPen(QPen(clr, 2));
                pPainter->drawEllipse(ptScreen, trackSize + 3, trackSize + 3);
            }
            
            // Hover effect (additional to highlight/focus)
            if (isHovered) {
                pPainter->setPen(QPen(Qt::cyan, 2));
                pPainter->setBrush(Qt::NoBrush);
                pPainter->drawEllipse(ptScreen, trackSize + 8, trackSize + 8);
            }

            bool drawnCustomImage = false;
            // Draw custom image if available for this track
            if (!track.imagePath.isEmpty()) {
                // Ensure pixmap cached; if not, attempt load
                if (!m_trackPixmaps.contains(track.nTrkId)) {
                    QPixmap pix(track.imagePath);
                    if (!pix.isNull()) {
                        m_trackPixmaps.insert(track.nTrkId, pix);
                    }
                }

                if (m_trackPixmaps.contains(track.nTrkId)) {
                    const QPixmap &pix = m_trackPixmaps.value(track.nTrkId);
                    int baseSize = isFocused ? 40 : (isHighlighted ? 32 : 24);
                    
                    // Round heading to nearest 5 degrees for better cache hits
                    int roundedHeading = static_cast<int>(track.heading / 5.0) * 5;
                    QString cacheKey = QString("%1_%2_%3").arg(track.nTrkId).arg(baseSize).arg(roundedHeading);
                    
                    QPixmap rotated;
                    if (m_rotatedImageCache.contains(cacheKey)) {
                        rotated = m_rotatedImageCache.value(cacheKey);
                    } else {
                        QPixmap scaled = pix.scaled(baseSize, baseSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                        QTransform transform;
                        transform.translate(scaled.width() / 2.0, scaled.height() / 2.0);
                        transform.rotate(-roundedHeading);
                        transform.translate(-scaled.width() / 2.0, -scaled.height() / 2.0);
                        rotated = scaled.transformed(transform, Qt::SmoothTransformation);
                        
                        // Limit cache size to prevent memory issues
                        if (m_rotatedImageCache.size() > 500) {
                            m_rotatedImageCache.clear();
                        }
                        m_rotatedImageCache.insert(cacheKey, rotated);
                    }

                    // Draw centered at ptScreen
                    QPointF topLeft(ptScreen.x() - rotated.width() / 2.0,
                                    ptScreen.y() - rotated.height() / 2.0);
                    pPainter->drawPixmap(topLeft, rotated);
                    drawnCustomImage = true;
                }
            }

            if (!drawnCustomImage) {
                // Fallback: draw a small default drone-like marker oriented by heading
                int baseSize = isFocused ? 28 : (isHighlighted ? 22 : 16);
                int roundedHeading = static_cast<int>(track.heading / 5.0) * 5;
                QString cacheKey = QString("def_%1_%2_%3").arg(baseSize).arg(clr.name()).arg(roundedHeading);
                
                QPixmap rotated;
                if (m_rotatedImageCache.contains(cacheKey)) {
                    rotated = m_rotatedImageCache.value(cacheKey);
                } else {
                    QPixmap defaultIcon;
                    QString iconKey = QString("%1x%2_%3").arg(baseSize).arg(baseSize).arg(clr.name());
                    if (m_defaultIconCache.contains(iconKey)) {
                        defaultIcon = m_defaultIconCache.value(iconKey);
                    } else {
                        defaultIcon = getDefaultDronePixmap(baseSize, clr, Qt::white);
                        m_defaultIconCache.insert(iconKey, defaultIcon);
                    }
                    
                    QTransform transform;
                    transform.translate(defaultIcon.width() / 2.0, defaultIcon.height() / 2.0);
                    transform.rotate(-roundedHeading);
                    transform.translate(-defaultIcon.width() / 2.0, -defaultIcon.height() / 2.0);
                    rotated = defaultIcon.transformed(transform, Qt::SmoothTransformation);
                    
                    if (m_rotatedImageCache.size() > 500) {
                        m_rotatedImageCache.clear();
                    }
                    m_rotatedImageCache.insert(cacheKey, rotated);
                }
                
                QPointF topLeft(ptScreen.x() - rotated.width() / 2.0,
                                ptScreen.y() - rotated.height() / 2.0);
                pPainter->drawPixmap(topLeft, rotated);
            }

            // Draw speed vector instead of simple heading line
            // DISABLED: drawSpeedVector(pPainter, track, ptScreen, clr);

            if (pixelPerDegree > TEXT_VISIBLE_THRESHOLD) {
                pPainter->setFont(QFont("century", 11, 80, true));
                // Label
                pPainter->setPen(Qt::white);
                pPainter->drawText(ptScreen + QPointF(6, -6), QString::number(track.nTrkId));
            }
        }

        // Simplified blip animation - only for highlighted/focused tracks to improve performance
        if (isHighlighted || isFocused || isHovered) {
            int nMaxRadius = 20;
            int nCurrentRadius = 4 + (nAnimFrame * (nMaxRadius - 4) / 20);
            int nAlpha = 255 - (nAnimFrame * 255 / 20);

            // Simplified blip - single color with alpha, no gradient
            QColor blipColor = clr;
            blipColor.setAlpha(nAlpha);

            pPainter->setPen(Qt::NoPen);
            pPainter->setBrush(blipColor);
            pPainter->drawEllipse(ptScreen, nCurrentRadius, nCurrentRadius);
        }
        
        // Draw history trail if enabled - optimized version
        if (track.showHistory && !track.historyPoints.isEmpty()) {
            int totalPoints = track.historyPoints.size();
            
            // Only draw every Nth point for better performance with large histories
            int step = (totalPoints > 50) ? 2 : 1;
            
            QPainterPath historyPath;
            bool firstPoint = true;
            
            for (int i = 0; i < totalPoints; i += step) {
                const stTrackHistoryPoint &histPoint = track.historyPoints[i];
                QPointF histScreen = mapToPixel.transform(QgsPointXY(histPoint.lon, histPoint.lat)).toQPointF();
                
                if (firstPoint) {
                    historyPath.moveTo(histScreen);
                    firstPoint = false;
                } else {
                    historyPath.lineTo(histScreen);
                }
            }
            
            // Draw the history trail line
            QColor trailColor = clr;
            trailColor.setAlpha(120);
            pPainter->setPen(QPen(trailColor, 1.5, Qt::DashLine));
            pPainter->setBrush(Qt::NoBrush);
            pPainter->drawPath(historyPath);
            
            // Draw line from last history point to current position
            const stTrackHistoryPoint &lastPoint = track.historyPoints.last();
            QPointF lastScreen = mapToPixel.transform(QgsPointXY(lastPoint.lon, lastPoint.lat)).toQPointF();
            
            QColor connectColor = clr;
            connectColor.setAlpha(180);
            pPainter->setPen(QPen(connectColor, 2, Qt::SolidLine));
            pPainter->drawLine(lastScreen, ptScreen);
        }
    }

    // Draw focused track datatip (always visible, follows track)
    if (m_focusedTrackId != -1) {
        for (const stTrackDisplayInfo &track : listTracks) {
            if (track.nTrkId == m_focusedTrackId) {
                QPointF focusedScreen = mapToPixel.transform(QgsPointXY(track.lon, track.lat)).toQPointF();
                
                // Draw drone internal details if this track has an associated drone
                if (track.pDrone) {
                    drawDroneInternalDetails(pPainter, track, focusedScreen);
                } else {
                    // Otherwise draw regular datatip
                    drawFocusedTrackDatatip(pPainter, track, focusedScreen);
                }
                break;
            }
        }
    }
    
    // Draw tooltip for hovered track (draw last so it's on top, but not for focused tracks)
    if (hasHoveredTrack && m_hoveredTrackId != -1 && m_hoveredTrackId != m_focusedTrackId) {
        drawTooltip(pPainter, hoveredTrack, m_mousePos);
    }
}


QPixmap CTrackLayer::getDefaultDronePixmap(int size, const QColor &color, const QColor &accent)
{
    // Create a triangular arrow-like shape to represent a drone nose-forward
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPointF center(size / 2.0, size / 2.0);
    double bodyRadius = size * 0.35;

    // Body circle
    painter.setPen(Qt::NoPen);
    QColor bodyColor = color;
    bodyColor.setAlpha(230);
    painter.setBrush(bodyColor);
    painter.drawEllipse(center, bodyRadius, bodyRadius);

    // Nose triangle pointing up (will be rotated later)
    QPainterPath nose;
    double tipY = size * 0.08;
    double baseY = size * 0.45;
    double halfWidth = size * 0.18;
    nose.moveTo(center.x(), tipY);
    nose.lineTo(center.x() - halfWidth, baseY);
    nose.lineTo(center.x() + halfWidth, baseY);
    nose.closeSubpath();
    QColor noseColor = accent;
    noseColor.setAlpha(240);
    painter.setBrush(noseColor);
    painter.drawPath(nose);

    // Tail fin
    QPainterPath tail;
    double tailTop = size * 0.55;
    double tailBottom = size * 0.9;
    double tailHalfWidth = size * 0.12;
    tail.moveTo(center.x(), tailTop);
    tail.lineTo(center.x() - tailHalfWidth, tailBottom);
    tail.lineTo(center.x() + tailHalfWidth, tailBottom);
    tail.closeSubpath();
    QColor tailColor = color.darker(130);
    tailColor.setAlpha(220);
    painter.setBrush(tailColor);
    painter.drawPath(tail);

    // Outline
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(255,255,255,180), 1));
    painter.drawEllipse(center, bodyRadius, bodyRadius);

    painter.end();
    return pix;
}

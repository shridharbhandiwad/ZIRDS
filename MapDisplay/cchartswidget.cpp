#include "cchartswidget.h"
#include "../cdatawarehouse.h"
#include "globalmacros.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainterPath>

CChartsWidget::CChartsWidget(QWidget *parent)
    : QDockWidget("Radar Charts", parent)
{
    setupUI();
    applyRichStyle();

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &CChartsWidget::updateCharts);
    m_updateTimer->start(500); // Update every 500ms
}

CChartsWidget::~CChartsWidget()
{
}

void CChartsWidget::setupUI()
{
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);

    // Toolbar
    QHBoxLayout *toolbarLayout = new QHBoxLayout();

    QLabel *lblTitle = new QLabel("📊 Radar Display Charts");
    lblTitle->setStyleSheet("color: #ffffff; font-size: 14px; font-weight: bold;");
    toolbarLayout->addWidget(lblTitle);

    toolbarLayout->addStretch();

    QPushButton *btnExport = new QPushButton("Export");
    btnExport->setStyleSheet(
        "QPushButton {"
        "   background-color: #667eea;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 6px 12px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #5568d3;"
        "}"
    );
    connect(btnExport, &QPushButton::clicked, this, &CChartsWidget::exportChart);
    toolbarLayout->addWidget(btnExport);

    mainLayout->addLayout(toolbarLayout);

    // Tab widget for different chart types
    m_tabWidget = new QTabWidget();
    m_tabWidget->setTabPosition(QTabWidget::North);

    createBScopeTab();
    createCScopeTab();
    createRHITab();
    createTimeSeriesTab();

    mainLayout->addWidget(m_tabWidget);

    setWidget(mainWidget);
}

void CChartsWidget::createBScopeTab()
{
    QWidget *bscopeTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(bscopeTab);
    layout->setContentsMargins(0, 0, 0, 0);

    m_bscopeChart = new CCustomChart(CCustomChart::BScope, this);
    layout->addWidget(m_bscopeChart);

    QLabel *lblInfo = new QLabel("📡 B-Scope: Range vs Azimuth Display");
    lblInfo->setStyleSheet("color: #a0aec0; font-size: 11px; padding: 4px;");
    lblInfo->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblInfo);

    m_tabWidget->addTab(bscopeTab, "📡 B-Scope");
}

void CChartsWidget::createCScopeTab()
{
    QWidget *cscopeTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(cscopeTab);
    layout->setContentsMargins(0, 0, 0, 0);

    m_cscopeChart = new CCustomChart(CCustomChart::CScope, this);
    layout->addWidget(m_cscopeChart);

    QLabel *lblInfo = new QLabel("🎯 C-Scope: Azimuth vs Elevation Display");
    lblInfo->setStyleSheet("color: #a0aec0; font-size: 11px; padding: 4px;");
    lblInfo->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblInfo);

    m_tabWidget->addTab(cscopeTab, "🎯 C-Scope");
}

void CChartsWidget::createRHITab()
{
    QWidget *rhiTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(rhiTab);
    layout->setContentsMargins(0, 0, 0, 0);

    m_rhiChart = new CCustomChart(CCustomChart::RHI, this);
    layout->addWidget(m_rhiChart);

    QLabel *lblInfo = new QLabel("📐 RHI: Range Height Indicator");
    lblInfo->setStyleSheet("color: #a0aec0; font-size: 11px; padding: 4px;");
    lblInfo->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblInfo);

    m_tabWidget->addTab(rhiTab, "📐 RHI");
}

void CChartsWidget::createTimeSeriesTab()
{
    QWidget *timeTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(timeTab);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    // Selector for time series type
    QHBoxLayout *selectorLayout = new QHBoxLayout();
    QLabel *lblSelect = new QLabel("Chart Type:");
    lblSelect->setStyleSheet("color: #ffffff; font-weight: bold;");
    selectorLayout->addWidget(lblSelect);

    m_comboTimeSeriesType = new QComboBox();
    m_comboTimeSeriesType->addItems({
        "📏 Range vs Time",
        "🧭 Azimuth vs Time",
        "📐 Elevation vs Time",
        "📶 RCS vs Time",
        "⚡ Speed vs Time"
    });
    m_comboTimeSeriesType->setStyleSheet(
        "QComboBox {"
        "   background-color: #2d3748;"
        "   color: #ffffff;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 6px;"
        "   padding: 6px;"
        "}"
        "QComboBox:hover {"
        "   border: 2px solid #667eea;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #2d3748;"
        "   color: #ffffff;"
        "   selection-background-color: #667eea;"
        "}"
    );
    connect(m_comboTimeSeriesType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CChartsWidget::onChartTypeChanged);
    selectorLayout->addWidget(m_comboTimeSeriesType);
    selectorLayout->addStretch();

    layout->addLayout(selectorLayout);

    // Create default chart (Range vs Time)
    m_rangeTimeChart = new CCustomChart(CCustomChart::RangeTime, this);
    layout->addWidget(m_rangeTimeChart);

    // Store other charts (hidden initially)
    m_azimuthTimeChart = new CCustomChart(CCustomChart::AzimuthTime, this);
    m_elevationTimeChart = new CCustomChart(CCustomChart::ElevationTime, this);
    m_rcsTimeChart = new CCustomChart(CCustomChart::RCSTime, this);
    m_speedTimeChart = new CCustomChart(CCustomChart::SpeedTime, this);

    m_azimuthTimeChart->hide();
    m_elevationTimeChart->hide();
    m_rcsTimeChart->hide();
    m_speedTimeChart->hide();

    layout->addWidget(m_azimuthTimeChart);
    layout->addWidget(m_elevationTimeChart);
    layout->addWidget(m_rcsTimeChart);
    layout->addWidget(m_speedTimeChart);

    m_tabWidget->addTab(timeTab, "📈 Time Series");
}

void CChartsWidget::updateCharts()
{
    // Update all charts with current data
    if (m_bscopeChart) m_bscopeChart->updateData();
    if (m_cscopeChart) m_cscopeChart->updateData();
    if (m_rhiChart) m_rhiChart->updateData();
    if (m_rangeTimeChart && m_rangeTimeChart->isVisible()) m_rangeTimeChart->updateData();
    if (m_azimuthTimeChart && m_azimuthTimeChart->isVisible()) m_azimuthTimeChart->updateData();
    if (m_elevationTimeChart && m_elevationTimeChart->isVisible()) m_elevationTimeChart->updateData();
    if (m_rcsTimeChart && m_rcsTimeChart->isVisible()) m_rcsTimeChart->updateData();
    if (m_speedTimeChart && m_speedTimeChart->isVisible()) m_speedTimeChart->updateData();
}

void CChartsWidget::onChartTypeChanged(int index)
{
    // Hide all time series charts
    m_rangeTimeChart->hide();
    m_azimuthTimeChart->hide();
    m_elevationTimeChart->hide();
    m_rcsTimeChart->hide();
    m_speedTimeChart->hide();

    // Show selected chart
    switch (index) {
        case 0: m_rangeTimeChart->show(); break;
        case 1: m_azimuthTimeChart->show(); break;
        case 2: m_elevationTimeChart->show(); break;
        case 3: m_rcsTimeChart->show(); break;
        case 4: m_speedTimeChart->show(); break;
    }
}

void CChartsWidget::exportChart()
{
    QMessageBox::information(this, "Export", "Chart export functionality - Coming soon!");
}

void CChartsWidget::applyRichStyle()
{
    setStyleSheet(
        "QDockWidget {"
        "   background-color: #1a202c;"
        "   color: #ffffff;"
        "}"
        "QDockWidget::title {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2);"
        "   padding: 10px;"
        "   text-align: center;"
        "}"
    );

    m_tabWidget->setStyleSheet(
        "QTabWidget::pane {"
        "   background-color: #1a202c;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 8px;"
        "}"
        "QTabBar::tab {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2d3748, stop:1 #1a202c);"
        "   color: #a0aec0;"
        "   padding: 12px 20px;"
        "   margin: 2px;"
        "   border-top-left-radius: 8px;"
        "   border-top-right-radius: 8px;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "}"
        "QTabBar::tab:selected {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2);"
        "   color: white;"
        "}"
        "QTabBar::tab:hover:!selected {"
        "   background: #2d3748;"
        "   color: #ffffff;"
        "}"
    );
}

// ============================================================================
// CCustomChart Implementation
// ============================================================================

CCustomChart::CCustomChart(ChartType type, QWidget *parent)
    : QWidget(parent),
      m_chartType(type),
      m_hoveredIndex(-1),
      m_showTooltip(false),
      m_zoomLevel(1.0)
{
    setMinimumHeight(400);
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);

    setStyleSheet("background-color: #0f172a; border-radius: 8px;");
}

CCustomChart::~CCustomChart()
{
}

void CCustomChart::setChartType(ChartType type)
{
    m_chartType = type;
    update();
}

void CCustomChart::updateData()
{
    QList<stTrackDisplayInfo> tracks = CDataWarehouse::getInstance()->getTrackList();

    m_currentData.clear();
    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

    for (const stTrackDisplayInfo &track : tracks) {
        TrackData data;
        data.trackId = track.nTrkId;
        data.range = track.range;
        data.azimuth = track.azimuth;
        data.elevation = track.elevation;
        data.rcs = 20.0; // Placeholder RCS value
        data.speed = 0.0; // Will be calculated from history
        data.timestamp = currentTime;

        // Color based on identity
        switch (track.nTrackIden) {
            case TRACK_IDENTITY_FRIEND:
                data.color = QColor(46, 204, 113);
                break;
            case TRACK_IDENTITY_HOSTILE:
                data.color = QColor(231, 76, 60);
                break;
            case TRACK_IDENTITY_UNKNOWN:
                data.color = QColor(241, 196, 15);
                break;
            default:
                data.color = QColor(149, 165, 166);
                break;
        }

        m_currentData.append(data);

        // Store in historical data (keep last 100 points)
        if (!m_historicalData.contains(track.nTrkId)) {
            m_historicalData[track.nTrkId] = QList<TrackData>();
        }
        m_historicalData[track.nTrkId].append(data);
        if (m_historicalData[track.nTrkId].size() > 100) {
            m_historicalData[track.nTrkId].removeFirst();
        }

        // Calculate speed from history
        if (m_historicalData[track.nTrkId].size() > 1) {
            auto &history = m_historicalData[track.nTrkId];
            auto &prev = history[history.size() - 2];
            auto &curr = history[history.size() - 1];

            double timeDiff = (curr.timestamp - prev.timestamp) / 1000.0; // seconds
            if (timeDiff > 0) {
                double rangeDiff = curr.range - prev.range;
                data.speed = rangeDiff / timeDiff;
            }
        }
    }

    update();
}

void CCustomChart::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw based on chart type
    switch (m_chartType) {
        case BScope:
            drawBScope(&painter);
            break;
        case CScope:
            drawCScope(&painter);
            break;
        case RHI:
            drawRHI(&painter);
            break;
        case RangeTime:
        case AzimuthTime:
        case ElevationTime:
        case RCSTime:
        case SpeedTime:
            drawTimeSeries(&painter);
            break;
    }

    // Draw tooltip if hovering
    if (m_showTooltip) {
        drawTooltip(&painter);
    }
}

void CCustomChart::drawBScope(QPainter *painter)
{
    QRectF chartRect = rect().adjusted(60, 40, -40, -60);

    // Draw grid and axes
    drawGrid(painter, chartRect);
    drawAxes(painter, chartRect, "Azimuth (°)", "Range (km)");

    // Draw tracks
    for (int i = 0; i < m_currentData.size(); ++i) {
        const auto &data = m_currentData[i];

        // Convert azimuth and range to chart coordinates
        double azimuthNorm = (data.azimuth + 180.0) / 360.0; // Normalize to 0-1
        double rangeNorm = data.range / 50000.0; // Max range 50km

        double x = chartRect.left() + chartRect.width() * azimuthNorm;
        double y = chartRect.bottom() - chartRect.height() * rangeNorm;

        // Highlight if hovered
        if (i == m_hoveredIndex) {
            painter->setPen(QPen(Qt::white, 3));
            painter->drawEllipse(QPointF(x, y), 12, 12);
        }

        // Draw track point
        painter->setPen(QPen(data.color, 2));
        painter->setBrush(data.color);
        painter->drawEllipse(QPointF(x, y), 6, 6);

        // Draw track ID
        painter->setPen(Qt::white);
        painter->setFont(QFont("Arial", 9, QFont::Bold));
        painter->drawText(QPointF(x + 10, y - 5), QString::number(data.trackId));
    }

    // Draw legend
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 10, QFont::Bold));
    painter->drawText(rect().adjusted(10, 10, 0, 0), "B-Scope Display");
}

void CCustomChart::drawCScope(QPainter *painter)
{
    QRectF chartRect = rect().adjusted(60, 40, -40, -60);

    drawGrid(painter, chartRect);
    drawAxes(painter, chartRect, "Azimuth (°)", "Elevation (°)");

    for (int i = 0; i < m_currentData.size(); ++i) {
        const auto &data = m_currentData[i];

        double azimuthNorm = (data.azimuth + 180.0) / 360.0;
        double elevationNorm = (data.elevation + 90.0) / 180.0;

        double x = chartRect.left() + chartRect.width() * azimuthNorm;
        double y = chartRect.bottom() - chartRect.height() * elevationNorm;

        if (i == m_hoveredIndex) {
            painter->setPen(QPen(Qt::white, 3));
            painter->drawEllipse(QPointF(x, y), 12, 12);
        }

        painter->setPen(QPen(data.color, 2));
        painter->setBrush(data.color);
        painter->drawEllipse(QPointF(x, y), 6, 6);

        painter->setPen(Qt::white);
        painter->setFont(QFont("Arial", 9, QFont::Bold));
        painter->drawText(QPointF(x + 10, y - 5), QString::number(data.trackId));
    }

    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 10, QFont::Bold));
    painter->drawText(rect().adjusted(10, 10, 0, 0), "C-Scope Display");
}

void CCustomChart::drawRHI(QPainter *painter)
{
    QRectF chartRect = rect().adjusted(60, 40, -40, -60);

    drawGrid(painter, chartRect);
    drawAxes(painter, chartRect, "Range (km)", "Height (km)");

    for (int i = 0; i < m_currentData.size(); ++i) {
        const auto &data = m_currentData[i];

        double rangeNorm = data.range / 50000.0;
        double height = data.range * qSin(qDegreesToRadians(data.elevation));
        double heightNorm = height / 15000.0; // Max height 15km

        double x = chartRect.left() + chartRect.width() * rangeNorm;
        double y = chartRect.bottom() - chartRect.height() * heightNorm;

        if (i == m_hoveredIndex) {
            painter->setPen(QPen(Qt::white, 3));
            painter->drawEllipse(QPointF(x, y), 12, 12);
        }

        painter->setPen(QPen(data.color, 2));
        painter->setBrush(data.color);
        painter->drawEllipse(QPointF(x, y), 6, 6);

        painter->setPen(Qt::white);
        painter->setFont(QFont("Arial", 9, QFont::Bold));
        painter->drawText(QPointF(x + 10, y - 5), QString::number(data.trackId));
    }

    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 10, QFont::Bold));
    painter->drawText(rect().adjusted(10, 10, 0, 0), "RHI Display");
}

void CCustomChart::drawTimeSeries(QPainter *painter)
{
    QRectF chartRect = rect().adjusted(60, 40, -40, -60);

    QString yLabel;
    switch (m_chartType) {
        case RangeTime: yLabel = "Range (km)"; break;
        case AzimuthTime: yLabel = "Azimuth (°)"; break;
        case ElevationTime: yLabel = "Elevation (°)"; break;
        case RCSTime: yLabel = "RCS (dBsm)"; break;
        case SpeedTime: yLabel = "Speed (m/s)"; break;
        default: yLabel = "Value"; break;
    }

    drawGrid(painter, chartRect);
    drawAxes(painter, chartRect, "Time (s)", yLabel);

    // Draw time series for each track
    for (auto it = m_historicalData.begin(); it != m_historicalData.end(); ++it) {
        int trackId = it.key();
        const auto &history = it.value();

        if (history.size() < 2) continue;

        QColor trackColor = history.last().color;
        QPainterPath path;

        qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        double timeWindow = 60000.0; // 60 seconds window

        for (int i = 0; i < history.size(); ++i) {
            const auto &data = history[i];

            double timeNorm = 1.0 - (currentTime - data.timestamp) / timeWindow;
            if (timeNorm < 0) continue;

            double valueNorm = 0.0;
            switch (m_chartType) {
                case RangeTime:
                    valueNorm = data.range / 50000.0;
                    break;
                case AzimuthTime:
                    valueNorm = (data.azimuth + 180.0) / 360.0;
                    break;
                case ElevationTime:
                    valueNorm = (data.elevation + 90.0) / 180.0;
                    break;
                case RCSTime:
                    valueNorm = (data.rcs + 20.0) / 80.0;
                    break;
                case SpeedTime:
                    valueNorm = qAbs(data.speed) / 100.0;
                    break;
                default:
                    break;
            }

            double x = chartRect.left() + chartRect.width() * timeNorm;
            double y = chartRect.bottom() - chartRect.height() * valueNorm;

            if (i == 0) {
                path.moveTo(x, y);
            } else {
                path.lineTo(x, y);
            }
        }

        painter->setPen(QPen(trackColor, 2));
        painter->drawPath(path);

        // Draw last point
        if (!history.isEmpty()) {
            const auto &lastData = history.last();
            double timeNorm = 1.0 - (currentTime - lastData.timestamp) / timeWindow;

            if (timeNorm >= 0) {
                double valueNorm = 0.0;
                switch (m_chartType) {
                    case RangeTime: valueNorm = lastData.range / 50000.0; break;
                    case AzimuthTime: valueNorm = (lastData.azimuth + 180.0) / 360.0; break;
                    case ElevationTime: valueNorm = (lastData.elevation + 90.0) / 180.0; break;
                    case RCSTime: valueNorm = (lastData.rcs + 20.0) / 80.0; break;
                    case SpeedTime: valueNorm = qAbs(lastData.speed) / 100.0; break;
                    default: break;
                }

                double x = chartRect.left() + chartRect.width() * timeNorm;
                double y = chartRect.bottom() - chartRect.height() * valueNorm;

                painter->setPen(QPen(trackColor, 2));
                painter->setBrush(trackColor);
                painter->drawEllipse(QPointF(x, y), 5, 5);

                // Draw track ID
                painter->setPen(Qt::white);
                painter->setFont(QFont("Arial", 9, QFont::Bold));
                painter->drawText(QPointF(x + 8, y - 3), QString::number(trackId));
            }
        }
    }

    // Draw title
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 10, QFont::Bold));
    QString title;
    switch (m_chartType) {
        case RangeTime: title = "Range vs Time"; break;
        case AzimuthTime: title = "Azimuth vs Time"; break;
        case ElevationTime: title = "Elevation vs Time"; break;
        case RCSTime: title = "RCS vs Time"; break;
        case SpeedTime: title = "Speed vs Time"; break;
        default: title = "Time Series"; break;
    }
    painter->drawText(rect().adjusted(10, 10, 0, 0), title);
}

void CCustomChart::drawGrid(QPainter *painter, const QRectF &rect)
{
    painter->setPen(QPen(QColor(74, 85, 104, 100), 1));

    // Vertical grid lines
    for (int i = 0; i <= 10; ++i) {
        double x = rect.left() + (rect.width() / 10) * i;
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }

    // Horizontal grid lines
    for (int i = 0; i <= 10; ++i) {
        double y = rect.top() + (rect.height() / 10) * i;
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }
}

void CCustomChart::drawAxes(QPainter *painter, const QRectF &rect, const QString &xLabel, const QString &yLabel)
{
    painter->setPen(QPen(QColor(160, 174, 192), 2));

    // X axis
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());

    // Y axis
    painter->drawLine(rect.bottomLeft(), rect.topLeft());

    // Labels
    painter->setPen(QColor(226, 232, 240));
    painter->setFont(QFont("Arial", 10, QFont::Bold));

    // X label
    painter->drawText(QRectF(rect.left(), rect.bottom() + 20, rect.width(), 30),
                     Qt::AlignCenter, xLabel);

    // Y label (rotated)
    painter->save();
    painter->translate(rect.left() - 45, rect.top() + rect.height() / 2);
    painter->rotate(-90);
    painter->drawText(QRectF(-100, 0, 200, 30), Qt::AlignCenter, yLabel);
    painter->restore();
}

void CCustomChart::drawTooltip(QPainter *painter)
{
    if (m_hoveredIndex < 0 || m_hoveredIndex >= m_currentData.size()) return;

    const auto &data = m_currentData[m_hoveredIndex];

    QStringList lines;
    lines << QString("Track ID: %1").arg(data.trackId);
    lines << QString("Range: %1 km").arg(data.range / 1000.0, 0, 'f', 2);
    lines << QString("Azimuth: %1°").arg(data.azimuth, 0, 'f', 1);
    lines << QString("Elevation: %1°").arg(data.elevation, 0, 'f', 1);

    QString tooltipText = lines.join("\n");

    QFont font("Arial", 9);
    QFontMetrics fm(font);

    int maxWidth = 0;
    for (const QString &line : lines) {
        maxWidth = qMax(maxWidth, fm.horizontalAdvance(line));
    }

    int tooltipWidth = maxWidth + 20;
    int tooltipHeight = lines.count() * (fm.height() + 2) + 15;

    QPointF tooltipPos = m_mousePos + QPointF(15, -tooltipHeight - 10);

    // Keep within bounds
    if (tooltipPos.x() + tooltipWidth > width()) {
        tooltipPos.setX(m_mousePos.x() - tooltipWidth - 15);
    }
    if (tooltipPos.y() < 0) {
        tooltipPos.setY(m_mousePos.y() + 20);
    }

    QRectF tooltipRect(tooltipPos, QSizeF(tooltipWidth, tooltipHeight));

    // Shadow
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 120));
    painter->drawRoundedRect(tooltipRect.adjusted(2, 2, 2, 2), 6, 6);

    // Background
    QLinearGradient bgGradient(tooltipRect.topLeft(), tooltipRect.bottomLeft());
    bgGradient.setColorAt(0, QColor(45, 55, 72, 250));
    bgGradient.setColorAt(1, QColor(30, 41, 59, 250));
    painter->setBrush(bgGradient);
    painter->drawRoundedRect(tooltipRect, 6, 6);

    // Border
    painter->setPen(QPen(data.color, 2));
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(tooltipRect, 6, 6);

    // Text
    painter->setPen(Qt::white);
    painter->setFont(font);
    int yOffset = 10;
    for (const QString &line : lines) {
        painter->drawText(QPointF(tooltipPos.x() + 10, tooltipPos.y() + yOffset + fm.ascent()), line);
        yOffset += fm.height() + 2;
    }
}

void CCustomChart::mouseMoveEvent(QMouseEvent *event)
{
    m_mousePos = event->pos();
    m_hoveredIndex = findNearestDataPoint(m_mousePos);
    m_showTooltip = (m_hoveredIndex != -1);
    update();
}

void CCustomChart::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    // Handle click events if needed
}

void CCustomChart::wheelEvent(QWheelEvent *event)
{
    // Zoom functionality
    double delta = event->angleDelta().y() / 120.0;
    m_zoomLevel += delta * 0.1;
    m_zoomLevel = qBound(0.5, m_zoomLevel, 3.0);
    update();
}

void CCustomChart::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_showTooltip = false;
    m_hoveredIndex = -1;
    update();
}

int CCustomChart::findNearestDataPoint(const QPointF &mousePos)
{
    QRectF chartRect = rect().adjusted(60, 40, -40, -60);

    if (!chartRect.contains(mousePos)) {
        return -1;
    }

    int nearestIndex = -1;
    double minDistance = 20.0; // Max distance to consider

    for (int i = 0; i < m_currentData.size(); ++i) {
        QPointF dataPos = getDataPointPosition(i, chartRect);
        double distance = QLineF(mousePos, dataPos).length();

        if (distance < minDistance) {
            minDistance = distance;
            nearestIndex = i;
        }
    }

    return nearestIndex;
}

QPointF CCustomChart::getDataPointPosition(int index, const QRectF &rect)
{
    if (index < 0 || index >= m_currentData.size()) {
        return QPointF();
    }

    const auto &data = m_currentData[index];
    double x = 0, y = 0;

    switch (m_chartType) {
        case BScope: {
            double azimuthNorm = (data.azimuth + 180.0) / 360.0;
            double rangeNorm = data.range / 50000.0;
            x = rect.left() + rect.width() * azimuthNorm;
            y = rect.bottom() - rect.height() * rangeNorm;
            break;
        }
        case CScope: {
            double azimuthNorm = (data.azimuth + 180.0) / 360.0;
            double elevationNorm = (data.elevation + 90.0) / 180.0;
            x = rect.left() + rect.width() * azimuthNorm;
            y = rect.bottom() - rect.height() * elevationNorm;
            break;
        }
        case RHI: {
            double rangeNorm = data.range / 50000.0;
            double height = data.range * qSin(qDegreesToRadians(data.elevation));
            double heightNorm = height / 15000.0;
            x = rect.left() + rect.width() * rangeNorm;
            y = rect.bottom() - rect.height() * heightNorm;
            break;
        }
        default:
            break;
    }

    return QPointF(x, y);
}

#include "chomepositionhighlightlayer.h"
#include <QPainter>
#include <QDateTime>
#include <QtMath>

CHomePositionHighlightLayer::CHomePositionHighlightLayer(QgsMapCanvas *canvas)
    : QgsMapCanvasItem(canvas)
    , m_canvas(canvas)
    , m_animationTimer(new QTimer(this))
    , m_isAnimating(false)
    , m_animationDuration(3000)
    , m_animationStartTime(0)
{
    setZValue(150); // Higher than PPI layer (100) to appear on top
    hide(); // Initially hidden

    // Setup animation timer
    connect(m_animationTimer, &QTimer::timeout, this, &CHomePositionHighlightLayer::updateAnimation);
}

CHomePositionHighlightLayer::~CHomePositionHighlightLayer()
{
    stopAnimation();
}

void CHomePositionHighlightLayer::setCenter(const QgsPointXY &center)
{
    m_center = center;
    updatePosition();
}

void CHomePositionHighlightLayer::startAnimation(int durationMs)
{
    if (m_isAnimating) {
        stopAnimation();
    }

    m_animationDuration = durationMs;
    m_animationStartTime = QDateTime::currentMSecsSinceEpoch();
    m_isAnimating = true;

    createCircles();
    show();

    // Start animation timer at target FPS
    m_animationTimer->start(1000 / ANIMATION_FPS);

    qDebug() << "Home position highlight animation started for" << durationMs << "ms";
}

void CHomePositionHighlightLayer::stopAnimation()
{
    if (!m_isAnimating) return;

    m_animationTimer->stop();
    m_isAnimating = false;
    clearCircles();
    hide();
    update();

    qDebug() << "Home position highlight animation stopped";
}

void CHomePositionHighlightLayer::createCircles()
{
    m_circles.clear();

    // Create initial circles with staggered start times
    for (int i = 0; i < NUM_CIRCLES; ++i) {
        Circle circle;
        circle.radius = 0.0;
        circle.opacity = 1.0;
        circle.age = -i * 0.25; // Stagger start times (negative age = not yet started)
        m_circles.append(circle);
    }
}

void CHomePositionHighlightLayer::clearCircles()
{
    m_circles.clear();
}

void CHomePositionHighlightLayer::updateAnimation()
{
    if (!m_isAnimating) return;

    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 elapsed = currentTime - m_animationStartTime;
    double progress = static_cast<double>(elapsed) / m_animationDuration;

    // Check if animation should end
    if (progress >= 1.0) {
        stopAnimation();
        return;
    }

    // Update each circle
    bool hasActiveCircles = false;
    for (int i = 0; i < m_circles.size(); ++i) {
        Circle &circle = m_circles[i];
        
        // Update age based on progress
        circle.age = progress - (i * 0.25);
        
        if (circle.age >= 0.0 && circle.age <= 1.0) {
            hasActiveCircles = true;
            
            // Expand radius with easing (ease-out)
            double easedAge = 1.0 - qPow(1.0 - circle.age, 3); // Cubic ease-out
            circle.radius = MAX_RADIUS * easedAge;
            
            // Fade out opacity (linear fade starting at 50% progress)
            if (circle.age > 0.5) {
                circle.opacity = 2.0 * (1.0 - circle.age);
            } else {
                circle.opacity = 1.0;
            }
        }
    }

    updatePosition();
    update();

    if (!hasActiveCircles) {
        stopAnimation();
    }
}

void CHomePositionHighlightLayer::updatePosition()
{
    if (!m_canvas) return;

    const QgsMapToPixel &mapToPixel = m_canvas->mapSettings().mapToPixel();
    m_centerScreen = mapToPixel.transform(m_center).toQPointF();
    update();
}

QRectF CHomePositionHighlightLayer::boundingRect() const
{
    if (!m_canvas || !m_isAnimating) {
        return QRectF();
    }

    // Calculate bounding rectangle large enough for maximum circle radius
    double radiusPixels = MAX_RADIUS / m_canvas->mapSettings().mapUnitsPerPixel();
    double padding = CIRCLE_WIDTH + 10; // Extra padding for line width and antialiasing
    
    return QRectF(
        m_centerScreen.x() - radiusPixels - padding,
        m_centerScreen.y() - radiusPixels - padding,
        (radiusPixels + padding) * 2,
        (radiusPixels + padding) * 2
    );
}

void CHomePositionHighlightLayer::paint(QPainter *painter)
{
    if (!m_canvas || !m_isAnimating || m_circles.isEmpty()) {
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    // Calculate meters to pixels conversion
    double metersPerDegreeLat = 111132.0;
    double metersPerDegreeLon = 111320.0 * std::cos(qDegreesToRadians(m_center.y()));
    
    double pixelPerDegree = 1.0 / m_canvas->mapUnitsPerPixel();
    double pixelPerMeterX = pixelPerDegree / metersPerDegreeLon;
    double pixelPerMeterY = pixelPerDegree / metersPerDegreeLat;
    double avgPixelPerMeter = (pixelPerMeterX + pixelPerMeterY) / 2.0;

    // Draw each expanding circle
    for (const Circle &circle : m_circles) {
        if (circle.age < 0.0 || circle.age > 1.0 || circle.opacity <= 0.0) {
            continue; // Skip circles that haven't started or are fully faded
        }

        double radiusPixels = circle.radius * avgPixelPerMeter;
        
        // Create pen with current opacity
        QColor circleColor(0, 255, 255); // Cyan color (matching PPI rings)
        circleColor.setAlphaF(circle.opacity);
        
        QPen circlePen(circleColor, CIRCLE_WIDTH);
        painter->setPen(circlePen);
        
        // Draw the expanding circle
        painter->drawEllipse(m_centerScreen, radiusPixels, radiusPixels);
    }

    // Draw center marker (pulsing effect)
    double pulseScale = 1.0 + 0.3 * qSin(QDateTime::currentMSecsSinceEpoch() * 0.01);
    double markerRadius = 8.0 * pulseScale;
    
    QColor markerColor(255, 0, 0); // Red center marker
    markerColor.setAlphaF(0.9);
    
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(markerColor));
    painter->drawEllipse(m_centerScreen, markerRadius, markerRadius);
    
    // Draw inner white dot
    painter->setBrush(QBrush(QColor(255, 255, 255, 230)));
    painter->drawEllipse(m_centerScreen, markerRadius * 0.4, markerRadius * 0.4);
}

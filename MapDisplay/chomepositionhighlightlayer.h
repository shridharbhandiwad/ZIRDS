#ifndef CHOMEPOSITIONHIGHLIGHTLAYER_H
#define CHOMEPOSITIONHIGHLIGHTLAYER_H

#include <qgsmapcanvas.h>
#include <qgsmapcanvasitem.h>
#include <QTimer>
#include <QList>

/**
 * @brief Animated expanding circles overlay to highlight home position
 * 
 * This layer draws expanding and fading circles centered on the home/radar position
 * to provide visual feedback when navigating to home or zoom-to-fit operations.
 */
class CHomePositionHighlightLayer : public QObject, public QgsMapCanvasItem
{
    Q_OBJECT

public:
    explicit CHomePositionHighlightLayer(QgsMapCanvas *canvas);
    ~CHomePositionHighlightLayer();

    /**
     * @brief Set the center position for the highlight animation
     * @param center Position in map coordinates (lon, lat)
     */
    void setCenter(const QgsPointXY &center);

    /**
     * @brief Start the expanding circles animation
     * @param durationMs Duration of animation in milliseconds (default: 3000ms)
     */
    void startAnimation(int durationMs = 3000);

    /**
     * @brief Stop the animation and hide the layer
     */
    void stopAnimation();

    /**
     * @brief Check if animation is currently running
     */
    bool isAnimating() const { return m_isAnimating; }

    void paint(QPainter *painter) override;

protected:
    QRectF boundingRect() const override;
    void updatePosition() override;

private slots:
    void updateAnimation();

private:
    struct Circle {
        double radius;      // Current radius in meters
        double opacity;     // Current opacity (0.0 - 1.0)
        double age;         // Age of circle (0.0 - 1.0, where 1.0 is fully expanded)
    };

    QgsMapCanvas *m_canvas;
    QgsPointXY m_center;              // Center position in map coordinates
    QPointF m_centerScreen;           // Center position in screen coordinates
    
    QTimer *m_animationTimer;
    QList<Circle> m_circles;
    
    bool m_isAnimating;
    int m_animationDuration;          // Total animation duration in ms
    qint64 m_animationStartTime;      // Animation start timestamp
    
    // Animation parameters
    static constexpr int NUM_CIRCLES = 3;         // Number of expanding circles
    static constexpr double MAX_RADIUS = 1500.0;  // Maximum radius in meters
    static constexpr double CIRCLE_WIDTH = 3.0;   // Line width for circles
    static constexpr int ANIMATION_FPS = 60;      // Animation frame rate

    void createCircles();
    void clearCircles();
};

#endif // CHOMEPOSITIONHIGHLIGHTLAYER_H

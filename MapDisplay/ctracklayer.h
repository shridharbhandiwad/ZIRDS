//#ifndef CTRACKLAYER_H
//#define CTRACKLAYER_H

//#include <qgsmapcanvasitem.h>
//#include <qgsmapcanvas.h>
//#include <QTimer>

//class CTrackLayer : public QObject, public QgsMapCanvasItem
//{
//    Q_OBJECT
//public:
//    CTrackLayer(QgsMapCanvas *canvas);

//    void paint(QPainter *painter) override;
//    QRectF boundingRect() const override;

//private slots:
//    void _UpdateAnimation(); //!< Timer-based animation tick

//private:
//    QgsMapCanvas *m_canvas;
//    QTimer m_timer;
//};

//#endif // CTRACKLAYER_H


#ifndef CTRACKLAYER_H
#define CTRACKLAYER_H

#include <qgsmapcanvasitem.h>
#include <qgsmapcanvas.h>
#include <QTimer>
#include <QMenu>
#include <QSet>
#include <QPixmap>
#include <QHash>
#include <QMap>

// Forward declaration
struct stTrackDisplayInfo;

class CTrackLayer : public QObject, public QgsMapCanvasItem
{
    Q_OBJECT
public:
    CTrackLayer(QgsMapCanvas *canvas);
    ~CTrackLayer();

    void paint(QPainter *painter) override;
    QRectF boundingRect() const override;

signals:
    void trackRightClicked(int trackId, const QPoint& globalPos);

protected:
    /**
     * @brief Event filter to capture mouse events from canvas
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void _UpdateAnimation(); //!< Timer-based animation tick

private slots:
    //void showContextMenu(const QPoint& pos);

private:
    QgsMapCanvas *m_canvas;
    QTimer m_timer;
    QMenu *m_contextMenu;

    // Tooltip support members
    int m_hoveredTrackId;      //!< Currently hovered track ID (-1 if none)
    QPointF m_mousePos;        //!< Current mouse position for tooltip placement
    int m_rightClickedTrackId; //!< Track ID for context menu
    
    // Track state members
    QSet<int> m_highlightedTracks; //!< Set of highlighted track IDs
    int m_focusedTrackId;          //!< Currently focused track ID (-1 if none)
    
    // Drone image members
    QPixmap m_droneIcon;           //!< Default drone icon image
    QMap<int, QPixmap> m_trackImages; //!< Custom images for specific track IDs

    // Cached images for track icons
    QHash<int, QPixmap> m_trackPixmaps; //!< Cache of loaded images by track ID
    QHash<QString, QPixmap> m_defaultIconCache; //!< Cache for generated default icons

    /**
     * @brief Creates the context menu for tracks
     */
    void createContextMenu();

    // Helpers for icon rendering
    QPixmap getDefaultDronePixmap(int size, const QColor &color, const QColor &accent);

    /**
     * @brief Detects if a track is at the given position
     * @param pos Mouse position in screen coordinates
     * @return Track ID if found, -1 otherwise
     */
    int getTrackAtPosition(const QPointF &pos);

    /**
     * @brief Draws tooltip with track information
     * @param pPainter QPainter instance
     * @param trackInfo Track information to display
     * @param screenPos Screen position for tooltip
     */
    void drawTooltip(QPainter *pPainter, const stTrackDisplayInfo &trackInfo, const QPointF &screenPos);
    
    /**
     * @brief Draws speed vector for a track
     * @param pPainter QPainter instance
     * @param trackInfo Track information
     * @param screenPos Screen position of track
     * @param trackColor Color of the track
     */
    void drawSpeedVector(QPainter *pPainter, const stTrackDisplayInfo &trackInfo, 
                        const QPointF &screenPos, const QColor &trackColor);
    
    /**
     * @brief Draws focused track datatip that follows the track
     * @param pPainter QPainter instance
     * @param trackInfo Track information
     * @param screenPos Screen position of track
     */
    void drawFocusedTrackDatatip(QPainter *pPainter, const stTrackDisplayInfo &trackInfo, 
                                const QPointF &screenPos);
    
    /**
     * @brief Draws drone internal details panel
     * @param pPainter QPainter instance
     * @param trackInfo Track information with drone pointer
     * @param screenPos Screen position of track
     */
    void drawDroneInternalDetails(QPainter *pPainter, const stTrackDisplayInfo &trackInfo, 
                                  const QPointF &screenPos);
    
    /**
     * @brief Draws a rotated drone image based on heading
     * @param pPainter QPainter instance
     * @param image Drone image to draw
     * @param screenPos Screen position of track
     * @param heading Heading angle in degrees
     * @param scale Scale factor for the image
     */
    void drawRotatedDroneImage(QPainter *pPainter, const QPixmap &image, 
                               const QPointF &screenPos, double heading, double scale = 1.0);

private slots:
    void onFocusTrack();
    void onDeleteTrack();
    void onLoadTrackImage();
    void onToggleTrackHistory();
    void onHighlightTrack();
};

#endif // CTRACKLAYER_H


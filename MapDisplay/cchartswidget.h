#ifndef CCHARTSWIDGET_H
#define CCHARTSWIDGET_H

#include <QDockWidget>
#include <QTabWidget>
#include <QTimer>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>

// Forward declarations
class CCustomChart;

class CChartsWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CChartsWidget(QWidget *parent = nullptr);
    ~CChartsWidget();

private slots:
    void updateCharts();
    void onChartTypeChanged(int index);
    void exportChart();

private:
    void setupUI();
    void createBScopeTab();
    void createCScopeTab();
    void createRHITab();
    void createTimeSeriesTab();
    void applyRichStyle();

    QTabWidget *m_tabWidget;
    QTimer *m_updateTimer;

    // Chart widgets
    CCustomChart *m_bscopeChart;
    CCustomChart *m_cscopeChart;
    CCustomChart *m_rhiChart;
    CCustomChart *m_rangeTimeChart;
    CCustomChart *m_azimuthTimeChart;
    CCustomChart *m_elevationTimeChart;
    CCustomChart *m_rcsTimeChart;
    CCustomChart *m_speedTimeChart;

    QComboBox *m_comboTimeSeriesType;
};

// Custom Chart Widget for radar displays
class CCustomChart : public QWidget
{
    Q_OBJECT

public:
    enum ChartType {
        BScope,
        CScope,
        RHI,
        RangeTime,
        AzimuthTime,
        ElevationTime,
        RCSTime,
        SpeedTime
    };

    explicit CCustomChart(ChartType type, QWidget *parent = nullptr);
    ~CCustomChart();

    void setChartType(ChartType type);
    void updateData();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void drawBScope(QPainter *painter);
    void drawCScope(QPainter *painter);
    void drawRHI(QPainter *painter);
    void drawTimeSeries(QPainter *painter);
    void drawGrid(QPainter *painter, const QRectF &rect);
    void drawAxes(QPainter *painter, const QRectF &rect, const QString &xLabel, const QString &yLabel);
    void drawTooltip(QPainter *painter);

    QPointF getDataPointPosition(int index, const QRectF &rect);
    int findNearestDataPoint(const QPointF &mousePos);

    ChartType m_chartType;
    QPointF m_mousePos;
    int m_hoveredIndex;
    bool m_showTooltip;
    double m_zoomLevel;
    QPointF m_panOffset;

    // Data storage
    struct TrackData {
        int trackId;
        double range;
        double azimuth;
        double elevation;
        double rcs;
        double speed;
        qint64 timestamp;
        QColor color;
    };

    QList<TrackData> m_currentData;
    QMap<int, QList<TrackData>> m_historicalData; // trackId -> history
};

#endif // CCHARTSWIDGET_H

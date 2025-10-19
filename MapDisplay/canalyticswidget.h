#ifndef CANALYTICSWIDGET_H
#define CANALYTICSWIDGET_H

#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QProgressBar>
#include <QTimer>
#include <QScrollArea>
#include <QFrame>
#include <QTableWidget>
#include <QHeaderView>
#include <QSplitter>
#include "../globalstructs.h"

class CAnalyticsWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CAnalyticsWidget(QWidget *parent = nullptr);
    ~CAnalyticsWidget();

public slots:
    void updateAnalytics();
    void onTrackSelected(int trackId);
    void clearSelectedTrack();

private slots:
    void onRefreshRequested();
    void onExportRequested();
    void onTimeWindowChanged(int index);

private:
    void setupUI();
    void createOverallStatsSection();
    void createSelectedTrackSection();
    void createDetailedStatsSection();
    void applyModernStyle();
    
    void updateOverallStats(const QList<stTrackDisplayInfo> &tracks);
    void updateSelectedTrackStats(const stTrackDisplayInfo &track);
    void updateDetailedStatsTable(const QList<stTrackDisplayInfo> &tracks);
    
    QString formatDuration(qint64 milliseconds);
    QString getIdentityString(int identity);
    QColor getIdentityColor(int identity);

    // UI Components
    QWidget *m_mainWidget;
    QSplitter *m_mainSplitter;
    QScrollArea *m_scrollArea;
    
    // Overall Statistics Section
    QGroupBox *m_overallStatsGroup;
    QLabel *m_totalTracksLabel;
    QLabel *m_activeTracksLabel;
    QLabel *m_friendlyTracksLabel;
    QLabel *m_hostileTracksLabel;
    QLabel *m_unknownTracksLabel;
    QLabel *m_avgRangeLabel;
    QLabel *m_maxRangeLabel;
    QLabel *m_minRangeLabel;
    QLabel *m_avgSpeedLabel;
    QLabel *m_maxSpeedLabel;
    QProgressBar *m_friendlyProgress;
    QProgressBar *m_hostileProgress;
    QProgressBar *m_unknownProgress;
    
    // Selected Track Section
    QGroupBox *m_selectedTrackGroup;
    QLabel *m_selectedTrackIdLabel;
    QLabel *m_selectedTrackIdentityLabel;
    QLabel *m_selectedTrackRangeLabel;
    QLabel *m_selectedTrackAzimuthLabel;
    QLabel *m_selectedTrackElevationLabel;
    QLabel *m_selectedTrackSpeedLabel;
    QLabel *m_selectedTrackHeadingLabel;
    QLabel *m_selectedTrackSNRLabel;
    QLabel *m_selectedTrackPositionLabel;
    QLabel *m_selectedTrackTimeLabel;
    QPushButton *m_clearSelectionButton;
    
    // Detailed Statistics Table
    QGroupBox *m_detailedStatsGroup;
    QTableWidget *m_detailedStatsTable;
    
    // Controls
    QHBoxLayout *m_controlsLayout;
    QComboBox *m_timeWindowCombo;
    QPushButton *m_refreshButton;
    QPushButton *m_exportButton;
    
    // Data
    int m_selectedTrackId;
    QTimer *m_updateTimer;
    
    // Statistics tracking
    struct TrackStatistics {
        int totalTracks;
        int activeTracks;
        int friendlyTracks;
        int hostileTracks;
        int unknownTracks;
        double avgRange;
        double maxRange;
        double minRange;
        double avgSpeed;
        double maxSpeed;
        double totalDistance;
        qint64 sessionDuration;
    };
    
    TrackStatistics m_currentStats;
    QMap<int, qint64> m_trackFirstSeen;
    QMap<int, QList<stTrackDisplayInfo>> m_trackHistory;
};

#endif // CANALYTICSWIDGET_H
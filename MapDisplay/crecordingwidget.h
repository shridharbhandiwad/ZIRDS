#ifndef CRECORDINGWIDGET_H
#define CRECORDINGWIDGET_H

#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QComboBox>
#include <QTimer>
#include <QFile>
#include <QDataStream>
#include "../globalstructs.h"

// Forward declaration
class CReplayWindow;

/**
 * @brief Widget for recording and replaying radar sessions
 * 
 * Features:
 * - Record track data to files
 * - Replay recorded sessions
 * - Manage recording library
 * - Export recordings in various formats
 */
class CRecordingWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CRecordingWidget(QWidget *parent = nullptr);
    ~CRecordingWidget();

public slots:
    void startRecording();
    void stopRecording();
    void pauseRecording();
    void startReplay();
    void stopReplay();
    void pauseReplay();
    
signals:
    void replayTrackData(stTrackRecvInfo track);

private slots:
    void recordTrackData();
    void replayFrame();
    void onRecordingSelected(QListWidgetItem *item);
    void deleteRecording();
    void exportRecording();
    void refreshRecordings();
    void onReplaySpeedChanged(int index);
    void openReplayWindow();
    void onReplayWindowClosed();

private:
    void setupUI();
    void createRecordingSection();
    void createReplaySection();
    void createLibrarySection();
    void applyModernStyle();
    
    void loadRecordingsList();
    void saveRecordingMetadata();
    
    struct RecordedFrame {
        qint64 timestamp;
        QList<stTrackRecvInfo> tracks;
    };
    
    // UI Components
    QWidget *m_mainWidget;
    QVBoxLayout *m_mainLayout;
    
    // Recording Section
    QGroupBox *m_recordingGroup;
    QPushButton *m_recordButton;
    QPushButton *m_stopRecordButton;
    QPushButton *m_pauseRecordButton;
    QLabel *m_recordStatusLabel;
    QLabel *m_recordTimeLabel;
    QLabel *m_recordSizeLabel;
    
    // Replay Section
    QGroupBox *m_replayGroup;
    QPushButton *m_playButton;
    QPushButton *m_stopPlayButton;
    QPushButton *m_pausePlayButton;
    QLabel *m_replayStatusLabel;
    QLabel *m_replayProgressLabel;
    QComboBox *m_replaySpeedCombo;
    
    // Library Section
    QGroupBox *m_libraryGroup;
    QListWidget *m_recordingsList;
    QPushButton *m_deleteButton;
    QPushButton *m_exportButton;
    QPushButton *m_refreshButton;
    
    // Recording State
    bool m_isRecording;
    bool m_isRecordingPaused;
    QFile *m_recordingFile;
    QDataStream *m_recordingStream;
    qint64 m_recordingStartTime;
    qint64 m_recordingFrameCount;
    QString m_currentRecordingPath;
    QTimer *m_recordTimer;
    
    // Replay State
    bool m_isReplaying;
    bool m_isReplayPaused;
    QFile *m_replayFile;
    QDataStream *m_replayStream;
    QList<RecordedFrame> m_replayFrames;
    int m_currentReplayFrame;
    qint64 m_replayStartTime;
    QTimer *m_replayTimer;
    double m_replaySpeed;
    
    // Replay window
    CReplayWindow *m_replayWindow;
    QPushButton *m_replayWindowButton;
    
    static const QString RECORDINGS_DIR;
    static constexpr int RECORD_INTERVAL = 1000; // 1 second
};

#endif // CRECORDINGWIDGET_H

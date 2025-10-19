#ifndef CREPLAYWINDOW_H
#define CREPLAYWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QProgressBar>
#include <QTimer>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QFrame>
#include <QSpacerItem>

/**
 * @brief Standalone replay window with video player functionality
 * 
 * Features:
 * - Video playback with controls
 * - Minimize/Maximize/Close buttons
 * - Progress bar and time display
 * - Play/Pause/Stop controls
 * - Speed control
 * - Modern themed UI
 */
class CReplayWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CReplayWindow(QWidget *parent = nullptr);
    ~CReplayWindow();

    void setVideoFile(const QString &filePath);
    void showReplayWindow();

public slots:
    void playVideo();
    void pauseVideo();
    void stopVideo();
    void setPlaybackSpeed(double speed);
    void seekToPosition(int position);

private slots:
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onStateChanged(QMediaPlayer::State state);
    void onSpeedChanged(int value);
    void minimizeWindow();
    void maximizeWindow();
    void closeWindow();

signals:
    void windowClosed();

private:
    void setupUI();
    void createTitleBar();
    void createVideoArea();
    void createControlsArea();
    void applyModernStyle();
    void updateTimeDisplay();

    // UI Components
    QVBoxLayout *m_mainLayout;
    
    // Title bar
    QFrame *m_titleBar;
    QHBoxLayout *m_titleLayout;
    QLabel *m_titleLabel;
    QPushButton *m_minimizeButton;
    QPushButton *m_maximizeButton;
    QPushButton *m_closeButton;
    
    // Video area
    QVideoWidget *m_videoWidget;
    QMediaPlayer *m_mediaPlayer;
    
    // Controls area
    QFrame *m_controlsFrame;
    QVBoxLayout *m_controlsLayout;
    
    // Playback controls
    QHBoxLayout *m_playbackLayout;
    QPushButton *m_playButton;
    QPushButton *m_pauseButton;
    QPushButton *m_stopButton;
    QLabel *m_timeLabel;
    QSlider *m_positionSlider;
    QLabel *m_durationLabel;
    
    // Speed control
    QHBoxLayout *m_speedLayout;
    QLabel *m_speedLabel;
    QSlider *m_speedSlider;
    QLabel *m_speedValueLabel;
    
    // Progress bar
    QProgressBar *m_progressBar;
    
    // State
    bool m_isMaximized;
    QRect m_normalGeometry;
    qint64 m_duration;
    double m_playbackSpeed;
    
    // Drag functionality for title bar
    bool m_dragging;
    QPoint m_dragPosition;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
};

#endif // CREPLAYWINDOW_H
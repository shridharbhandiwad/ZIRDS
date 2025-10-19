#include "creplaywindow.h"
#include <QMouseEvent>
#include <QCloseEvent>
#include <QApplication>
#include <QScreen>
#include <QFileInfo>
#include <QTime>
#include <QDebug>

CReplayWindow::CReplayWindow(QWidget *parent)
    : QWidget(parent)
    , m_isMaximized(false)
    , m_duration(0)
    , m_playbackSpeed(1.0)
    , m_dragging(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    setMinimumSize(640, 480);
    resize(800, 600);
    
    setupUI();
    applyModernStyle();
    
    // Initialize media player
    m_mediaPlayer = new QMediaPlayer(this);
    m_mediaPlayer->setVideoOutput(m_videoWidget);
    
    // Connect media player signals
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged,
            this, &CReplayWindow::onPositionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged,
            this, &CReplayWindow::onDurationChanged);
    connect(m_mediaPlayer, QOverload<QMediaPlayer::State>::of(&QMediaPlayer::stateChanged),
            this, &CReplayWindow::onStateChanged);
}

CReplayWindow::~CReplayWindow()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
    }
}

void CReplayWindow::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    createTitleBar();
    createVideoArea();
    createControlsArea();
}

void CReplayWindow::createTitleBar()
{
    m_titleBar = new QFrame();
    m_titleBar->setFixedHeight(40);
    m_titleBar->setObjectName("titleBar");
    
    m_titleLayout = new QHBoxLayout(m_titleBar);
    m_titleLayout->setContentsMargins(12, 0, 8, 0);
    m_titleLayout->setSpacing(8);
    
    m_titleLabel = new QLabel("🎬 Replay Window");
    m_titleLabel->setObjectName("titleLabel");
    m_titleLayout->addWidget(m_titleLabel);
    
    m_titleLayout->addStretch();
    
    // Window control buttons
    m_minimizeButton = new QPushButton("−");
    m_minimizeButton->setFixedSize(30, 30);
    m_minimizeButton->setObjectName("minimizeButton");
    connect(m_minimizeButton, &QPushButton::clicked, this, &CReplayWindow::minimizeWindow);
    m_titleLayout->addWidget(m_minimizeButton);
    
    m_maximizeButton = new QPushButton("□");
    m_maximizeButton->setFixedSize(30, 30);
    m_maximizeButton->setObjectName("maximizeButton");
    connect(m_maximizeButton, &QPushButton::clicked, this, &CReplayWindow::maximizeWindow);
    m_titleLayout->addWidget(m_maximizeButton);
    
    m_closeButton = new QPushButton("✕");
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setObjectName("closeButton");
    connect(m_closeButton, &QPushButton::clicked, this, &CReplayWindow::closeWindow);
    m_titleLayout->addWidget(m_closeButton);
    
    m_mainLayout->addWidget(m_titleBar);
}

void CReplayWindow::createVideoArea()
{
    m_videoWidget = new QVideoWidget();
    m_videoWidget->setMinimumHeight(300);
    m_videoWidget->setStyleSheet("background-color: #000000; border: 2px solid #4a5568;");
    m_mainLayout->addWidget(m_videoWidget, 1);
}

void CReplayWindow::createControlsArea()
{
    m_controlsFrame = new QFrame();
    m_controlsFrame->setFixedHeight(120);
    m_controlsFrame->setObjectName("controlsFrame");
    
    m_controlsLayout = new QVBoxLayout(m_controlsFrame);
    m_controlsLayout->setContentsMargins(12, 8, 12, 8);
    m_controlsLayout->setSpacing(8);
    
    // Progress bar
    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(false);
    m_progressBar->setFixedHeight(6);
    m_progressBar->setObjectName("progressBar");
    m_controlsLayout->addWidget(m_progressBar);
    
    // Playback controls
    m_playbackLayout = new QHBoxLayout();
    m_playbackLayout->setSpacing(12);
    
    m_playButton = new QPushButton("▶");
    m_playButton->setFixedSize(50, 40);
    m_playButton->setObjectName("playButton");
    connect(m_playButton, &QPushButton::clicked, this, &CReplayWindow::playVideo);
    m_playbackLayout->addWidget(m_playButton);
    
    m_pauseButton = new QPushButton("⏸");
    m_pauseButton->setFixedSize(50, 40);
    m_pauseButton->setObjectName("pauseButton");
    m_pauseButton->setEnabled(false);
    connect(m_pauseButton, &QPushButton::clicked, this, &CReplayWindow::pauseVideo);
    m_playbackLayout->addWidget(m_pauseButton);
    
    m_stopButton = new QPushButton("⏹");
    m_stopButton->setFixedSize(50, 40);
    m_stopButton->setObjectName("stopButton");
    m_stopButton->setEnabled(false);
    connect(m_stopButton, &QPushButton::clicked, this, &CReplayWindow::stopVideo);
    m_playbackLayout->addWidget(m_stopButton);
    
    m_playbackLayout->addSpacing(20);
    
    m_timeLabel = new QLabel("00:00");
    m_timeLabel->setObjectName("timeLabel");
    m_timeLabel->setMinimumWidth(50);
    m_playbackLayout->addWidget(m_timeLabel);
    
    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setRange(0, 0);
    m_positionSlider->setObjectName("positionSlider");
    connect(m_positionSlider, &QSlider::sliderPressed, this, [this]() {
        m_mediaPlayer->pause();
    });
    connect(m_positionSlider, &QSlider::sliderReleased, this, [this]() {
        seekToPosition(m_positionSlider->value());
        if (m_mediaPlayer->state() == QMediaPlayer::PausedState) {
            m_mediaPlayer->play();
        }
    });
    m_playbackLayout->addWidget(m_positionSlider, 1);
    
    m_durationLabel = new QLabel("00:00");
    m_durationLabel->setObjectName("durationLabel");
    m_durationLabel->setMinimumWidth(50);
    m_playbackLayout->addWidget(m_durationLabel);
    
    m_controlsLayout->addLayout(m_playbackLayout);
    
    // Speed control
    m_speedLayout = new QHBoxLayout();
    m_speedLayout->setSpacing(8);
    
    m_speedLabel = new QLabel("Speed:");
    m_speedLabel->setObjectName("speedLabel");
    m_speedLayout->addWidget(m_speedLabel);
    
    m_speedSlider = new QSlider(Qt::Horizontal);
    m_speedSlider->setRange(25, 400); // 0.25x to 4.0x
    m_speedSlider->setValue(100); // 1.0x
    m_speedSlider->setFixedWidth(150);
    m_speedSlider->setObjectName("speedSlider");
    connect(m_speedSlider, &QSlider::valueChanged, this, &CReplayWindow::onSpeedChanged);
    m_speedLayout->addWidget(m_speedSlider);
    
    m_speedValueLabel = new QLabel("1.0x");
    m_speedValueLabel->setObjectName("speedValueLabel");
    m_speedValueLabel->setMinimumWidth(40);
    m_speedLayout->addWidget(m_speedValueLabel);
    
    m_speedLayout->addStretch();
    m_controlsLayout->addLayout(m_speedLayout);
    
    m_mainLayout->addWidget(m_controlsFrame);
}

void CReplayWindow::applyModernStyle()
{
    setStyleSheet(
        "CReplayWindow {"
        "   background-color: #1e293b;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 12px;"
        "}"
        
        "#titleBar {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2);"
        "   border-top-left-radius: 10px;"
        "   border-top-right-radius: 10px;"
        "   border-bottom: 2px solid #4a5568;"
        "}"
        
        "#titleLabel {"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "}"
        
        "#minimizeButton, #maximizeButton {"
        "   background-color: rgba(255, 255, 255, 0.1);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 15px;"
        "   font-weight: bold;"
        "   font-size: 16px;"
        "}"
        
        "#minimizeButton:hover, #maximizeButton:hover {"
        "   background-color: rgba(255, 255, 255, 0.2);"
        "}"
        
        "#closeButton {"
        "   background-color: rgba(239, 68, 68, 0.8);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 15px;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "}"
        
        "#closeButton:hover {"
        "   background-color: rgba(239, 68, 68, 1.0);"
        "}"
        
        "#controlsFrame {"
        "   background-color: #2d3748;"
        "   border-bottom-left-radius: 10px;"
        "   border-bottom-right-radius: 10px;"
        "   border-top: 1px solid #4a5568;"
        "}"
        
        "#progressBar {"
        "   background-color: #1a202c;"
        "   border: 1px solid #4a5568;"
        "   border-radius: 3px;"
        "}"
        
        "#progressBar::chunk {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2);"
        "   border-radius: 2px;"
        "}"
        
        "#playButton, #pauseButton, #stopButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #667eea, stop:1 #764ba2);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   font-weight: bold;"
        "   font-size: 16px;"
        "}"
        
        "#playButton:hover, #pauseButton:hover, #stopButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #5568d3, stop:1 #6b3fa0);"
        "}"
        
        "#playButton:disabled, #pauseButton:disabled, #stopButton:disabled {"
        "   background-color: #4a5568;"
        "   color: #a0aec0;"
        "}"
        
        "#timeLabel, #durationLabel, #speedLabel, #speedValueLabel {"
        "   color: #e2e8f0;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "}"
        
        "#positionSlider, #speedSlider {"
        "   background: transparent;"
        "}"
        
        "#positionSlider::groove:horizontal, #speedSlider::groove:horizontal {"
        "   background: #1a202c;"
        "   height: 6px;"
        "   border-radius: 3px;"
        "   border: 1px solid #4a5568;"
        "}"
        
        "#positionSlider::handle:horizontal, #speedSlider::handle:horizontal {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #667eea, stop:1 #764ba2);"
        "   width: 16px;"
        "   height: 16px;"
        "   margin: -5px 0;"
        "   border-radius: 8px;"
        "   border: 2px solid white;"
        "}"
        
        "#positionSlider::sub-page:horizontal, #speedSlider::sub-page:horizontal {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2);"
        "   border-radius: 3px;"
        "}"
    );
}

void CReplayWindow::setVideoFile(const QString &filePath)
{
    if (QFileInfo::exists(filePath)) {
        m_mediaPlayer->setMedia(QUrl::fromLocalFile(filePath));
        m_titleLabel->setText("🎬 " + QFileInfo(filePath).baseName());
    }
}

void CReplayWindow::showReplayWindow()
{
    show();
    raise();
    activateWindow();
}

void CReplayWindow::playVideo()
{
    m_mediaPlayer->play();
}

void CReplayWindow::pauseVideo()
{
    m_mediaPlayer->pause();
}

void CReplayWindow::stopVideo()
{
    m_mediaPlayer->stop();
    m_positionSlider->setValue(0);
    updateTimeDisplay();
}

void CReplayWindow::setPlaybackSpeed(double speed)
{
    m_playbackSpeed = speed;
    m_mediaPlayer->setPlaybackRate(speed);
    m_speedValueLabel->setText(QString::number(speed, 'f', 1) + "x");
}

void CReplayWindow::seekToPosition(int position)
{
    if (m_duration > 0) {
        qint64 seekPosition = (position * m_duration) / 100;
        m_mediaPlayer->setPosition(seekPosition);
    }
}

void CReplayWindow::onPositionChanged(qint64 position)
{
    if (m_duration > 0) {
        int progress = (position * 100) / m_duration;
        m_positionSlider->setValue(progress);
        m_progressBar->setValue(progress);
    }
    updateTimeDisplay();
}

void CReplayWindow::onDurationChanged(qint64 duration)
{
    m_duration = duration;
    m_positionSlider->setRange(0, 100);
    updateTimeDisplay();
}

void CReplayWindow::onStateChanged(QMediaPlayer::State state)
{
    switch (state) {
    case QMediaPlayer::PlayingState:
        m_playButton->setEnabled(false);
        m_pauseButton->setEnabled(true);
        m_stopButton->setEnabled(true);
        break;
    case QMediaPlayer::PausedState:
        m_playButton->setEnabled(true);
        m_pauseButton->setEnabled(false);
        m_stopButton->setEnabled(true);
        break;
    case QMediaPlayer::StoppedState:
        m_playButton->setEnabled(true);
        m_pauseButton->setEnabled(false);
        m_stopButton->setEnabled(false);
        break;
    }
}

void CReplayWindow::onSpeedChanged(int value)
{
    double speed = value / 100.0;
    setPlaybackSpeed(speed);
}

void CReplayWindow::updateTimeDisplay()
{
    qint64 position = m_mediaPlayer->position();
    
    QTime currentTime = QTime(0, 0).addMSecs(position);
    m_timeLabel->setText(currentTime.toString("mm:ss"));
    
    if (m_duration > 0) {
        QTime totalTime = QTime(0, 0).addMSecs(m_duration);
        m_durationLabel->setText(totalTime.toString("mm:ss"));
    }
}

void CReplayWindow::minimizeWindow()
{
    showMinimized();
}

void CReplayWindow::maximizeWindow()
{
    if (m_isMaximized) {
        setGeometry(m_normalGeometry);
        m_maximizeButton->setText("□");
        m_isMaximized = false;
    } else {
        m_normalGeometry = geometry();
        QScreen *screen = QApplication::primaryScreen();
        setGeometry(screen->availableGeometry());
        m_maximizeButton->setText("❐");
        m_isMaximized = true;
    }
}

void CReplayWindow::closeWindow()
{
    close();
}

void CReplayWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_titleBar->geometry().contains(event->pos())) {
        m_dragging = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void CReplayWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_dragging) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void CReplayWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
}

void CReplayWindow::closeEvent(QCloseEvent *event)
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
    }
    emit windowClosed();
    event->accept();
}
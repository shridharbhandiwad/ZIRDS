#include "crecordingwidget.h"
#include "creplaywindow.h"
#include "../cdatawarehouse.h"
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

const QString CRecordingWidget::RECORDINGS_DIR = QDir::homePath() + "/RadarRecordings";

CRecordingWidget::CRecordingWidget(QWidget *parent)
    : QDockWidget("📹 Recording & Replay", parent)
    , m_isRecording(false)
    , m_isRecordingPaused(false)
    , m_recordingFile(nullptr)
    , m_recordingStream(nullptr)
    , m_recordingStartTime(0)
    , m_recordingFrameCount(0)
    , m_isReplaying(false)
    , m_isReplayPaused(false)
    , m_replayFile(nullptr)
    , m_replayStream(nullptr)
    , m_currentReplayFrame(0)
    , m_replayStartTime(0)
    , m_replaySpeed(1.0)
    , m_replayWindow(nullptr)
{
    setupUI();
    applyModernStyle();
    
    // Create recordings directory
    QDir().mkpath(RECORDINGS_DIR);
    
    // Initialize timers
    m_recordTimer = new QTimer(this);
    connect(m_recordTimer, &QTimer::timeout, this, &CRecordingWidget::recordTrackData);
    
    m_replayTimer = new QTimer(this);
    connect(m_replayTimer, &QTimer::timeout, this, &CRecordingWidget::replayFrame);
    
    // Load existing recordings
    loadRecordingsList();
}

CRecordingWidget::~CRecordingWidget()
{
    if (m_isRecording) {
        stopRecording();
    }
    if (m_isReplaying) {
        stopReplay();
    }
}

void CRecordingWidget::setupUI()
{
    m_mainWidget = new QWidget(this);
    m_mainLayout = new QVBoxLayout(m_mainWidget);
    m_mainLayout->setSpacing(12);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);
    
    createRecordingSection();
    createReplaySection();
    createLibrarySection();
    
    m_mainLayout->addStretch();
    setWidget(m_mainWidget);
}

void CRecordingWidget::createRecordingSection()
{
    m_recordingGroup = new QGroupBox("🔴 Recording", m_mainWidget);
    QVBoxLayout *layout = new QVBoxLayout(m_recordingGroup);
    
    // Status labels
    m_recordStatusLabel = new QLabel("Status: <span style='color: #94a3b8;'>Idle</span>");
    m_recordStatusLabel->setAlignment(Qt::AlignCenter);
    QFont font = m_recordStatusLabel->font();
    font.setPointSize(11);
    font.setBold(true);
    m_recordStatusLabel->setFont(font);
    layout->addWidget(m_recordStatusLabel);
    
    m_recordTimeLabel = new QLabel("Duration: 00:00:00");
    m_recordTimeLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_recordTimeLabel);
    
    m_recordSizeLabel = new QLabel("Frames: 0");
    m_recordSizeLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_recordSizeLabel);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_recordButton = new QPushButton("● Record");
    m_pauseRecordButton = new QPushButton("⏸ Pause");
    m_stopRecordButton = new QPushButton("⏹ Stop");
    
    m_recordButton->setMinimumHeight(35);
    m_pauseRecordButton->setMinimumHeight(35);
    m_stopRecordButton->setMinimumHeight(35);
    
    m_pauseRecordButton->setEnabled(false);
    m_stopRecordButton->setEnabled(false);
    
    buttonLayout->addWidget(m_recordButton);
    buttonLayout->addWidget(m_pauseRecordButton);
    buttonLayout->addWidget(m_stopRecordButton);
    
    layout->addLayout(buttonLayout);
    
    connect(m_recordButton, &QPushButton::clicked, this, &CRecordingWidget::startRecording);
    connect(m_pauseRecordButton, &QPushButton::clicked, this, &CRecordingWidget::pauseRecording);
    connect(m_stopRecordButton, &QPushButton::clicked, this, &CRecordingWidget::stopRecording);
    
    m_mainLayout->addWidget(m_recordingGroup);
}

void CRecordingWidget::createReplaySection()
{
    m_replayGroup = new QGroupBox("▶️ Replay", m_mainWidget);
    QVBoxLayout *layout = new QVBoxLayout(m_replayGroup);
    
    // Status labels
    m_replayStatusLabel = new QLabel("Status: <span style='color: #94a3b8;'>Idle</span>");
    m_replayStatusLabel->setAlignment(Qt::AlignCenter);
    QFont font = m_replayStatusLabel->font();
    font.setPointSize(11);
    font.setBold(true);
    m_replayStatusLabel->setFont(font);
    layout->addWidget(m_replayStatusLabel);
    
    m_replayProgressLabel = new QLabel("Progress: 0 / 0");
    m_replayProgressLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_replayProgressLabel);
    
    // Replay speed
    QHBoxLayout *speedLayout = new QHBoxLayout();
    speedLayout->addWidget(new QLabel("Speed:"));
    m_replaySpeedCombo = new QComboBox();
    m_replaySpeedCombo->addItem("0.25x", 0.25);
    m_replaySpeedCombo->addItem("0.5x", 0.5);
    m_replaySpeedCombo->addItem("1x", 1.0);
    m_replaySpeedCombo->addItem("2x", 2.0);
    m_replaySpeedCombo->addItem("4x", 4.0);
    m_replaySpeedCombo->setCurrentIndex(2); // 1x default
    speedLayout->addWidget(m_replaySpeedCombo);
    layout->addLayout(speedLayout);
    
    connect(m_replaySpeedCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CRecordingWidget::onReplaySpeedChanged);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_playButton = new QPushButton("▶ Play");
    m_pausePlayButton = new QPushButton("⏸ Pause");
    m_stopPlayButton = new QPushButton("⏹ Stop");
    
    m_playButton->setMinimumHeight(35);
    m_pausePlayButton->setMinimumHeight(35);
    m_stopPlayButton->setMinimumHeight(35);
    
    m_playButton->setEnabled(false);
    m_pausePlayButton->setEnabled(false);
    m_stopPlayButton->setEnabled(false);
    
    buttonLayout->addWidget(m_playButton);
    buttonLayout->addWidget(m_pausePlayButton);
    buttonLayout->addWidget(m_stopPlayButton);
    
    layout->addLayout(buttonLayout);
    
    // Replay window button
    m_replayWindowButton = new QPushButton("🎬 Open Replay Window");
    m_replayWindowButton->setMinimumHeight(35);
    m_replayWindowButton->setEnabled(false);
    connect(m_replayWindowButton, &QPushButton::clicked, this, &CRecordingWidget::openReplayWindow);
    layout->addWidget(m_replayWindowButton);
    
    connect(m_playButton, &QPushButton::clicked, this, &CRecordingWidget::startReplay);
    connect(m_pausePlayButton, &QPushButton::clicked, this, &CRecordingWidget::pauseReplay);
    connect(m_stopPlayButton, &QPushButton::clicked, this, &CRecordingWidget::stopReplay);
    
    m_mainLayout->addWidget(m_replayGroup);
}

void CRecordingWidget::createLibrarySection()
{
    m_libraryGroup = new QGroupBox("📚 Recording Library", m_mainWidget);
    QVBoxLayout *layout = new QVBoxLayout(m_libraryGroup);
    
    m_recordingsList = new QListWidget();
    m_recordingsList->setMaximumHeight(200);
    layout->addWidget(m_recordingsList);
    
    connect(m_recordingsList, &QListWidget::itemClicked,
            this, &CRecordingWidget::onRecordingSelected);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_deleteButton = new QPushButton("🗑 Delete");
    m_exportButton = new QPushButton("💾 Export");
    m_refreshButton = new QPushButton("🔄 Refresh");
    
    m_deleteButton->setEnabled(false);
    m_exportButton->setEnabled(false);
    
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addWidget(m_exportButton);
    buttonLayout->addWidget(m_refreshButton);
    
    layout->addLayout(buttonLayout);
    
    connect(m_deleteButton, &QPushButton::clicked, this, &CRecordingWidget::deleteRecording);
    connect(m_exportButton, &QPushButton::clicked, this, &CRecordingWidget::exportRecording);
    connect(m_refreshButton, &QPushButton::clicked, this, &CRecordingWidget::refreshRecordings);
    
    m_mainLayout->addWidget(m_libraryGroup);
}

void CRecordingWidget::applyModernStyle()
{
    setStyleSheet(
        "QDockWidget {"
        "   background-color: #1e293b;"
        "   color: #ffffff;"
        "   border: 1px solid #475569;"
        "}"
        "QDockWidget::title {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2);"
        "   color: white;"
        "   padding: 10px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "   border-radius: 4px;"
        "}"
        "QGroupBox {"
        "   background-color: #2d3748;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 10px;"
        "   margin-top: 16px;"
        "   padding-top: 18px;"
        "   font-weight: bold;"
        "   color: #ffffff;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top left;"
        "   padding: 6px 12px;"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2);"
        "   border-radius: 6px;"
        "   color: white;"
        "   font-size: 12px;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #667eea, stop:1 #764ba2);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 8px 12px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #5568d3, stop:1 #6b3fa0);"
        "}"
        "QPushButton:disabled {"
        "   background-color: #4a5568;"
        "   color: #a0aec0;"
        "}"
        "QLabel {"
        "   color: #e2e8f0;"
        "   font-size: 11px;"
        "}"
        "QListWidget {"
        "   background-color: #1a202c;"
        "   color: #ffffff;"
        "   border: 1px solid #4a5568;"
        "   border-radius: 6px;"
        "}"
        "QListWidget::item {"
        "   padding: 8px;"
        "   border-bottom: 1px solid #4a5568;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #667eea;"
        "   color: white;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #374151;"
        "}"
        "QComboBox {"
        "   background-color: #1a202c;"
        "   color: #ffffff;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 6px;"
        "   padding: 4px;"
        "}"
    );
}

void CRecordingWidget::startRecording()
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    m_currentRecordingPath = RECORDINGS_DIR + "/recording_" + timestamp + ".rdr";
    
    m_recordingFile = new QFile(m_currentRecordingPath);
    if (!m_recordingFile->open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Recording Error",
                            "Failed to create recording file:\n" + m_currentRecordingPath);
        delete m_recordingFile;
        m_recordingFile = nullptr;
        return;
    }
    
    m_recordingStream = new QDataStream(m_recordingFile);
    m_recordingStream->setVersion(QDataStream::Qt_5_15);
    
    // Write header
    *m_recordingStream << QString("RADAR_RECORDING_V1");
    *m_recordingStream << QDateTime::currentDateTime();
    
    m_isRecording = true;
    m_isRecordingPaused = false;
    m_recordingStartTime = QDateTime::currentMSecsSinceEpoch();
    m_recordingFrameCount = 0;
    
    m_recordTimer->start(RECORD_INTERVAL);
    
    m_recordStatusLabel->setText("Status: <span style='color: #667eea;'>● Recording</span>");
    m_recordButton->setEnabled(false);
    m_pauseRecordButton->setEnabled(true);
    m_stopRecordButton->setEnabled(true);
    
    qDebug() << "[Recording] Started:" << m_currentRecordingPath;
}

void CRecordingWidget::stopRecording()
{
    if (!m_isRecording) return;
    
    m_recordTimer->stop();
    m_isRecording = false;
    m_isRecordingPaused = false;
    
    if (m_recordingStream) {
        delete m_recordingStream;
        m_recordingStream = nullptr;
    }
    
    if (m_recordingFile) {
        m_recordingFile->close();
        delete m_recordingFile;
        m_recordingFile = nullptr;
    }
    
    m_recordStatusLabel->setText("Status: <span style='color: #4ade80;'>Saved</span>");
    m_recordButton->setEnabled(true);
    m_pauseRecordButton->setEnabled(false);
    m_stopRecordButton->setEnabled(false);
    
    // Refresh recordings list
    loadRecordingsList();
    
    qDebug() << "[Recording] Stopped. Frames:" << m_recordingFrameCount;
    
    QMessageBox::information(this, "Recording Saved",
                            QString("Recording saved with %1 frames").arg(m_recordingFrameCount));
}

void CRecordingWidget::pauseRecording()
{
    if (!m_isRecording) return;
    
    if (m_isRecordingPaused) {
        m_recordTimer->start(RECORD_INTERVAL);
        m_isRecordingPaused = false;
        m_recordStatusLabel->setText("Status: <span style='color: #667eea;'>● Recording</span>");
        m_pauseRecordButton->setText("⏸ Pause");
    } else {
        m_recordTimer->stop();
        m_isRecordingPaused = true;
        m_recordStatusLabel->setText("Status: <span style='color: #fbbf24;'>⏸ Paused</span>");
        m_pauseRecordButton->setText("▶ Resume");
    }
}

void CRecordingWidget::recordTrackData()
{
    if (!m_isRecording || m_isRecordingPaused || !m_recordingStream) return;
    
    // Get current tracks
    QList<stTrackDisplayInfo> displayTracks = CDataWarehouse::getInstance()->getTrackList();
    
    // Convert to RecvInfo format and write
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    *m_recordingStream << timestamp;
    *m_recordingStream << displayTracks.count();
    
    // For simplicity, we'll store basic track data
    // In a real implementation, you'd store stTrackRecvInfo
    for (const stTrackDisplayInfo &track : displayTracks) {
        *m_recordingStream << track.nTrkId;
        *m_recordingStream << track.x;
        *m_recordingStream << track.y;
        *m_recordingStream << track.z;
        *m_recordingStream << track.heading;
        *m_recordingStream << track.velocity;
        *m_recordingStream << track.nTrackIden;
    }
    
    m_recordingFrameCount++;
    
    // Update UI
    qint64 elapsed = timestamp - m_recordingStartTime;
    int hours = elapsed / 3600000;
    int minutes = (elapsed % 3600000) / 60000;
    int seconds = (elapsed % 60000) / 1000;
    
    m_recordTimeLabel->setText(QString("Duration: %1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0')));
    
    m_recordSizeLabel->setText(QString("Frames: %1").arg(m_recordingFrameCount));
}

void CRecordingWidget::startReplay()
{
    if (m_isReplayPaused) {
        m_isReplayPaused = false;
        m_replayTimer->start(RECORD_INTERVAL / m_replaySpeed);
        m_replayStatusLabel->setText("Status: <span style='color: #4ade80;'>▶ Playing</span>");
        m_pausePlayButton->setText("⏸ Pause");
        return;
    }
    
    QListWidgetItem *selectedItem = m_recordingsList->currentItem();
    if (!selectedItem) {
        QMessageBox::information(this, "No Selection", "Please select a recording to replay");
        return;
    }
    
    QString filename = selectedItem->data(Qt::UserRole).toString();
    m_replayFile = new QFile(filename);
    
    if (!m_replayFile->open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Replay Error", "Failed to open recording file");
        delete m_replayFile;
        m_replayFile = nullptr;
        return;
    }
    
    m_replayStream = new QDataStream(m_replayFile);
    m_replayStream->setVersion(QDataStream::Qt_5_15);
    
    // Read header
    QString header;
    QDateTime recordTime;
    *m_replayStream >> header >> recordTime;
    
    if (header != "RADAR_RECORDING_V1") {
        QMessageBox::warning(this, "Invalid File", "Invalid recording file format");
        m_replayFile->close();
        delete m_replayFile;
        m_replayFile = nullptr;
        return;
    }
    
    // Load all frames
    m_replayFrames.clear();
    while (!m_replayStream->atEnd()) {
        RecordedFrame frame;
        *m_replayStream >> frame.timestamp;
        
        int trackCount;
        *m_replayStream >> trackCount;
        
        for (int i = 0; i < trackCount; ++i) {
            stTrackRecvInfo track;
            int trackId, identity;
            float x, y, z, heading, velocity;
            
            *m_replayStream >> trackId >> x >> y >> z >> heading >> velocity >> identity;
            
            track.nTrkId = trackId;
            track.x = x;
            track.y = y;
            track.z = z;
            track.heading = heading;
            track.velocity = velocity;
            track.nTrackIden = identity;
            track.usMsgId = 100;
            
            frame.tracks.append(track);
        }
        
        m_replayFrames.append(frame);
    }
    
    m_currentReplayFrame = 0;
    m_isReplaying = true;
    m_isReplayPaused = false;
    m_replayStartTime = QDateTime::currentMSecsSinceEpoch();
    
    m_replayTimer->start(RECORD_INTERVAL / m_replaySpeed);
    
    m_replayStatusLabel->setText("Status: <span style='color: #4ade80;'>▶ Playing</span>");
    m_playButton->setEnabled(false);
    m_pausePlayButton->setEnabled(true);
    m_stopPlayButton->setEnabled(true);
    
    qDebug() << "[Replay] Started with" << m_replayFrames.count() << "frames";
}

void CRecordingWidget::stopReplay()
{
    if (!m_isReplaying) return;
    
    m_replayTimer->stop();
    m_isReplaying = false;
    m_isReplayPaused = false;
    
    if (m_replayStream) {
        delete m_replayStream;
        m_replayStream = nullptr;
    }
    
    if (m_replayFile) {
        m_replayFile->close();
        delete m_replayFile;
        m_replayFile = nullptr;
    }
    
    m_replayFrames.clear();
    m_currentReplayFrame = 0;
    
    m_replayStatusLabel->setText("Status: <span style='color: #94a3b8;'>Idle</span>");
    m_replayProgressLabel->setText("Progress: 0 / 0");
    m_playButton->setEnabled(true);
    m_pausePlayButton->setEnabled(false);
    m_stopPlayButton->setEnabled(false);
    m_pausePlayButton->setText("⏸ Pause");
    
    qDebug() << "[Replay] Stopped";
}

void CRecordingWidget::pauseReplay()
{
    if (!m_isReplaying) return;
    
    if (m_isReplayPaused) {
        m_replayTimer->start(RECORD_INTERVAL / m_replaySpeed);
        m_isReplayPaused = false;
        m_replayStatusLabel->setText("Status: <span style='color: #4ade80;'>▶ Playing</span>");
        m_pausePlayButton->setText("⏸ Pause");
    } else {
        m_replayTimer->stop();
        m_isReplayPaused = true;
        m_replayStatusLabel->setText("Status: <span style='color: #fbbf24;'>⏸ Paused</span>");
        m_pausePlayButton->setText("▶ Resume");
    }
}

void CRecordingWidget::replayFrame()
{
    if (!m_isReplaying || m_isReplayPaused || m_currentReplayFrame >= m_replayFrames.count()) {
        if (m_currentReplayFrame >= m_replayFrames.count()) {
            stopReplay();
            QMessageBox::information(this, "Replay Complete", "Replay finished");
        }
        return;
    }
    
    const RecordedFrame &frame = m_replayFrames[m_currentReplayFrame];
    
    // Emit tracks for replay
    for (const stTrackRecvInfo &track : frame.tracks) {
        emit replayTrackData(track);
    }
    
    m_currentReplayFrame++;
    
    // Update progress
    m_replayProgressLabel->setText(QString("Progress: %1 / %2")
        .arg(m_currentReplayFrame)
        .arg(m_replayFrames.count()));
}

void CRecordingWidget::onRecordingSelected(QListWidgetItem *item)
{
    m_deleteButton->setEnabled(true);
    m_exportButton->setEnabled(true);
    m_playButton->setEnabled(true);
    m_replayWindowButton->setEnabled(true);
}

void CRecordingWidget::deleteRecording()
{
    QListWidgetItem *selectedItem = m_recordingsList->currentItem();
    if (!selectedItem) return;
    
    QString filename = selectedItem->data(Qt::UserRole).toString();
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete Recording",
        "Are you sure you want to delete this recording?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        QFile::remove(filename);
        loadRecordingsList();
        m_deleteButton->setEnabled(false);
        m_exportButton->setEnabled(false);
        m_playButton->setEnabled(false);
        m_replayWindowButton->setEnabled(false);
    }
}

void CRecordingWidget::exportRecording()
{
    QListWidgetItem *selectedItem = m_recordingsList->currentItem();
    if (!selectedItem) return;
    
    QString sourceFile = selectedItem->data(Qt::UserRole).toString();
    QString destFile = QFileDialog::getSaveFileName(this, "Export Recording",
        QDir::homePath(), "Radar Recording (*.rdr)");
    
    if (!destFile.isEmpty()) {
        QFile::copy(sourceFile, destFile);
        QMessageBox::information(this, "Export Complete", "Recording exported successfully");
    }
}

void CRecordingWidget::refreshRecordings()
{
    loadRecordingsList();
}

void CRecordingWidget::loadRecordingsList()
{
    m_recordingsList->clear();
    
    QDir recordingsDir(RECORDINGS_DIR);
    QStringList recordings = recordingsDir.entryList(QStringList() << "*.rdr", QDir::Files, QDir::Time);
    
    for (const QString &recording : recordings) {
        QString fullPath = recordingsDir.filePath(recording);
        QFileInfo fileInfo(fullPath);
        
        QString displayText = QString("%1 (%2 KB) - %3")
            .arg(fileInfo.fileName())
            .arg(fileInfo.size() / 1024)
            .arg(fileInfo.lastModified().toString("yyyy-MM-dd HH:mm"));
        
        QListWidgetItem *item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, fullPath);
        m_recordingsList->addItem(item);
    }
}

void CRecordingWidget::onReplaySpeedChanged(int index)
{
    m_replaySpeed = m_replaySpeedCombo->itemData(index).toDouble();
    if (m_isReplaying && !m_isReplayPaused) {
        m_replayTimer->setInterval(RECORD_INTERVAL / m_replaySpeed);
    }
}

void CRecordingWidget::openReplayWindow()
{
    QListWidgetItem *selectedItem = m_recordingsList->currentItem();
    if (!selectedItem) {
        QMessageBox::information(this, "No Selection", "Please select a recording to replay");
        return;
    }
    
    if (!m_replayWindow) {
        m_replayWindow = new CReplayWindow(this);
        connect(m_replayWindow, &CReplayWindow::windowClosed,
                this, &CRecordingWidget::onReplayWindowClosed);
    }
    
    QString filename = selectedItem->data(Qt::UserRole).toString();
    
    // For now, we'll show the replay window without a video file
    // In a real implementation, you would convert the recording to a video format
    // or implement a custom visualization player
    m_replayWindow->showReplayWindow();
    
    QMessageBox::information(this, "Replay Window", 
        "Replay window opened. Note: Video playback requires conversion of radar data to video format.");
}

void CRecordingWidget::onReplayWindowClosed()
{
    // Clean up when replay window is closed
    if (m_replayWindow) {
        m_replayWindow->deleteLater();
        m_replayWindow = nullptr;
    }
}

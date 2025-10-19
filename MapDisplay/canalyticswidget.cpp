#include "canalyticswidget.h"
#include "../cdatawarehouse.h"
#include <QDateTime>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QtMath>

CAnalyticsWidget::CAnalyticsWidget(QWidget *parent)
    : QDockWidget("📊 Track Analytics", parent),
      m_selectedTrackId(-1)
{
    setupUI();
    applyModernStyle();
    
    // Initialize statistics
    memset(&m_currentStats, 0, sizeof(m_currentStats));
    
    // Setup update timer
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &CAnalyticsWidget::updateAnalytics);
    m_updateTimer->start(1000); // Update every second
}

CAnalyticsWidget::~CAnalyticsWidget()
{
}

void CAnalyticsWidget::setupUI()
{
    m_mainWidget = new QWidget(this);
    setWidget(m_mainWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainWidget);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);
    
    // Controls section
    m_controlsLayout = new QHBoxLayout();
    
    QLabel *timeWindowLabel = new QLabel("Time Window:");
    timeWindowLabel->setStyleSheet("color: #ffffff; font-weight: bold;");
    m_controlsLayout->addWidget(timeWindowLabel);
    
    m_timeWindowCombo = new QComboBox();
    m_timeWindowCombo->addItems({"Last 1 min", "Last 5 min", "Last 15 min", "Last 30 min", "Last 1 hour", "All Time"});
    m_timeWindowCombo->setCurrentIndex(2); // Default to 15 minutes
    connect(m_timeWindowCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CAnalyticsWidget::onTimeWindowChanged);
    m_controlsLayout->addWidget(m_timeWindowCombo);
    
    m_controlsLayout->addStretch();
    
    m_refreshButton = new QPushButton("🔄 Refresh");
    connect(m_refreshButton, &QPushButton::clicked, this, &CAnalyticsWidget::onRefreshRequested);
    m_controlsLayout->addWidget(m_refreshButton);
    
    m_exportButton = new QPushButton("📤 Export");
    connect(m_exportButton, &QPushButton::clicked, this, &CAnalyticsWidget::onExportRequested);
    m_controlsLayout->addWidget(m_exportButton);
    
    mainLayout->addLayout(m_controlsLayout);
    
    // Create main splitter for better space utilization
    m_mainSplitter = new QSplitter(Qt::Vertical);
    
    // Create sections
    createOverallStatsSection();
    createSelectedTrackSection();
    createDetailedStatsSection();
    
    m_mainSplitter->addWidget(m_overallStatsGroup);
    m_mainSplitter->addWidget(m_selectedTrackGroup);
    m_mainSplitter->addWidget(m_detailedStatsGroup);
    
    // Set splitter proportions
    m_mainSplitter->setStretchFactor(0, 1); // Overall stats
    m_mainSplitter->setStretchFactor(1, 1); // Selected track
    m_mainSplitter->setStretchFactor(2, 2); // Detailed table
    
    mainLayout->addWidget(m_mainSplitter);
}

void CAnalyticsWidget::createOverallStatsSection()
{
    m_overallStatsGroup = new QGroupBox("📈 Overall Statistics");
    QGridLayout *layout = new QGridLayout(m_overallStatsGroup);
    layout->setSpacing(8);
    
    // Track counts
    layout->addWidget(new QLabel("Total Tracks:"), 0, 0);
    m_totalTracksLabel = new QLabel("0");
    m_totalTracksLabel->setStyleSheet("color: #00ffff; font-weight: bold; font-size: 14px;");
    layout->addWidget(m_totalTracksLabel, 0, 1);
    
    layout->addWidget(new QLabel("Active Tracks:"), 0, 2);
    m_activeTracksLabel = new QLabel("0");
    m_activeTracksLabel->setStyleSheet("color: #00ff00; font-weight: bold; font-size: 14px;");
    layout->addWidget(m_activeTracksLabel, 0, 3);
    
    // Identity breakdown with progress bars
    layout->addWidget(new QLabel("🟢 Friendly:"), 1, 0);
    m_friendlyTracksLabel = new QLabel("0");
    m_friendlyTracksLabel->setStyleSheet("color: #2ecc71; font-weight: bold;");
    layout->addWidget(m_friendlyTracksLabel, 1, 1);
    m_friendlyProgress = new QProgressBar();
    m_friendlyProgress->setMaximumHeight(15);
    m_friendlyProgress->setStyleSheet("QProgressBar::chunk { background-color: #2ecc71; }");
    layout->addWidget(m_friendlyProgress, 1, 2, 1, 2);
    
    layout->addWidget(new QLabel("🔴 Hostile:"), 2, 0);
    m_hostileTracksLabel = new QLabel("0");
    m_hostileTracksLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    layout->addWidget(m_hostileTracksLabel, 2, 1);
    m_hostileProgress = new QProgressBar();
    m_hostileProgress->setMaximumHeight(15);
    m_hostileProgress->setStyleSheet("QProgressBar::chunk { background-color: #e74c3c; }");
    layout->addWidget(m_hostileProgress, 2, 2, 1, 2);
    
    layout->addWidget(new QLabel("🟡 Unknown:"), 3, 0);
    m_unknownTracksLabel = new QLabel("0");
    m_unknownTracksLabel->setStyleSheet("color: #f1c40f; font-weight: bold;");
    layout->addWidget(m_unknownTracksLabel, 3, 1);
    m_unknownProgress = new QProgressBar();
    m_unknownProgress->setMaximumHeight(15);
    m_unknownProgress->setStyleSheet("QProgressBar::chunk { background-color: #f1c40f; }");
    layout->addWidget(m_unknownProgress, 3, 2, 1, 2);
    
    // Separator line
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("color: #4a5568;");
    layout->addWidget(separator, 4, 0, 1, 4);
    
    // Range statistics
    layout->addWidget(new QLabel("📏 Avg Range:"), 5, 0);
    m_avgRangeLabel = new QLabel("0.0 km");
    m_avgRangeLabel->setStyleSheet("color: #ffffff; font-weight: bold;");
    layout->addWidget(m_avgRangeLabel, 5, 1);
    
    layout->addWidget(new QLabel("📏 Max Range:"), 5, 2);
    m_maxRangeLabel = new QLabel("0.0 km");
    m_maxRangeLabel->setStyleSheet("color: #ff6b6b; font-weight: bold;");
    layout->addWidget(m_maxRangeLabel, 5, 3);
    
    layout->addWidget(new QLabel("📏 Min Range:"), 6, 0);
    m_minRangeLabel = new QLabel("0.0 km");
    m_minRangeLabel->setStyleSheet("color: #4ecdc4; font-weight: bold;");
    layout->addWidget(m_minRangeLabel, 6, 1);
    
    // Speed statistics
    layout->addWidget(new QLabel("⚡ Avg Speed:"), 6, 2);
    m_avgSpeedLabel = new QLabel("0.0 m/s");
    m_avgSpeedLabel->setStyleSheet("color: #ffffff; font-weight: bold;");
    layout->addWidget(m_avgSpeedLabel, 6, 3);
    
    layout->addWidget(new QLabel("⚡ Max Speed:"), 7, 0);
    m_maxSpeedLabel = new QLabel("0.0 m/s");
    m_maxSpeedLabel->setStyleSheet("color: #ff9f43; font-weight: bold;");
    layout->addWidget(m_maxSpeedLabel, 7, 1);
}

void CAnalyticsWidget::createSelectedTrackSection()
{
    m_selectedTrackGroup = new QGroupBox("🎯 Selected Track Details");
    QGridLayout *layout = new QGridLayout(m_selectedTrackGroup);
    layout->setSpacing(8);
    
    // Track ID and Identity
    layout->addWidget(new QLabel("Track ID:"), 0, 0);
    m_selectedTrackIdLabel = new QLabel("None Selected");
    m_selectedTrackIdLabel->setStyleSheet("color: #a0aec0; font-weight: bold; font-size: 14px;");
    layout->addWidget(m_selectedTrackIdLabel, 0, 1);
    
    layout->addWidget(new QLabel("Identity:"), 0, 2);
    m_selectedTrackIdentityLabel = new QLabel("-");
    m_selectedTrackIdentityLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    layout->addWidget(m_selectedTrackIdentityLabel, 0, 3);
    
    // Position and Range
    layout->addWidget(new QLabel("📏 Range:"), 1, 0);
    m_selectedTrackRangeLabel = new QLabel("-");
    m_selectedTrackRangeLabel->setStyleSheet("color: #ffffff; font-weight: bold;");
    layout->addWidget(m_selectedTrackRangeLabel, 1, 1);
    
    layout->addWidget(new QLabel("🧭 Azimuth:"), 1, 2);
    m_selectedTrackAzimuthLabel = new QLabel("-");
    m_selectedTrackAzimuthLabel->setStyleSheet("color: #ffffff; font-weight: bold;");
    layout->addWidget(m_selectedTrackAzimuthLabel, 1, 3);
    
    layout->addWidget(new QLabel("📐 Elevation:"), 2, 0);
    m_selectedTrackElevationLabel = new QLabel("-");
    m_selectedTrackElevationLabel->setStyleSheet("color: #ffffff; font-weight: bold;");
    layout->addWidget(m_selectedTrackElevationLabel, 2, 1);
    
    layout->addWidget(new QLabel("⚡ Speed:"), 2, 2);
    m_selectedTrackSpeedLabel = new QLabel("-");
    m_selectedTrackSpeedLabel->setStyleSheet("color: #ffffff; font-weight: bold;");
    layout->addWidget(m_selectedTrackSpeedLabel, 2, 3);
    
    layout->addWidget(new QLabel("🧭 Heading:"), 3, 0);
    m_selectedTrackHeadingLabel = new QLabel("-");
    m_selectedTrackHeadingLabel->setStyleSheet("color: #ffffff; font-weight: bold;");
    layout->addWidget(m_selectedTrackHeadingLabel, 3, 1);
    
    layout->addWidget(new QLabel("📶 SNR:"), 3, 2);
    m_selectedTrackSNRLabel = new QLabel("-");
    m_selectedTrackSNRLabel->setStyleSheet("color: #ffffff; font-weight: bold;");
    layout->addWidget(m_selectedTrackSNRLabel, 3, 3);
    
    layout->addWidget(new QLabel("📍 Position:"), 4, 0);
    m_selectedTrackPositionLabel = new QLabel("-");
    m_selectedTrackPositionLabel->setStyleSheet("color: #ffffff; font-weight: bold;");
    layout->addWidget(m_selectedTrackPositionLabel, 4, 1, 1, 2);
    
    layout->addWidget(new QLabel("⏰ Last Update:"), 5, 0);
    m_selectedTrackTimeLabel = new QLabel("-");
    m_selectedTrackTimeLabel->setStyleSheet("color: #ffffff; font-weight: bold;");
    layout->addWidget(m_selectedTrackTimeLabel, 5, 1, 1, 2);
    
    // Clear selection button
    m_clearSelectionButton = new QPushButton("❌ Clear Selection");
    connect(m_clearSelectionButton, &QPushButton::clicked, this, &CAnalyticsWidget::clearSelectedTrack);
    layout->addWidget(m_clearSelectionButton, 5, 3);
}

void CAnalyticsWidget::createDetailedStatsSection()
{
    m_detailedStatsGroup = new QGroupBox("📋 Detailed Track Statistics");
    QVBoxLayout *layout = new QVBoxLayout(m_detailedStatsGroup);
    
    m_detailedStatsTable = new QTableWidget();
    m_detailedStatsTable->setColumnCount(9);
    QStringList headers = {"Track ID", "Identity", "Range (km)", "Azimuth (°)", "Elevation (°)", 
                          "Speed (m/s)", "Heading (°)", "SNR (dB)", "Last Seen"};
    m_detailedStatsTable->setHorizontalHeaderLabels(headers);
    
    // Configure table
    m_detailedStatsTable->horizontalHeader()->setStretchLastSection(true);
    m_detailedStatsTable->setAlternatingRowColors(true);
    m_detailedStatsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_detailedStatsTable->setSortingEnabled(true);
    m_detailedStatsTable->verticalHeader()->setVisible(false);
    
    // Connect selection signal
    connect(m_detailedStatsTable, &QTableWidget::itemSelectionChanged, this, [this]() {
        int currentRow = m_detailedStatsTable->currentRow();
        if (currentRow >= 0) {
            QTableWidgetItem *item = m_detailedStatsTable->item(currentRow, 0);
            if (item) {
                int trackId = item->text().toInt();
                onTrackSelected(trackId);
            }
        }
    });
    
    layout->addWidget(m_detailedStatsTable);
}

void CAnalyticsWidget::updateAnalytics()
{
    QList<stTrackDisplayInfo> tracks = CDataWarehouse::getInstance()->getTrackList();
    
    updateOverallStats(tracks);
    updateDetailedStatsTable(tracks);
    
    // Update selected track if one is selected
    if (m_selectedTrackId != -1) {
        for (const auto &track : tracks) {
            if (track.nTrkId == m_selectedTrackId) {
                updateSelectedTrackStats(track);
                break;
            }
        }
    }
}

void CAnalyticsWidget::updateOverallStats(const QList<stTrackDisplayInfo> &tracks)
{
    m_currentStats = TrackStatistics(); // Reset
    
    if (tracks.isEmpty()) {
        m_totalTracksLabel->setText("0");
        m_activeTracksLabel->setText("0");
        m_friendlyTracksLabel->setText("0");
        m_hostileTracksLabel->setText("0");
        m_unknownTracksLabel->setText("0");
        m_avgRangeLabel->setText("0.0 km");
        m_maxRangeLabel->setText("0.0 km");
        m_minRangeLabel->setText("0.0 km");
        m_avgSpeedLabel->setText("0.0 m/s");
        m_maxSpeedLabel->setText("0.0 m/s");
        
        m_friendlyProgress->setValue(0);
        m_hostileProgress->setValue(0);
        m_unknownProgress->setValue(0);
        return;
    }
    
    double totalRange = 0.0;
    double totalSpeed = 0.0;
    double maxRange = 0.0;
    double minRange = std::numeric_limits<double>::max();
    double maxSpeed = 0.0;
    
    for (const auto &track : tracks) {
        m_currentStats.totalTracks++;
        m_currentStats.activeTracks++; // All tracks in the list are considered active
        
        // Count by identity
        switch (track.nTrackIden) {
            case TRACK_IDENTITY_FRIEND:
                m_currentStats.friendlyTracks++;
                break;
            case TRACK_IDENTITY_HOSTILE:
                m_currentStats.hostileTracks++;
                break;
            case TRACK_IDENTITY_UNKNOWN:
                m_currentStats.unknownTracks++;
                break;
        }
        
        // Range statistics
        totalRange += track.range;
        maxRange = qMax(maxRange, track.range);
        minRange = qMin(minRange, track.range);
        
        // Speed statistics
        totalSpeed += qAbs(track.velocity);
        maxSpeed = qMax(maxSpeed, qAbs(track.velocity));
    }
    
    // Calculate averages
    m_currentStats.avgRange = totalRange / tracks.size();
    m_currentStats.maxRange = maxRange;
    m_currentStats.minRange = minRange;
    m_currentStats.avgSpeed = totalSpeed / tracks.size();
    m_currentStats.maxSpeed = maxSpeed;
    
    // Update UI
    m_totalTracksLabel->setText(QString::number(m_currentStats.totalTracks));
    m_activeTracksLabel->setText(QString::number(m_currentStats.activeTracks));
    m_friendlyTracksLabel->setText(QString::number(m_currentStats.friendlyTracks));
    m_hostileTracksLabel->setText(QString::number(m_currentStats.hostileTracks));
    m_unknownTracksLabel->setText(QString::number(m_currentStats.unknownTracks));
    
    m_avgRangeLabel->setText(QString("%1 km").arg(m_currentStats.avgRange / 1000.0, 0, 'f', 2));
    m_maxRangeLabel->setText(QString("%1 km").arg(m_currentStats.maxRange / 1000.0, 0, 'f', 2));
    m_minRangeLabel->setText(QString("%1 km").arg(m_currentStats.minRange / 1000.0, 0, 'f', 2));
    m_avgSpeedLabel->setText(QString("%1 m/s").arg(m_currentStats.avgSpeed, 0, 'f', 1));
    m_maxSpeedLabel->setText(QString("%1 m/s").arg(m_currentStats.maxSpeed, 0, 'f', 1));
    
    // Update progress bars
    int totalTracks = m_currentStats.totalTracks;
    if (totalTracks > 0) {
        m_friendlyProgress->setMaximum(totalTracks);
        m_friendlyProgress->setValue(m_currentStats.friendlyTracks);
        
        m_hostileProgress->setMaximum(totalTracks);
        m_hostileProgress->setValue(m_currentStats.hostileTracks);
        
        m_unknownProgress->setMaximum(totalTracks);
        m_unknownProgress->setValue(m_currentStats.unknownTracks);
    }
}

void CAnalyticsWidget::updateSelectedTrackStats(const stTrackDisplayInfo &track)
{
    m_selectedTrackIdLabel->setText(QString::number(track.nTrkId));
    m_selectedTrackIdLabel->setStyleSheet("color: #00ffff; font-weight: bold; font-size: 14px;");
    
    QString identityText = getIdentityString(track.nTrackIden);
    QColor identityColor = getIdentityColor(track.nTrackIden);
    m_selectedTrackIdentityLabel->setText(identityText);
    m_selectedTrackIdentityLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 14px;")
                                               .arg(identityColor.name()));
    
    m_selectedTrackRangeLabel->setText(QString("%1 km").arg(track.range / 1000.0, 0, 'f', 2));
    m_selectedTrackAzimuthLabel->setText(QString("%1°").arg(track.azimuth, 0, 'f', 1));
    m_selectedTrackElevationLabel->setText(QString("%1°").arg(track.elevation, 0, 'f', 1));
    m_selectedTrackSpeedLabel->setText(QString("%1 m/s").arg(track.velocity, 0, 'f', 1));
    m_selectedTrackHeadingLabel->setText(QString("%1°").arg(track.heading, 0, 'f', 1));
    m_selectedTrackSNRLabel->setText(QString("%1 dB").arg(track.snr, 0, 'f', 1));
    m_selectedTrackPositionLabel->setText(QString("Lat: %1°, Lon: %2°")
                                         .arg(track.lat, 0, 'f', 5)
                                         .arg(track.lon, 0, 'f', 5));
    
    QDateTime lastUpdate = QDateTime::fromMSecsSinceEpoch(track.nTrackTime);
    m_selectedTrackTimeLabel->setText(lastUpdate.toString("hh:mm:ss"));
}

void CAnalyticsWidget::updateDetailedStatsTable(const QList<stTrackDisplayInfo> &tracks)
{
    m_detailedStatsTable->setRowCount(tracks.size());
    
    for (int i = 0; i < tracks.size(); ++i) {
        const auto &track = tracks[i];
        
        // Track ID
        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(track.nTrkId));
        idItem->setTextAlignment(Qt::AlignCenter);
        m_detailedStatsTable->setItem(i, 0, idItem);
        
        // Identity
        QTableWidgetItem *identityItem = new QTableWidgetItem(getIdentityString(track.nTrackIden));
        identityItem->setTextAlignment(Qt::AlignCenter);
        identityItem->setForeground(QBrush(getIdentityColor(track.nTrackIden)));
        m_detailedStatsTable->setItem(i, 1, identityItem);
        
        // Range
        QTableWidgetItem *rangeItem = new QTableWidgetItem(QString("%1").arg(track.range / 1000.0, 0, 'f', 2));
        rangeItem->setTextAlignment(Qt::AlignCenter);
        m_detailedStatsTable->setItem(i, 2, rangeItem);
        
        // Azimuth
        QTableWidgetItem *azimuthItem = new QTableWidgetItem(QString("%1").arg(track.azimuth, 0, 'f', 1));
        azimuthItem->setTextAlignment(Qt::AlignCenter);
        m_detailedStatsTable->setItem(i, 3, azimuthItem);
        
        // Elevation
        QTableWidgetItem *elevationItem = new QTableWidgetItem(QString("%1").arg(track.elevation, 0, 'f', 1));
        elevationItem->setTextAlignment(Qt::AlignCenter);
        m_detailedStatsTable->setItem(i, 4, elevationItem);
        
        // Speed
        QTableWidgetItem *speedItem = new QTableWidgetItem(QString("%1").arg(track.velocity, 0, 'f', 1));
        speedItem->setTextAlignment(Qt::AlignCenter);
        m_detailedStatsTable->setItem(i, 5, speedItem);
        
        // Heading
        QTableWidgetItem *headingItem = new QTableWidgetItem(QString("%1").arg(track.heading, 0, 'f', 1));
        headingItem->setTextAlignment(Qt::AlignCenter);
        m_detailedStatsTable->setItem(i, 6, headingItem);
        
        // SNR
        QTableWidgetItem *snrItem = new QTableWidgetItem(QString("%1").arg(track.snr, 0, 'f', 1));
        snrItem->setTextAlignment(Qt::AlignCenter);
        m_detailedStatsTable->setItem(i, 7, snrItem);
        
        // Last Seen
        QDateTime lastSeen = QDateTime::fromMSecsSinceEpoch(track.nTrackTime);
        QTableWidgetItem *timeItem = new QTableWidgetItem(lastSeen.toString("hh:mm:ss"));
        timeItem->setTextAlignment(Qt::AlignCenter);
        m_detailedStatsTable->setItem(i, 8, timeItem);
        
        // Highlight selected track
        if (track.nTrkId == m_selectedTrackId) {
            for (int col = 0; col < m_detailedStatsTable->columnCount(); ++col) {
                QTableWidgetItem *item = m_detailedStatsTable->item(i, col);
                if (item) {
                    item->setBackground(QBrush(QColor(102, 126, 234, 100)));
                }
            }
        }
    }
    
    // Auto-resize columns
    m_detailedStatsTable->resizeColumnsToContents();
}

void CAnalyticsWidget::onTrackSelected(int trackId)
{
    m_selectedTrackId = trackId;
    
    // Find and update selected track info immediately
    QList<stTrackDisplayInfo> tracks = CDataWarehouse::getInstance()->getTrackList();
    for (const auto &track : tracks) {
        if (track.nTrkId == trackId) {
            updateSelectedTrackStats(track);
            break;
        }
    }
    
    // Update table highlighting
    updateDetailedStatsTable(tracks);
}

void CAnalyticsWidget::clearSelectedTrack()
{
    m_selectedTrackId = -1;
    
    // Clear selected track display
    m_selectedTrackIdLabel->setText("None Selected");
    m_selectedTrackIdLabel->setStyleSheet("color: #a0aec0; font-weight: bold; font-size: 14px;");
    m_selectedTrackIdentityLabel->setText("-");
    m_selectedTrackIdentityLabel->setStyleSheet("color: #a0aec0; font-weight: bold; font-size: 14px;");
    m_selectedTrackRangeLabel->setText("-");
    m_selectedTrackAzimuthLabel->setText("-");
    m_selectedTrackElevationLabel->setText("-");
    m_selectedTrackSpeedLabel->setText("-");
    m_selectedTrackHeadingLabel->setText("-");
    m_selectedTrackSNRLabel->setText("-");
    m_selectedTrackPositionLabel->setText("-");
    m_selectedTrackTimeLabel->setText("-");
    
    // Update table to remove highlighting
    QList<stTrackDisplayInfo> tracks = CDataWarehouse::getInstance()->getTrackList();
    updateDetailedStatsTable(tracks);
}

void CAnalyticsWidget::onRefreshRequested()
{
    updateAnalytics();
}

void CAnalyticsWidget::onExportRequested()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export Analytics", 
                                                   QString("analytics_%1.csv")
                                                   .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
                                                   "CSV Files (*.csv)");
    
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            
            // Write header
            stream << "Track ID,Identity,Range (km),Azimuth (°),Elevation (°),Speed (m/s),Heading (°),SNR (dB),Last Seen\n";
            
            // Write data
            QList<stTrackDisplayInfo> tracks = CDataWarehouse::getInstance()->getTrackList();
            for (const auto &track : tracks) {
                stream << track.nTrkId << ","
                       << getIdentityString(track.nTrackIden) << ","
                       << QString::number(track.range / 1000.0, 'f', 2) << ","
                       << QString::number(track.azimuth, 'f', 1) << ","
                       << QString::number(track.elevation, 'f', 1) << ","
                       << QString::number(track.velocity, 'f', 1) << ","
                       << QString::number(track.heading, 'f', 1) << ","
                       << QString::number(track.snr, 'f', 1) << ","
                       << QDateTime::fromMSecsSinceEpoch(track.nTrackTime).toString("hh:mm:ss") << "\n";
            }
            
            file.close();
            QMessageBox::information(this, "Export Complete", 
                                   QString("Analytics exported to:\n%1").arg(fileName));
        } else {
            QMessageBox::warning(this, "Export Failed", "Could not write to file.");
        }
    }
}

void CAnalyticsWidget::onTimeWindowChanged(int index)
{
    Q_UNUSED(index);
    // Time window filtering can be implemented here if needed
    updateAnalytics();
}

QString CAnalyticsWidget::formatDuration(qint64 milliseconds)
{
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    qint64 hours = minutes / 60;
    
    if (hours > 0) {
        return QString("%1h %2m %3s").arg(hours).arg(minutes % 60).arg(seconds % 60);
    } else if (minutes > 0) {
        return QString("%1m %2s").arg(minutes).arg(seconds % 60);
    } else {
        return QString("%1s").arg(seconds);
    }
}

QString CAnalyticsWidget::getIdentityString(int identity)
{
    switch (identity) {
        case TRACK_IDENTITY_FRIEND:
            return "🟢 Friendly";
        case TRACK_IDENTITY_HOSTILE:
            return "🔴 Hostile";
        case TRACK_IDENTITY_UNKNOWN:
            return "🟡 Unknown";
        default:
            return "⚪ Default";
    }
}

QColor CAnalyticsWidget::getIdentityColor(int identity)
{
    switch (identity) {
        case TRACK_IDENTITY_FRIEND:
            return QColor(46, 204, 113);
        case TRACK_IDENTITY_HOSTILE:
            return QColor(231, 76, 60);
        case TRACK_IDENTITY_UNKNOWN:
            return QColor(241, 196, 15);
        default:
            return QColor(149, 165, 166);
    }
}

void CAnalyticsWidget::applyModernStyle()
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
        "   color: white;"
        "   font-weight: bold;"
        "}"
        "QGroupBox {"
        "   background-color: #2d3748;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 8px;"
        "   margin-top: 12px;"
        "   padding-top: 12px;"
        "   font-weight: bold;"
        "   color: #ffffff;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top left;"
        "   padding: 4px 8px;"
        "   background-color: #667eea;"
        "   border-radius: 4px;"
        "   color: white;"
        "}"
        "QLabel {"
        "   color: #a0aec0;"
        "   font-size: 12px;"
        "}"
        "QProgressBar {"
        "   background-color: #4a5568;"
        "   border: 1px solid #2d3748;"
        "   border-radius: 4px;"
        "   text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "   border-radius: 3px;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #667eea, stop:1 #764ba2);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   padding: 8px 12px;"
        "   font-weight: bold;"
        "   font-size: 11px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #5568d3, stop:1 #6b3fa0);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4c51bf, stop:1 #5a32a3);"
        "}"
        "QComboBox {"
        "   background-color: #2d3748;"
        "   color: #ffffff;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 6px;"
        "   padding: 6px;"
        "   font-size: 11px;"
        "}"
        "QComboBox:hover {"
        "   border: 2px solid #667eea;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #2d3748;"
        "   color: #ffffff;"
        "   selection-background-color: #667eea;"
        "}"
        "QTableWidget {"
        "   background-color: #1a202c;"
        "   alternate-background-color: #2d3748;"
        "   gridline-color: #4a5568;"
        "   color: #ffffff;"
        "   font-size: 11px;"
        "   border: 1px solid #4a5568;"
        "   border-radius: 6px;"
        "}"
        "QHeaderView::section {"
        "   background-color: #667eea;"
        "   color: white;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "   padding: 8px;"
        "   border: none;"
        "}"
        "QTableWidget::item:selected {"
        "   background-color: #667eea;"
        "}"
        "QSplitter::handle {"
        "   background-color: #4a5568;"
        "}"
        "QFrame {"
        "   color: #4a5568;"
        "}"
    );
}
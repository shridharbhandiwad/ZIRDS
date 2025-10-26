#include "csimulationwidget.h"
#include <QDateTime>
#include <QtMath>
#include <QHeaderView>
#include <QDebug>
#include <QRandomGenerator>
#include <QHostAddress>

CSimulationWidget::CSimulationWidget(QWidget *parent)
    : QDockWidget("🎮 Track Simulation Control", parent)
    , m_trackCount(5)
    , m_maxRange(10000.0) // 10km in meters
    , m_updateRate(1000) // 1 second
    , m_azimuthSpread(360)
    , m_minSpeed(10.0)
    , m_maxSpeed(50.0)
    , m_randomMovement(false)
    , m_isRunning(false)
    , m_isPaused(false)
    , m_packetsSent(0)
    , m_simulationStartTime(0)
{
    setupUI();
    applyModernStyle();

    // Initialize UDP socket
    m_udpSocket = new QUdpSocket(this);

    // Initialize simulation timer
    m_simulationTimer = new QTimer(this);
    connect(m_simulationTimer, &QTimer::timeout, this, &CSimulationWidget::updateSimulation);
}

CSimulationWidget::~CSimulationWidget()
{
    if (m_isRunning) {
        stopSimulation();
    }
}

void CSimulationWidget::setupUI()
{
    m_mainWidget = new QWidget(this);
    m_mainLayout = new QVBoxLayout(m_mainWidget);
    m_mainLayout->setSpacing(12);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);

    createControlsSection();
    createParametersSection();
    createTrackListSection();
    createStatusSection();

    m_mainLayout->addStretch();
    setWidget(m_mainWidget);
}

void CSimulationWidget::createControlsSection()
{
    m_controlsGroup = new QGroupBox("⚡ Simulation Controls", m_mainWidget);
    QVBoxLayout *layout = new QVBoxLayout(m_controlsGroup);

    // Status label
    m_statusLabel = new QLabel("Status: <span style='color: #FFA500;'>Ready</span>");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    QFont statusFont = m_statusLabel->font();
    statusFont.setPointSize(12);
    statusFont.setBold(true);
    m_statusLabel->setFont(statusFont);
    layout->addWidget(m_statusLabel);

    // Button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    m_startButton = new QPushButton("▶ Start");
    m_stopButton = new QPushButton("⏹ Stop");
    m_pauseButton = new QPushButton("⏸ Pause");
    m_resetButton = new QPushButton("🔄 Reset");

    m_startButton->setMinimumHeight(40);
    m_stopButton->setMinimumHeight(40);
    m_pauseButton->setMinimumHeight(40);
    m_resetButton->setMinimumHeight(40);

    m_stopButton->setEnabled(false);
    m_pauseButton->setEnabled(false);

    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_pauseButton);
    buttonLayout->addWidget(m_stopButton);
    buttonLayout->addWidget(m_resetButton);

    layout->addLayout(buttonLayout);

    // Connect signals
    connect(m_startButton, &QPushButton::clicked, this, &CSimulationWidget::startSimulation);
    connect(m_stopButton, &QPushButton::clicked, this, &CSimulationWidget::stopSimulation);
    connect(m_pauseButton, &QPushButton::clicked, this, &CSimulationWidget::pauseSimulation);
    connect(m_resetButton, &QPushButton::clicked, this, &CSimulationWidget::resetSimulation);

    m_mainLayout->addWidget(m_controlsGroup);
}

void CSimulationWidget::createParametersSection()
{
    m_parametersGroup = new QGroupBox("⚙️ Simulation Parameters", m_mainWidget);
    QGridLayout *layout = new QGridLayout(m_parametersGroup);
    layout->setSpacing(10);

    int row = 0;

    // Track Count
    layout->addWidget(new QLabel("Number of Tracks:"), row, 0);
    m_trackCountSpinBox = new QSpinBox();
    m_trackCountSpinBox->setRange(1, 1000);
    m_trackCountSpinBox->setValue(m_trackCount);
    m_trackCountSpinBox->setSuffix(" tracks");
    layout->addWidget(m_trackCountSpinBox, row++, 1);

    // Max Range
    layout->addWidget(new QLabel("Maximum Range:"), row, 0);
    m_maxRangeSpinBox = new QDoubleSpinBox();
    m_maxRangeSpinBox->setRange(1000, 50000);
    m_maxRangeSpinBox->setValue(m_maxRange);
    m_maxRangeSpinBox->setSuffix(" m");
    m_maxRangeSpinBox->setDecimals(0);
    layout->addWidget(m_maxRangeSpinBox, row++, 1);

    // Update Rate
    layout->addWidget(new QLabel("Update Rate:"), row, 0);
    m_updateRateSpinBox = new QSpinBox();
    m_updateRateSpinBox->setRange(100, 5000);
    m_updateRateSpinBox->setValue(m_updateRate);
    m_updateRateSpinBox->setSuffix(" ms");
    m_updateRateSpinBox->setSingleStep(100);
    layout->addWidget(m_updateRateSpinBox, row++, 1);

    // Azimuth Spread
    layout->addWidget(new QLabel("Azimuth Spread:"), row, 0);
    m_azimuthSpreadSpinBox = new QSpinBox();
    m_azimuthSpreadSpinBox->setRange(30, 360);
    m_azimuthSpreadSpinBox->setValue(m_azimuthSpread);
    m_azimuthSpreadSpinBox->setSuffix("°");
    layout->addWidget(m_azimuthSpreadSpinBox, row++, 1);

    // Min Speed
    layout->addWidget(new QLabel("Min Speed:"), row, 0);
    m_minSpeedSpinBox = new QDoubleSpinBox();
    m_minSpeedSpinBox->setRange(1, 100);
    m_minSpeedSpinBox->setValue(m_minSpeed);
    m_minSpeedSpinBox->setSuffix(" m/s");
    layout->addWidget(m_minSpeedSpinBox, row++, 1);

    // Max Speed
    layout->addWidget(new QLabel("Max Speed:"), row, 0);
    m_maxSpeedSpinBox = new QDoubleSpinBox();
    m_maxSpeedSpinBox->setRange(10, 500);
    m_maxSpeedSpinBox->setValue(m_maxSpeed);
    m_maxSpeedSpinBox->setSuffix(" m/s");
    layout->addWidget(m_maxSpeedSpinBox, row++, 1);

    // Random Movement
    m_randomMovementCheckBox = new QCheckBox("Random Movement");
    m_randomMovementCheckBox->setChecked(m_randomMovement);
    layout->addWidget(m_randomMovementCheckBox, row++, 0, 1, 2);

    // Track Identity
    layout->addWidget(new QLabel("Default Identity:"), row, 0);
    m_identityCombo = new QComboBox();
    m_identityCombo->addItem("🔵 Friend", TRACK_IDENTITY_FRIEND);
    m_identityCombo->addItem("🔴 Hostile", TRACK_IDENTITY_HOSTILE);
    m_identityCombo->addItem("⚪ Unknown", TRACK_IDENTITY_UNKNOWN);
    m_identityCombo->addItem("🎲 Random", -1);
    m_identityCombo->setCurrentIndex(3); // Random by default
    layout->addWidget(m_identityCombo, row++, 1);

    // Connect parameter change signals
    connect(m_trackCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &CSimulationWidget::onTrackCountChanged);
    connect(m_maxRangeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &CSimulationWidget::onMaxRangeChanged);
    connect(m_updateRateSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &CSimulationWidget::onUpdateRateChanged);
    connect(m_azimuthSpreadSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &CSimulationWidget::onAzimuthSpreadChanged);

    m_mainLayout->addWidget(m_parametersGroup);
}

void CSimulationWidget::createTrackListSection()
{
    m_trackListGroup = new QGroupBox("📊 Simulated Tracks", m_mainWidget);
    QVBoxLayout *layout = new QVBoxLayout(m_trackListGroup);

    m_trackTable = new QTableWidget();
    m_trackTable->setColumnCount(7);
    m_trackTable->setHorizontalHeaderLabels({
        "ID", "Range (m)", "Azimuth (°)", "Heading (°)", "Speed (m/s)", "Identity", "Status"
    });

    m_trackTable->horizontalHeader()->setStretchLastSection(true);
    m_trackTable->setAlternatingRowColors(true);
    m_trackTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_trackTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_trackTable->setMaximumHeight(250);

    layout->addWidget(m_trackTable);
    m_mainLayout->addWidget(m_trackListGroup);
}

void CSimulationWidget::createStatusSection()
{
    m_statusGroup = new QGroupBox("📈 Simulation Statistics", m_mainWidget);
    QGridLayout *layout = new QGridLayout(m_statusGroup);

    int row = 0;

    layout->addWidget(new QLabel("Active Tracks:"), row, 0);
    m_activeTracksLabel = new QLabel("0");
    m_activeTracksLabel->setStyleSheet("font-weight: bold; color: #4ade80;");
    layout->addWidget(m_activeTracksLabel, row++, 1);

    layout->addWidget(new QLabel("Packets Sent:"), row, 0);
    m_packetsSentLabel = new QLabel("0");
    m_packetsSentLabel->setStyleSheet("font-weight: bold; color: #60a5fa;");
    layout->addWidget(m_packetsSentLabel, row++, 1);

    layout->addWidget(new QLabel("Simulation Time:"), row, 0);
    m_simulationTimeLabel = new QLabel("00:00:00");
    m_simulationTimeLabel->setStyleSheet("font-weight: bold; color: #fbbf24;");
    layout->addWidget(m_simulationTimeLabel, row++, 1);

    layout->addWidget(new QLabel("CPU Usage:"), row, 0);
    m_cpuUsageBar = new QProgressBar();
    m_cpuUsageBar->setRange(0, 100);
    m_cpuUsageBar->setValue(15);
    m_cpuUsageBar->setFormat("%v%");
    layout->addWidget(m_cpuUsageBar, row++, 1);

    m_mainLayout->addWidget(m_statusGroup);
}

void CSimulationWidget::applyModernStyle()
{
    setStyleSheet(
        "QDockWidget {"
        "   background-color: #f8fafc;"
        "   color: #1e293b;"
        "   border: 1px solid #e2e8f0;"
        "}"
        "QDockWidget::title {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3b82f6, stop:1 #2563eb);"
        "   color: white;"
        "   padding: 10px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "   border-radius: 4px;"
        "}"
        "QGroupBox {"
        "   background-color: #ffffff;"
        "   border: 2px solid #e2e8f0;"
        "   border-radius: 10px;"
        "   margin-top: 16px;"
        "   padding-top: 18px;"
        "   font-weight: bold;"
        "   color: #1e293b;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top left;"
        "   padding: 6px 12px;"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3b82f6, stop:1 #2563eb);"
        "   border-radius: 6px;"
        "   color: white;"
        "   font-size: 12px;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #3b82f6, stop:1 #2563eb);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px 16px;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2563eb, stop:1 #1d4ed8);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #1d4ed8, stop:1 #1e40af);"
        "}"
        "QPushButton:disabled {"
        "   background-color: #e2e8f0;"
        "   color: #94a3b8;"
        "}"
        "QSpinBox, QDoubleSpinBox, QComboBox {"
        "   background-color: #ffffff;"
        "   color: #1e293b;"
        "   border: 2px solid #e2e8f0;"
        "   border-radius: 6px;"
        "   padding: 6px;"
        "   font-size: 11px;"
        "}"
        "QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {"
        "   border: 2px solid #3b82f6;"
        "}"
        "QLabel {"
        "   color: #334155;"
        "   font-size: 11px;"
        "}"
        "QTableWidget {"
        "   background-color: #ffffff;"
        "   color: #1e293b;"
        "   border: 1px solid #e2e8f0;"
        "   border-radius: 6px;"
        "   gridline-color: #e2e8f0;"
        "}"
        "QTableWidget::item {"
        "   padding: 4px;"
        "}"
        "QTableWidget::item:selected {"
        "   background-color: #3b82f6;"
        "   color: white;"
        "}"
        "QHeaderView::section {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f1f5f9, stop:1 #e2e8f0);"
        "   color: #1e293b;"
        "   padding: 6px;"
        "   border: 1px solid #e2e8f0;"
        "   font-weight: bold;"
        "   font-size: 10px;"
        "}"
        "QCheckBox {"
        "   color: #1e293b;"
        "   spacing: 8px;"
        "}"
        "QCheckBox::indicator {"
        "   width: 18px;"
        "   height: 18px;"
        "   border: 2px solid #e2e8f0;"
        "   border-radius: 4px;"
        "   background-color: #ffffff;"
        "}"
        "QCheckBox::indicator:checked {"
        "   background-color: #3b82f6;"
        "   border: 2px solid #3b82f6;"
        "}"
        "QProgressBar {"
        "   background-color: #e2e8f0;"
        "   border: 2px solid #cbd5e1;"
        "   border-radius: 6px;"
        "   text-align: center;"
        "   color: #1e293b;"
        "   font-weight: bold;"
        "}"
        "QProgressBar::chunk {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3b82f6, stop:1 #2563eb);"
        "   border-radius: 4px;"
        "}"
    );
}

void CSimulationWidget::startSimulation()
{
    if (m_isPaused) {
        // Resume from pause
        m_isPaused = false;
        m_simulationTimer->start(m_updateRate);
        m_statusLabel->setText("Status: <span style='color: #4ade80;'>Running</span>");
        m_pauseButton->setText("⏸ Pause");
    } else {
        // Start new simulation
        m_isRunning = true;
        m_isPaused = false;
        m_packetsSent = 0;
        m_simulationStartTime = QDateTime::currentMSecsSinceEpoch();

        // Generate initial tracks
        generateInitialTracks();

        // Start the simulation timer
        m_simulationTimer->start(m_updateRate);

        // Update UI
        m_statusLabel->setText("Status: <span style='color: #4ade80;'>Running</span>");
        m_startButton->setEnabled(false);
        m_stopButton->setEnabled(true);
        m_pauseButton->setEnabled(true);

        // Disable parameter changes during simulation
        m_parametersGroup->setEnabled(false);

        qDebug() << "[Simulation] Started with" << m_trackCount << "tracks";
    }
}

void CSimulationWidget::stopSimulation()
{
    m_isRunning = false;
    m_isPaused = false;
    m_simulationTimer->stop();

    // Update UI
    m_statusLabel->setText("Status: <span style='color: #ef4444;'>Stopped</span>");
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_pauseButton->setEnabled(false);
    m_pauseButton->setText("⏸ Pause");

    // Re-enable parameter changes
    m_parametersGroup->setEnabled(true);

    qDebug() << "[Simulation] Stopped. Total packets sent:" << m_packetsSent;
}

void CSimulationWidget::pauseSimulation()
{
    if (m_isPaused) {
        startSimulation(); // Resume
    } else {
        m_isPaused = true;
        m_simulationTimer->stop();
        m_statusLabel->setText("Status: <span style='color: #fbbf24;'>Paused</span>");
        m_pauseButton->setText("▶ Resume");
        qDebug() << "[Simulation] Paused";
    }
}

void CSimulationWidget::resetSimulation()
{
    stopSimulation();

    // Clear all tracks
    m_simulatedTracks.clear();
    m_trackTable->setRowCount(0);

    // Reset statistics
    m_packetsSent = 0;
    m_activeTracksLabel->setText("0");
    m_packetsSentLabel->setText("0");
    m_simulationTimeLabel->setText("00:00:00");
    m_cpuUsageBar->setValue(0);

    m_statusLabel->setText("Status: <span style='color: #FFA500;'>Ready</span>");

    qDebug() << "[Simulation] Reset";
}

void CSimulationWidget::updateSimulation()
{
    if (!m_isRunning || m_isPaused) {
        return;
    }

    // Update track positions
    updateTrackPositions();

    // Send all tracks via UDP
    for (const stTrackRecvInfo &track : m_simulatedTracks) {
        sendTrackData(track);
    }

    // Update UI
    updateTrackTable();

    // Update statistics
    m_activeTracksLabel->setText(QString::number(m_simulatedTracks.count()));
    m_packetsSentLabel->setText(QString::number(m_packetsSent));

    // Update simulation time
    qint64 elapsedMs = QDateTime::currentMSecsSinceEpoch() - m_simulationStartTime;
    int hours = elapsedMs / 3600000;
    int minutes = (elapsedMs % 3600000) / 60000;
    int seconds = (elapsedMs % 60000) / 1000;
    m_simulationTimeLabel->setText(QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0')));

    // Simulate CPU usage (15-30%)
    int cpuUsage = 15 + QRandomGenerator::global()->bounded(15);
    m_cpuUsageBar->setValue(cpuUsage);
}

void CSimulationWidget::generateInitialTracks()
{
    m_simulatedTracks.clear();

    for (int i = 0; i < m_trackCount; ++i) {
        stTrackRecvInfo track = createRandomTrack(i + 1);
        m_simulatedTracks.append(track);
    }

    qDebug() << "[Simulation] Generated" << m_trackCount << "initial tracks";
}

stTrackRecvInfo CSimulationWidget::createRandomTrack(int trackId)
{
    stTrackRecvInfo track;
    track.usMsgId = 100;
    track.nTrkId = trackId;

    // Generate random position within max range
    double range = QRandomGenerator::global()->bounded(1000, (int)m_maxRange);

    // Distribute azimuth angles evenly across the spread
    double azimuthStart = (360.0 - m_azimuthSpread) / 2.0;
    double azimuth = azimuthStart + (m_azimuthSpread * trackId / static_cast<double>(m_trackCount));
    azimuth = fmod(azimuth, 360.0);

    // Convert polar to Cartesian
    double azimuthRad = qDegreesToRadians(azimuth);
    track.x = range * qCos(azimuthRad);
    track.y = range * qSin(azimuthRad);
    track.z = QRandomGenerator::global()->bounded(0, 5000); // 0-5km altitude

    // Random velocity
    track.velocity = QRandomGenerator::global()->bounded((int)m_minSpeed, (int)m_maxSpeed);

    // Random heading
    track.heading = QRandomGenerator::global()->bounded(0, 360);

    // Assign identity
    int identitySelection = m_identityCombo->currentData().toInt();
    if (identitySelection == -1) {
        // Random identity
        int identities[] = {TRACK_IDENTITY_FRIEND, TRACK_IDENTITY_HOSTILE, TRACK_IDENTITY_UNKNOWN};
        track.nTrackIden = identities[QRandomGenerator::global()->bounded(3)];
    } else {
        track.nTrackIden = identitySelection;
    }

    return track;
}

void CSimulationWidget::updateTrackPositions()
{
    double deltaTime = m_updateRate / 1000.0; // Convert to seconds

    for (stTrackRecvInfo &track : m_simulatedTracks) {
        // Update position based on heading and velocity
        double headingRad = qDegreesToRadians(track.heading);
        double dx = track.velocity * deltaTime * qCos(headingRad);
        double dy = track.velocity * deltaTime * qSin(headingRad);

        track.x += dx;
        track.y += dy;

        // Random movement if enabled
        if (m_randomMovementCheckBox->isChecked()) {
            // Randomly adjust heading slightly
            double headingChange = QRandomGenerator::global()->bounded(-10, 10);
            track.heading = fmod(track.heading + headingChange + 360.0, 360.0);
        }

        // Keep tracks within bounds (bounce off edges)
        double range = qSqrt(track.x * track.x + track.y * track.y);
        if (range > m_maxRange * 1.5) {
            // Reverse direction
            track.heading = fmod(track.heading + 180.0, 360.0);
        }
    }
}

void CSimulationWidget::sendTrackData(const stTrackRecvInfo &track)
{
    // Send via UDP for compatibility
    QByteArray datagram(reinterpret_cast<const char*>(&track), sizeof(stTrackRecvInfo));

    qint64 bytesSent = m_udpSocket->writeDatagram(
        datagram,
        QHostAddress(UDP_ADDRESS),
        UDP_PORT
    );

    if (bytesSent == sizeof(stTrackRecvInfo)) {
        m_packetsSent++;
    } else {
        qWarning() << "[Simulation] Failed to send track" << track.nTrkId;
    }

    // Also emit signal for direct connection to data warehouse
    emit simulatedTrackData(track);
}

void CSimulationWidget::updateTrackTable()
{
    m_trackTable->setRowCount(m_simulatedTracks.count());

    for (int i = 0; i < m_simulatedTracks.count(); ++i) {
        const stTrackRecvInfo &track = m_simulatedTracks[i];

        // Calculate range and azimuth for display
        double range = qSqrt(track.x * track.x + track.y * track.y);
        double azimuth = qRadiansToDegrees(qAtan2(track.y, track.x));
        if (azimuth < 0) azimuth += 360.0;

        // ID
        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(track.nTrkId));
        idItem->setTextAlignment(Qt::AlignCenter);
        m_trackTable->setItem(i, 0, idItem);

        // Range
        QTableWidgetItem *rangeItem = new QTableWidgetItem(QString::number(range, 'f', 0));
        rangeItem->setTextAlignment(Qt::AlignCenter);
        m_trackTable->setItem(i, 1, rangeItem);

        // Azimuth
        QTableWidgetItem *azItem = new QTableWidgetItem(QString::number(azimuth, 'f', 1));
        azItem->setTextAlignment(Qt::AlignCenter);
        m_trackTable->setItem(i, 2, azItem);

        // Heading
        QTableWidgetItem *headingItem = new QTableWidgetItem(QString::number(track.heading, 'f', 1));
        headingItem->setTextAlignment(Qt::AlignCenter);
        m_trackTable->setItem(i, 3, headingItem);

        // Speed
        QTableWidgetItem *speedItem = new QTableWidgetItem(QString::number(track.velocity, 'f', 1));
        speedItem->setTextAlignment(Qt::AlignCenter);
        m_trackTable->setItem(i, 4, speedItem);

        // Identity
        QString identity;
        switch (track.nTrackIden) {
            case TRACK_IDENTITY_FRIEND: identity = "🔵 Friend"; break;
            case TRACK_IDENTITY_HOSTILE: identity = "🔴 Hostile"; break;
            case TRACK_IDENTITY_UNKNOWN: identity = "⚪ Unknown"; break;
            default: identity = "⚫ Default"; break;
        }
        QTableWidgetItem *identityItem = new QTableWidgetItem(identity);
        identityItem->setTextAlignment(Qt::AlignCenter);
        m_trackTable->setItem(i, 5, identityItem);

        // Status
        QTableWidgetItem *statusItem = new QTableWidgetItem("✓ Active");
        statusItem->setTextAlignment(Qt::AlignCenter);
        statusItem->setForeground(QBrush(QColor("#4ade80")));
        m_trackTable->setItem(i, 6, statusItem);
    }
}

void CSimulationWidget::onTrackCountChanged(int count)
{
    m_trackCount = count;
}

void CSimulationWidget::onMaxRangeChanged(double range)
{
    m_maxRange = range;
}

void CSimulationWidget::onUpdateRateChanged(int rate)
{
    m_updateRate = rate;
    if (m_isRunning && !m_isPaused) {
        m_simulationTimer->setInterval(m_updateRate);
    }
}

void CSimulationWidget::onAzimuthSpreadChanged(int spread)
{
    m_azimuthSpread = spread;
}

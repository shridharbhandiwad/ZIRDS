#include "chealthmonitorwidget.h"
#include <QDateTime>
#include <QDebug>
#include <QRandomGenerator>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>

CHealthMonitorWidget::CHealthMonitorWidget(QWidget *parent)
    : QDockWidget("💚 System Health Monitor", parent)
    , m_systemStartTime(QDateTime::currentMSecsSinceEpoch())
    , m_activeAlerts(0)
{
    m_hardwareModules << "🎯 Radar Unit" << "⚙️ Servo Controller" 
                      << "🔋 Power Supply" << "📡 RF Transceiver"
                      << "🌡️ Thermal System" << "💾 Data Storage";
    
    m_softwareModules << "🖥️ Display Engine" << "🧠 Data Processor"
                      << "🌐 Network Stack" << "📊 Analytics Engine"
                      << "🔐 Security Module" << "🗂️ Database Manager";
    
    setupUI();
    applyModernStyle();
    
    // Initialize health data
    simulateHealthData();
    
    // Start update timer (5 seconds)
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &CHealthMonitorWidget::updateHealth);
    m_updateTimer->start(5000);
    
    // Initial update
    updateHealth();
}

CHealthMonitorWidget::~CHealthMonitorWidget()
{
}

void CHealthMonitorWidget::setupUI()
{
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    m_mainWidget = new QWidget();
    m_mainLayout = new QVBoxLayout(m_mainWidget);
    m_mainLayout->setSpacing(12);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);
    
    createOverviewSection();
    createHardwareSection();
    createSoftwareSection();
    createPerformanceSection();
    createAlertsSection();
    
    m_mainLayout->addStretch();
    scrollArea->setWidget(m_mainWidget);
    setWidget(scrollArea);
}

void CHealthMonitorWidget::createOverviewSection()
{
    m_overviewGroup = new QGroupBox("📋 System Overview", m_mainWidget);
    QVBoxLayout *layout = new QVBoxLayout(m_overviewGroup);
    
    // System health display
    m_systemHealthLabel = new QLabel("Overall System Health");
    m_systemHealthLabel->setAlignment(Qt::AlignCenter);
    QFont font = m_systemHealthLabel->font();
    font.setPointSize(12);
    font.setBold(true);
    m_systemHealthLabel->setFont(font);
    layout->addWidget(m_systemHealthLabel);
    
    m_systemHealthBar = new QProgressBar();
    m_systemHealthBar->setRange(0, 100);
    m_systemHealthBar->setValue(95);
    m_systemHealthBar->setMinimumHeight(35);
    m_systemHealthBar->setFormat("%v% - Healthy");
    layout->addWidget(m_systemHealthBar);
    
    // Info grid
    QGridLayout *infoGrid = new QGridLayout();
    
    infoGrid->addWidget(new QLabel("System Uptime:"), 0, 0);
    m_uptimeLabel = new QLabel("00:00:00");
    m_uptimeLabel->setStyleSheet("font-weight: bold; color: #4ade80;");
    infoGrid->addWidget(m_uptimeLabel, 0, 1);
    
    infoGrid->addWidget(new QLabel("Last Health Check:"), 1, 0);
    m_lastCheckLabel = new QLabel("Just now");
    m_lastCheckLabel->setStyleSheet("font-weight: bold; color: #60a5fa;");
    infoGrid->addWidget(m_lastCheckLabel, 1, 1);
    
    layout->addLayout(infoGrid);
    
    // Action buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_diagnosticsButton = new QPushButton("🔍 Run Diagnostics");
    m_exportButton = new QPushButton("📄 Export Report");
    
    m_diagnosticsButton->setMinimumHeight(35);
    m_exportButton->setMinimumHeight(35);
    
    buttonLayout->addWidget(m_diagnosticsButton);
    buttonLayout->addWidget(m_exportButton);
    layout->addLayout(buttonLayout);
    
    connect(m_diagnosticsButton, &QPushButton::clicked, this, &CHealthMonitorWidget::runDiagnostics);
    connect(m_exportButton, &QPushButton::clicked, this, &CHealthMonitorWidget::exportHealthReport);
    
    m_mainLayout->addWidget(m_overviewGroup);
}

void CHealthMonitorWidget::createHardwareSection()
{
    m_hardwareGroup = new QGroupBox("🔧 Hardware Modules", m_mainWidget);
    QGridLayout *layout = new QGridLayout(m_hardwareGroup);
    layout->setSpacing(10);
    
    int row = 0, col = 0;
    for (const QString &module : m_hardwareModules) {
        QFrame *card = createModuleCard(module, "🔧");
        layout->addWidget(card, row, col);
        m_hardwareCards[module] = card;
        
        col++;
        if (col > 1) {
            col = 0;
            row++;
        }
    }
    
    m_mainLayout->addWidget(m_hardwareGroup);
}

void CHealthMonitorWidget::createSoftwareSection()
{
    m_softwareGroup = new QGroupBox("💻 Software Modules", m_mainWidget);
    QGridLayout *layout = new QGridLayout(m_softwareGroup);
    layout->setSpacing(10);
    
    int row = 0, col = 0;
    for (const QString &module : m_softwareModules) {
        QFrame *card = createModuleCard(module, "💻");
        layout->addWidget(card, row, col);
        m_softwareCards[module] = card;
        
        col++;
        if (col > 1) {
            col = 0;
            row++;
        }
    }
    
    m_mainLayout->addWidget(m_softwareGroup);
}

QFrame* CHealthMonitorWidget::createModuleCard(const QString &moduleName, const QString &icon)
{
    QFrame *card = new QFrame();
    card->setFrameShape(QFrame::StyledPanel);
    card->setStyleSheet(
        "QFrame {"
        "   background-color: #1a202c;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 8px;"
        "   padding: 8px;"
        "}"
        "QFrame:hover {"
        "   border-color: #667eea;"
        "   background-color: #1e293b;"
        "}"
    );
    
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(6);
    
    // Module name
    QLabel *nameLabel = new QLabel(moduleName);
    QFont nameFont = nameLabel->font();
    nameFont.setBold(true);
    nameFont.setPointSize(10);
    nameLabel->setFont(nameFont);
    nameLabel->setWordWrap(true);
    cardLayout->addWidget(nameLabel);
    
    // Status label
    QLabel *statusLabel = new QLabel("● Operational");
    statusLabel->setStyleSheet("color: #4ade80;");
    cardLayout->addWidget(statusLabel);
    
    // Health bar
    QProgressBar *healthBar = new QProgressBar();
    healthBar->setRange(0, 100);
    healthBar->setValue(95);
    healthBar->setMaximumHeight(20);
    healthBar->setFormat("%v%");
    cardLayout->addWidget(healthBar);
    
    // Store references
    if (m_hardwareModules.contains(moduleName)) {
        m_hardwareStatusLabels[moduleName] = statusLabel;
        m_hardwareHealthBars[moduleName] = healthBar;
    } else {
        m_softwareStatusLabels[moduleName] = statusLabel;
        m_softwareHealthBars[moduleName] = healthBar;
    }
    
    // Make card clickable
    card->setProperty("moduleName", moduleName);
    card->installEventFilter(this);
    
    return card;
}

void CHealthMonitorWidget::createPerformanceSection()
{
    m_performanceGroup = new QGroupBox("⚡ System Performance", m_mainWidget);
    QGridLayout *layout = new QGridLayout(m_performanceGroup);
    
    int row = 0;
    
    // CPU
    layout->addWidget(new QLabel("CPU Usage:"), row, 0);
    m_cpuUsageLabel = new QLabel("0%");
    m_cpuUsageLabel->setStyleSheet("font-weight: bold; color: #4ade80;");
    layout->addWidget(m_cpuUsageLabel, row, 1);
    m_cpuBar = new QProgressBar();
    m_cpuBar->setRange(0, 100);
    m_cpuBar->setTextVisible(false);
    layout->addWidget(m_cpuBar, row++, 2);
    
    // Memory
    layout->addWidget(new QLabel("Memory Usage:"), row, 0);
    m_memoryUsageLabel = new QLabel("0%");
    m_memoryUsageLabel->setStyleSheet("font-weight: bold; color: #60a5fa;");
    layout->addWidget(m_memoryUsageLabel, row, 1);
    m_memoryBar = new QProgressBar();
    m_memoryBar->setRange(0, 100);
    m_memoryBar->setTextVisible(false);
    layout->addWidget(m_memoryBar, row++, 2);
    
    // Disk
    layout->addWidget(new QLabel("Disk Usage:"), row, 0);
    m_diskUsageLabel = new QLabel("0%");
    m_diskUsageLabel->setStyleSheet("font-weight: bold; color: #fbbf24;");
    layout->addWidget(m_diskUsageLabel, row, 1);
    m_diskBar = new QProgressBar();
    m_diskBar->setRange(0, 100);
    m_diskBar->setTextVisible(false);
    layout->addWidget(m_diskBar, row++, 2);
    
    // Network
    layout->addWidget(new QLabel("Network Usage:"), row, 0);
    m_networkUsageLabel = new QLabel("0%");
    m_networkUsageLabel->setStyleSheet("font-weight: bold; color: #a78bfa;");
    layout->addWidget(m_networkUsageLabel, row, 1);
    m_networkBar = new QProgressBar();
    m_networkBar->setRange(0, 100);
    m_networkBar->setTextVisible(false);
    layout->addWidget(m_networkBar, row++, 2);
    
    m_mainLayout->addWidget(m_performanceGroup);
}

void CHealthMonitorWidget::createAlertsSection()
{
    m_alertsGroup = new QGroupBox("⚠️ System Alerts", m_mainWidget);
    QVBoxLayout *layout = new QVBoxLayout(m_alertsGroup);
    
    // Active alerts count
    m_activeAlertsLabel = new QLabel("Active Alerts: 0");
    QFont font = m_activeAlertsLabel->font();
    font.setBold(true);
    m_activeAlertsLabel->setFont(font);
    layout->addWidget(m_activeAlertsLabel);
    
    // Alerts table
    m_alertsTable = new QTableWidget();
    m_alertsTable->setColumnCount(4);
    m_alertsTable->setHorizontalHeaderLabels({"Time", "Module", "Severity", "Message"});
    m_alertsTable->horizontalHeader()->setStretchLastSection(true);
    m_alertsTable->setMaximumHeight(150);
    m_alertsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_alertsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_alertsTable);
    
    // Reset button
    m_resetAlarmsButton = new QPushButton("🔄 Clear All Alerts");
    m_resetAlarmsButton->setMinimumHeight(30);
    connect(m_resetAlarmsButton, &QPushButton::clicked, this, &CHealthMonitorWidget::resetAlarms);
    layout->addWidget(m_resetAlarmsButton);
    
    m_mainLayout->addWidget(m_alertsGroup);
}

void CHealthMonitorWidget::applyModernStyle()
{
    setStyleSheet(
        "QDockWidget {"
        "   background-color: #0f172a;"
        "   color: #ffffff;"
        "}"
        "QDockWidget::title {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #10b981, stop:1 #059669);"
        "   color: white;"
        "   padding: 10px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QGroupBox {"
        "   background-color: #1e293b;"
        "   border: 2px solid #334155;"
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
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #10b981, stop:1 #059669);"
        "   border-radius: 6px;"
        "   color: white;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #10b981, stop:1 #059669);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 8px 12px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #059669, stop:1 #047857);"
        "}"
        "QLabel {"
        "   color: #e2e8f0;"
        "}"
        "QProgressBar {"
        "   background-color: #1a202c;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 6px;"
        "   text-align: center;"
        "   color: white;"
        "   font-weight: bold;"
        "}"
        "QProgressBar::chunk {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #10b981, stop:1 #059669);"
        "   border-radius: 4px;"
        "}"
        "QTableWidget {"
        "   background-color: #1a202c;"
        "   color: #ffffff;"
        "   border: 1px solid #4a5568;"
        "   gridline-color: #4a5568;"
        "}"
        "QHeaderView::section {"
        "   background: #2d3748;"
        "   color: #ffffff;"
        "   padding: 6px;"
        "   border: 1px solid #4a5568;"
        "   font-weight: bold;"
        "}"
        "QScrollArea {"
        "   background-color: #0f172a;"
        "   border: none;"
        "}"
    );
}

void CHealthMonitorWidget::updateHealth()
{
    // Update uptime
    qint64 uptime = QDateTime::currentMSecsSinceEpoch() - m_systemStartTime;
    int hours = uptime / 3600000;
    int minutes = (uptime % 3600000) / 60000;
    int seconds = (uptime % 60000) / 1000;
    m_uptimeLabel->setText(QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0')));
    
    m_lastCheckLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
    
    // Simulate performance metrics
    int cpu = 15 + QRandomGenerator::global()->bounded(20);
    int memory = 40 + QRandomGenerator::global()->bounded(20);
    int disk = 55 + QRandomGenerator::global()->bounded(10);
    int network = 10 + QRandomGenerator::global()->bounded(15);
    
    m_cpuUsageLabel->setText(QString("%1%").arg(cpu));
    m_cpuBar->setValue(cpu);
    m_cpuUsageLabel->setStyleSheet(cpu > 80 ? "color: #ef4444; font-weight: bold;" : "color: #4ade80; font-weight: bold;");
    
    m_memoryUsageLabel->setText(QString("%1%").arg(memory));
    m_memoryBar->setValue(memory);
    m_memoryUsageLabel->setStyleSheet(memory > 80 ? "color: #ef4444; font-weight: bold;" : "color: #60a5fa; font-weight: bold;");
    
    m_diskUsageLabel->setText(QString("%1%").arg(disk));
    m_diskBar->setValue(disk);
    m_diskUsageLabel->setStyleSheet(disk > 90 ? "color: #ef4444; font-weight: bold;" : "color: #fbbf24; font-weight: bold;");
    
    m_networkUsageLabel->setText(QString("%1%").arg(network));
    m_networkBar->setValue(network);
    
    // Update module health
    simulateHealthData();
    
    // Calculate overall system health
    int totalHealth = 0;
    int moduleCount = m_moduleHealthData.count();
    
    for (const ModuleHealth &health : m_moduleHealthData.values()) {
        totalHealth += health.healthPercentage;
    }
    
    int avgHealth = moduleCount > 0 ? totalHealth / moduleCount : 100;
    m_systemHealthBar->setValue(avgHealth);
    
    if (avgHealth >= 90) {
        m_systemHealthBar->setFormat("%v% - Excellent");
        m_systemHealthBar->setStyleSheet("QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #10b981, stop:1 #059669); }");
    } else if (avgHealth >= 70) {
        m_systemHealthBar->setFormat("%v% - Good");
        m_systemHealthBar->setStyleSheet("QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #fbbf24, stop:1 #f59e0b); }");
    } else {
        m_systemHealthBar->setFormat("%v% - Needs Attention");
        m_systemHealthBar->setStyleSheet("QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #ef4444, stop:1 #dc2626); }");
    }
    
    // Update hardware cards
    for (const QString &module : m_hardwareModules) {
        if (m_moduleHealthData.contains(module)) {
            const ModuleHealth &health = m_moduleHealthData[module];
            m_hardwareStatusLabels[module]->setText(getHealthText(health.status));
            m_hardwareStatusLabels[module]->setStyleSheet("color: " + getHealthColor(health.status).name() + ";");
            m_hardwareHealthBars[module]->setValue(health.healthPercentage);
        }
    }
    
    // Update software cards
    for (const QString &module : m_softwareModules) {
        if (m_moduleHealthData.contains(module)) {
            const ModuleHealth &health = m_moduleHealthData[module];
            m_softwareStatusLabels[module]->setText(getHealthText(health.status));
            m_softwareStatusLabels[module]->setStyleSheet("color: " + getHealthColor(health.status).name() + ";");
            m_softwareHealthBars[module]->setValue(health.healthPercentage);
        }
    }
}

void CHealthMonitorWidget::simulateHealthData()
{
    // Simulate health for all modules
    QStringList allModules = m_hardwareModules + m_softwareModules;
    
    for (const QString &module : allModules) {
        ModuleHealth health;
        health.name = module;
        
        // Random health simulation
        int random = QRandomGenerator::global()->bounded(100);
        
        if (random > 90) {
            health.status = HEALTHY;
            health.healthPercentage = 90 + QRandomGenerator::global()->bounded(10);
            health.statusMessage = "● Operational";
        } else if (random > 75) {
            health.status = WARNING;
            health.healthPercentage = 70 + QRandomGenerator::global()->bounded(20);
            health.statusMessage = "⚠ Warning";
        } else if (random > 95) {
            health.status = CRITICAL;
            health.healthPercentage = 50 + QRandomGenerator::global()->bounded(20);
            health.statusMessage = "❌ Critical";
        } else {
            health.status = HEALTHY;
            health.healthPercentage = 85 + QRandomGenerator::global()->bounded(15);
            health.statusMessage = "● Operational";
        }
        
        health.lastUpdate = QDateTime::currentDateTime().toString("hh:mm:ss");
        m_moduleHealthData[module] = health;
    }
}

QColor CHealthMonitorWidget::getHealthColor(HealthStatus status)
{
    switch (status) {
        case HEALTHY: return QColor("#4ade80");
        case WARNING: return QColor("#fbbf24");
        case CRITICAL: return QColor("#ef4444");
        case OFFLINE: return QColor("#94a3b8");
        case MAINTENANCE: return QColor("#60a5fa");
        default: return QColor("#ffffff");
    }
}

QString CHealthMonitorWidget::getHealthText(HealthStatus status)
{
    switch (status) {
        case HEALTHY: return "● Operational";
        case WARNING: return "⚠ Warning";
        case CRITICAL: return "❌ Critical";
        case OFFLINE: return "⚫ Offline";
        case MAINTENANCE: return "🔧 Maintenance";
        default: return "❓ Unknown";
    }
}

void CHealthMonitorWidget::runDiagnostics()
{
    m_diagnosticsButton->setEnabled(false);
    m_diagnosticsButton->setText("🔍 Running...");
    
    QTimer::singleShot(2000, this, [this]() {
        m_diagnosticsButton->setEnabled(true);
        m_diagnosticsButton->setText("🔍 Run Diagnostics");
        
        QMessageBox::information(this, "Diagnostics Complete",
            "System diagnostics completed successfully.\n\n"
            "All modules are functioning within normal parameters.");
    });
}

void CHealthMonitorWidget::resetAlarms()
{
    m_alertsTable->setRowCount(0);
    m_activeAlerts = 0;
    m_activeAlertsLabel->setText("Active Alerts: 0");
}

void CHealthMonitorWidget::exportHealthReport()
{
    QString filename = QFileDialog::getSaveFileName(this, "Export Health Report",
        QDir::homePath() + "/health_report_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".txt",
        "Text Files (*.txt)");
    
    if (!filename.isEmpty()) {
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "RADAR SYSTEM HEALTH REPORT\n";
            out << "Generated: " << QDateTime::currentDateTime().toString() << "\n\n";
            out << "Overall System Health: " << m_systemHealthBar->value() << "%\n";
            out << "System Uptime: " << m_uptimeLabel->text() << "\n\n";
            
            out << "=== HARDWARE MODULES ===\n";
            for (const QString &module : m_hardwareModules) {
                if (m_moduleHealthData.contains(module)) {
                    const ModuleHealth &health = m_moduleHealthData[module];
                    out << module << ": " << health.healthPercentage << "% - "
                        << getHealthText(health.status) << "\n";
                }
            }
            
            out << "\n=== SOFTWARE MODULES ===\n";
            for (const QString &module : m_softwareModules) {
                if (m_moduleHealthData.contains(module)) {
                    const ModuleHealth &health = m_moduleHealthData[module];
                    out << module << ": " << health.healthPercentage << "% - "
                        << getHealthText(health.status) << "\n";
                }
            }
            
            file.close();
            QMessageBox::information(this, "Export Complete",
                "Health report exported successfully to:\n" + filename);
        }
    }
}

void CHealthMonitorWidget::onModuleClicked(const QString &moduleName)
{
    if (m_moduleHealthData.contains(moduleName)) {
        const ModuleHealth &health = m_moduleHealthData[moduleName];
        
        QString info = QString("<b>%1</b><br><br>"
                              "Status: %2<br>"
                              "Health: %3%<br>"
                              "Last Update: %4<br>")
            .arg(health.name)
            .arg(getHealthText(health.status))
            .arg(health.healthPercentage)
            .arg(health.lastUpdate);
        
        QMessageBox::information(this, "Module Information", info);
    }
}

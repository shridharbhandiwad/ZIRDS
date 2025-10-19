#include "cpredictivemaintenancewidget.h"
#include <QDateTime>
#include <QDebug>
#include <QRandomGenerator>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>

CPredictiveMaintenanceWidget::CPredictiveMaintenanceWidget(QWidget *parent)
    : QDockWidget("🔮 Predictive Maintenance", parent)
    , m_systemStartTime(QDateTime::currentMSecsSinceEpoch())
{
    setupUI();
    applyModernStyle();
    
    // Initialize maintenance data
    simulateMaintenanceData();
    
    // Start update timer (10 seconds)
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &CPredictiveMaintenanceWidget::updateStatus);
    m_updateTimer->start(10000);
    
    // Initial update
    updateStatus();
}

CPredictiveMaintenanceWidget::~CPredictiveMaintenanceWidget()
{
}

void CPredictiveMaintenanceWidget::setupUI()
{
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    m_mainWidget = new QWidget();
    m_mainLayout = new QVBoxLayout(m_mainWidget);
    m_mainLayout->setSpacing(12);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);
    
    createOverviewSection();
    createComponentsSection();
    createScheduleSection();
    createRecommendationsSection();
    
    m_mainLayout->addStretch();
    scrollArea->setWidget(m_mainWidget);
    setWidget(scrollArea);
}

void CPredictiveMaintenanceWidget::createOverviewSection()
{
    m_overviewGroup = new QGroupBox("📊 Maintenance Overview", m_mainWidget);
    QVBoxLayout *layout = new QVBoxLayout(m_overviewGroup);
    
    // System condition display
    m_systemConditionLabel = new QLabel("Overall System Condition");
    m_systemConditionLabel->setAlignment(Qt::AlignCenter);
    QFont font = m_systemConditionLabel->font();
    font.setPointSize(12);
    font.setBold(true);
    m_systemConditionLabel->setFont(font);
    layout->addWidget(m_systemConditionLabel);
    
    m_systemConditionBar = new QProgressBar();
    m_systemConditionBar->setRange(0, 100);
    m_systemConditionBar->setValue(85);
    m_systemConditionBar->setMinimumHeight(35);
    m_systemConditionBar->setFormat("%v% - Good Condition");
    layout->addWidget(m_systemConditionBar);
    
    // Info grid
    QGridLayout *infoGrid = new QGridLayout();
    
    infoGrid->addWidget(new QLabel("Next Maintenance:"), 0, 0);
    m_upcomingMaintenanceLabel = new QLabel("In 15 days");
    m_upcomingMaintenanceLabel->setStyleSheet("font-weight: bold; color: #fbbf24;");
    infoGrid->addWidget(m_upcomingMaintenanceLabel, 0, 1);
    
    infoGrid->addWidget(new QLabel("Critical Components:"), 1, 0);
    m_criticalComponentsLabel = new QLabel("0");
    m_criticalComponentsLabel->setStyleSheet("font-weight: bold; color: #4ade80;");
    infoGrid->addWidget(m_criticalComponentsLabel, 1, 1);
    
    layout->addLayout(infoGrid);
    
    // Action buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_scheduleButton = new QPushButton("📅 Schedule Maintenance");
    m_historyButton = new QPushButton("📜 View History");
    
    m_scheduleButton->setMinimumHeight(35);
    m_historyButton->setMinimumHeight(35);
    
    buttonLayout->addWidget(m_scheduleButton);
    buttonLayout->addWidget(m_historyButton);
    layout->addLayout(buttonLayout);
    
    connect(m_scheduleButton, &QPushButton::clicked, this, &CPredictiveMaintenanceWidget::scheduleMaintenanceNow);
    connect(m_historyButton, &QPushButton::clicked, this, &CPredictiveMaintenanceWidget::viewMaintenanceHistory);
    
    m_mainLayout->addWidget(m_overviewGroup);
}

void CPredictiveMaintenanceWidget::createComponentsSection()
{
    m_componentsGroup = new QGroupBox("🔧 Component Status", m_mainWidget);
    QVBoxLayout *layout = new QVBoxLayout(m_componentsGroup);
    
    QLabel *infoLabel = new QLabel("Click on a component for detailed information");
    infoLabel->setStyleSheet("color: #94a3b8; font-style: italic;");
    layout->addWidget(infoLabel);
    
    m_componentsList = new QListWidget();
    m_componentsList->setMinimumHeight(300);
    layout->addWidget(m_componentsList);
    
    connect(m_componentsList, &QListWidget::itemClicked,
            this, &CPredictiveMaintenanceWidget::onComponentClicked);
    
    m_mainLayout->addWidget(m_componentsGroup);
}

void CPredictiveMaintenanceWidget::createScheduleSection()
{
    m_scheduleGroup = new QGroupBox("📅 Upcoming Maintenance Schedule", m_mainWidget);
    QVBoxLayout *layout = new QVBoxLayout(m_scheduleGroup);
    
    m_scheduleTable = new QTableWidget();
    m_scheduleTable->setColumnCount(4);
    m_scheduleTable->setHorizontalHeaderLabels({"Component", "Days Until", "Type", "Priority"});
    m_scheduleTable->horizontalHeader()->setStretchLastSection(true);
    m_scheduleTable->setMaximumHeight(200);
    m_scheduleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_scheduleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_scheduleTable);
    
    m_mainLayout->addWidget(m_scheduleGroup);
}

void CPredictiveMaintenanceWidget::createRecommendationsSection()
{
    m_recommendationsGroup = new QGroupBox("💡 Maintenance Recommendations", m_mainWidget);
    QVBoxLayout *layout = new QVBoxLayout(m_recommendationsGroup);
    
    m_recommendationsText = new QLabel();
    m_recommendationsText->setWordWrap(true);
    m_recommendationsText->setTextFormat(Qt::RichText);
    m_recommendationsText->setMinimumHeight(100);
    m_recommendationsText->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    layout->addWidget(m_recommendationsText);
    
    m_exportButton = new QPushButton("📄 Export Maintenance Report");
    m_exportButton->setMinimumHeight(35);
    connect(m_exportButton, &QPushButton::clicked, this, &CPredictiveMaintenanceWidget::exportMaintenanceReport);
    layout->addWidget(m_exportButton);
    
    m_mainLayout->addWidget(m_recommendationsGroup);
}

void CPredictiveMaintenanceWidget::applyModernStyle()
{
    setStyleSheet(
        "QDockWidget {"
        "   background-color: #0f172a;"
        "   color: #ffffff;"
        "}"
        "QDockWidget::title {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #8b5cf6, stop:1 #6d28d9);"
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
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #8b5cf6, stop:1 #6d28d9);"
        "   border-radius: 6px;"
        "   color: white;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #8b5cf6, stop:1 #6d28d9);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 8px 12px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #7c3aed, stop:1 #5b21b6);"
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
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #8b5cf6, stop:1 #6d28d9);"
        "   border-radius: 4px;"
        "}"
        "QListWidget {"
        "   background-color: #1a202c;"
        "   color: #ffffff;"
        "   border: 1px solid #4a5568;"
        "}"
        "QListWidget::item {"
        "   padding: 10px;"
        "   border-bottom: 1px solid #4a5568;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #8b5cf6;"
        "   color: white;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #374151;"
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

void CPredictiveMaintenanceWidget::simulateMaintenanceData()
{
    m_componentData.clear();
    
    // Define components with different maintenance needs
    QStringList components = {
        "Radar Antenna Array", "RF Transmitter", "RF Receiver", "Servo Motor Azimuth",
        "Servo Motor Elevation", "Power Supply Unit", "Cooling System", "Signal Processor",
        "Data Storage Array", "Network Interface", "Control Computer", "Backup Battery"
    };
    
    QStringList categories = {"RF Equipment", "Mechanical", "Power", "Processing", "Storage", "Electrical"};
    
    for (const QString &component : components) {
        ComponentStatus status;
        status.name = component;
        status.category = categories[QRandomGenerator::global()->bounded(categories.size())];
        
        // Simulate lifetime and maintenance needs
        status.lifetimePercentage = 60 + QRandomGenerator::global()->bounded(40);
        status.operatingHours = 1000 + QRandomGenerator::global()->bounded(5000);
        
        // Calculate days until maintenance based on lifetime
        if (status.lifetimePercentage > 90) {
            status.daysUntilMaintenance = 90 + QRandomGenerator::global()->bounded(180);
            status.status = EXCELLENT;
        } else if (status.lifetimePercentage > 75) {
            status.daysUntilMaintenance = 60 + QRandomGenerator::global()->bounded(90);
            status.status = GOOD;
        } else if (status.lifetimePercentage > 60) {
            status.daysUntilMaintenance = 30 + QRandomGenerator::global()->bounded(60);
            status.status = FAIR;
        } else if (status.lifetimePercentage > 40) {
            status.daysUntilMaintenance = 7 + QRandomGenerator::global()->bounded(30);
            status.status = POOR;
        } else if (status.lifetimePercentage > 20) {
            status.daysUntilMaintenance = 1 + QRandomGenerator::global()->bounded(7);
            status.status = SERVICE_REQUIRED;
        } else {
            status.daysUntilMaintenance = 0;
            status.status = CRITICAL;
        }
        
        // Last maintenance date (random in past 180 days)
        int daysAgo = QRandomGenerator::global()->bounded(180);
        status.lastMaintenanceDate = QDateTime::currentDateTime().addDays(-daysAgo).toSecsSinceEpoch();
        
        // Recommendations
        switch (status.status) {
            case EXCELLENT:
                status.recommendations = "Continue normal operation. No immediate action required.";
                break;
            case GOOD:
                status.recommendations = "Schedule routine inspection within the next quarter.";
                break;
            case FAIR:
                status.recommendations = "Plan maintenance within the next month. Monitor performance closely.";
                break;
            case POOR:
                status.recommendations = "Maintenance required soon. Order replacement parts if needed.";
                break;
            case SERVICE_REQUIRED:
                status.recommendations = "⚠️ Schedule immediate maintenance. Component showing signs of wear.";
                break;
            case CRITICAL:
                status.recommendations = "❌ URGENT: Immediate service required! Component may fail soon.";
                break;
        }
        
        m_componentData[component] = status;
    }
}

void CPredictiveMaintenanceWidget::updateStatus()
{
    // Update component list
    m_componentsList->clear();
    
    // Sort components by days until maintenance
    QList<ComponentStatus> sortedComponents = m_componentData.values();
    std::sort(sortedComponents.begin(), sortedComponents.end(),
              [](const ComponentStatus &a, const ComponentStatus &b) {
                  return a.daysUntilMaintenance < b.daysUntilMaintenance;
              });
    
    int criticalCount = 0;
    int minDays = 999;
    
    for (const ComponentStatus &component : sortedComponents) {
        QListWidgetItem *item = createComponentItem(component);
        m_componentsList->addItem(item);
        
        if (component.status == CRITICAL || component.status == SERVICE_REQUIRED) {
            criticalCount++;
        }
        
        if (component.daysUntilMaintenance < minDays) {
            minDays = component.daysUntilMaintenance;
        }
    }
    
    // Update overview
    m_criticalComponentsLabel->setText(QString::number(criticalCount));
    if (criticalCount > 0) {
        m_criticalComponentsLabel->setStyleSheet("font-weight: bold; color: #ef4444;");
    } else {
        m_criticalComponentsLabel->setStyleSheet("font-weight: bold; color: #4ade80;");
    }
    
    if (minDays < 7) {
        m_upcomingMaintenanceLabel->setText(QString("In %1 days - URGENT").arg(minDays));
        m_upcomingMaintenanceLabel->setStyleSheet("font-weight: bold; color: #ef4444;");
    } else if (minDays < 30) {
        m_upcomingMaintenanceLabel->setText(QString("In %1 days").arg(minDays));
        m_upcomingMaintenanceLabel->setStyleSheet("font-weight: bold; color: #fbbf24;");
    } else {
        m_upcomingMaintenanceLabel->setText(QString("In %1 days").arg(minDays));
        m_upcomingMaintenanceLabel->setStyleSheet("font-weight: bold; color: #4ade80;");
    }
    
    // Calculate overall condition
    int totalCondition = 0;
    for (const ComponentStatus &component : m_componentData.values()) {
        totalCondition += component.lifetimePercentage;
    }
    int avgCondition = totalCondition / m_componentData.count();
    
    m_systemConditionBar->setValue(avgCondition);
    if (avgCondition >= 85) {
        m_systemConditionBar->setFormat("%v% - Excellent Condition");
    } else if (avgCondition >= 70) {
        m_systemConditionBar->setFormat("%v% - Good Condition");
    } else if (avgCondition >= 50) {
        m_systemConditionBar->setFormat("%v% - Fair Condition");
    } else {
        m_systemConditionBar->setFormat("%v% - Poor Condition");
    }
    
    // Update schedule table
    m_scheduleTable->setRowCount(0);
    int row = 0;
    
    for (const ComponentStatus &component : sortedComponents) {
        if (component.daysUntilMaintenance < 60) { // Show next 60 days
            m_scheduleTable->insertRow(row);
            
            m_scheduleTable->setItem(row, 0, new QTableWidgetItem(component.name));
            m_scheduleTable->setItem(row, 1, new QTableWidgetItem(QString::number(component.daysUntilMaintenance)));
            
            QString type = component.daysUntilMaintenance < 7 ? "Emergency" : 
                          component.daysUntilMaintenance < 30 ? "Preventive" : "Routine";
            m_scheduleTable->setItem(row, 2, new QTableWidgetItem(type));
            
            QString priority = component.status == CRITICAL ? "🔴 Critical" :
                              component.status == SERVICE_REQUIRED ? "🟠 High" :
                              component.status == POOR ? "🟡 Medium" : "🟢 Low";
            m_scheduleTable->setItem(row, 3, new QTableWidgetItem(priority));
            
            row++;
        }
    }
    
    // Update recommendations
    QString recommendations = "<b>Key Recommendations:</b><br><br>";
    
    if (criticalCount > 0) {
        recommendations += "🔴 <b>URGENT:</b> " + QString::number(criticalCount) + 
                          " component(s) require immediate attention.<br><br>";
    }
    
    recommendations += "• Regular preventive maintenance scheduled for next month<br>";
    recommendations += "• Thermal system showing optimal performance<br>";
    recommendations += "• All safety systems operational<br>";
    recommendations += "• Data backup systems functioning normally<br>";
    
    m_recommendationsText->setText(recommendations);
}

QListWidgetItem* CPredictiveMaintenanceWidget::createComponentItem(const ComponentStatus &component)
{
    QString statusIcon = getStatusIcon(component.status);
    QString text = QString("%1 <b>%2</b>\n"
                          "Condition: %3% | Maintenance in %4 days\n"
                          "Category: %5")
        .arg(statusIcon)
        .arg(component.name)
        .arg(component.lifetimePercentage)
        .arg(component.daysUntilMaintenance)
        .arg(component.category);
    
    QListWidgetItem *item = new QListWidgetItem(text);
    item->setData(Qt::UserRole, component.name);
    
    // Set text color based on status
    QColor color = getMaintenanceColor(component.status);
    item->setForeground(QBrush(color));
    
    return item;
}

QColor CPredictiveMaintenanceWidget::getMaintenanceColor(MaintenanceStatus status)
{
    switch (status) {
        case EXCELLENT: return QColor("#4ade80");
        case GOOD: return QColor("#60a5fa");
        case FAIR: return QColor("#fbbf24");
        case POOR: return QColor("#fb923c");
        case SERVICE_REQUIRED: return QColor("#f97316");
        case CRITICAL: return QColor("#ef4444");
        default: return QColor("#ffffff");
    }
}

QString CPredictiveMaintenanceWidget::getMaintenanceText(MaintenanceStatus status)
{
    switch (status) {
        case EXCELLENT: return "Excellent";
        case GOOD: return "Good";
        case FAIR: return "Fair";
        case POOR: return "Poor";
        case SERVICE_REQUIRED: return "Service Required";
        case CRITICAL: return "Critical";
        default: return "Unknown";
    }
}

QString CPredictiveMaintenanceWidget::getStatusIcon(MaintenanceStatus status)
{
    switch (status) {
        case EXCELLENT: return "✅";
        case GOOD: return "🟢";
        case FAIR: return "🟡";
        case POOR: return "🟠";
        case SERVICE_REQUIRED: return "⚠️";
        case CRITICAL: return "🔴";
        default: return "❓";
    }
}

void CPredictiveMaintenanceWidget::scheduleMaintenanceNow()
{
    QMessageBox::information(this, "Schedule Maintenance",
        "Maintenance scheduling interface.\n\n"
        "This would typically integrate with your maintenance management system "
        "to schedule technician visits and order parts.");
}

void CPredictiveMaintenanceWidget::viewMaintenanceHistory()
{
    QString history = "MAINTENANCE HISTORY\n\n";
    
    for (const ComponentStatus &component : m_componentData.values()) {
        QDateTime lastMaint = QDateTime::fromSecsSinceEpoch(component.lastMaintenanceDate);
        history += QString("%1:\n  Last serviced: %2\n  Operating hours: %3\n\n")
            .arg(component.name)
            .arg(lastMaint.toString("yyyy-MM-dd"))
            .arg(component.operatingHours);
    }
    
    QMessageBox::information(this, "Maintenance History", history);
}

void CPredictiveMaintenanceWidget::onComponentClicked(QListWidgetItem *item)
{
    QString componentName = item->data(Qt::UserRole).toString();
    
    if (m_componentData.contains(componentName)) {
        const ComponentStatus &component = m_componentData[componentName];
        
        QDateTime lastMaint = QDateTime::fromSecsSinceEpoch(component.lastMaintenanceDate);
        
        QString info = QString("<b>%1</b><br><br>"
                              "<b>Category:</b> %2<br>"
                              "<b>Status:</b> %3<br>"
                              "<b>Condition:</b> %4%<br>"
                              "<b>Operating Hours:</b> %5<br>"
                              "<b>Days Until Maintenance:</b> %6<br>"
                              "<b>Last Maintenance:</b> %7<br><br>"
                              "<b>Recommendations:</b><br>%8")
            .arg(component.name)
            .arg(component.category)
            .arg(getMaintenanceText(component.status))
            .arg(component.lifetimePercentage)
            .arg(component.operatingHours)
            .arg(component.daysUntilMaintenance)
            .arg(lastMaint.toString("yyyy-MM-dd"))
            .arg(component.recommendations);
        
        QMessageBox::information(this, "Component Details", info);
    }
}

void CPredictiveMaintenanceWidget::exportMaintenanceReport()
{
    QString filename = QFileDialog::getSaveFileName(this, "Export Maintenance Report",
        QDir::homePath() + "/maintenance_report_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".txt",
        "Text Files (*.txt)");
    
    if (!filename.isEmpty()) {
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "PREDICTIVE MAINTENANCE REPORT\n";
            out << "Generated: " << QDateTime::currentDateTime().toString() << "\n\n";
            out << "Overall System Condition: " << m_systemConditionBar->value() << "%\n";
            out << "Critical Components: " << m_criticalComponentsLabel->text() << "\n\n";
            
            out << "=== COMPONENT STATUS ===\n";
            for (const ComponentStatus &component : m_componentData.values()) {
                out << component.name << ":\n";
                out << "  Status: " << getMaintenanceText(component.status) << "\n";
                out << "  Condition: " << component.lifetimePercentage << "%\n";
                out << "  Days Until Maintenance: " << component.daysUntilMaintenance << "\n";
                out << "  Operating Hours: " << component.operatingHours << "\n";
                out << "  Recommendations: " << component.recommendations << "\n\n";
            }
            
            file.close();
            QMessageBox::information(this, "Export Complete",
                "Maintenance report exported successfully to:\n" + filename);
        }
    }
}

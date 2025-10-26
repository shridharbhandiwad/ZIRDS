#include "ccontrolswindow.h"
#include "MapDisplay/cconfigpanelwidget.h"
#include "MapDisplay/cchartswidget.h"
#include "MapDisplay/cinterfacespanelwidget.h"
#include "MapDisplay/canalyticswidget.h"
#include "MapDisplay/csimulationwidget.h"
#include "MapDisplay/crecordingwidget.h"
#include "MapDisplay/chealthmonitorwidget.h"
#include "MapDisplay/cpredictivemaintenancewidget.h"
#include "cdatawarehouse.h"
#include <QApplication>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QStatusBar>
#include <QIcon>
#include <QDebug>

CControlsWindow::CControlsWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_tabWidget(nullptr)
    , m_configPanel(nullptr)
    , m_chartsWidget(nullptr)
    , m_interfacesPanel(nullptr)
    , m_analyticsWidget(nullptr)
    , m_simulationWidget(nullptr)
    , m_recordingWidget(nullptr)
    , m_healthMonitorWidget(nullptr)
    , m_predictiveMaintenanceWidget(nullptr)
    , m_statusTimer(new QTimer(this))
    , m_settings(new QSettings("RadarDisplay", "ControlsWindow", this))
{
    setWindowTitle("🎛️ ZIRDS - Control Center | © Zoppler Systems");
    setWindowIcon(QIcon(":/images/resources/zoppler_logo.png"));
    setMinimumSize(900, 650);
    resize(950, 700); // Set initial size

    setupUI();
    applyLightTheme();
    connectSignals();

    // Setup status timer
    connect(m_statusTimer, &QTimer::timeout, this, &CControlsWindow::updateStatusBar);
    m_statusTimer->start(2000);

    loadSettings();
}

CControlsWindow::~CControlsWindow()
{
    saveSettings();
}

void CControlsWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(6, 6, 6, 6);
    m_mainLayout->setSpacing(6);

    setupTabs();

    // Setup status bar
    statusBar()->setStyleSheet(
        "QStatusBar {"
        "   background-color: #ffffff;"
        "   color: #000000;"
        "   border-top: 2px solid #000000;"
        "   font-size: 12px;"
        "   font-weight: 400;"
        "   padding: 6px;"
        "   letter-spacing: 0.2px;"
        "}"
    );
}

void CControlsWindow::setupTabs()
{
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::North);
    m_tabWidget->setMovable(true);
    m_tabWidget->setTabsClosable(false);

    // Create all widgets
    m_configPanel = new CConfigPanelWidget(this);
    m_interfacesPanel = new CInterfacesPanelWidget(this);
    m_simulationWidget = new CSimulationWidget(this);
    m_recordingWidget = new CRecordingWidget(this);
    m_analyticsWidget = new CAnalyticsWidget(this);
    m_chartsWidget = new CChartsWidget(this);
    m_healthMonitorWidget = new CHealthMonitorWidget(this);
    m_predictiveMaintenanceWidget = new CPredictiveMaintenanceWidget(this);

    // Add tabs with icons and rich descriptions
    m_tabWidget->addTab(m_configPanel, "⚙️ Configuration");
    m_tabWidget->addTab(m_interfacesPanel, "🔌 Interfaces");
    m_tabWidget->addTab(m_simulationWidget, "🎮 Simulation");
    m_tabWidget->addTab(m_recordingWidget, "📹 Recording");
    m_tabWidget->addTab(m_analyticsWidget, "📊 Analytics");
    m_tabWidget->addTab(m_chartsWidget, "📈 Charts");
    m_tabWidget->addTab(m_healthMonitorWidget, "💚 Health");
    m_tabWidget->addTab(m_predictiveMaintenanceWidget, "🔧 Maintenance");

    // Set tab tooltips
    m_tabWidget->setTabToolTip(TAB_CONFIG, "System configuration and map controls");
    m_tabWidget->setTabToolTip(TAB_INTERFACES, "Hardware interfaces and servo controls");
    m_tabWidget->setTabToolTip(TAB_SIMULATION, "Track simulation and scenario generation");
    m_tabWidget->setTabToolTip(TAB_RECORDING, "Data recording and replay functionality");
    m_tabWidget->setTabToolTip(TAB_ANALYTICS, "Track analytics and statistics");
    m_tabWidget->setTabToolTip(TAB_CHARTS, "Real-time charts and graphs");
    m_tabWidget->setTabToolTip(TAB_HEALTH, "System health monitoring");
    m_tabWidget->setTabToolTip(TAB_MAINTENANCE, "Predictive maintenance and diagnostics");

    // Connect tab change signal
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &CControlsWindow::onTabChanged);

    m_mainLayout->addWidget(m_tabWidget);
}

void CControlsWindow::applyLightTheme()
{
    // Pure white theme with black text
    QString lightTheme =
        "CControlsWindow {"
        "   background-color: #ffffff;"
        "   color: #000000;"
        "}"
        "QWidget {"
        "   background-color: #ffffff;"
        "   color: #000000;"
        "   font-family: 'Poppins', 'Inter', 'Roboto', 'Segoe UI', system-ui, sans-serif;"
        "   font-weight: 400;"
        "   letter-spacing: 0.3px;"
        "}"
        "QTabWidget {"
        "   background-color: #ffffff;"
        "   border: 2px solid #000000;"
        "   border-radius: 8px;"
        "}"
        "QTabWidget::pane {"
        "   background-color: #ffffff;"
        "   border: 2px solid #000000;"
        "   border-radius: 8px;"
        "   margin-top: -2px;"
        "}"
        "QTabBar {"
        "   background-color: #ffffff;"
        "}"
        "QTabBar::tab {"
        "   background-color: #ffffff;"
        "   color: #000000;"
        "   border: 2px solid #000000;"
        "   border-bottom: none;"
        "   border-top-left-radius: 6px;"
        "   border-top-right-radius: 6px;"
        "   padding: 8px 16px;"
        "   margin-right: 2px;"
        "   font-weight: 500;"
        "   font-size: 13px;"
        "   min-width: 100px;"
        "   letter-spacing: 0.4px;"
        "}"
        "QTabBar::tab:selected {"
        "   background-color: #e0e0e0;"
        "   color: #000000;"
        "   border-color: #000000;"
        "}"
        "QTabBar::tab:hover:!selected {"
        "   background-color: #f0f0f0;"
        "   color: #000000;"
        "}"
        "QPushButton {"
        "   background-color: #ffffff;"
        "   color: #000000;"
        "   border: 2px solid #000000;"
        "   border-radius: 6px;"
        "   padding: 6px 14px;"
        "   font-weight: 500;"
        "   font-size: 12px;"
        "   min-height: 24px;"
        "   letter-spacing: 0.3px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #f0f0f0;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #e0e0e0;"
        "}"
        "QPushButton:checked {"
        "   background-color: #d0d0d0;"
        "   border: 2px solid #000000;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #f5f5f5;"
        "   color: #999999;"
        "   border-color: #cccccc;"
        "}"
        "QGroupBox {"
        "   background-color: #ffffff;"
        "   border: 2px solid #000000;"
        "   border-radius: 6px;"
        "   margin-top: 10px;"
        "   padding-top: 10px;"
        "   font-weight: 500;"
        "   color: #000000;"
        "   font-size: 12px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top left;"
        "   padding: 3px 10px;"
        "   background-color: #ffffff;"
        "   border: 1px solid #000000;"
        "   border-radius: 3px;"
        "   color: #000000;"
        "   font-weight: 500;"
        "   font-size: 12px;"
        "   letter-spacing: 0.3px;"
        "}"
        "QLabel {"
        "   color: #000000;"
        "   font-weight: 400;"
        "   letter-spacing: 0.2px;"
        "}"
        "QLineEdit {"
        "   background-color: #ffffff;"
        "   border: 2px solid #000000;"
        "   border-radius: 4px;"
        "   padding: 6px 10px;"
        "   font-size: 12px;"
        "   font-weight: 400;"
        "   color: #000000;"
        "   letter-spacing: 0.2px;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #000000;"
        "}"
        "QComboBox {"
        "   background-color: #ffffff;"
        "   border: 2px solid #000000;"
        "   border-radius: 4px;"
        "   padding: 6px 10px;"
        "   font-size: 12px;"
        "   font-weight: 400;"
        "   color: #000000;"
        "   letter-spacing: 0.2px;"
        "}"
        "QComboBox:focus {"
        "   border-color: #000000;"
        "}"
        "QSlider::groove:horizontal {"
        "   background-color: #e0e0e0;"
        "   height: 6px;"
        "   border-radius: 3px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background-color: #ffffff;"
        "   border: 2px solid #000000;"
        "   width: 18px;"
        "   margin: -6px 0;"
        "   border-radius: 9px;"
        "}"
        "QSlider::handle:horizontal:hover {"
        "   background-color: #f0f0f0;"
        "}";

    setStyleSheet(lightTheme);
}

void CControlsWindow::connectSignals()
{
    // Connect config panel signals
    connect(m_configPanel, &CConfigPanelWidget::mapHomeRequested,
            this, &CControlsWindow::mapHomeRequested);
    connect(m_configPanel, &CConfigPanelWidget::openMapsRequested,
            this, &CControlsWindow::openMapsRequested);
    connect(m_configPanel, &CConfigPanelWidget::flushRequested,
            this, &CControlsWindow::flushRequested);
    connect(m_configPanel, &CConfigPanelWidget::exitRequested,
            this, &CControlsWindow::exitRequested);
    connect(m_configPanel, &CConfigPanelWidget::zoomLevelChanged,
            this, &CControlsWindow::zoomChanged);
    connect(m_configPanel, &CConfigPanelWidget::opacityChanged,
            this, &CControlsWindow::opacityChanged);
    connect(m_configPanel, &CConfigPanelWidget::trackSizeChanged,
            this, &CControlsWindow::trackSizeChanged);
    connect(m_configPanel, &CConfigPanelWidget::trackFilterChanged,
            this, &CControlsWindow::trackFilterChanged);
    connect(m_configPanel, &CConfigPanelWidget::animationSpeedChanged,
            this, &CControlsWindow::animationSpeedChanged);
    connect(m_configPanel, &CConfigPanelWidget::gridVisibilityChanged,
            this, &CControlsWindow::gridVisibilityChanged);
    connect(m_configPanel, &CConfigPanelWidget::compassVisibilityChanged,
            this, &CControlsWindow::compassVisibilityChanged);
    connect(m_configPanel, &CConfigPanelWidget::chartsRequested,
            this, &CControlsWindow::chartsRequested);
    connect(m_configPanel, &CConfigPanelWidget::historyLimitChanged,
            this, &CControlsWindow::historyLimitChanged);

    // Connect history limit changes directly to data warehouse
    connect(m_configPanel, &CConfigPanelWidget::historyLimitChanged,
            CDataWarehouse::getInstance(), &CDataWarehouse::setHistoryLimit);

    // Connect simulation widget to data warehouse
    connect(m_simulationWidget, &CSimulationWidget::simulatedTrackData,
            CDataWarehouse::getInstance(), &CDataWarehouse::slotUpdateTrackData);

    // Connect recording widget to data warehouse
    connect(m_recordingWidget, &CRecordingWidget::replayTrackData,
            CDataWarehouse::getInstance(), &CDataWarehouse::slotUpdateTrackData);

    // Connect interfaces panel debug signals
    connect(m_interfacesPanel, &CInterfacesPanelWidget::servoAzimuthChanged,
            this, [](double az) { qDebug() << "Servo azimuth:" << az; });
    connect(m_interfacesPanel, &CInterfacesPanelWidget::servoElevationChanged,
            this, [](double el) { qDebug() << "Servo elevation:" << el; });
    connect(m_interfacesPanel, &CInterfacesPanelWidget::radarScanStartRequested,
            this, []() { qDebug() << "Radar scan start requested"; });
    connect(m_interfacesPanel, &CInterfacesPanelWidget::gunFireRequested,
            this, []() { qDebug() << "Gun fire requested!"; });
    connect(m_interfacesPanel, &CInterfacesPanelWidget::missileLaunchRequested,
            this, []() { qDebug() << "Missile launch requested!"; });
}

void CControlsWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_1:
        m_tabWidget->setCurrentIndex(TAB_CONFIG);
        break;
    case Qt::Key_2:
        m_tabWidget->setCurrentIndex(TAB_INTERFACES);
        break;
    case Qt::Key_3:
        m_tabWidget->setCurrentIndex(TAB_SIMULATION);
        break;
    case Qt::Key_4:
        m_tabWidget->setCurrentIndex(TAB_RECORDING);
        break;
    case Qt::Key_5:
        m_tabWidget->setCurrentIndex(TAB_ANALYTICS);
        break;
    case Qt::Key_6:
        m_tabWidget->setCurrentIndex(TAB_CHARTS);
        break;
    case Qt::Key_7:
        m_tabWidget->setCurrentIndex(TAB_HEALTH);
        break;
    case Qt::Key_8:
        m_tabWidget->setCurrentIndex(TAB_MAINTENANCE);
        break;
    case Qt::Key_Tab:
        // Cycle through tabs
        if (event->modifiers() & Qt::ControlModifier) {
            int nextIndex = (m_tabWidget->currentIndex() + 1) % m_tabWidget->count();
            m_tabWidget->setCurrentIndex(nextIndex);
        }
        break;
    }
    QMainWindow::keyPressEvent(event);
}

void CControlsWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

void CControlsWindow::onTabChanged(int index)
{
    QString tabName = m_tabWidget->tabText(index);
    statusBar()->showMessage(QString("Active Tab: %1").arg(tabName), 3000);

    qDebug() << "Tab changed to:" << tabName << "(" << index << ")";
}

void CControlsWindow::updateStatusBar()
{
    QList<stTrackDisplayInfo> tracks = CDataWarehouse::getInstance()->getTrackList();
    QString currentTab = m_tabWidget->tabText(m_tabWidget->currentIndex());

    QString statusMsg = QString("🎛️ Control Center | Active Tab: %1 | Tracks: %2 | Keys: 1-8 for tabs, Ctrl+Tab to cycle")
        .arg(currentTab)
        .arg(tracks.count());

    statusBar()->showMessage(statusMsg);
}

void CControlsWindow::saveSettings()
{
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("windowState", saveState());
    m_settings->setValue("currentTab", m_tabWidget->currentIndex());
}

void CControlsWindow::loadSettings()
{
    restoreGeometry(m_settings->value("geometry").toByteArray());
    restoreState(m_settings->value("windowState").toByteArray());

    int currentTab = m_settings->value("currentTab", 0).toInt();
    if (currentTab >= 0 && currentTab < m_tabWidget->count()) {
        m_tabWidget->setCurrentIndex(currentTab);
    }
}

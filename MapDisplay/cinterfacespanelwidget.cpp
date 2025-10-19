#include "cinterfacespanelwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QSpacerItem>
#include <QDateTime>

CInterfacesPanelWidget::CInterfacesPanelWidget(QWidget *parent)
    : QDockWidget("⚡ Interfaces", parent)
{
    setupUI();
    applyRichStyle();
}

CInterfacesPanelWidget::~CInterfacesPanelWidget()
{
}

void CInterfacesPanelWidget::setupUI()
{
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::West);
    
    createServoTab();
    createRadarTab();
    createGunTab();
    createMissileTab();
    createEOTSTab();
    createLoggingTab();
    
    mainLayout->addWidget(m_tabWidget);
    setWidget(mainWidget);
}

QPushButton* CInterfacesPanelWidget::createStyledButton(const QString &text, const QString &icon, const QColor &color)
{
    QPushButton *btn = new QPushButton(icon + " " + text);
    btn->setMinimumHeight(45);
    btn->setCursor(Qt::PointingHandCursor);
    
    QString colorHex = color.name();
    QString hoverColor = color.lighter(115).name();
    QString pressColor = color.darker(115).name();
    
    btn->setStyleSheet(QString(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:1 %2);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 10px;"
        "   padding: 12px 20px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "   text-align: left;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %3, stop:1 %1);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %4, stop:1 %2);"
        "}"
        "QPushButton:disabled {"
        "   background-color: #4a5568;"
        "   color: #a0aec0;"
        "}"
    ).arg(colorHex).arg(color.darker(110).name()).arg(hoverColor).arg(pressColor));
    
    return btn;
}

QGroupBox* CInterfacesPanelWidget::createStyledGroup(const QString &title)
{
    QGroupBox *group = new QGroupBox(title);
    group->setStyleSheet(
        "QGroupBox {"
        "   background-color: #2d3748;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 12px;"
        "   margin-top: 16px;"
        "   padding: 16px;"
        "   font-weight: bold;"
        "   color: #ffffff;"
        "   font-size: 13px;"
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
    );
    return group;
}

QString CInterfacesPanelWidget::getSliderStyle()
{
    return QString(
        "QSlider::groove:horizontal {"
        "   background: #1a202c;"
        "   height: 8px;"
        "   border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #667eea, stop:1 #764ba2);"
        "   width: 18px;"
        "   height: 18px;"
        "   margin: -5px 0;"
        "   border-radius: 9px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2);"
        "   border-radius: 4px;"
        "}"
    );
}

QString CInterfacesPanelWidget::getCheckboxStyle(const QString &color)
{
    return QString(
        "QCheckBox {"
        "   color: #ffffff;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "   spacing: 8px;"
        "}"
        "QCheckBox::indicator {"
        "   width: 20px;"
        "   height: 20px;"
        "   border-radius: 4px;"
        "   border: 2px solid #4a5568;"
        "   background-color: #1a202c;"
        "}"
        "QCheckBox::indicator:checked {"
        "   background-color: %1;"
        "   border: 2px solid %1;"
        "}"
        "QCheckBox::indicator:hover {"
        "   border: 2px solid %1;"
        "}"
    ).arg(color);
}

void CInterfacesPanelWidget::createServoTab()
{
    QWidget *servoTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(servoTab);
    layout->setSpacing(12);
    layout->setContentsMargins(12, 12, 12, 12);
    
    // Position Control Group
    QGroupBox *posGroup = createStyledGroup("Position Control");
    QGridLayout *posLayout = new QGridLayout(posGroup);
    posLayout->setSpacing(10);
    
    QLabel *lblAz = new QLabel("Azimuth (°):");
    lblAz->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    posLayout->addWidget(lblAz, 0, 0);
    
    m_spinServoAz = new QDoubleSpinBox();
    m_spinServoAz->setRange(-180.0, 180.0);
    m_spinServoAz->setValue(0.0);
    m_spinServoAz->setDecimals(2);
    m_spinServoAz->setStyleSheet(
        "QDoubleSpinBox { background-color: #1a202c; color: #ffffff; border: 2px solid #4a5568; "
        "border-radius: 6px; padding: 8px; font-size: 12px; }"
        "QDoubleSpinBox:focus { border: 2px solid #667eea; }"
    );
    connect(m_spinServoAz, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &CInterfacesPanelWidget::servoAzimuthChanged);
    posLayout->addWidget(m_spinServoAz, 0, 1);
    
    m_lblServoAzPos = new QLabel("Current: 0.00°");
    m_lblServoAzPos->setStyleSheet("color: #48bb78; font-size: 11px; font-weight: bold;");
    posLayout->addWidget(m_lblServoAzPos, 0, 2);
    
    QLabel *lblEl = new QLabel("Elevation (°):");
    lblEl->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    posLayout->addWidget(lblEl, 1, 0);
    
    m_spinServoEl = new QDoubleSpinBox();
    m_spinServoEl->setRange(-90.0, 90.0);
    m_spinServoEl->setValue(0.0);
    m_spinServoEl->setDecimals(2);
    m_spinServoEl->setStyleSheet(m_spinServoAz->styleSheet());
    connect(m_spinServoEl, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &CInterfacesPanelWidget::servoElevationChanged);
    posLayout->addWidget(m_spinServoEl, 1, 1);
    
    m_lblServoElPos = new QLabel("Current: 0.00°");
    m_lblServoElPos->setStyleSheet("color: #48bb78; font-size: 11px; font-weight: bold;");
    posLayout->addWidget(m_lblServoElPos, 1, 2);
    
    layout->addWidget(posGroup);
    
    // Speed Control Group
    QGroupBox *speedGroup = createStyledGroup("Movement Speed");
    QGridLayout *speedLayout = new QGridLayout(speedGroup);
    speedLayout->setSpacing(10);
    
    QLabel *lblSpeed = new QLabel("Speed:");
    lblSpeed->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    speedLayout->addWidget(lblSpeed, 0, 0);
    
    m_sliderServoSpeed = new QSlider(Qt::Horizontal);
    m_sliderServoSpeed->setRange(1, 100);
    m_sliderServoSpeed->setValue(50);
    m_sliderServoSpeed->setStyleSheet(getSliderStyle());
    speedLayout->addWidget(m_sliderServoSpeed, 0, 1);
    
    m_spinServoSpeed = new QSpinBox();
    m_spinServoSpeed->setRange(1, 100);
    m_spinServoSpeed->setValue(50);
    m_spinServoSpeed->setSuffix(" %");
    m_spinServoSpeed->setMaximumWidth(70);
    m_spinServoSpeed->setStyleSheet(
        "QSpinBox { background-color: #1a202c; color: #ffffff; border: 2px solid #4a5568; "
        "border-radius: 6px; padding: 4px; }"
    );
    speedLayout->addWidget(m_spinServoSpeed, 0, 2);
    
    connect(m_sliderServoSpeed, &QSlider::valueChanged, m_spinServoSpeed, &QSpinBox::setValue);
    connect(m_spinServoSpeed, QOverload<int>::of(&QSpinBox::valueChanged), m_sliderServoSpeed, &QSlider::setValue);
    connect(m_sliderServoSpeed, &QSlider::valueChanged, this, &CInterfacesPanelWidget::servoSpeedChanged);
    
    layout->addWidget(speedGroup);
    
    // Status Group
    QGroupBox *statusGroup = createStyledGroup("Status");
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);
    statusLayout->setSpacing(8);
    
    m_progressServo = new QProgressBar();
    m_progressServo->setRange(0, 100);
    m_progressServo->setValue(0);
    m_progressServo->setTextVisible(true);
    m_progressServo->setFormat("Movement: %p%");
    m_progressServo->setStyleSheet(
        "QProgressBar { background-color: #1a202c; border: 2px solid #4a5568; border-radius: 6px; "
        "text-align: center; color: white; font-weight: bold; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2); "
        "border-radius: 4px; }"
    );
    statusLayout->addWidget(m_progressServo);
    
    m_chkServoAuto = new QCheckBox("Auto Tracking Mode");
    m_chkServoAuto->setStyleSheet(getCheckboxStyle("#f59e0b"));
    statusLayout->addWidget(m_chkServoAuto);
    
    layout->addWidget(statusGroup);
    
    // Control Buttons Group
    QGroupBox *controlGroup = createStyledGroup("Controls");
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
    controlLayout->setSpacing(10);
    
    m_btnServoHome = createStyledButton("Home Position", "🏠", QColor(52, 211, 153));
    connect(m_btnServoHome, &QPushButton::clicked, this, &CInterfacesPanelWidget::servoHomeRequested);
    controlLayout->addWidget(m_btnServoHome);
    
    m_btnServoStop = createStyledButton("Emergency Stop", "⛔", QColor(239, 68, 68));
    connect(m_btnServoStop, &QPushButton::clicked, this, &CInterfacesPanelWidget::servoStopRequested);
    controlLayout->addWidget(m_btnServoStop);
    
    m_btnServoCal = createStyledButton("Calibrate", "⚙️", QColor(139, 92, 246));
    connect(m_btnServoCal, &QPushButton::clicked, this, &CInterfacesPanelWidget::servoCalibrationRequested);
    controlLayout->addWidget(m_btnServoCal);
    
    layout->addWidget(controlGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(servoTab, "🔧\nServo");
}

void CInterfacesPanelWidget::createRadarTab()
{
    QWidget *radarTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(radarTab);
    layout->setSpacing(12);
    layout->setContentsMargins(12, 12, 12, 12);
    
    // Mode Control Group
    QGroupBox *modeGroup = createStyledGroup("Radar Mode");
    QVBoxLayout *modeLayout = new QVBoxLayout(modeGroup);
    modeLayout->setSpacing(10);
    
    QLabel *lblMode = new QLabel("Operating Mode:");
    lblMode->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    modeLayout->addWidget(lblMode);
    
    m_comboRadarMode = new QComboBox();
    m_comboRadarMode->addItems({"🔍 Search", "🎯 Track", "📡 Surveillance", "🌊 Weather", "⚡ Combat"});
    m_comboRadarMode->setStyleSheet(
        "QComboBox { background-color: #1a202c; color: #ffffff; border: 2px solid #4a5568; "
        "border-radius: 8px; padding: 10px; font-size: 12px; }"
        "QComboBox:hover { border: 2px solid #667eea; }"
        "QComboBox QAbstractItemView { background-color: #2d3748; color: #ffffff; "
        "selection-background-color: #667eea; border: 2px solid #4a5568; }"
    );
    connect(m_comboRadarMode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CInterfacesPanelWidget::radarModeChanged);
    modeLayout->addWidget(m_comboRadarMode);
    
    layout->addWidget(modeGroup);
    
    // Range & Gain Group
    QGroupBox *rangeGroup = createStyledGroup("Range & Gain");
    QGridLayout *rangeLayout = new QGridLayout(rangeGroup);
    rangeLayout->setSpacing(10);
    
    QLabel *lblRange = new QLabel("Range (km):");
    lblRange->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    rangeLayout->addWidget(lblRange, 0, 0);
    
    m_spinRadarRange = new QDoubleSpinBox();
    m_spinRadarRange->setRange(1.0, 500.0);
    m_spinRadarRange->setValue(50.0);
    m_spinRadarRange->setDecimals(1);
    m_spinRadarRange->setStyleSheet(
        "QDoubleSpinBox { background-color: #1a202c; color: #ffffff; border: 2px solid #4a5568; "
        "border-radius: 6px; padding: 8px; }"
    );
    connect(m_spinRadarRange, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &CInterfacesPanelWidget::radarRangeChanged);
    rangeLayout->addWidget(m_spinRadarRange, 0, 1);
    
    QLabel *lblGain = new QLabel("Gain:");
    lblGain->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    rangeLayout->addWidget(lblGain, 1, 0);
    
    m_sliderRadarGain = new QSlider(Qt::Horizontal);
    m_sliderRadarGain->setRange(0, 100);
    m_sliderRadarGain->setValue(70);
    m_sliderRadarGain->setStyleSheet(getSliderStyle());
    rangeLayout->addWidget(m_sliderRadarGain, 1, 1);
    
    m_spinRadarGain = new QSpinBox();
    m_spinRadarGain->setRange(0, 100);
    m_spinRadarGain->setValue(70);
    m_spinRadarGain->setSuffix(" dB");
    m_spinRadarGain->setMaximumWidth(70);
    m_spinRadarGain->setStyleSheet("QSpinBox { background-color: #1a202c; color: #ffffff; "
                                    "border: 2px solid #4a5568; border-radius: 6px; padding: 4px; }");
    rangeLayout->addWidget(m_spinRadarGain, 1, 2);
    
    connect(m_sliderRadarGain, &QSlider::valueChanged, m_spinRadarGain, &QSpinBox::setValue);
    connect(m_spinRadarGain, QOverload<int>::of(&QSpinBox::valueChanged), m_sliderRadarGain, &QSlider::setValue);
    connect(m_sliderRadarGain, &QSlider::valueChanged, this, &CInterfacesPanelWidget::radarGainChanged);
    
    layout->addWidget(rangeGroup);
    
    // Status Group
    QGroupBox *statusGroup = createStyledGroup("Radar Status");
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);
    statusLayout->setSpacing(8);
    
    m_lblRadarStatus = new QLabel("Status: ⚪ Standby");
    m_lblRadarStatus->setStyleSheet("color: #fbbf24; font-size: 13px; font-weight: bold; "
                                     "background-color: #1a202c; padding: 10px; border-radius: 6px;");
    statusLayout->addWidget(m_lblRadarStatus);
    
    m_chkRadarDoppler = new QCheckBox("Doppler Processing");
    m_chkRadarDoppler->setChecked(true);
    m_chkRadarDoppler->setStyleSheet(getCheckboxStyle("#3b82f6"));
    statusLayout->addWidget(m_chkRadarDoppler);
    
    m_chkRadarMTI = new QCheckBox("MTI Filter");
    m_chkRadarMTI->setChecked(false);
    m_chkRadarMTI->setStyleSheet(getCheckboxStyle("#3b82f6"));
    statusLayout->addWidget(m_chkRadarMTI);
    
    layout->addWidget(statusGroup);
    
    // Control Buttons
    QGroupBox *controlGroup = createStyledGroup("Controls");
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
    controlLayout->setSpacing(10);
    
    m_btnRadarStart = createStyledButton("Start Scan", "▶️", QColor(52, 211, 153));
    connect(m_btnRadarStart, &QPushButton::clicked, this, &CInterfacesPanelWidget::radarScanStartRequested);
    controlLayout->addWidget(m_btnRadarStart);
    
    m_btnRadarStop = createStyledButton("Stop Scan", "⏹️", QColor(239, 68, 68));
    connect(m_btnRadarStop, &QPushButton::clicked, this, &CInterfacesPanelWidget::radarScanStopRequested);
    controlLayout->addWidget(m_btnRadarStop);
    
    m_btnRadarReset = createStyledButton("Reset Radar", "🔄", QColor(251, 146, 60));
    connect(m_btnRadarReset, &QPushButton::clicked, this, &CInterfacesPanelWidget::radarResetRequested);
    controlLayout->addWidget(m_btnRadarReset);
    
    layout->addWidget(controlGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(radarTab, "📡\nRadar");
}

void CInterfacesPanelWidget::createGunTab()
{
    QWidget *gunTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(gunTab);
    layout->setSpacing(12);
    layout->setContentsMargins(12, 12, 12, 12);
    
    // Targeting Group
    QGroupBox *targetGroup = createStyledGroup("Gun Targeting");
    QGridLayout *targetLayout = new QGridLayout(targetGroup);
    targetLayout->setSpacing(10);
    
    QLabel *lblGunAz = new QLabel("Azimuth (°):");
    lblGunAz->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    targetLayout->addWidget(lblGunAz, 0, 0);
    
    m_spinGunAz = new QDoubleSpinBox();
    m_spinGunAz->setRange(-180.0, 180.0);
    m_spinGunAz->setValue(0.0);
    m_spinGunAz->setDecimals(2);
    m_spinGunAz->setStyleSheet("QDoubleSpinBox { background-color: #1a202c; color: #ffffff; "
                                "border: 2px solid #4a5568; border-radius: 6px; padding: 8px; }");
    connect(m_spinGunAz, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &CInterfacesPanelWidget::gunAzimuthChanged);
    targetLayout->addWidget(m_spinGunAz, 0, 1);
    
    QLabel *lblGunEl = new QLabel("Elevation (°):");
    lblGunEl->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    targetLayout->addWidget(lblGunEl, 1, 0);
    
    m_spinGunEl = new QDoubleSpinBox();
    m_spinGunEl->setRange(-20.0, 85.0);
    m_spinGunEl->setValue(0.0);
    m_spinGunEl->setDecimals(2);
    m_spinGunEl->setStyleSheet(m_spinGunAz->styleSheet());
    connect(m_spinGunEl, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &CInterfacesPanelWidget::gunElevationChanged);
    targetLayout->addWidget(m_spinGunEl, 1, 1);
    
    layout->addWidget(targetGroup);
    
    // Ammunition Group
    QGroupBox *ammoGroup = createStyledGroup("Ammunition");
    QGridLayout *ammoLayout = new QGridLayout(ammoGroup);
    ammoLayout->setSpacing(10);
    
    QLabel *lblRounds = new QLabel("Burst Size:");
    lblRounds->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    ammoLayout->addWidget(lblRounds, 0, 0);
    
    m_spinGunRounds = new QSpinBox();
    m_spinGunRounds->setRange(1, 100);
    m_spinGunRounds->setValue(10);
    m_spinGunRounds->setSuffix(" rounds");
    m_spinGunRounds->setStyleSheet("QSpinBox { background-color: #1a202c; color: #ffffff; "
                                    "border: 2px solid #4a5568; border-radius: 6px; padding: 8px; }");
    connect(m_spinGunRounds, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &CInterfacesPanelWidget::gunRoundsChanged);
    ammoLayout->addWidget(m_spinGunRounds, 0, 1);
    
    m_lblGunRoundsRemaining = new QLabel("Remaining: 1000 rounds");
    m_lblGunRoundsRemaining->setStyleSheet("color: #48bb78; font-size: 12px; font-weight: bold;");
    ammoLayout->addWidget(m_lblGunRoundsRemaining, 1, 0, 1, 2);
    
    layout->addWidget(ammoGroup);
    
    // Status Group
    QGroupBox *statusGroup = createStyledGroup("Gun Status");
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);
    statusLayout->setSpacing(8);
    
    QLabel *lblTemp = new QLabel("Barrel Temperature:");
    lblTemp->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    statusLayout->addWidget(lblTemp);
    
    m_progressGunTemp = new QProgressBar();
    m_progressGunTemp->setRange(0, 100);
    m_progressGunTemp->setValue(25);
    m_progressGunTemp->setTextVisible(true);
    m_progressGunTemp->setFormat("%p% (Safe)");
    m_progressGunTemp->setStyleSheet(
        "QProgressBar { background-color: #1a202c; border: 2px solid #4a5568; border-radius: 6px; "
        "text-align: center; color: white; font-weight: bold; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #48bb78, stop:0.7 #fbbf24, stop:1 #ef4444); border-radius: 4px; }"
    );
    statusLayout->addWidget(m_progressGunTemp);
    
    m_chkGunSafety = new QCheckBox("🔒 Safety Lock (ON)");
    m_chkGunSafety->setChecked(true);
    m_chkGunSafety->setStyleSheet(getCheckboxStyle("#ef4444"));
    connect(m_chkGunSafety, &QCheckBox::toggled, this, [this](bool checked) {
        m_chkGunSafety->setText(checked ? "🔒 Safety Lock (ON)" : "🔓 Safety Lock (OFF)");
        emit gunSafetyToggled(checked);
    });
    statusLayout->addWidget(m_chkGunSafety);
    
    m_chkGunAutoTrack = new QCheckBox("Auto Track Target");
    m_chkGunAutoTrack->setStyleSheet(getCheckboxStyle("#f59e0b"));
    statusLayout->addWidget(m_chkGunAutoTrack);
    
    layout->addWidget(statusGroup);
    
    // Fire Control
    QGroupBox *fireGroup = createStyledGroup("⚠️ Fire Control");
    QVBoxLayout *fireLayout = new QVBoxLayout(fireGroup);
    fireLayout->setSpacing(10);
    
    m_btnGunFire = createStyledButton("FIRE", "🔥", QColor(239, 68, 68));
    m_btnGunFire->setMinimumHeight(60);
    m_btnGunFire->setEnabled(false);
    connect(m_btnGunFire, &QPushButton::clicked, this, &CInterfacesPanelWidget::gunFireRequested);
    connect(m_chkGunSafety, &QCheckBox::toggled, this, [this](bool checked) {
        m_btnGunFire->setEnabled(!checked);
    });
    fireLayout->addWidget(m_btnGunFire);
    
    m_btnGunCease = createStyledButton("Cease Fire", "⏹️", QColor(251, 146, 60));
    fireLayout->addWidget(m_btnGunCease);
    
    layout->addWidget(fireGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(gunTab, "🔫\nGun");
}

void CInterfacesPanelWidget::createMissileTab()
{
    QWidget *missileTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(missileTab);
    layout->setSpacing(12);
    layout->setContentsMargins(12, 12, 12, 12);
    
    // Missile Selection Group
    QGroupBox *selectGroup = createStyledGroup("Missile Configuration");
    QVBoxLayout *selectLayout = new QVBoxLayout(selectGroup);
    selectLayout->setSpacing(10);
    
    QLabel *lblType = new QLabel("Missile Type:");
    lblType->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    selectLayout->addWidget(lblType);
    
    m_comboMissileType = new QComboBox();
    m_comboMissileType->addItems({"🚀 AIM-120 AMRAAM", "🎯 AIM-9 Sidewinder", "💣 AGM-88 HARM", 
                                   "🌊 Harpoon", "⚡ SM-2"});
    m_comboMissileType->setStyleSheet(
        "QComboBox { background-color: #1a202c; color: #ffffff; border: 2px solid #4a5568; "
        "border-radius: 8px; padding: 10px; font-size: 12px; }"
        "QComboBox QAbstractItemView { background-color: #2d3748; color: #ffffff; "
        "selection-background-color: #667eea; }"
    );
    connect(m_comboMissileType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CInterfacesPanelWidget::missileTypeChanged);
    selectLayout->addWidget(m_comboMissileType);
    
    layout->addWidget(selectGroup);
    
    // Target Selection Group
    QGroupBox *targetGroup = createStyledGroup("Target Selection");
    QGridLayout *targetLayout = new QGridLayout(targetGroup);
    targetLayout->setSpacing(10);
    
    QLabel *lblTarget = new QLabel("Target Track ID:");
    lblTarget->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    targetLayout->addWidget(lblTarget, 0, 0);
    
    m_spinMissileTarget = new QSpinBox();
    m_spinMissileTarget->setRange(1, 999);
    m_spinMissileTarget->setValue(1);
    m_spinMissileTarget->setPrefix("TID: ");
    m_spinMissileTarget->setStyleSheet("QSpinBox { background-color: #1a202c; color: #ffffff; "
                                        "border: 2px solid #4a5568; border-radius: 6px; padding: 8px; }");
    connect(m_spinMissileTarget, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &CInterfacesPanelWidget::missileTargetLocked);
    targetLayout->addWidget(m_spinMissileTarget, 0, 1);
    
    QLabel *lblLock = new QLabel("Target Lock:");
    lblLock->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    targetLayout->addWidget(lblLock, 1, 0);
    
    m_progressMissileLock = new QProgressBar();
    m_progressMissileLock->setRange(0, 100);
    m_progressMissileLock->setValue(0);
    m_progressMissileLock->setTextVisible(true);
    m_progressMissileLock->setFormat("Lock Quality: %p%");
    m_progressMissileLock->setStyleSheet(
        "QProgressBar { background-color: #1a202c; border: 2px solid #4a5568; border-radius: 6px; "
        "text-align: center; color: white; font-weight: bold; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #ef4444, stop:0.5 #fbbf24, stop:1 #48bb78); border-radius: 4px; }"
    );
    targetLayout->addWidget(m_progressMissileLock, 1, 1);
    
    layout->addWidget(targetGroup);
    
    // Status Group
    QGroupBox *statusGroup = createStyledGroup("Missile Status");
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);
    statusLayout->setSpacing(8);
    
    m_lblMissileStatus = new QLabel("Status: ⚪ Safe");
    m_lblMissileStatus->setStyleSheet("color: #fbbf24; font-size: 13px; font-weight: bold; "
                                       "background-color: #1a202c; padding: 10px; border-radius: 6px;");
    statusLayout->addWidget(m_lblMissileStatus);
    
    m_chkMissileArm = new QCheckBox("⚠️ ARM Missile");
    m_chkMissileArm->setChecked(false);
    m_chkMissileArm->setStyleSheet(getCheckboxStyle("#ef4444"));
    connect(m_chkMissileArm, &QCheckBox::toggled, this, &CInterfacesPanelWidget::missileArmToggled);
    statusLayout->addWidget(m_chkMissileArm);
    
    m_chkMissileAutoEngage = new QCheckBox("Auto Engage Mode");
    m_chkMissileAutoEngage->setStyleSheet(getCheckboxStyle("#f59e0b"));
    statusLayout->addWidget(m_chkMissileAutoEngage);
    
    layout->addWidget(statusGroup);
    
    // Launch Control
    QGroupBox *launchGroup = createStyledGroup("⚠️ Launch Control");
    QVBoxLayout *launchLayout = new QVBoxLayout(launchGroup);
    launchLayout->setSpacing(10);
    
    m_btnMissileLaunch = createStyledButton("LAUNCH", "🚀", QColor(239, 68, 68));
    m_btnMissileLaunch->setMinimumHeight(60);
    m_btnMissileLaunch->setEnabled(false);
    connect(m_btnMissileLaunch, &QPushButton::clicked, this, &CInterfacesPanelWidget::missileLaunchRequested);
    connect(m_chkMissileArm, &QCheckBox::toggled, this, [this](bool checked) {
        m_btnMissileLaunch->setEnabled(checked);
    });
    launchLayout->addWidget(m_btnMissileLaunch);
    
    m_btnMissileAbort = createStyledButton("Abort Launch", "⛔", QColor(251, 146, 60));
    connect(m_btnMissileAbort, &QPushButton::clicked, this, &CInterfacesPanelWidget::missileAbortRequested);
    launchLayout->addWidget(m_btnMissileAbort);
    
    layout->addWidget(launchGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(missileTab, "🚀\nMissile");
}

void CInterfacesPanelWidget::createEOTSTab()
{
    QWidget *eotsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(eotsTab);
    layout->setSpacing(12);
    layout->setContentsMargins(12, 12, 12, 12);
    
    // Optical Control Group
    QGroupBox *opticalGroup = createStyledGroup("Optical Controls");
    QGridLayout *opticalLayout = new QGridLayout(opticalGroup);
    opticalLayout->setSpacing(10);
    
    QLabel *lblZoom = new QLabel("Zoom:");
    lblZoom->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    opticalLayout->addWidget(lblZoom, 0, 0);
    
    m_spinEOTSZoom = new QDoubleSpinBox();
    m_spinEOTSZoom->setRange(1.0, 50.0);
    m_spinEOTSZoom->setValue(1.0);
    m_spinEOTSZoom->setDecimals(1);
    m_spinEOTSZoom->setSuffix("x");
    m_spinEOTSZoom->setStyleSheet("QDoubleSpinBox { background-color: #1a202c; color: #ffffff; "
                                   "border: 2px solid #4a5568; border-radius: 6px; padding: 8px; }");
    connect(m_spinEOTSZoom, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &CInterfacesPanelWidget::eotsZoomChanged);
    opticalLayout->addWidget(m_spinEOTSZoom, 0, 1);
    
    QLabel *lblBrightness = new QLabel("Brightness:");
    lblBrightness->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    opticalLayout->addWidget(lblBrightness, 1, 0);
    
    m_sliderEOTSBrightness = new QSlider(Qt::Horizontal);
    m_sliderEOTSBrightness->setRange(0, 100);
    m_sliderEOTSBrightness->setValue(50);
    m_sliderEOTSBrightness->setStyleSheet(getSliderStyle());
    opticalLayout->addWidget(m_sliderEOTSBrightness, 1, 1);
    
    m_spinEOTSBrightness = new QSpinBox();
    m_spinEOTSBrightness->setRange(0, 100);
    m_spinEOTSBrightness->setValue(50);
    m_spinEOTSBrightness->setMaximumWidth(60);
    m_spinEOTSBrightness->setStyleSheet("QSpinBox { background-color: #1a202c; color: #ffffff; "
                                         "border: 2px solid #4a5568; border-radius: 6px; padding: 4px; }");
    opticalLayout->addWidget(m_spinEOTSBrightness, 1, 2);
    
    connect(m_sliderEOTSBrightness, &QSlider::valueChanged, m_spinEOTSBrightness, &QSpinBox::setValue);
    connect(m_spinEOTSBrightness, QOverload<int>::of(&QSpinBox::valueChanged), m_sliderEOTSBrightness, &QSlider::setValue);
    connect(m_sliderEOTSBrightness, &QSlider::valueChanged, this, &CInterfacesPanelWidget::eotsBrightnessChanged);
    
    QLabel *lblContrast = new QLabel("Contrast:");
    lblContrast->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    opticalLayout->addWidget(lblContrast, 2, 0);
    
    m_sliderEOTSContrast = new QSlider(Qt::Horizontal);
    m_sliderEOTSContrast->setRange(0, 100);
    m_sliderEOTSContrast->setValue(50);
    m_sliderEOTSContrast->setStyleSheet(getSliderStyle());
    opticalLayout->addWidget(m_sliderEOTSContrast, 2, 1);
    
    m_spinEOTSContrast = new QSpinBox();
    m_spinEOTSContrast->setRange(0, 100);
    m_spinEOTSContrast->setValue(50);
    m_spinEOTSContrast->setMaximumWidth(60);
    m_spinEOTSContrast->setStyleSheet(m_spinEOTSBrightness->styleSheet());
    opticalLayout->addWidget(m_spinEOTSContrast, 2, 2);
    
    connect(m_sliderEOTSContrast, &QSlider::valueChanged, m_spinEOTSContrast, &QSpinBox::setValue);
    connect(m_spinEOTSContrast, QOverload<int>::of(&QSpinBox::valueChanged), m_sliderEOTSContrast, &QSlider::setValue);
    connect(m_sliderEOTSContrast, &QSlider::valueChanged, this, &CInterfacesPanelWidget::eotsContrastChanged);
    
    layout->addWidget(opticalGroup);
    
    // Mode Selection Group
    QGroupBox *modeGroup = createStyledGroup("Imaging Mode");
    QVBoxLayout *modeLayout = new QVBoxLayout(modeGroup);
    modeLayout->setSpacing(10);
    
    m_comboEOTSMode = new QComboBox();
    m_comboEOTSMode->addItems({"📷 Visual", "🌡️ Thermal (FLIR)", "🌃 Night Vision", 
                                "🎨 False Color", "📊 Edge Enhance"});
    m_comboEOTSMode->setStyleSheet(
        "QComboBox { background-color: #1a202c; color: #ffffff; border: 2px solid #4a5568; "
        "border-radius: 8px; padding: 10px; font-size: 12px; }"
        "QComboBox QAbstractItemView { background-color: #2d3748; color: #ffffff; "
        "selection-background-color: #667eea; }"
    );
    modeLayout->addWidget(m_comboEOTSMode);
    
    m_chkEOTSIR = new QCheckBox("IR Mode (Thermal)");
    m_chkEOTSIR->setStyleSheet(getCheckboxStyle("#ef4444"));
    connect(m_chkEOTSIR, &QCheckBox::toggled, this, &CInterfacesPanelWidget::eotsIRModeToggled);
    modeLayout->addWidget(m_chkEOTSIR);
    
    m_chkEOTSTracking = new QCheckBox("Auto Tracking");
    m_chkEOTSTracking->setStyleSheet(getCheckboxStyle("#f59e0b"));
    connect(m_chkEOTSTracking, &QCheckBox::toggled, this, &CInterfacesPanelWidget::eotsTrackingToggled);
    modeLayout->addWidget(m_chkEOTSTracking);
    
    layout->addWidget(modeGroup);
    
    // Laser Rangefinder Group
    QGroupBox *laserGroup = createStyledGroup("Laser Rangefinder");
    QVBoxLayout *laserLayout = new QVBoxLayout(laserGroup);
    laserLayout->setSpacing(10);
    
    m_btnEOTSLaser = createStyledButton("Fire Laser Range", "⚡", QColor(139, 92, 246));
    connect(m_btnEOTSLaser, &QPushButton::clicked, this, &CInterfacesPanelWidget::eotsLaserRangeRequested);
    laserLayout->addWidget(m_btnEOTSLaser);
    
    QLabel *lblLaserWarning = new QLabel("⚠️ Laser radiation hazard");
    lblLaserWarning->setStyleSheet("color: #fbbf24; font-size: 10px; font-style: italic;");
    laserLayout->addWidget(lblLaserWarning);
    
    layout->addWidget(laserGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(eotsTab, "👁️\nEOTS");
}

void CInterfacesPanelWidget::createLoggingTab()
{
    QWidget *loggingTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(loggingTab);
    layout->setSpacing(12);
    layout->setContentsMargins(12, 12, 12, 12);
    
    // Logging Group
    QGroupBox *logGroup = createStyledGroup("System Logging");
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logLayout->setSpacing(10);
    
    // Log level selection
    QHBoxLayout *logLevelLayout = new QHBoxLayout();
    QLabel *lblLogLevel = new QLabel("Log Level:");
    lblLogLevel->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    logLevelLayout->addWidget(lblLogLevel);
    
    m_comboLogLevel = new QComboBox();
    m_comboLogLevel->addItems({"🔍 Debug", "ℹ️ Info", "⚠️ Warning", "❌ Error", "💥 Critical"});
    m_comboLogLevel->setCurrentIndex(1);
    m_comboLogLevel->setStyleSheet(
        "QComboBox { background-color: #1a202c; color: #ffffff; border: 2px solid #4a5568; "
        "border-radius: 6px; padding: 6px; font-size: 11px; }"
    );
    logLevelLayout->addWidget(m_comboLogLevel);
    logLayout->addLayout(logLevelLayout);
    
    // Log options
    m_chkLogTimestamp = new QCheckBox("Show Timestamp");
    m_chkLogTimestamp->setChecked(true);
    m_chkLogTimestamp->setStyleSheet(getCheckboxStyle("#3b82f6"));
    logLayout->addWidget(m_chkLogTimestamp);
    
    m_chkLogLevel = new QCheckBox("Show Log Level");
    m_chkLogLevel->setChecked(true);
    m_chkLogLevel->setStyleSheet(getCheckboxStyle("#3b82f6"));
    logLayout->addWidget(m_chkLogLevel);
    
    // Log text area
    m_textLog = new QTextEdit();
    m_textLog->setReadOnly(true);
    m_textLog->setMaximumHeight(200);
    m_textLog->setStyleSheet(
        "QTextEdit { background-color: #0a0e1a; color: #48bb78; border: 2px solid #4a5568; "
        "border-radius: 8px; padding: 10px; font-family: 'Courier New', monospace; font-size: 11px; }"
    );
    m_textLog->append("[" + QDateTime::currentDateTime().toString("HH:mm:ss") + "] System initialized");
    logLayout->addWidget(m_textLog);
    
    // Log control buttons
    QHBoxLayout *logButtonLayout = new QHBoxLayout();
    m_btnLogStart = createStyledButton("Start", "▶️", QColor(52, 211, 153));
    m_btnLogStart->setMinimumHeight(35);
    connect(m_btnLogStart, &QPushButton::clicked, this, &CInterfacesPanelWidget::loggingStarted);
    logButtonLayout->addWidget(m_btnLogStart);
    
    m_btnLogStop = createStyledButton("Stop", "⏹️", QColor(239, 68, 68));
    m_btnLogStop->setMinimumHeight(35);
    connect(m_btnLogStop, &QPushButton::clicked, this, &CInterfacesPanelWidget::loggingStopped);
    logButtonLayout->addWidget(m_btnLogStop);
    
    m_btnLogClear = createStyledButton("Clear", "🗑️", QColor(156, 163, 175));
    m_btnLogClear->setMinimumHeight(35);
    connect(m_btnLogClear, &QPushButton::clicked, this, [this]() {
        m_textLog->clear();
        emit loggingCleared();
    });
    logButtonLayout->addWidget(m_btnLogClear);
    
    m_btnLogSave = createStyledButton("Save", "💾", QColor(59, 130, 246));
    m_btnLogSave->setMinimumHeight(35);
    logButtonLayout->addWidget(m_btnLogSave);
    
    logLayout->addLayout(logButtonLayout);
    layout->addWidget(logGroup);
    
    // Screen Recording Group
    QGroupBox *recGroup = createStyledGroup("Screen Recording");
    QVBoxLayout *recLayout = new QVBoxLayout(recGroup);
    recLayout->setSpacing(10);
    
    m_lblRecStatus = new QLabel("Status: ⚪ Idle");
    m_lblRecStatus->setStyleSheet("color: #a0aec0; font-size: 12px; font-weight: bold; "
                                   "background-color: #1a202c; padding: 8px; border-radius: 6px;");
    recLayout->addWidget(m_lblRecStatus);
    
    m_progressRecTime = new QProgressBar();
    m_progressRecTime->setRange(0, 3600); // 1 hour max
    m_progressRecTime->setValue(0);
    m_progressRecTime->setFormat("Duration: %v seconds");
    m_progressRecTime->setStyleSheet(
        "QProgressBar { background-color: #1a202c; border: 2px solid #4a5568; border-radius: 6px; "
        "text-align: center; color: white; }"
        "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #667eea, stop:1 #764ba2); border-radius: 4px; }"
    );
    recLayout->addWidget(m_progressRecTime);
    
    QHBoxLayout *recButtonLayout = new QHBoxLayout();
    m_btnRecStart = createStyledButton("Record", "⏺️", QColor(239, 68, 68));
    m_btnRecStart->setMinimumHeight(40);
    connect(m_btnRecStart, &QPushButton::clicked, this, &CInterfacesPanelWidget::recordingStarted);
    recButtonLayout->addWidget(m_btnRecStart);
    
    m_btnRecPause = createStyledButton("Pause", "⏸️", QColor(251, 146, 60));
    m_btnRecPause->setMinimumHeight(40);
    m_btnRecPause->setEnabled(false);
    connect(m_btnRecPause, &QPushButton::clicked, this, &CInterfacesPanelWidget::recordingPaused);
    recButtonLayout->addWidget(m_btnRecPause);
    
    m_btnRecStop = createStyledButton("Stop", "⏹️", QColor(156, 163, 175));
    m_btnRecStop->setMinimumHeight(40);
    m_btnRecStop->setEnabled(false);
    connect(m_btnRecStop, &QPushButton::clicked, this, &CInterfacesPanelWidget::recordingStopped);
    recButtonLayout->addWidget(m_btnRecStop);
    
    recLayout->addLayout(recButtonLayout);
    layout->addWidget(recGroup);
    
    // Replay Group
    QGroupBox *replayGroup = createStyledGroup("Session Replay");
    QVBoxLayout *replayLayout = new QVBoxLayout(replayGroup);
    replayLayout->setSpacing(10);
    
    m_progressReplay = new QProgressBar();
    m_progressReplay->setRange(0, 100);
    m_progressReplay->setValue(0);
    m_progressReplay->setFormat("Replay: %p%");
    m_progressReplay->setStyleSheet(m_progressRecTime->styleSheet());
    replayLayout->addWidget(m_progressReplay);
    
    QGridLayout *replaySpeedLayout = new QGridLayout();
    QLabel *lblSpeed = new QLabel("Speed:");
    lblSpeed->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    replaySpeedLayout->addWidget(lblSpeed, 0, 0);
    
    m_sliderReplaySpeed = new QSlider(Qt::Horizontal);
    m_sliderReplaySpeed->setRange(1, 400);
    m_sliderReplaySpeed->setValue(100);
    m_sliderReplaySpeed->setStyleSheet(getSliderStyle());
    replaySpeedLayout->addWidget(m_sliderReplaySpeed, 0, 1);
    
    m_spinReplaySpeed = new QDoubleSpinBox();
    m_spinReplaySpeed->setRange(0.1, 4.0);
    m_spinReplaySpeed->setValue(1.0);
    m_spinReplaySpeed->setDecimals(1);
    m_spinReplaySpeed->setSuffix("x");
    m_spinReplaySpeed->setMaximumWidth(70);
    m_spinReplaySpeed->setStyleSheet("QDoubleSpinBox { background-color: #1a202c; color: #ffffff; "
                                      "border: 2px solid #4a5568; border-radius: 6px; padding: 4px; }");
    replaySpeedLayout->addWidget(m_spinReplaySpeed, 0, 2);
    
    connect(m_sliderReplaySpeed, &QSlider::valueChanged, this, [this](int value) {
        m_spinReplaySpeed->setValue(value / 100.0);
    });
    connect(m_spinReplaySpeed, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
        m_sliderReplaySpeed->setValue(static_cast<int>(value * 100));
        emit replaySpeedChanged(value);
    });
    
    replayLayout->addLayout(replaySpeedLayout);
    
    QHBoxLayout *replayButtonLayout = new QHBoxLayout();
    m_btnReplayLoad = createStyledButton("Load", "📂", QColor(59, 130, 246));
    m_btnReplayLoad->setMinimumHeight(35);
    replayButtonLayout->addWidget(m_btnReplayLoad);
    
    m_btnReplayStart = createStyledButton("Play", "▶️", QColor(52, 211, 153));
    m_btnReplayStart->setMinimumHeight(35);
    connect(m_btnReplayStart, &QPushButton::clicked, this, &CInterfacesPanelWidget::replayStarted);
    replayButtonLayout->addWidget(m_btnReplayStart);
    
    m_btnReplayPause = createStyledButton("Pause", "⏸️", QColor(251, 146, 60));
    m_btnReplayPause->setMinimumHeight(35);
    connect(m_btnReplayPause, &QPushButton::clicked, this, &CInterfacesPanelWidget::replayPaused);
    replayButtonLayout->addWidget(m_btnReplayPause);
    
    m_btnReplayStop = createStyledButton("Stop", "⏹️", QColor(239, 68, 68));
    m_btnReplayStop->setMinimumHeight(35);
    connect(m_btnReplayStop, &QPushButton::clicked, this, &CInterfacesPanelWidget::replayStopped);
    replayButtonLayout->addWidget(m_btnReplayStop);
    
    replayLayout->addLayout(replayButtonLayout);
    layout->addWidget(replayGroup);
    
    layout->addStretch();
    
    m_tabWidget->addTab(loggingTab, "📝\nLogging");
}

void CInterfacesPanelWidget::applyRichStyle()
{
    setStyleSheet(
        "QDockWidget {"
        "   background-color: #1a202c;"
        "   color: #ffffff;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "}"
        "QDockWidget::title {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2);"
        "   padding: 10px;"
        "   border-bottom: 2px solid #4a5568;"
        "   text-align: center;"
        "}"
        "QDockWidget::close-button, QDockWidget::float-button {"
        "   background-color: rgba(255, 255, 255, 0.1);"
        "   border-radius: 4px;"
        "   padding: 4px;"
        "}"
        "QDockWidget::close-button:hover, QDockWidget::float-button:hover {"
        "   background-color: rgba(255, 255, 255, 0.2);"
        "}"
    );
    
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane {"
        "   background-color: #1a202c;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 8px;"
        "   top: -2px;"
        "}"
        "QTabBar::tab {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2d3748, stop:1 #1a202c);"
        "   color: #a0aec0;"
        "   padding: 15px 10px;"
        "   margin: 2px;"
        "   border-top-left-radius: 8px;"
        "   border-bottom-left-radius: 8px;"
        "   font-size: 11px;"
        "   font-weight: bold;"
        "   min-width: 70px;"
        "   text-align: center;"
        "}"
        "QTabBar::tab:selected {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2);"
        "   color: white;"
        "}"
        "QTabBar::tab:hover:!selected {"
        "   background: #2d3748;"
        "   color: #ffffff;"
        "}"
    );
}

// Public slots
void CInterfacesPanelWidget::updateServoPosition(double azimuth, double elevation)
{
    m_lblServoAzPos->setText(QString("Current: %1°").arg(azimuth, 0, 'f', 2));
    m_lblServoElPos->setText(QString("Current: %1°").arg(elevation, 0, 'f', 2));
}

void CInterfacesPanelWidget::updateRadarStatus(const QString &status)
{
    m_lblRadarStatus->setText("Status: " + status);
}

void CInterfacesPanelWidget::updateGunStatus(int roundsRemaining)
{
    m_lblGunRoundsRemaining->setText(QString("Remaining: %1 rounds").arg(roundsRemaining));
}

void CInterfacesPanelWidget::updateMissileStatus(const QString &status)
{
    m_lblMissileStatus->setText("Status: " + status);
}

void CInterfacesPanelWidget::appendLogMessage(const QString &message)
{
    QString timestamp = m_chkLogTimestamp->isChecked() ? 
        "[" + QDateTime::currentDateTime().toString("HH:mm:ss") + "] " : "";
    m_textLog->append(timestamp + message);
}

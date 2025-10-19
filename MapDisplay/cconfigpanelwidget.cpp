#include "cconfigpanelwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>
#include <QSpacerItem>

CConfigPanelWidget::CConfigPanelWidget(QWidget *parent)
    : QDockWidget("Control Panel", parent)
{
    setupUI();
    applyRichStyle();
}

CConfigPanelWidget::~CConfigPanelWidget()
{
}

void CConfigPanelWidget::setupUI()
{
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::West);

    createMapTab();
    createTrackTab();
    createDisplayTab();
    createSystemTab();

    mainLayout->addWidget(m_tabWidget);
    setWidget(mainWidget);
}

QPushButton* CConfigPanelWidget::createStyledButton(const QString &text, const QString &icon, const QColor &color)
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
    ).arg(colorHex).arg(color.darker(110).name()).arg(hoverColor).arg(pressColor));

    return btn;
}

QGroupBox* CConfigPanelWidget::createStyledGroup(const QString &title)
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

void CConfigPanelWidget::createMapTab()
{
    QWidget *mapTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(mapTab);
    layout->setSpacing(12);
    layout->setContentsMargins(12, 12, 12, 12);

    // Map Navigation Group
    QGroupBox *navGroup = createStyledGroup("Map Navigation");
    QVBoxLayout *navLayout = new QVBoxLayout(navGroup);
    navLayout->setSpacing(10);

    m_btnMapHome = createStyledButton("Map Home", "🏠", QColor(102, 126, 234));
    connect(m_btnMapHome, &QPushButton::clicked, this, &CConfigPanelWidget::mapHomeRequested);
    navLayout->addWidget(m_btnMapHome);

    m_btnOpenMaps = createStyledButton("Open Map", "📂", QColor(52, 211, 153));
    connect(m_btnOpenMaps, &QPushButton::clicked, this, &CConfigPanelWidget::openMapsRequested);
    navLayout->addWidget(m_btnOpenMaps);

    m_btnSaveMaps = createStyledButton("Save View", "💾", QColor(251, 146, 60));
    navLayout->addWidget(m_btnSaveMaps);

    layout->addWidget(navGroup);

    // Zoom Control Group
    QGroupBox *zoomGroup = createStyledGroup("Zoom & Scale");
    QGridLayout *zoomLayout = new QGridLayout(zoomGroup);
    zoomLayout->setSpacing(10);

    QLabel *lblZoom = new QLabel("Zoom Level:");
    lblZoom->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    zoomLayout->addWidget(lblZoom, 0, 0);

    m_spinZoom = new QDoubleSpinBox();
    m_spinZoom->setRange(0.1, 50.0);
    m_spinZoom->setValue(1.0);
    m_spinZoom->setSingleStep(0.1);
    m_spinZoom->setStyleSheet(
        "QDoubleSpinBox {"
        "   background-color: #1a202c;"
        "   color: #ffffff;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 6px;"
        "   padding: 8px;"
        "   font-size: 12px;"
        "}"
        "QDoubleSpinBox:focus {"
        "   border: 2px solid #667eea;"
        "}"
    );
    connect(m_spinZoom, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &CConfigPanelWidget::zoomLevelChanged);
    zoomLayout->addWidget(m_spinZoom, 0, 1);

    layout->addWidget(zoomGroup);

    // Overlay Group
    QGroupBox *overlayGroup = createStyledGroup("Map Overlays");
    QVBoxLayout *overlayLayout = new QVBoxLayout(overlayGroup);
    overlayLayout->setSpacing(8);

    m_chkGrid = new QCheckBox("Show Grid Lines");
    m_chkGrid->setChecked(false);
    m_chkGrid->setStyleSheet(
        "QCheckBox {"
        "   color: #ffffff;"
        "   font-size: 12px;"
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
        "   background-color: #667eea;"
        "   border: 2px solid #667eea;"
        "   image: url(:/icons/checkmark.png);"
        "}"
        "QCheckBox::indicator:hover {"
        "   border: 2px solid #667eea;"
        "}"
    );
    connect(m_chkGrid, &QCheckBox::toggled, this, &CConfigPanelWidget::gridVisibilityChanged);
    overlayLayout->addWidget(m_chkGrid);

    m_chkCompass = new QCheckBox("Show Compass");
    m_chkCompass->setChecked(true);
    m_chkCompass->setStyleSheet(m_chkGrid->styleSheet());
    connect(m_chkCompass, &QCheckBox::toggled, this, &CConfigPanelWidget::compassVisibilityChanged);
    overlayLayout->addWidget(m_chkCompass);

    layout->addWidget(overlayGroup);
    layout->addStretch();

    m_tabWidget->addTab(mapTab, "🗺️\nMap");
}

void CConfigPanelWidget::createTrackTab()
{
    QWidget *trackTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(trackTab);
    layout->setSpacing(12);
    layout->setContentsMargins(12, 12, 12, 12);

    // Track Filter Group
    QGroupBox *filterGroup = createStyledGroup("Track Filters");
    QVBoxLayout *filterLayout = new QVBoxLayout(filterGroup);
    filterLayout->setSpacing(8);

    QString checkboxStyle =
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
        "}";

    m_chkShowFriend = new QCheckBox("✓ Show Friendly Tracks");
    m_chkShowFriend->setChecked(true);
    m_chkShowFriend->setStyleSheet(checkboxStyle.arg("#2ecc71"));
    filterLayout->addWidget(m_chkShowFriend);

    m_chkShowHostile = new QCheckBox("✖ Show Hostile Tracks");
    m_chkShowHostile->setChecked(true);
    m_chkShowHostile->setStyleSheet(checkboxStyle.arg("#e74c3c"));
    filterLayout->addWidget(m_chkShowHostile);

    m_chkShowUnknown = new QCheckBox("? Show Unknown Tracks");
    m_chkShowUnknown->setChecked(true);
    m_chkShowUnknown->setStyleSheet(checkboxStyle.arg("#f1c40f"));
    filterLayout->addWidget(m_chkShowUnknown);

    auto updateFilters = [this]() {
        emit trackFilterChanged(m_chkShowFriend->isChecked(),
                               m_chkShowHostile->isChecked(),
                               m_chkShowUnknown->isChecked());
    };

    connect(m_chkShowFriend, &QCheckBox::toggled, updateFilters);
    connect(m_chkShowHostile, &QCheckBox::toggled, updateFilters);
    connect(m_chkShowUnknown, &QCheckBox::toggled, updateFilters);

    layout->addWidget(filterGroup);

    // Track Display Group
    QGroupBox *displayGroup = createStyledGroup("Track Display");
    QGridLayout *displayLayout = new QGridLayout(displayGroup);
    displayLayout->setSpacing(10);

    QLabel *lblSize = new QLabel("Track Size:");
    lblSize->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    displayLayout->addWidget(lblSize, 0, 0);

    m_sliderTrackSize = new QSlider(Qt::Horizontal);
    m_sliderTrackSize->setRange(1, 20);
    m_sliderTrackSize->setValue(4);
    m_sliderTrackSize->setStyleSheet(
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
    displayLayout->addWidget(m_sliderTrackSize, 0, 1);

    m_spinTrackSize = new QSpinBox();
    m_spinTrackSize->setRange(1, 20);
    m_spinTrackSize->setValue(4);
    m_spinTrackSize->setMaximumWidth(60);
    m_spinTrackSize->setStyleSheet(
        "QSpinBox {"
        "   background-color: #1a202c;"
        "   color: #ffffff;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 6px;"
        "   padding: 4px;"
        "}"
    );
    displayLayout->addWidget(m_spinTrackSize, 0, 2);

    connect(m_sliderTrackSize, &QSlider::valueChanged, m_spinTrackSize, &QSpinBox::setValue);
    connect(m_spinTrackSize, QOverload<int>::of(&QSpinBox::valueChanged), m_sliderTrackSize, &QSlider::setValue);
    connect(m_sliderTrackSize, &QSlider::valueChanged, this, &CConfigPanelWidget::trackSizeChanged);

    layout->addWidget(displayGroup);

    // Animation Group
    QGroupBox *animGroup = createStyledGroup("Animation");
    QGridLayout *animLayout = new QGridLayout(animGroup);
    animLayout->setSpacing(10);

    QLabel *lblSpeed = new QLabel("Speed:");
    lblSpeed->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    animLayout->addWidget(lblSpeed, 0, 0);

    m_sliderAnimSpeed = new QSlider(Qt::Horizontal);
    m_sliderAnimSpeed->setRange(50, 500);
    m_sliderAnimSpeed->setValue(150);
    m_sliderAnimSpeed->setStyleSheet(m_sliderTrackSize->styleSheet());
    animLayout->addWidget(m_sliderAnimSpeed, 0, 1);

    m_spinAnimSpeed = new QSpinBox();
    m_spinAnimSpeed->setRange(50, 500);
    m_spinAnimSpeed->setValue(150);
    m_spinAnimSpeed->setSuffix(" ms");
    m_spinAnimSpeed->setMaximumWidth(80);
    m_spinAnimSpeed->setStyleSheet(m_spinTrackSize->styleSheet());
    animLayout->addWidget(m_spinAnimSpeed, 0, 2);

    connect(m_sliderAnimSpeed, &QSlider::valueChanged, m_spinAnimSpeed, &QSpinBox::setValue);
    connect(m_spinAnimSpeed, QOverload<int>::of(&QSpinBox::valueChanged), m_sliderAnimSpeed, &QSlider::setValue);
    connect(m_sliderAnimSpeed, &QSlider::valueChanged, this, &CConfigPanelWidget::animationSpeedChanged);

    layout->addWidget(animGroup);

    // Track History Group
    QGroupBox *historyGroup = createStyledGroup("Track History");
    QGridLayout *historyLayout = new QGridLayout(historyGroup);
    historyLayout->setSpacing(10);

    QLabel *lblHistoryLimit = new QLabel("Max History Points:");
    lblHistoryLimit->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    historyLayout->addWidget(lblHistoryLimit, 0, 0);

    m_sliderHistoryLimit = new QSlider(Qt::Horizontal);
    m_sliderHistoryLimit->setRange(10, 200);
    m_sliderHistoryLimit->setValue(50);
    m_sliderHistoryLimit->setTickPosition(QSlider::TicksBelow);
    m_sliderHistoryLimit->setTickInterval(50);
    m_sliderHistoryLimit->setStyleSheet(m_sliderTrackSize->styleSheet());
    historyLayout->addWidget(m_sliderHistoryLimit, 0, 1);

    m_spinHistoryLimit = new QSpinBox();
    m_spinHistoryLimit->setRange(10, 200);
    m_spinHistoryLimit->setValue(50);
    m_spinHistoryLimit->setMaximumWidth(70);
    m_spinHistoryLimit->setStyleSheet(m_spinTrackSize->styleSheet());
    historyLayout->addWidget(m_spinHistoryLimit, 0, 2);

    connect(m_sliderHistoryLimit, &QSlider::valueChanged, m_spinHistoryLimit, &QSpinBox::setValue);
    connect(m_spinHistoryLimit, QOverload<int>::of(&QSpinBox::valueChanged), m_sliderHistoryLimit, &QSlider::setValue);
    connect(m_sliderHistoryLimit, &QSlider::valueChanged, this, &CConfigPanelWidget::historyLimitChanged);

    QLabel *lblHistoryDesc = new QLabel("📍 Right-click a track to toggle history trail");
    lblHistoryDesc->setStyleSheet("color: #a0aec0; font-size: 11px; font-style: italic;");
    lblHistoryDesc->setWordWrap(true);
    historyLayout->addWidget(lblHistoryDesc, 1, 0, 1, 3);

    layout->addWidget(historyGroup);

    // Additional Options
    QGroupBox *optionsGroup = createStyledGroup("Options");
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsGroup);
    optionsLayout->setSpacing(8);

    m_chkShowLabels = new QCheckBox("Show Track Labels");
    m_chkShowLabels->setChecked(true);
    m_chkShowLabels->setStyleSheet(checkboxStyle.arg("#667eea"));
    optionsLayout->addWidget(m_chkShowLabels);

    m_chkShowTrails = new QCheckBox("Show Track Trails");
    m_chkShowTrails->setChecked(false);
    m_chkShowTrails->setStyleSheet(checkboxStyle.arg("#667eea"));
    optionsLayout->addWidget(m_chkShowTrails);

    layout->addWidget(optionsGroup);

    // ADD CHARTS GROUP HERE
    QGroupBox *chartsGroup = createStyledGroup("Charts & Analysis");
    QVBoxLayout *chartsLayout = new QVBoxLayout(chartsGroup);
    chartsLayout->setSpacing(10);

    QPushButton *btnCharts = createStyledButton("Open Charts", "📊", QColor(139, 92, 246));
    connect(btnCharts, &QPushButton::clicked, this, &CConfigPanelWidget::chartsRequested);
    chartsLayout->addWidget(btnCharts);

    layout->addWidget(chartsGroup);

    layout->addStretch();

    m_tabWidget->addTab(trackTab, "🎯\nTracks");
}

void CConfigPanelWidget::createDisplayTab()
{
    QWidget *displayTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(displayTab);
    layout->setSpacing(12);
    layout->setContentsMargins(12, 12, 12, 12);

    // Opacity Group
    QGroupBox *opacityGroup = createStyledGroup("Map Opacity");
    QGridLayout *opacityLayout = new QGridLayout(opacityGroup);
    opacityLayout->setSpacing(10);

    QLabel *lblOpacity = new QLabel("Opacity:");
    lblOpacity->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    opacityLayout->addWidget(lblOpacity, 0, 0);

    m_sliderOpacity = new QSlider(Qt::Horizontal);
    m_sliderOpacity->setRange(0, 100);
    m_sliderOpacity->setValue(100);
    m_sliderOpacity->setStyleSheet(
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
    opacityLayout->addWidget(m_sliderOpacity, 0, 1);

    m_spinOpacity = new QSpinBox();
    m_spinOpacity->setRange(0, 100);
    m_spinOpacity->setValue(100);
    m_spinOpacity->setSuffix(" %");
    m_spinOpacity->setMaximumWidth(70);
    m_spinOpacity->setStyleSheet(
        "QSpinBox {"
        "   background-color: #1a202c;"
        "   color: #ffffff;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 6px;"
        "   padding: 4px;"
        "}"
    );
    opacityLayout->addWidget(m_spinOpacity, 0, 2);

    connect(m_sliderOpacity, &QSlider::valueChanged, m_spinOpacity, &QSpinBox::setValue);
    connect(m_spinOpacity, QOverload<int>::of(&QSpinBox::valueChanged), m_sliderOpacity, &QSlider::setValue);
    connect(m_sliderOpacity, &QSlider::valueChanged, this, &CConfigPanelWidget::opacityChanged);

    layout->addWidget(opacityGroup);

    // Theme Group
    QGroupBox *themeGroup = createStyledGroup("Color Theme");
    QVBoxLayout *themeLayout = new QVBoxLayout(themeGroup);
    themeLayout->setSpacing(10);

    QLabel *lblTheme = new QLabel("Select Theme:");
    lblTheme->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    themeLayout->addWidget(lblTheme);

    m_comboTheme = new QComboBox();
    m_comboTheme->addItems({"🌙 Dark Mode", "☀️ Light Mode", "🌆 Twilight", "🌊 Ocean", "🌲 Forest"});
    m_comboTheme->setStyleSheet(
        "QComboBox {"
        "   background-color: #1a202c;"
        "   color: #ffffff;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 8px;"
        "   padding: 10px;"
        "   font-size: 12px;"
        "}"
        "QComboBox:hover {"
        "   border: 2px solid #667eea;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "   padding-right: 10px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #2d3748;"
        "   color: #ffffff;"
        "   selection-background-color: #667eea;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 6px;"
        "}"
    );
    themeLayout->addWidget(m_comboTheme);

    layout->addWidget(themeGroup);

    // Rendering Options
    QGroupBox *renderGroup = createStyledGroup("Rendering");
    QVBoxLayout *renderLayout = new QVBoxLayout(renderGroup);
    renderLayout->setSpacing(8);

    QString checkboxStyle =
        "QCheckBox {"
        "   color: #ffffff;"
        "   font-size: 12px;"
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
        "   background-color: #667eea;"
        "   border: 2px solid #667eea;"
        "}"
        "QCheckBox::indicator:hover {"
        "   border: 2px solid #667eea;"
        "}";

    m_chkAntialiasing = new QCheckBox("Enable Antialiasing");
    m_chkAntialiasing->setChecked(true);
    m_chkAntialiasing->setStyleSheet(checkboxStyle);
    renderLayout->addWidget(m_chkAntialiasing);

    m_chkSmoothPan = new QCheckBox("Smooth Pan/Zoom");
    m_chkSmoothPan->setChecked(true);
    m_chkSmoothPan->setStyleSheet(checkboxStyle);
    renderLayout->addWidget(m_chkSmoothPan);

    layout->addWidget(renderGroup);
    layout->addStretch();

    m_tabWidget->addTab(displayTab, "🎨\nDisplay");
}

void CConfigPanelWidget::createSystemTab()
{
    QWidget *systemTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(systemTab);
    layout->setSpacing(12);
    layout->setContentsMargins(12, 12, 12, 12);

    // System Actions Group
    QGroupBox *actionsGroup = createStyledGroup("System Actions");
    QVBoxLayout *actionsLayout = new QVBoxLayout(actionsGroup);
    actionsLayout->setSpacing(10);

    m_btnFlush = createStyledButton("Flush Data", "🔄", QColor(251, 146, 60));
    connect(m_btnFlush, &QPushButton::clicked, this, &CConfigPanelWidget::flushRequested);
    actionsLayout->addWidget(m_btnFlush);

    m_btnResetSettings = createStyledButton("Reset Settings", "⚙️", QColor(156, 163, 175));
    actionsLayout->addWidget(m_btnResetSettings);

    m_btnExit = createStyledButton("Exit Application", "🚪", QColor(239, 68, 68));
    connect(m_btnExit, &QPushButton::clicked, this, &CConfigPanelWidget::exitRequested);
    actionsLayout->addWidget(m_btnExit);

    layout->addWidget(actionsGroup);

    // Auto Update Group
    QGroupBox *updateGroup = createStyledGroup("Auto Update");
    QVBoxLayout *updateLayout = new QVBoxLayout(updateGroup);
    updateLayout->setSpacing(10);

    m_chkAutoUpdate = new QCheckBox("Enable Auto Update");
    m_chkAutoUpdate->setChecked(true);
    m_chkAutoUpdate->setStyleSheet(
        "QCheckBox {"
        "   color: #ffffff;"
        "   font-size: 12px;"
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
        "   background-color: #667eea;"
        "   border: 2px solid #667eea;"
        "}"
    );
    updateLayout->addWidget(m_chkAutoUpdate);

    QHBoxLayout *intervalLayout = new QHBoxLayout();
    QLabel *lblInterval = new QLabel("Update Interval:");
    lblInterval->setStyleSheet("color: #e2e8f0; font-size: 12px;");
    intervalLayout->addWidget(lblInterval);

    m_spinUpdateInterval = new QSpinBox();
    m_spinUpdateInterval->setRange(100, 5000);
    m_spinUpdateInterval->setValue(1000);
    m_spinUpdateInterval->setSuffix(" ms");
    m_spinUpdateInterval->setStyleSheet(
        "QSpinBox {"
        "   background-color: #1a202c;"
        "   color: #ffffff;"
        "   border: 2px solid #4a5568;"
        "   border-radius: 6px;"
        "   padding: 8px;"
        "}"
    );
    intervalLayout->addWidget(m_spinUpdateInterval);
    updateLayout->addLayout(intervalLayout);

    layout->addWidget(updateGroup);

    // Info Display
    QGroupBox *infoGroup = createStyledGroup("System Information");
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);
    infoLayout->setSpacing(8);

    QLabel *lblVersion = new QLabel("Version: 1.0.0");
    lblVersion->setStyleSheet("color: #a0aec0; font-size: 11px;");
    infoLayout->addWidget(lblVersion);

    QLabel *lblQt = new QLabel("Qt Version: " + QString(QT_VERSION_STR));
    lblQt->setStyleSheet("color: #a0aec0; font-size: 11px;");
    infoLayout->addWidget(lblQt);

    QLabel *lblBuild = new QLabel("Build: Release");
    lblBuild->setStyleSheet("color: #a0aec0; font-size: 11px;");
    infoLayout->addWidget(lblBuild);

    layout->addWidget(infoGroup);
    layout->addStretch();

    m_tabWidget->addTab(systemTab, "⚙️\nSystem");
}

void CConfigPanelWidget::applyRichStyle()
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

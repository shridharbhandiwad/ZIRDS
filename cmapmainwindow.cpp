#include "cmapmainwindow.h"
#include "ui_cmapmainwindow.h"
#include "cppiwindow.h"
#include "ccontrolswindow.h"
#include "cdatawarehouse.h"
#include <QFileDialog>
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QGuiApplication>
#include <QScreen>

CMapMainWindow::CMapMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMapMainWindow),
    m_ppiWindow(nullptr),
    m_controlsWindow(nullptr),
    m_isDualMonitor(false),
    m_isFullscreen(false),
    m_settings(new QSettings("RadarDisplay", "MainWindow", this))
{
    ui->setupUi(this);
    setWindowTitle("🎯 ZIRDS - Window Manager");
    setWindowIcon(QIcon(":/images/resources/zoppler_logo.png"));
    
    setupMonitors();
    setupDualWindows();
    applyLightTheme();
    connectWindowSignals();
    connectManagerButtons();
    
    loadWindowSettings();
    
    // Update status
    ui->statusBar->showMessage(QString("Ready | Monitors: %1 | Dual Monitor: %2")
                              .arg(m_screens.count())
                              .arg(m_isDualMonitor ? "Available" : "Not Available"));
    
    // Auto-arrange for dual monitor if available
    if (m_isDualMonitor) {
        QTimer::singleShot(500, [this]() {
            onArrangeDualMonitor();
            // Ensure PPI stays on top after dual monitor arrangement
            QTimer::singleShot(200, [this]() {
                m_ppiWindow->raise();
                m_ppiWindow->activateWindow();
            });
        });
    } else {
        // For single monitor, ensure windows are maximized and PPI is on front
        QTimer::singleShot(500, [this]() {
            m_ppiWindow->showMaximized();
            m_controlsWindow->showMaximized();
            m_ppiWindow->raise();
            m_ppiWindow->activateWindow();
        });
    }
}

CMapMainWindow::~CMapMainWindow()
{
    saveWindowSettings();
    delete ui;
}

void CMapMainWindow::setupDualWindows()
{
    // Create PPI window (Map + Track Table)
    m_ppiWindow = new CPPIWindow();
    m_ppiWindow->setWindowTitle("🎯 ZIRDS - PPI Display | © Zoppler Systems");
    
    // Create Controls window (All control panels)
    m_controlsWindow = new CControlsWindow();
    m_controlsWindow->setWindowTitle("🎛️ ZIRDS - Control Center | © Zoppler Systems");
    
    // Show both windows maximized
    m_ppiWindow->showMaximized();
    m_controlsWindow->showMaximized();
    
    // Set PPI window as front (welcome window)
    m_ppiWindow->raise();
    m_ppiWindow->activateWindow();
}

void CMapMainWindow::connectWindowSignals()
{
    // Connect PPI window signals
    connect(m_ppiWindow, &CPPIWindow::trackSelected,
            this, &CMapMainWindow::onTrackSelected);
    connect(m_ppiWindow, &CPPIWindow::trackDoubleClicked,
            this, &CMapMainWindow::onTrackDoubleClicked);
    connect(m_ppiWindow, &CPPIWindow::mapHomeRequested,
            this, &CMapMainWindow::onMapHomeRequested);
    connect(m_ppiWindow, &CPPIWindow::newMapRequested,
            this, &CMapMainWindow::onNewMapRequested);
    
    // Connect Controls window signals
    connect(m_controlsWindow, &CControlsWindow::mapHomeRequested,
            this, &CMapMainWindow::onMapHomeRequested);
    connect(m_controlsWindow, &CControlsWindow::openMapsRequested,
            this, &CMapMainWindow::onOpenMapsRequested);
    connect(m_controlsWindow, &CControlsWindow::flushRequested,
            this, &CMapMainWindow::onFlushRequested);
    connect(m_controlsWindow, &CControlsWindow::exitRequested,
            this, &CMapMainWindow::onExitRequested);
    connect(m_controlsWindow, &CControlsWindow::zoomChanged,
            this, &CMapMainWindow::onZoomChanged);
    connect(m_controlsWindow, &CControlsWindow::opacityChanged,
            this, &CMapMainWindow::onOpacityChanged);
    connect(m_controlsWindow, &CControlsWindow::trackSizeChanged,
            this, &CMapMainWindow::onTrackSizeChanged);
    connect(m_controlsWindow, &CControlsWindow::trackFilterChanged,
            this, &CMapMainWindow::onTrackFilterChanged);
    connect(m_controlsWindow, &CControlsWindow::animationSpeedChanged,
            this, &CMapMainWindow::onAnimationSpeedChanged);
    connect(m_controlsWindow, &CControlsWindow::gridVisibilityChanged,
            this, &CMapMainWindow::onGridVisibilityChanged);
    connect(m_controlsWindow, &CControlsWindow::compassVisibilityChanged,
            this, &CMapMainWindow::onCompassVisibilityChanged);
    connect(m_controlsWindow, &CControlsWindow::chartsRequested,
            this, &CMapMainWindow::onChartsRequested);
    
    // Forward track selection to analytics widget
    connect(this, &CMapMainWindow::onTrackSelected,
            m_controlsWindow->getAnalyticsWidget(), &CAnalyticsWidget::onTrackSelected);
}

void CMapMainWindow::connectManagerButtons()
{
    // Connect UI buttons to slots
    connect(ui->showPPIButton, &QPushButton::clicked, this, &CMapMainWindow::onShowPPIWindow);
    connect(ui->showControlsButton, &QPushButton::clicked, this, &CMapMainWindow::onShowControlsWindow);
    connect(ui->arrangeDualButton, &QPushButton::clicked, this, &CMapMainWindow::onArrangeDualMonitor);
    connect(ui->fullscreenButton, &QPushButton::clicked, this, &CMapMainWindow::onToggleFullscreen);
}

void CMapMainWindow::applyLightTheme()
{
    // Light theme for main window manager
    setStyleSheet(
        "CMapMainWindow {"
        "   background-color: #f8fafc;"
        "   color: #1e293b;"
        "}"
        "QWidget {"
        "   background-color: #ffffff;"
        "   color: #1e293b;"
        "   font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #3b82f6, stop:1 #2563eb);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 8px 16px;"
        "   font-weight: 600;"
        "   font-size: 12px;"
        "   min-height: 28px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2563eb, stop:1 #1d4ed8);"
        "}"
        "QLabel {"
        "   color: #334155;"
        "   font-weight: 500;"
        "}"
        "QStatusBar {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #f8fafc, stop:1 #e2e8f0);"
        "   color: #334155;"
        "   border-top: 2px solid #3b82f6;"
        "   font-size: 12px;"
        "   font-weight: 500;"
        "   padding: 6px;"
        "}"
    );
}

void CMapMainWindow::setupMonitors()
{
    m_screens = QGuiApplication::screens();
    m_isDualMonitor = m_screens.count() >= 2;
    
    qDebug() << "Detected" << m_screens.count() << "monitors";
    for (int i = 0; i < m_screens.count(); ++i) {
        QScreen *screen = m_screens[i];
        qDebug() << "Monitor" << i << ":" << screen->name() 
                 << "Size:" << screen->size() 
                 << "Geometry:" << screen->geometry();
    }
}

void CMapMainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_F1:
        onShowPPIWindow();
        break;
    case Qt::Key_F2:
        onShowControlsWindow();
        break;
    case Qt::Key_F3:
        onArrangeDualMonitor();
        break;
    case Qt::Key_F4:
        onToggleFullscreen();
        break;
    case Qt::Key_Escape:
        if (m_isFullscreen) {
            onToggleFullscreen();
        }
        break;
    }
    QMainWindow::keyPressEvent(event);
}

void CMapMainWindow::closeEvent(QCloseEvent *event)
{
    // Close all windows when main window closes
    if (m_ppiWindow) {
        m_ppiWindow->close();
    }
    if (m_controlsWindow) {
        m_controlsWindow->close();
    }
    
    saveWindowSettings();
    event->accept();
}

// Window management slots
void CMapMainWindow::onShowPPIWindow()
{
    if (m_ppiWindow) {
        m_ppiWindow->show();
        m_ppiWindow->raise();
        m_ppiWindow->activateWindow();
    }
}

void CMapMainWindow::onShowControlsWindow()
{
    if (m_controlsWindow) {
        m_controlsWindow->show();
        m_controlsWindow->raise();
        m_controlsWindow->activateWindow();
    }
}

void CMapMainWindow::onArrangeDualMonitor()
{
    if (!m_isDualMonitor || m_screens.count() < 2) {
        QMessageBox::information(this, "Dual Monitor", 
            "Dual monitor setup requires at least 2 displays.\n"
            "Currently detected: " + QString::number(m_screens.count()) + " display(s)");
        return;
    }
    
    // Arrange PPI window on first monitor
    QScreen *primaryScreen = m_screens[0];
    QRect primaryGeometry = primaryScreen->availableGeometry();
    m_ppiWindow->setGeometry(primaryGeometry);
    m_ppiWindow->showMaximized();
    
    // Arrange Controls window on second monitor
    QScreen *secondaryScreen = m_screens[1];
    QRect secondaryGeometry = secondaryScreen->availableGeometry();
    m_controlsWindow->setGeometry(secondaryGeometry);
    m_controlsWindow->showMaximized();
    
    qDebug() << "Arranged windows for dual monitor setup";
}

void CMapMainWindow::onToggleFullscreen()
{
    m_isFullscreen = !m_isFullscreen;
    
    if (m_isFullscreen) {
        if (m_ppiWindow) m_ppiWindow->showFullScreen();
        if (m_controlsWindow) m_controlsWindow->showFullScreen();
    } else {
        if (m_ppiWindow) m_ppiWindow->showNormal();
        if (m_controlsWindow) m_controlsWindow->showNormal();
    }
    
    qDebug() << "Fullscreen mode:" << m_isFullscreen;
}

// Inter-window communication slots
void CMapMainWindow::onTrackSelected(int trackId)
{
    qDebug() << "Track selected:" << trackId;
    
    // Forward to analytics widget in controls window
    if (m_controlsWindow && m_controlsWindow->getAnalyticsWidget()) {
        m_controlsWindow->getAnalyticsWidget()->onTrackSelected(trackId);
    }
}

void CMapMainWindow::onTrackDoubleClicked(int trackId)
{
    qDebug() << "Track double-clicked:" << trackId;
    // Track focusing is handled by individual windows
}

void CMapMainWindow::onMapHomeRequested()
{
    if (m_ppiWindow && m_ppiWindow->getMapCanvas()) {
        m_ppiWindow->getMapCanvas()->mapHome();
    }
    qDebug() << "Map home requested";
}

void CMapMainWindow::onNewMapRequested(const QString& mapPath)
{
    qDebug() << "New map requested:" << mapPath;
    // Map loading is handled by PPI window
}

// Control panel forwarding slots
void CMapMainWindow::onOpenMapsRequested()
{
    QString filter = "Map Files (*.tif *.tiff *.png *.bmp *.jpg *.jpeg *.TIF *.TIFF *.PNG *.BMP *.JPG *.JPEG)";
    QString mapPath = QFileDialog::getOpenFileName(
        this,
        "Select Map File",
        QString(),
        filter
    );
    
    if (!mapPath.isEmpty() && m_ppiWindow && m_ppiWindow->getMapCanvas()) {
        m_ppiWindow->getMapCanvas()->importRasterMap(mapPath);
        qDebug() << "Map loaded:" << mapPath;
    }
}

void CMapMainWindow::onFlushRequested()
{
    qDebug() << "Flush requested";
    // TODO: Implement flush functionality
}

void CMapMainWindow::onExitRequested()
{
    QApplication::quit();
}

void CMapMainWindow::onZoomChanged(double zoom)
{
    qDebug() << "Zoom changed to:" << zoom;
    // TODO: Implement zoom change
}

void CMapMainWindow::onOpacityChanged(int opacity)
{
    qDebug() << "Opacity changed to:" << opacity;
    // TODO: Implement opacity change
}

void CMapMainWindow::onTrackSizeChanged(int size)
{
    qDebug() << "Track size changed to:" << size;
    // TODO: Implement track size change
}

void CMapMainWindow::onTrackFilterChanged(bool showFriend, bool showHostile, bool showUnknown)
{
    qDebug() << "Track filter changed - Friend:" << showFriend
             << "Hostile:" << showHostile << "Unknown:" << showUnknown;
    // TODO: Implement track filtering
}

void CMapMainWindow::onAnimationSpeedChanged(int speed)
{
    qDebug() << "Animation speed changed to:" << speed;
    // TODO: Implement animation speed change
}

void CMapMainWindow::onGridVisibilityChanged(bool visible)
{
    qDebug() << "Grid visibility:" << visible;
    // TODO: Implement grid visibility
}

void CMapMainWindow::onCompassVisibilityChanged(bool visible)
{
    qDebug() << "Compass visibility:" << visible;
    // TODO: Implement compass visibility
}

void CMapMainWindow::onChartsRequested()
{
    if (m_controlsWindow && m_controlsWindow->getChartsWidget()) {
        // Switch to charts tab in controls window
        qDebug() << "Charts requested - switching to charts tab";
    }
}

void CMapMainWindow::saveWindowSettings()
{
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("windowState", saveState());
    m_settings->setValue("isDualMonitor", m_isDualMonitor);
    m_settings->setValue("isFullscreen", m_isFullscreen);
    
    if (m_ppiWindow) {
        m_settings->setValue("ppiGeometry", m_ppiWindow->saveGeometry());
        m_settings->setValue("ppiState", m_ppiWindow->saveState());
    }
    
    if (m_controlsWindow) {
        m_settings->setValue("controlsGeometry", m_controlsWindow->saveGeometry());
        m_settings->setValue("controlsState", m_controlsWindow->saveState());
    }
}

void CMapMainWindow::loadWindowSettings()
{
    restoreGeometry(m_settings->value("geometry").toByteArray());
    restoreState(m_settings->value("windowState").toByteArray());
    
    m_isDualMonitor = m_settings->value("isDualMonitor", m_isDualMonitor).toBool();
    m_isFullscreen = m_settings->value("isFullscreen", false).toBool();
    
    if (m_ppiWindow) {
        m_ppiWindow->restoreGeometry(m_settings->value("ppiGeometry").toByteArray());
        m_ppiWindow->restoreState(m_settings->value("ppiState").toByteArray());
    }
    
    if (m_controlsWindow) {
        m_controlsWindow->restoreGeometry(m_settings->value("controlsGeometry").toByteArray());
        m_controlsWindow->restoreState(m_settings->value("controlsState").toByteArray());
    }
}

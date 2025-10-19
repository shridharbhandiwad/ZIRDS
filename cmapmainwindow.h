//#ifndef CMAPMAINWINDOW_H
//#define CMAPMAINWINDOW_H

//#include <QMainWindow>
//#include <QTimer>

//namespace Ui {
//class CMapMainWindow;
//}

//class CMapMainWindow : public QMainWindow
//{
//    Q_OBJECT

//public:
//    explicit CMapMainWindow(QWidget *parent = nullptr);
//    ~CMapMainWindow();

//protected:
//    void keyPressEvent(QKeyEvent *event);

//private slots:
//    void slotMouseRead(QString mouseRead);
//    void on_pushButton_MapHome_clicked();

//    void on_pushButton_OpenMaps_clicked();

//    void on_pushButton_FLUSH_clicked();

//    void on_pushButton_EXIT_clicked();

//    void updateTrackTable();
//private:
//    Ui::CMapMainWindow *ui;
//    QTimer _m_updateTimer;
//};

//#endif // CMAPMAINWINDOW_H



//#ifndef CMAPMAINWINDOW_H
//#define CMAPMAINWINDOW_H

//#include <QMainWindow>
//#include <QTimer>
//#include <QKeyEvent>

//// Forward declarations
//class CTrackTableWidget;

//namespace Ui {
//class CMapMainWindow;
//}

//class CMapMainWindow : public QMainWindow
//{
//    Q_OBJECT

//public:
//    explicit CMapMainWindow(QWidget *parent = nullptr);
//    ~CMapMainWindow();

//protected:
//    /**
//     * @brief Handle keyboard shortcuts
//     * @param event Key event
//     *
//     * Shortcuts:
//     * - T: Toggle track table visibility
//     * - H: Map home view
//     * - F: Toggle controls
//     */
//    void keyPressEvent(QKeyEvent *event) override;

//private slots:
//    /**
//     * @brief Update status bar with mouse position
//     * @param mouseRead Mouse position string
//     */
//    void slotMouseRead(QString mouseRead);

//    /**
//     * @brief Reset map to home view
//     */
//    void on_pushButton_MapHome_clicked();

//    /**
//     * @brief Open raster map file dialog
//     */
//    void on_pushButton_OpenMaps_clicked();

//    /**
//     * @brief Flush/clear operation
//     */
//    void on_pushButton_FLUSH_clicked();

//    /**
//     * @brief Exit application
//     */
//    void on_pushButton_EXIT_clicked();

//    /**
//     * @brief Update track table with current data
//     * @note This is kept for backward compatibility
//     * The CTrackTableWidget handles updates automatically
//     */
//    void updateTrackTable();

//    /**
//     * @brief Handle track selection from table
//     * @param trackId Selected track ID
//     */
//    void onTrackSelected(int trackId);

//    /**
//     * @brief Handle track double-click from table
//     * @param trackId Double-clicked track ID
//     * Centers map on the selected track
//     */
//    void onTrackDoubleClicked(int trackId);

//private:
//    Ui::CMapMainWindow *ui;
//    QTimer _m_updateTimer;

//    /**
//     * @brief Rich dockable track table widget
//     */
//    CTrackTableWidget *m_trackTable;

//    /**
//     * @brief Initialize and setup the dockable track table
//     */
//    void setupTrackTable();

//    /**
//     * @brief Apply modern dark theme to the application
//     */
//    void applyModernTheme();
//};

//#endif // CMAPMAINWINDOW_H



#ifndef CMAPMAINWINDOW_H
#define CMAPMAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QSettings>
#include "MapDisplay/cmapcanvas.h"


// Forward declarations
class CPPIWindow;
class CControlsWindow;
class CMapCanvas;

namespace Ui {
class CMapMainWindow;
}

/**
 * @brief Main Window Manager for Dual Monitor Setup
 * 
 * This class manages two separate windows:
 * 1. PPI Window: Map canvas + Track table
 * 2. Controls Window: All control panels and widgets
 * 
 * Designed for dual monitor radar display systems.
 */
class CMapMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMapMainWindow(QWidget *parent = nullptr);
    ~CMapMainWindow();

protected:
    /**
     * @brief Handle keyboard shortcuts for window management
     * @param event Key event
     *
     * Shortcuts:
     * - F1: Show/Hide PPI Window
     * - F2: Show/Hide Controls Window
     * - F3: Arrange windows for dual monitor
     * - F4: Toggle fullscreen mode
     * - ESC: Exit fullscreen
     */
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Window management slots
    void onShowPPIWindow();
    void onShowControlsWindow();
    void onArrangeDualMonitor();
    void onToggleFullscreen();
    
    // Inter-window communication slots
    void onTrackSelected(int trackId);
    void onTrackDoubleClicked(int trackId);
    void onMapHomeRequested();
    void onNewMapRequested(const QString& mapPath);
    
    // Control panel forwarding slots
    void onOpenMapsRequested();
    void onFlushRequested();
    void onExitRequested();
    void onZoomChanged(double zoom);
    void onOpacityChanged(int opacity);
    void onTrackSizeChanged(int size);
    void onTrackFilterChanged(bool showFriend, bool showHostile, bool showUnknown);
    void onAnimationSpeedChanged(int speed);
    void onGridVisibilityChanged(bool visible);
    void onCompassVisibilityChanged(bool visible);
    void onChartsRequested();
    void applyLightTheme();

private:
    /**
     * @brief Setup dual window architecture
     */
    void setupDualWindows();
    
    /**
     * @brief Connect signals between windows
     */
    void connectWindowSignals();
    
    /**
     * @brief Connect manager window buttons
     */
    void connectManagerButtons();
    
    /**
     * @brief Detect and setup monitors
     */
    void setupMonitors();
    
    /**
     * @brief Save window positions and states
     */
    void saveWindowSettings();
    
    /**
     * @brief Load window positions and states
     */
    void loadWindowSettings();

    // UI
    Ui::CMapMainWindow *ui;
    
    // Window instances
    CPPIWindow *m_ppiWindow;
    CControlsWindow *m_controlsWindow;
    
    // Monitor information
    QList<QScreen*> m_screens;
    bool m_isDualMonitor;
    bool m_isFullscreen;
    
    // Settings
    QSettings *m_settings;
};

#endif // CMAPMAINWINDOW_H

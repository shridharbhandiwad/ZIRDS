#ifndef CPPIWINDOW_H
#define CPPIWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSplitter>
#include "MapDisplay/canalyticswidget.h"

// Forward declarations
class CMapCanvas;
class CTrackTableWidget;
class CAnalyticsWidget;

namespace Ui {
class CPPIWindow;
}

/**
 * @brief PPI (Plan Position Indicator) Window for dual monitor setup
 *
 * This window contains:
 * - Map canvas with PPI display
 * - Track table widget
 * - Settings toolbar with map controls
 * - Right-click context menu for tracks
 */
class CPPIWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CPPIWindow(QWidget *parent = nullptr);
    ~CPPIWindow();

    /**
     * @brief Get the map canvas widget
     * @return Pointer to map canvas
     */
    CMapCanvas* getMapCanvas() const { return m_mapCanvas; }

    /**
     * @brief Get the track table widget
     * @return Pointer to track table
     */
    CTrackTableWidget* getTrackTable() const { return m_trackTable; }

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // Settings toolbar actions
    void onLoadNewMap();
    void onDisableMap();
    void onZoomFitToScreen();
    void onMapHome();
    void onToggleGrid();
    void onToggleCompass();
    void onToggleTrackTable();
    void onSettings();

    // Track context menu actions
    void onFocusTrack();
    void onDeleteTrack();
    void onLoadTrackImage();
    void onToggleTrackHistory();
    void onHighlightTrack();

    // Track table interactions
    void onTrackSelected(int trackId);
    void onTrackDoubleClicked(int trackId);
    void onTrackRightClicked(int trackId, const QPoint& globalPos);

    // Status updates
    void updateStatusBar();
    
    // Logo setup
    void setupZopplerLogo();

signals:
    void trackSelected(int trackId);
    void trackDoubleClicked(int trackId);
    void mapHomeRequested();
    void newMapRequested(const QString& mapPath);

private:
    void setupUI();
    void setupSettingsToolbar();
    void setupTrackTable();
    void setupMapCanvas();
    void applyLightTheme();
    void createTrackContextMenu();
    void saveSettings();
    void loadSettings();

    // UI components
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_settingsLayout;

    // Settings toolbar
    QPushButton *m_loadMapBtn;
    QPushButton *m_disableMapBtn;
    QPushButton *m_zoomFitBtn;
    QPushButton *m_homeBtn;
    QPushButton *m_gridBtn;
    QPushButton *m_compassBtn;
    QPushButton *m_toggleTableBtn;
    QPushButton *m_settingsBtn;
    QLabel *m_statusLabel;
    
    // Zoppler logo
    QLabel *m_logoLabel;

    // Main components
    QSplitter *m_splitter;
    CMapCanvas *m_mapCanvas;
    CTrackTableWidget *m_trackTable;

    // Context menu
    QMenu *m_trackContextMenu;
    QAction *m_focusTrackAction;
    QAction *m_deleteTrackAction;
    QAction *m_loadImageAction;
    QAction *m_toggleHistoryAction;
    QAction *m_highlightAction;

    // State
    int m_selectedTrackId;
    bool m_gridVisible;
    bool m_compassVisible;
    bool m_mapEnabled;
    int m_maxHistoryPoints;

    // Timer for status updates
    QTimer *m_statusTimer;

    // Settings
    QSettings *m_settings;
};

#endif // CPPIWINDOW_H

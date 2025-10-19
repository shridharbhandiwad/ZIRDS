#ifndef CCONTROLSWINDOW_H
#define CCONTROLSWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QSettings>

// Forward declarations
class CConfigPanelWidget;
class CChartsWidget;
class CInterfacesPanelWidget;
class CAnalyticsWidget;
class CSimulationWidget;
class CRecordingWidget;
class CHealthMonitorWidget;
class CPredictiveMaintenanceWidget;

namespace Ui {
class CControlsWindow;
}

/**
 * @brief Controls Window for dual monitor setup
 * 
 * This window contains all control panels and widgets:
 * - Configuration Panel
 * - Charts Widget
 * - Interfaces Panel
 * - Analytics Widget
 * - Simulation Widget
 * - Recording Widget
 * - Health Monitor Widget
 * - Predictive Maintenance Widget
 */
class CControlsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CControlsWindow(QWidget *parent = nullptr);
    ~CControlsWindow();

    // Getters for widgets
    CConfigPanelWidget* getConfigPanel() const { return m_configPanel; }
    CChartsWidget* getChartsWidget() const { return m_chartsWidget; }
    CInterfacesPanelWidget* getInterfacesPanel() const { return m_interfacesPanel; }
    CAnalyticsWidget* getAnalyticsWidget() const { return m_analyticsWidget; }
    CSimulationWidget* getSimulationWidget() const { return m_simulationWidget; }
    CRecordingWidget* getRecordingWidget() const { return m_recordingWidget; }
    CHealthMonitorWidget* getHealthMonitorWidget() const { return m_healthMonitorWidget; }
    CPredictiveMaintenanceWidget* getPredictiveMaintenanceWidget() const { return m_predictiveMaintenanceWidget; }

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onTabChanged(int index);
    void updateStatusBar();

signals:
    // Config panel signals
    void mapHomeRequested();
    void openMapsRequested();
    void flushRequested();
    void exitRequested();
    void zoomChanged(double zoom);
    void opacityChanged(int opacity);
    void trackSizeChanged(int size);
    void trackFilterChanged(bool showFriend, bool showHostile, bool showUnknown);
    void animationSpeedChanged(int speed);
    void gridVisibilityChanged(bool visible);
    void compassVisibilityChanged(bool visible);
    void chartsRequested();
    void historyLimitChanged(int limit);

private:
    void setupUI();
    void setupTabs();
    void applyLightTheme();
    void connectSignals();
    void saveSettings();
    void loadSettings();
    
    // UI components
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QTabWidget *m_tabWidget;
    
    // Tab widgets
    CConfigPanelWidget *m_configPanel;
    CChartsWidget *m_chartsWidget;
    CInterfacesPanelWidget *m_interfacesPanel;
    CAnalyticsWidget *m_analyticsWidget;
    CSimulationWidget *m_simulationWidget;
    CRecordingWidget *m_recordingWidget;
    CHealthMonitorWidget *m_healthMonitorWidget;
    CPredictiveMaintenanceWidget *m_predictiveMaintenanceWidget;
    
    // Status timer
    QTimer *m_statusTimer;
    
    // Settings
    QSettings *m_settings;
    
    // Tab indices for easy access
    enum TabIndex {
        TAB_CONFIG = 0,
        TAB_INTERFACES = 1,
        TAB_SIMULATION = 2,
        TAB_RECORDING = 3,
        TAB_ANALYTICS = 4,
        TAB_CHARTS = 5,
        TAB_HEALTH = 6,
        TAB_MAINTENANCE = 7
    };
};

#endif // CCONTROLSWINDOW_H
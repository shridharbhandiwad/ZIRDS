#ifndef CHEALTHMONITORWIDGET_H
#define CHEALTHMONITORWIDGET_H

#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QTableWidget>
#include <QMap>
#include <QDateTime>
#include <QFrame>

/**
 * @brief Widget for monitoring system health status
 * 
 * Monitors:
 * - Hardware modules (Radar, Servo, Power, Communication)
 * - Software modules (Display, Data Processing, Network)
 * - Performance metrics (CPU, Memory, Disk, Network)
 * - System diagnostics and alerts
 */
class CHealthMonitorWidget : public QDockWidget
{
    Q_OBJECT

public:
    enum HealthStatus {
        HEALTHY,
        WARNING,
        CRITICAL,
        OFFLINE,
        MAINTENANCE
    };

    struct ModuleHealth {
        QString name;
        HealthStatus status;
        int healthPercentage;
        QString lastUpdate;
        QString statusMessage;
        QMap<QString, QString> metrics;
    };

    explicit CHealthMonitorWidget(QWidget *parent = nullptr);
    ~CHealthMonitorWidget();

public slots:
    void updateHealth();
    void runDiagnostics();
    void resetAlarms();

private slots:
    void onModuleClicked(const QString &moduleName);
    void exportHealthReport();

private:
    void setupUI();
    void createOverviewSection();
    void createHardwareSection();
    void createSoftwareSection();
    void createPerformanceSection();
    void createAlertsSection();
    void applyModernStyle();
    
    void updateModuleStatus(const QString &moduleName);
    void simulateHealthData();
    QColor getHealthColor(HealthStatus status);
    QString getHealthText(HealthStatus status);
    QFrame* createModuleCard(const QString &moduleName, const QString &icon);
    
    // UI Components
    QWidget *m_mainWidget;
    QVBoxLayout *m_mainLayout;
    
    // Overview Section
    QGroupBox *m_overviewGroup;
    QLabel *m_systemHealthLabel;
    QProgressBar *m_systemHealthBar;
    QLabel *m_uptimeLabel;
    QLabel *m_lastCheckLabel;
    QPushButton *m_diagnosticsButton;
    QPushButton *m_exportButton;
    
    // Hardware Section
    QGroupBox *m_hardwareGroup;
    QMap<QString, QFrame*> m_hardwareCards;
    QMap<QString, QLabel*> m_hardwareStatusLabels;
    QMap<QString, QProgressBar*> m_hardwareHealthBars;
    
    // Software Section
    QGroupBox *m_softwareGroup;
    QMap<QString, QFrame*> m_softwareCards;
    QMap<QString, QLabel*> m_softwareStatusLabels;
    QMap<QString, QProgressBar*> m_softwareHealthBars;
    
    // Performance Section
    QGroupBox *m_performanceGroup;
    QLabel *m_cpuUsageLabel;
    QProgressBar *m_cpuBar;
    QLabel *m_memoryUsageLabel;
    QProgressBar *m_memoryBar;
    QLabel *m_diskUsageLabel;
    QProgressBar *m_diskBar;
    QLabel *m_networkUsageLabel;
    QProgressBar *m_networkBar;
    
    // Alerts Section
    QGroupBox *m_alertsGroup;
    QTableWidget *m_alertsTable;
    QPushButton *m_resetAlarmsButton;
    QLabel *m_activeAlertsLabel;
    
    // Data
    QTimer *m_updateTimer;
    QMap<QString, ModuleHealth> m_moduleHealthData;
    qint64 m_systemStartTime;
    int m_activeAlerts;
    
    // Hardware modules
    QStringList m_hardwareModules;
    
    // Software modules
    QStringList m_softwareModules;
};

#endif // CHEALTHMONITORWIDGET_H

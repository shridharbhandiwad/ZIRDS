#ifndef CPREDICTIVEMAINTENANCEWIDGET_H
#define CPREDICTIVEMAINTENANCEWIDGET_H

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
#include <QListWidget>

/**
 * @brief Widget for predictive maintenance monitoring
 * 
 * Features:
 * - Component lifetime tracking
 * - Maintenance schedule predictions
 * - Wear and tear analysis
 * - Proactive alerts for upcoming maintenance
 * - Service history tracking
 */
class CPredictiveMaintenanceWidget : public QDockWidget
{
    Q_OBJECT

public:
    enum MaintenanceStatus {
        EXCELLENT,
        GOOD,
        FAIR,
        POOR,
        SERVICE_REQUIRED,
        CRITICAL
    };

    struct ComponentStatus {
        QString name;
        QString category;
        MaintenanceStatus status;
        int lifetimePercentage;
        int daysUntilMaintenance;
        qint64 operatingHours;
        qint64 lastMaintenanceDate;
        QString recommendations;
    };

    explicit CPredictiveMaintenanceWidget(QWidget *parent = nullptr);
    ~CPredictiveMaintenanceWidget();

public slots:
    void updateStatus();
    void scheduleMaintenanceNow();
    void viewMaintenanceHistory();

private slots:
    void onComponentClicked(QListWidgetItem *item);
    void exportMaintenanceReport();

private:
    void setupUI();
    void createOverviewSection();
    void createComponentsSection();
    void createScheduleSection();
    void createRecommendationsSection();
    void applyModernStyle();
    
    void simulateMaintenanceData();
    QColor getMaintenanceColor(MaintenanceStatus status);
    QString getMaintenanceText(MaintenanceStatus status);
    QString getStatusIcon(MaintenanceStatus status);
    QListWidgetItem* createComponentItem(const ComponentStatus &component);
    
    // UI Components
    QWidget *m_mainWidget;
    QVBoxLayout *m_mainLayout;
    
    // Overview Section
    QGroupBox *m_overviewGroup;
    QLabel *m_systemConditionLabel;
    QProgressBar *m_systemConditionBar;
    QLabel *m_upcomingMaintenanceLabel;
    QLabel *m_criticalComponentsLabel;
    QPushButton *m_scheduleButton;
    QPushButton *m_historyButton;
    
    // Components Section
    QGroupBox *m_componentsGroup;
    QListWidget *m_componentsList;
    
    // Schedule Section
    QGroupBox *m_scheduleGroup;
    QTableWidget *m_scheduleTable;
    
    // Recommendations Section
    QGroupBox *m_recommendationsGroup;
    QLabel *m_recommendationsText;
    QPushButton *m_exportButton;
    
    // Data
    QTimer *m_updateTimer;
    QMap<QString, ComponentStatus> m_componentData;
    qint64 m_systemStartTime;
};

#endif // CPREDICTIVEMAINTENANCEWIDGET_H

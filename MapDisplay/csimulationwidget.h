#ifndef CSIMULATIONWIDGET_H
#define CSIMULATIONWIDGET_H

#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QTimer>
#include <QUdpSocket>
#include <QTableWidget>
#include <QProgressBar>
#include "../globalstructs.h"

/**
 * @brief Widget for simulating radar tracks and sending them via UDP
 * 
 * Features:
 * - Generate multiple tracks within configurable range (up to 10km)
 * - Distribute tracks across different azimuth angles
 * - Real-time track simulation with configurable update rates
 * - UDP transmission to the radar display system
 * - Track visualization and status monitoring
 */
class CSimulationWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CSimulationWidget(QWidget *parent = nullptr);
    ~CSimulationWidget();

public slots:
    void startSimulation();
    void stopSimulation();
    void pauseSimulation();
    void resetSimulation();

signals:
    void simulatedTrackData(stTrackRecvInfo track);

private slots:
    void updateSimulation();
    void onTrackCountChanged(int count);
    void onMaxRangeChanged(double range);
    void onUpdateRateChanged(int rate);
    void onAzimuthSpreadChanged(int spread);
    void sendTrackData(const stTrackRecvInfo &track);
    void updateTrackTable();

private:
    void setupUI();
    void createControlsSection();
    void createParametersSection();
    void createTrackListSection();
    void createStatusSection();
    void applyModernStyle();
    
    void generateInitialTracks();
    void updateTrackPositions();
    stTrackRecvInfo createRandomTrack(int trackId);
    
    // UI Components
    QWidget *m_mainWidget;
    QVBoxLayout *m_mainLayout;
    
    // Controls Section
    QGroupBox *m_controlsGroup;
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QPushButton *m_pauseButton;
    QPushButton *m_resetButton;
    QLabel *m_statusLabel;
    
    // Parameters Section
    QGroupBox *m_parametersGroup;
    QSpinBox *m_trackCountSpinBox;
    QDoubleSpinBox *m_maxRangeSpinBox;
    QSpinBox *m_updateRateSpinBox;
    QSpinBox *m_azimuthSpreadSpinBox;
    QDoubleSpinBox *m_minSpeedSpinBox;
    QDoubleSpinBox *m_maxSpeedSpinBox;
    QCheckBox *m_randomMovementCheckBox;
    QComboBox *m_identityCombo;
    
    // Track List Section
    QGroupBox *m_trackListGroup;
    QTableWidget *m_trackTable;
    
    // Status Section
    QGroupBox *m_statusGroup;
    QLabel *m_activeTracksLabel;
    QLabel *m_packetsSentLabel;
    QLabel *m_simulationTimeLabel;
    QProgressBar *m_cpuUsageBar;
    
    // Simulation Data
    QTimer *m_simulationTimer;
    QUdpSocket *m_udpSocket;
    QList<stTrackRecvInfo> m_simulatedTracks;
    
    // Configuration
    int m_trackCount;
    double m_maxRange;
    int m_updateRate;
    int m_azimuthSpread;
    double m_minSpeed;
    double m_maxSpeed;
    bool m_randomMovement;
    
    // Statistics
    bool m_isRunning;
    bool m_isPaused;
    qint64 m_packetsSent;
    qint64 m_simulationStartTime;
    
    // UDP Configuration
    static constexpr quint16 UDP_PORT = 2025;
    static constexpr const char* UDP_ADDRESS = "127.0.0.1";
};

#endif // CSIMULATIONWIDGET_H

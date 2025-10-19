#ifndef CINTERFACESPANELWIDGET_H
#define CINTERFACESPANELWIDGET_H

#include <QDockWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>

class CInterfacesPanelWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CInterfacesPanelWidget(QWidget *parent = nullptr);
    ~CInterfacesPanelWidget();

signals:
    // Servo controller signals
    void servoAzimuthChanged(double azimuth);
    void servoElevationChanged(double elevation);
    void servoSpeedChanged(int speed);
    void servoHomeRequested();
    void servoStopRequested();
    void servoCalibrationRequested();
    
    // Radar controller signals
    void radarModeChanged(int mode);
    void radarRangeChanged(double range);
    void radarGainChanged(int gain);
    void radarScanStartRequested();
    void radarScanStopRequested();
    void radarResetRequested();
    
    // Gun controller signals
    void gunAzimuthChanged(double azimuth);
    void gunElevationChanged(double elevation);
    void gunFireRequested();
    void gunSafetyToggled(bool enabled);
    void gunRoundsChanged(int rounds);
    
    // Missile controller signals
    void missileTypeChanged(int type);
    void missileTargetLocked(int trackId);
    void missileLaunchRequested();
    void missileAbortRequested();
    void missileArmToggled(bool armed);
    
    // EOTS signals
    void eotsZoomChanged(double zoom);
    void eotsBrightnessChanged(int brightness);
    void eotsContrastChanged(int contrast);
    void eotsIRModeToggled(bool enabled);
    void eotsLaserRangeRequested();
    void eotsTrackingToggled(bool enabled);
    
    // Logging signals
    void loggingStarted();
    void loggingStopped();
    void loggingCleared();
    
    // Recording signals
    void recordingStarted();
    void recordingStopped();
    void recordingPaused();
    
    // Replay signals
    void replayStarted();
    void replayStopped();
    void replayPaused();
    void replaySpeedChanged(double speed);

public slots:
    void updateServoPosition(double azimuth, double elevation);
    void updateRadarStatus(const QString &status);
    void updateGunStatus(int roundsRemaining);
    void updateMissileStatus(const QString &status);
    void appendLogMessage(const QString &message);

private:
    void setupUI();
    void createServoTab();
    void createRadarTab();
    void createGunTab();
    void createMissileTab();
    void createEOTSTab();
    void createLoggingTab();
    
    // Apply rich styling
    void applyRichStyle();
    QPushButton* createStyledButton(const QString &text, const QString &icon, const QColor &color);
    QGroupBox* createStyledGroup(const QString &title);
    QString getSliderStyle();
    QString getCheckboxStyle(const QString &color = "#667eea");
    
    QTabWidget *m_tabWidget;
    
    // Servo tab widgets
    QDoubleSpinBox *m_spinServoAz;
    QDoubleSpinBox *m_spinServoEl;
    QSlider *m_sliderServoSpeed;
    QSpinBox *m_spinServoSpeed;
    QLabel *m_lblServoAzPos;
    QLabel *m_lblServoElPos;
    QProgressBar *m_progressServo;
    QPushButton *m_btnServoHome;
    QPushButton *m_btnServoStop;
    QPushButton *m_btnServoCal;
    QCheckBox *m_chkServoAuto;
    
    // Radar tab widgets
    QComboBox *m_comboRadarMode;
    QDoubleSpinBox *m_spinRadarRange;
    QSlider *m_sliderRadarGain;
    QSpinBox *m_spinRadarGain;
    QLabel *m_lblRadarStatus;
    QPushButton *m_btnRadarStart;
    QPushButton *m_btnRadarStop;
    QPushButton *m_btnRadarReset;
    QCheckBox *m_chkRadarDoppler;
    QCheckBox *m_chkRadarMTI;
    
    // Gun tab widgets
    QDoubleSpinBox *m_spinGunAz;
    QDoubleSpinBox *m_spinGunEl;
    QSpinBox *m_spinGunRounds;
    QLabel *m_lblGunRoundsRemaining;
    QProgressBar *m_progressGunTemp;
    QPushButton *m_btnGunFire;
    QPushButton *m_btnGunCease;
    QCheckBox *m_chkGunSafety;
    QCheckBox *m_chkGunAutoTrack;
    
    // Missile tab widgets
    QComboBox *m_comboMissileType;
    QSpinBox *m_spinMissileTarget;
    QLabel *m_lblMissileStatus;
    QProgressBar *m_progressMissileLock;
    QPushButton *m_btnMissileLaunch;
    QPushButton *m_btnMissileAbort;
    QCheckBox *m_chkMissileArm;
    QCheckBox *m_chkMissileAutoEngage;
    
    // EOTS tab widgets
    QDoubleSpinBox *m_spinEOTSZoom;
    QSlider *m_sliderEOTSBrightness;
    QSpinBox *m_spinEOTSBrightness;
    QSlider *m_sliderEOTSContrast;
    QSpinBox *m_spinEOTSContrast;
    QPushButton *m_btnEOTSLaser;
    QCheckBox *m_chkEOTSIR;
    QCheckBox *m_chkEOTSTracking;
    QComboBox *m_comboEOTSMode;
    
    // Logging tab widgets
    QTextEdit *m_textLog;
    QPushButton *m_btnLogStart;
    QPushButton *m_btnLogStop;
    QPushButton *m_btnLogClear;
    QPushButton *m_btnLogSave;
    QCheckBox *m_chkLogTimestamp;
    QCheckBox *m_chkLogLevel;
    QComboBox *m_comboLogLevel;
    
    // Recording widgets
    QPushButton *m_btnRecStart;
    QPushButton *m_btnRecStop;
    QPushButton *m_btnRecPause;
    QLabel *m_lblRecStatus;
    QProgressBar *m_progressRecTime;
    
    // Replay widgets
    QPushButton *m_btnReplayLoad;
    QPushButton *m_btnReplayStart;
    QPushButton *m_btnReplayStop;
    QPushButton *m_btnReplayPause;
    QSlider *m_sliderReplaySpeed;
    QDoubleSpinBox *m_spinReplaySpeed;
    QProgressBar *m_progressReplay;
};

#endif // CINTERFACESPANELWIDGET_H

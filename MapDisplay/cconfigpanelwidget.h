#ifndef CCONFIGPANELWIDGET_H
#define CCONFIGPANELWIDGET_H

#include <QDockWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>

class CConfigPanelWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CConfigPanelWidget(QWidget *parent = nullptr);
    ~CConfigPanelWidget();

signals:
    void mapHomeRequested();
    void openMapsRequested();
    void flushRequested();
    void exitRequested();
    void zoomLevelChanged(double zoom);
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
    void createMapTab();
    void createTrackTab();
    void createDisplayTab();
    void createSystemTab();

    // Apply rich styling
    void applyRichStyle();
    QPushButton* createStyledButton(const QString &text, const QString &icon, const QColor &color);
    QGroupBox* createStyledGroup(const QString &title);

    QTabWidget *m_tabWidget;

    // Map tab widgets
    QPushButton *m_btnMapHome;
    QPushButton *m_btnOpenMaps;
    QPushButton *m_btnSaveMaps;
    QDoubleSpinBox *m_spinZoom;
    QCheckBox *m_chkGrid;
    QCheckBox *m_chkCompass;

    // Track tab widgets
    QCheckBox *m_chkShowFriend;
    QCheckBox *m_chkShowHostile;
    QCheckBox *m_chkShowUnknown;
    QSlider *m_sliderTrackSize;
    QSpinBox *m_spinTrackSize;
    QSlider *m_sliderAnimSpeed;
    QSpinBox *m_spinAnimSpeed;
    QCheckBox *m_chkShowLabels;
    QCheckBox *m_chkShowTrails;
    QSlider *m_sliderHistoryLimit;
    QSpinBox *m_spinHistoryLimit;

    // Display tab widgets
    QSlider *m_sliderOpacity;
    QSpinBox *m_spinOpacity;
    QComboBox *m_comboTheme;
    QCheckBox *m_chkAntialiasing;
    QCheckBox *m_chkSmoothPan;

    // System tab widgets
    QPushButton *m_btnFlush;
    QPushButton *m_btnExit;
    QPushButton *m_btnResetSettings;
    QCheckBox *m_chkAutoUpdate;
    QSpinBox *m_spinUpdateInterval;
};

#endif // CCONFIGPANELWIDGET_H

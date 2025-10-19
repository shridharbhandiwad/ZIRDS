#ifndef CTRACKTABLEWIDGET_H
#define CTRACKTABLEWIDGET_H

#include <QDockWidget>
#include <QTableWidget>
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QMenu>

class CTrackTableWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CTrackTableWidget(QWidget *parent = nullptr);
    ~CTrackTableWidget();

private slots:
    void updateTrackTable();
    void onTrackSelectionChanged();
    void onFilterChanged(const QString &text);
    void onIdentityFilterChanged(int index);
    void onClearAllTracks();
    void onExportTracks();
    void onTableContextMenu(const QPoint &pos);

signals:
    void trackSelected(int trackId);
    void trackDoubleClicked(int trackId);
    void trackRightClicked(int trackId, const QPoint &globalPos);
    void trackImageLoaded(int trackId, const QString &imagePath);

private:
    void setupUI();
    void applyFilters();
    QString getIdentityString(int identity);
    QColor getIdentityColor(int identity);
    void createContextMenu();

    QTableWidget *m_tableWidget;
    QTimer *m_updateTimer;
    QLineEdit *m_filterEdit;
    QComboBox *m_identityFilter;
    QPushButton *m_clearButton;
    QPushButton *m_exportButton;
    QMenu *m_contextMenu;

    QString m_currentFilter;
    int m_currentIdentityFilter;
    int m_rightClickedTrackId;
};

#endif // CTRACKTABLEWIDGET_H

#ifndef _PARKINGWIDGET_H_
#define _PARKINGWIDGET_H_

#include <QtGui>
#include <QWidget>

class ExtendedLabel;
class ExtendedTableWidget;
class ParkingInfo;
class QTableWidgetItem;

/*! \brief A simple widget to hold parkinglot informations */
class ParkingWidget : public QWidget
{
Q_OBJECT

public:
    ParkingWidget(const QString &, QWidget * parent);
    void refresh();
    void refreshTimer();
    int count() const;
signals:
    void itemClicked(const QString &);
    void itemDoubleClicked(const QString &, const QString &);
    void parkinglotClicked(const QString &);
public slots:
    void clickListener(QTableWidgetItem * i);
    void doubleClickListener(QTableWidgetItem * i);
    void headerClicked();
protected:
    int findRow(const QString &) const;
    void addRow(const QStringList & list);
    void setRow(int row, const QStringList & l);
    void removeUnparkedCalls();
private:
    QString m_parking_id;
    ExtendedTableWidget * m_table; //!< The table holding the information
    ExtendedLabel * m_header;
    QStringList m_labels;
};

#endif /* _PARKINGWIDGET_H_ */

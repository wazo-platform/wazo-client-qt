#ifndef _CONFERENCE2_CONFCHAMBER_H_
#define _CONFERENCE2_CONFCHAMBER_H_
#include <QWidget>
#include <QDebug>
#include <QAbstractTableModel>
#include <QTableView>
#include <QModelIndex>
#include <QVBoxLayout>
#include <QMap>
#include <QHeaderView>

class ConfChamberModel : public QAbstractTableModel
{
    public:
        ConfChamberModel();
        enum ColOrder {
            ID, NAME, NUMBER, SINCE, ADMIN, ACTION_KICK, ACTION_MUTE, NB_COL
        };

    private:
        void sort(int, Qt::SortOrder);
        void timerEvent(QTimerEvent *);
        int rowCount(const QModelIndex&) const;
        int columnCount(const QModelIndex&) const;
        QVariant data(const QModelIndex&, int) const;
        QVariant headerData(int , Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;
        QMap<int, QString> m_row2id;

};

class ConfChamberView : public QTableView
{
    Q_OBJECT

    public:
        ConfChamberView(QWidget *parent, ConfChamberModel *model);
    private slots:
        void onViewClick(const QModelIndex &);
    protected:
        virtual void mousePressEvent(QMouseEvent *event);
    private:
        int lastPressed;
};

class ConfChamber : public QWidget
{
    Q_OBJECT

    public:
        ConfChamber(const QString &id);
};

#endif

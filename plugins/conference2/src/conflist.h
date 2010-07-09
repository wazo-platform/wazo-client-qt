#ifndef __XLET_CONFLIST
#define __XLET_CONFLIST

#include <QLabel>
#include <QVBoxLayout>
#include <QTableView>
#include <QModelIndex>
#include <QVariant>
#include <QAbstractTableModel>
#include <QHeaderView>

#include <baseengine.h>

class ConfListModel : public QAbstractTableModel
{
    public:
        ConfListModel();

    private:
        void timerEvent(QTimerEvent *);
        int rowCount(const QModelIndex&) const;
        int columnCount(const QModelIndex&) const;
        QVariant data(const QModelIndex&, int) const;
        QVariant headerData(int , Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;

        enum RowOrder {
            NUMBER, NAME, MEMBER_COUNT, ADMIN_COUNT, STARTED_SINCE, NB_COL
        };
};


class ConfList : public QWidget
{
    Q_OBJECT

    public:
        ConfList(QWidget *parent);
};

#endif

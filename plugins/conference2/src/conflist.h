#ifndef __XLET_CONFLIST
#define __XLET_CONFLIST

#include <QLabel>
#include <QVBoxLayout>
#include <QTableView>
#include <QModelIndex>
#include <QVariant>
#include <QAbstractTableModel>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>

#include <baseengine.h>

class ConfListModel : public QAbstractTableModel
{
    public:
        ConfListModel();
        enum ColOrder {
            ID, NUMBER, NAME, MEMBER_COUNT, ADMIN_COUNT, STARTED_SINCE, NB_COL
        };

    private:
        void timerEvent(QTimerEvent *);
        int rowCount(const QModelIndex&) const;
        int columnCount(const QModelIndex&) const;
        QVariant data(const QModelIndex&, int) const;
        QVariant headerData(int , Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;

};

class ConfListView : public QTableView
{
    Q_OBJECT

    public:
        ConfListView(QWidget *parent, ConfListModel *model);
    private slots:
        void onViewClick(const QModelIndex &);
    protected:
        virtual void mousePressEvent(QMouseEvent *event);
    signals:
        void openConfRoom();
    private:
        int lastPressed;
};


class ConfList : public QWidget
{
    Q_OBJECT

    public:
        ConfList(QWidget *parent);
};

#endif

#ifndef _CONFERENCE2_CONFLIST_H_
#define _CONFERENCE2_CONFLIST_H_

#include <QLabel>
#include <QTimer>
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

#include "conference.h"

class ConfListModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        ConfListModel();
        enum ColOrder {
            ID, NAME, NUMBER, PIN_REQUIRED, MODERATED,
            MEMBER_COUNT, STARTED_SINCE, NB_COL
        };

    private slots:
        void confRoomsChange(const QString &path, DStoreEvent event);
    protected:
        void timerEvent(QTimerEvent *event);
    private:
        void sort(int, Qt::SortOrder);
        int rowCount(const QModelIndex&) const;
        int columnCount(const QModelIndex&) const;
        QVariant data(const QModelIndex&, int) const;
        QVariant headerData(int , Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;
        QMap<int, QString> m_row2id;

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
    private:
        int lastPressed;
};


class ConfList : public QWidget
{
    Q_OBJECT

    public:
        ConfList(XletConference *parent);
    private slots:
        void openConfRoom();
    private:
        XletConference *manager;
};

#endif

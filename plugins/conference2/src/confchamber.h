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

#include "baseengine.h"

class ConfChamberModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        ConfChamberModel(const QString &);
        enum ColOrder {
            ID, NAME, NUMBER, SINCE, ADMIN, ACTION_KICK, ACTION_MUTE, NB_COL
        };
    private slots:
        void confRoomChange(const QString &path, DStoreEvent event);
    protected:
        void timerEvent(QTimerEvent *event);
    private:
        void extractRow2IdMap();
        void sort(int, Qt::SortOrder);
        int rowCount(const QModelIndex&) const;
        int columnCount(const QModelIndex&) const;
        QVariant data(const QModelIndex&, int) const;
        QVariant headerData(int , Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;
        QString m_id;
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

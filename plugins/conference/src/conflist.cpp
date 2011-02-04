/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $Revision$
 * $Date$
 */

#include "conflist.h"

enum ColOrder {
    ID, NAME, NUMBER, PIN_REQUIRED, MODERATED,
    MEMBER_COUNT, STARTED_SINCE, NB_COL
};

static QVariant COL_TITLE[NB_COL];

ConfListModel::ConfListModel()
    : QAbstractTableModel()
{
    b_engine->tree()->onChange("confrooms", this,
        SLOT(confRoomsChange(const QString &, DStoreEvent)));

    startTimer(1000);
    COL_TITLE[ID] = tr("Room UID");
    COL_TITLE[NUMBER] = tr("Number");
    COL_TITLE[NAME] = tr("Name");
    COL_TITLE[PIN_REQUIRED] = tr("Pin code");
    COL_TITLE[MEMBER_COUNT] = tr("Member count");
    COL_TITLE[MODERATED] = tr("Moderated");
    COL_TITLE[STARTED_SINCE] = tr("Started since");
}

void ConfListModel::timerEvent(QTimerEvent *)
{
    reset();
}

void ConfListModel::confRoomsChange(const QString &, DStoreEvent)
{
    m_roomList = b_engine->eVM("confrooms");

    int row = 0;
    if (m_roomList.size() != m_row2id.size()) {
        foreach(QString roomId, m_roomList.keys()) {
            m_row2id.insert(row++, roomId);
        }
    }
    reset();
}

Qt::ItemFlags ConfListModel::flags(const QModelIndex &) const
{
    return Qt::NoItemFlags;
}

int ConfListModel::rowCount(const QModelIndex&) const
{
    return m_roomList.size();
}

int ConfListModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant
ConfListModel::data(const QModelIndex &index,
                    int role) const
{
    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
        return QVariant();
    }

    int row = index.row(), col = index.column();

    if (m_row2id.contains(row))
        row = m_row2id[row].toInt();

    QString room = QString("confrooms/%0/").arg(row);
    switch (col) {
        case ID:
            return b_engine->eV(room + "id");
        case NUMBER:
            return b_engine->eV(room + "number");
        case NAME:
            return b_engine->eV(room + "name");
        case PIN_REQUIRED:
            return b_engine->eV(room + "pin").toString().isEmpty() ?
                       tr("No") : tr("Yes");
        case MODERATED:
            return b_engine->eV(room + "moderated").toInt() ?
                       tr("Yes") : tr("No");
        case MEMBER_COUNT:
            return b_engine->eVM(room + "in").size();
        case STARTED_SINCE:
        {
            QVariantMap UserIn = b_engine->eVM(room + "in");
            double time = 0;
            QString displayed = QString::fromUtf8("Ø");
            foreach (QString uid, UserIn.keys()) {
                double utime = UserIn[uid].toMap().value("time-start").toDouble();
                if ((time == 0) || (time > utime)) {
                    time = utime;
                }
            }
            if (time != 0) {
                displayed =
                    QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() -
                                          b_engine->timeDeltaServerClient() -
                                          time)
                                         .toUTC().toString("hh:mm:ss");
            }

            return displayed;
        }
        default:
            break;
    }
    return QVariant();
}

QVariant
ConfListModel::headerData(int section,
                          Qt::Orientation orientation,
                          int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        return COL_TITLE[section];
    }

    return QVariant();
}

void ConfListModel::sort(int column, Qt::SortOrder order)
{
    struct {
        static bool ascending(const QPair<int, QString> &a,
                              const QPair<int, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ?
                                               true : false;
        }
        static bool descending(const QPair<int, QString> &a,
                               const QPair<int, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ?
                                               false : true;
        }
    } sFun;

    QList<QPair<int, QString> > toSort;

    int i, e;
    for (i=0,e=rowCount(QModelIndex());i<e;i++) {
        toSort.append(QPair<int, QString>(index(i, ID).data().toInt(),
                                          index(i, column).data().toString()));
    }

    qSort(toSort.begin(), toSort.end(), (order == Qt::AscendingOrder) ?
                                         sFun.ascending :
                                         sFun.descending);

    for (i=0;i<e;i++) {
        m_row2id.insert(i, QString::number(toSort[i].first));
    }
    reset();
}


ConfListView::ConfListView(QWidget *parent, ConfListModel *model)
    : QTableView(parent)
{
    setSortingEnabled(true);
    setModel(model);
    setShowGrid(0);
    verticalHeader()->hide();
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setMovable(true);
    setStyleSheet("ConfListView {"
                    "border: none;"
                    "background: transparent;"
                    "color:black;"
                  "}");
    hideColumn(0);

    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(onViewClick(const QModelIndex &)));
}

void ConfListView::onViewClick(const QModelIndex &model)
{
    QString roomId = model.sibling(model.row(), ID).data().toString();
    QString roomName = model.sibling(model.row(), NAME).data().toString();
    QString roomNumber = model.sibling(model.row(), NUMBER).data().toString();

    if (roomId != "") {
        if (lastPressed&Qt::LeftButton) {
            b_engine->pasteToDial(roomNumber);
            QTimer *timer = new QTimer(this);
            timer->setSingleShot(true);
            timer->setProperty("id", roomId);
            connect(timer, SIGNAL(timeout()), parentWidget(), SLOT(openConfRoom()));
            timer->start(10);
        } else {
            QMenu *menu = new QMenu(this);

            QAction *action = new QAction(tr("Get in room %1 (%2)")
                                             .arg(roomName).arg(roomNumber), menu);

            action->setProperty("id", roomId);
            connect(action, SIGNAL(triggered(bool)),
                    parentWidget(), SLOT(openConfRoom()));
            connect(action, SIGNAL(triggered(bool)),
                    parentWidget(), SLOT(phoneConfRoom()));

            menu->addAction(action);
            menu->exec(QCursor::pos());
        }
    }
}

void ConfListView::mousePressEvent(QMouseEvent *event)
{
    lastPressed = event->button();
    QTableView::mousePressEvent(event);
}


ConfList::ConfList(XletConference *parent)
    : QWidget(), m_manager(parent)
{
    QVBoxLayout *vBox = new QVBoxLayout(this);
    QHBoxLayout *hBox = new QHBoxLayout();
    ConfListView *view = new ConfListView(this, new ConfListModel());

    view->setStyleSheet("ConfListView {"
                            "border: none;"
                            "background:transparent;"
                            "color:black;"
                        "}");
    view->verticalHeader()->hide();

    hBox->addStretch(1);
    hBox->addWidget(view, 8);
    hBox->addStretch(1);

    vBox->addLayout(hBox);
    setLayout(vBox);
}

void ConfList::phoneConfRoom()
{
    QString roomId = sender()->property("id").toString();
    QString roomNumber = b_engine->eV(QString("confrooms/%0/number").arg(roomId)).toString();

    b_engine->actionCall("originate", "user:special:me", "ext:" + roomNumber);
    m_manager->openConfRoom(roomId, true);
}

void ConfList::openConfRoom()
{
    m_manager->openConfRoom(sender()->property("id").toString());
}

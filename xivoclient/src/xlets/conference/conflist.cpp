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

#include <QSortFilterProxyModel>

#include "conflist.h"

enum ColOrder {
    ID, NAME, NUMBER, PIN_REQUIRED, MODERATED,
    MEMBER_COUNT, STARTED_SINCE, NB_COL
};

static QVariant COL_TITLE[NB_COL];

ConfListModel::ConfListModel(QWidget *parent)
    : QAbstractTableModel(parent)
{
    // qDebug() << Q_FUNC_INFO;

    startTimer(1000);
    COL_TITLE[ID] = tr("Room UID");
    COL_TITLE[NUMBER] = tr("Number");
    COL_TITLE[NAME] = tr("Name");
    COL_TITLE[PIN_REQUIRED] = tr("Pin code");
    COL_TITLE[MEMBER_COUNT] = tr("Member count");
    COL_TITLE[MODERATED] = tr("Moderated");
    COL_TITLE[STARTED_SINCE] = tr("Started since");

    connect(b_engine, SIGNAL(updateMeetmesConfig(const QString &)),
            this, SLOT(updateMeetmesConfig(const QString &)));
    connect(b_engine, SIGNAL(removeMeetmeConfig(const QString &)),
            this, SLOT(removeMeetmeConfig(const QString &)));
}

void ConfListModel::timerEvent(QTimerEvent *)
{
    reset();
}

void ConfListModel::updateMeetmesConfig(const QString &xid)
{
    if (! m_row2id.contains(xid)) {
        m_row2id.append(xid);
    }
}

void ConfListModel::removeMeetmeConfig(const QString &xid) {
    m_row2id.removeAll(xid);
}

Qt::ItemFlags ConfListModel::flags(const QModelIndex &) const
{
    return Qt::NoItemFlags;
}

int ConfListModel::rowCount(const QModelIndex&) const
{
    return m_row2id.size();
}

int ConfListModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant ConfListModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
        return QVariant();
    }

    /* Rows here are not the same than the lines displayed by the view,
     * as there is a proxy model between the view and this model,
     * that maps the displayed lines to the stored lines, see ConfList
     * constructor
     */
    int row = index.row(), col = index.column();
    QString meetme_id;

    if (m_row2id.size() > row) {
        meetme_id = m_row2id[row];
    }

    const MeetmeInfo * m = b_engine->meetme(meetme_id);
    if (!m) return QVariant();
    const QString & mm = m->admin_moderationmode();

    switch (col) {
    case ID:
        return m->xid();
    case NUMBER:
        return m->number();
    case NAME:
        return m->name();
    case PIN_REQUIRED:
        return m->pin_needed() ? tr("Yes") : tr("No");
    case MODERATED:
        return mm.isEmpty() || mm == "0" ? tr("No") : tr("Yes");
    case MEMBER_COUNT:
        return m->channels().size();
    case STARTED_SINCE:
        {
            const ChannelInfo * pseudochan = b_engine->channel(
                QString("%0/%1").arg(m->ipbxid()).arg(m->pseudochan()));
            uint now = QDateTime::currentDateTime().toTime_t();
            return QDateTime::fromTime_t(
                now - (pseudochan ? pseudochan->timestamp() : now) -
                b_engine->timeDeltaServerClient()).toUTC().toString("hh:mm:ss");
        }
    default:
        break;
    }
    return QVariant();
}

QVariant ConfListModel::headerData(int section,
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

ConfListView::ConfListView(QWidget *parent)
    : QTableView(parent)
{
    setSortingEnabled(true);
    setShowGrid(0);
    verticalHeader()->hide();
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setMovable(true);
    setStyleSheet("ConfListView {"
                    "border: none;"
                    "background: transparent;"
                    "color:black;"
                  "}");

    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(onViewClick(const QModelIndex &)));
}

void ConfListView::onViewClick(const QModelIndex &model)
{
    QString roomId = model.sibling(model.row(), ID).data().toString();
    QString roomName = model.sibling(model.row(), NAME).data().toString();
    QString roomNumber = model.sibling(model.row(), NUMBER).data().toString();

    // qDebug() << Q_FUNC_INFO << roomId << roomName << roomNumber;

    if (roomId != "") {
        if (lastPressed & Qt::LeftButton) {
            b_engine->pasteToDial(roomNumber);
            QTimer *timer = new QTimer(this);
            timer->setSingleShot(true);
            timer->setProperty("id", roomId);
            connect(timer, SIGNAL(timeout()), parentWidget(), SLOT(openConfRoom()));
            timer->start(10);
        } else {
            QMenu *menu = new QMenu(this);

            QAction *action = new QAction(
                tr("Get in room %1 (%2)").arg(roomName).arg(roomNumber), menu);

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
    // qDebug() << Q_FUNC_INFO;
    QVBoxLayout *vBox = new QVBoxLayout(this);
    QHBoxLayout *hBox = new QHBoxLayout();
    
    // this contains the data, unordered
    ConfListModel *model = new ConfListModel(this);
    
    // this maps the indexes between the sorted view and the unordered model
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setDynamicSortFilter(true); /* sorts right on insertion, instead
    of half a second after the window has appeared */
    
    // this displays the sorted data
    ConfListView *view = new ConfListView(this);
    view->setModel(proxyModel);
    view->hideColumn(ID);
    view->hideColumn(MODERATED);
    view->sortByColumn(NAME, Qt::AscendingOrder);

    hBox->addStretch(1);
    hBox->addWidget(view, 8);
    hBox->addStretch(1);

    vBox->addLayout(hBox);
    setLayout(vBox);
}

void ConfList::phoneConfRoom()
{
    QString roomId = sender()->property("id").toString();
    // qDebug() << Q_FUNC_INFO << "Room id" << roomId;

    const MeetmeInfo * m = b_engine->meetme(roomId);
    if (m) {
        QString roomNumber = m->number();
        b_engine->actionDialNumber(roomNumber);
        m_manager->openConfRoom(roomId, true);
    }
}

void ConfList::openConfRoom()
{
    // qDebug() << Q_FUNC_INFO;
    m_manager->openConfRoom(sender()->property("id").toString());
}

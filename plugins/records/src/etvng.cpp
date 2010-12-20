/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

#include "etvng.h"

enum ColOrder {
    ID, NAME, NUMBER, PIN_REQUIRED, MODERATED,
    MEMBER_COUNT, STARTED_SINCE, NB_COL
};

static QString braourk = "confrooms";

//
// ETVListProperties class
//

ETVListProperties::ETVListProperties()
{
    m_properties.clear();
    m_properties["display"] = "border: none; background:transparent; color:black;";
    m_properties["columns"] = "";
}

// to define, per column : "id" for "eV", type (boolean, date/time, phone)
void ETVListProperties::addProperty(const QString & title,
                                    const QString & eventfield,
                                    const QString & qttype,
                                    const QString & xivotype)
{
    QVariantList columns = m_properties.value("columns").toList();
    QVariantMap u;
    u["title"] = title;
    u["eventfield"] = eventfield;
    u["qttype"] = qttype;
    u["xivotype"] = xivotype;
    columns << u;
    m_properties["columns"] = columns;
}

QString ETVListProperties::title(int index) const
{
    return m_properties.value("columns").toList()[index].toMap().value("title").toString();
}

QString ETVListProperties::eventfield(int index) const
{
    return m_properties.value("columns").toList()[index].toMap().value("eventfield").toString();
}

QString ETVListProperties::qssdisplay() const
{
    return m_properties.value("display").toString();
}

int ETVListProperties::ncolumns() const
{
    return m_properties.value("columns").toList().size();
}

//
// ETVListModel class
//

ETVListModel::ETVListModel(const ETVListProperties * const qv)
    : QAbstractTableModel()
{
    m_fieldoptions = qv;
    b_engine->tree()->onChange(braourk, this,
                               SLOT(mylistChange(const QString &, DStoreEvent)));
    // startTimer(1000);
}

QString ETVListModel::qssdisplay() const
{
    return m_fieldoptions->qssdisplay();
}

void ETVListModel::timerEvent(QTimerEvent *)
{
    reset();
}

void ETVListModel::mylistChange(const QString &, DStoreEvent)
{
    m_myList = b_engine->eVM(braourk);

    int row = 0;
    if (m_myList.size() != m_row2id.size()) {
        foreach(QString myId, m_myList.keys()) {
            m_row2id.insert(row++, myId);
        }
    }
    reset();
}

Qt::ItemFlags ETVListModel::flags(const QModelIndex &) const
{
    return Qt::NoItemFlags;
}

int ETVListModel::rowCount(const QModelIndex&) const
{
    return m_myList.size();
}

int ETVListModel::columnCount(const QModelIndex &) const
{
    return m_fieldoptions->ncolumns();
}

QVariant ETVListModel::data(const QModelIndex & index, int role) const
{
    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
        return QVariant();
    }

    int row = index.row();
    int col = index.column();

    if (m_row2id.contains(row))
        row = m_row2id[row].toInt();

    QString eventfield = m_fieldoptions->eventfield(col);
    QString request = QString("%1/%2/%3").arg(braourk).arg(row).arg(eventfield);

    switch (col) {
        case ID:
        case NUMBER:
        case NAME:
            return b_engine->eV(request);
        case PIN_REQUIRED:
            return b_engine->eV(request).toString().isEmpty() ? tr("No") : tr("Yes");
        case MODERATED:
            return b_engine->eV(request).toInt() ? tr("Yes") : tr("No");
        case MEMBER_COUNT:
            return b_engine->eVM(request).size();
        case STARTED_SINCE:
        {
            QVariantMap UserIn = b_engine->eVM(request);
            double time = 0;
            QString displayed = QString::fromUtf8("Ø");
            foreach (QString uid, UserIn.keys()) {
                double utime = UserIn.value(uid).toMap().value("time-start").toDouble();
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

QVariant ETVListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        return m_fieldoptions->title(section);
    }

    return QVariant();
}

void ETVListModel::sort(int column, Qt::SortOrder order)
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
    for (i = 0, e = rowCount(QModelIndex()); i < e; i++) {
        toSort.append(QPair<int, QString>(index(i, ID).data().toInt(),
                                          index(i, column).data().toString()));
    }

    qSort(toSort.begin(), toSort.end(), (order == Qt::AscendingOrder) ?
                                         sFun.ascending :
                                         sFun.descending);

    for (i = 0; i < e; i++) {
        m_row2id.insert(i, QString::number(toSort[i].first));
    }
    reset();
}

//
// ETVListView class
//

ETVListView::ETVListView(QWidget * parent, ETVListModel * model)
    : QTableView(parent)
{
    setSortingEnabled(true);
    setModel(model);
    setShowGrid(0);
    verticalHeader()->hide();
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setMovable(true);
    setStyleSheet("ETVListView {" + model->qssdisplay() + "}");
    hideColumn(0);

    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(onViewClick(const QModelIndex &)));
}

void ETVListView::onViewClick(const QModelIndex & model)
{
    QString roomId = model.sibling(model.row(), ID).data().toString();
    QString roomName = model.sibling(model.row(), NAME).data().toString();
    QString roomNumber = model.sibling(model.row(), NUMBER).data().toString();

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

void ETVListView::mousePressEvent(QMouseEvent *event)
{
    lastPressed = event->button();
    QTableView::mousePressEvent(event);
}

//
// ETVListWidget class
//

ETVListWidget::ETVListWidget(const ETVListProperties * const qv,
                             XletRecords * parent)
    : QWidget(), m_manager(parent)
{
    QVBoxLayout  * vBox = new QVBoxLayout(this);
    QHBoxLayout  * hBox = new QHBoxLayout();
    ETVListModel * model = new ETVListModel(qv);
    ETVListView  * view = new ETVListView(this, model);

    view->setStyleSheet("ETVListView {" + model->qssdisplay() + "}");
    view->verticalHeader()->hide();

    hBox->addStretch(1);
    hBox->addWidget(view, 8);
    hBox->addStretch(1);

    vBox->addLayout(hBox);
    setLayout(vBox);
}

void ETVListWidget::phoneConfRoom()
{
    QString roomId = sender()->property("id").toString();
    QString roomNumber = b_engine->eV(QString("%1/%2/number").arg(braourk).arg(roomId)).toString();

    b_engine->actionCall("originate", "user:special:me", "ext:" + roomNumber);
    // m_manager->openConfRoom(roomId, true);
}

void ETVListWidget::openConfRoom()
{
    // m_manager->openConfRoom(sender()->property("id").toString());
}

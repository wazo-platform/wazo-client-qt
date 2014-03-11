/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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

#include <baseengine.h>
#include <phonenumber.h>

#include "history.h"

XLet* XLetHistoryPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/history_%1");
    return new LogWidget(parent);
}

LogWidgetModel::LogWidgetModel(int initialMode, QWidget * parent)
    : QAbstractTableModel(parent), m_sorted(false), m_sorted_column(0), m_sort_order(Qt::AscendingOrder)
{
    registerListener("history");
    m_mode = (HistoryMode) initialMode;
    m_history << QVariant() << QVariant() << QVariant();
    connect(b_engine, SIGNAL(settingsChanged()),
            this, SLOT(requestHistory()));
}

void LogWidgetModel::parseCommand(const QVariantMap &map) {
    updateHistory(map);
}

void LogWidgetModel::sort(int column, Qt::SortOrder order)
{
    m_sorted = true;
    m_sorted_column = column;
    m_sort_order = order;

    QList<QVariant> tosort = m_history[m_mode].toList();

    beginResetModel();
    if (order == Qt::AscendingOrder) {
        if (column == 0) {
            qSort(tosort.begin(), tosort.end(), ascendingOrderByNumber);
        } else if (column == 1) {
            qSort(tosort.begin(), tosort.end(), ascendingOrderByDate);
        } else if (column == 2) {
            qSort(tosort.begin(), tosort.end(), ascendingOrderByDuration);
        }
    } else {
        if (column == 0) {
            qSort(tosort.begin(), tosort.end(), descendingOrderByNumber);
        } else if (column == 1) {
            qSort(tosort.begin(), tosort.end(), descendingOrderByDate);
        } else if (column == 2) {
            qSort(tosort.begin(), tosort.end(), descendingOrderByDuration);
        }
    }

    m_history[m_mode] = tosort;
    endResetModel();
}

int LogWidgetModel::rowCount(const QModelIndex&) const
{
    int nbrow = 0;

    if (((m_history[m_mode].toList().count())))
        nbrow = (m_history[m_mode].toList()).count();

    return nbrow;
}

int LogWidgetModel::columnCount(const QModelIndex&) const
{
    if ((m_mode == OUTCALLS) || (m_mode == INCALLS))
        return 3;
    else if (m_mode == MISSEDCALLS)
        return 2;

    return 0;
}

QVariant LogWidgetModel::data(const QModelIndex &a, int role) const
{
    int row, column; row = a.row(); column = a.column();

    if (role == Qt::DisplayRole) {
        QVariantList histlist = m_history[m_mode].toList();
        if (((histlist.count()) &&
             ((histlist).value(row).toMap().count()))) {
            if (column == 0) {
                return histlist.value(row).toMap().value("fullname");
            } else if (column == 1) {
                QString qsd = histlist.value(row).toMap().value("calldate").toString();
                QDateTime qdt = QDateTime::fromString(qsd, Qt::ISODate);
                QString qsf = qdt.toString(Qt::DefaultLocaleLongDate); // Qt::DefaultLocaleShortDate
                return qsf;
            } else if (column == 2) {
                int duration = histlist.value(row).toMap().value("duration").toInt();
                int sec =   ( duration % 60);
                int min =   ( duration - sec ) / 60 % 60;
                int hou = ( ( duration - sec - min * 60 ) / 60 ) / 60;
                if (hou)
                    return tr("%1 hr %2 min %3 s").arg(hou).arg(min).arg(sec);
                else if (min)
                    return tr("%1 min %2 s").arg(min).arg(sec);
                else
                    return tr("%1 s").arg(sec);
            }
        }
    }

    return QVariant();
}

/*! \brief parse history command response
 */
void LogWidgetModel::updateHistory(const QVariantMap &p)
{
    int mode = p.value("mode").toInt();
    QVariantList h = p.value("history").toList();
    beginResetModel();
    if (mode == m_mode)
        m_history[m_mode] = h;
    if (m_sorted) {
        sort(m_sorted_column, m_sort_order);
    }
    endResetModel();
}

Qt::ItemFlags LogWidgetModel::flags(const QModelIndex &) const
{
    return Qt::NoItemFlags;
}

/*! \brief ask history for an extension */
void LogWidgetModel::requestHistory(HistoryMode mode, QString xuserid)
{
    if (mode == DEFAULT) {
        mode = m_mode;
    }
    if (xuserid.isEmpty()) {
        xuserid = b_engine->getFullId();
    }
    if (mode == OUTCALLS || mode == INCALLS || mode == MISSEDCALLS) {
        QVariantMap command;
        command["class"] = "history";
        command["xuserid"] = xuserid;
        command["size"] = QString::number(b_engine->getConfig("historysize").toUInt());
        command["mode"] = QString::number(mode);
        b_engine->sendJsonCommand(command);
    }
}

void LogWidgetModel::changeMode(bool active)
{
    if (active) {
        m_mode = (HistoryMode)sender()->property("mode").toInt();
        requestHistory(m_mode);
	beginResetModel();
        emit headerDataChanged(Qt::Horizontal, 0, 3);
	endResetModel();
    }
}

QVariant LogWidgetModel::headerData(int section,
                                    Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return QVariant(tr("Number"));
        else if (section == 1)
            return QVariant(tr("Date"));

        if ((section == 2) && ((m_mode == OUTCALLS) || (m_mode == INCALLS)))
            return QVariant(tr("Duration"));
    }

    return QVariant();
}

static inline
QRadioButton* buildRadioButton(QString text,
                               QString icon,
                               int mode,
                               QGroupBox *groupBox,
                               QHBoxLayout *hbox,
                               LogWidgetModel *m_viewmodel)
{
    QRadioButton *build = new QRadioButton(text, groupBox);

    build->setIcon(QIcon(QString(":/images/history/%0").arg(icon)));
    build->setProperty("mode", mode);
    build->setToolTip(build->text());
    hbox->addWidget(build);

    QObject::connect(build, SIGNAL(toggled(bool)), m_viewmodel, SLOT(changeMode(bool)));

    return build;
}

LogWidget::LogWidget(QWidget *parent)
    : XLet(parent), m_view(0), m_history_model(0)
{
    setTitle(tr("History"));

    QGroupBox *groupBox = new QGroupBox(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *hBox = new QHBoxLayout(groupBox);

    m_history_model = new LogWidgetModel(0, this);

    hBox->addStretch(1);
    buildRadioButton(tr("Sent calls"), "sent_call.png", OUTCALLS, groupBox, hBox, m_history_model)->setChecked(true);
    buildRadioButton(tr("Received calls"), "received_call.png", INCALLS, groupBox, hBox, m_history_model);
    buildRadioButton(tr("Missed calls"), "missed_call.png", MISSEDCALLS, groupBox, hBox, m_history_model);
    hBox->addStretch(1);

    m_view = new LogTableView(this, m_history_model);
    m_view->installEventFilter(this);

    layout->addWidget(groupBox);
    layout->addWidget(m_view);
}

LogTableView::LogTableView(QWidget *parent, LogWidgetModel *model)
    : QTableView(parent)
{
    setSortingEnabled(true);
    setModel(model);
    verticalHeader()->hide();
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(onViewClick(const QModelIndex &)));
}

/*
 *   You can't know which mouse button caused the onViewClick to be called
 *   through QApplication::mouseButtons or through event filtering the
 *   QTableView
 */
void LogTableView::mousePressEvent(QMouseEvent *event)
{
    m_lastPressed = event->button();
    QTableView::mousePressEvent(event);
}

void LogTableView::callOnClick(bool)
{
    QAction *calling_action = qobject_cast<QAction *>(sender());
    QString num_to_call = calling_action->property("num_to_call").toString();
    b_engine->actionDial(num_to_call);
}

void LogTableView::onViewClick(const QModelIndex &model)
{
    QString caller = model.sibling(model.row(), 0).data().toString();

    if (caller != "") {
        if (m_lastPressed & Qt::LeftButton) {
            b_engine->pasteToDial(caller);
        } else {
            QMenu *menu = new QMenu(this);

            QAction *action = new QAction(tr("Call %1").arg(caller), menu);
            action->setProperty("num_to_call", caller);
            connect(action, SIGNAL(triggered(bool)),
                    this, SLOT(callOnClick(bool)));

            menu->addAction(action);
            menu->exec(QCursor::pos());
        }
    }
}

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

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QGroupBox>
#include <QRadioButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>

#include "baseengine.h"
#include "logwidget.h"

LogWidgetModel* LogWidgetModel::self = NULL;

LogWidgetModel::LogWidgetModel(BaseEngine *b, int initialMode)
    : QAbstractTableModel(NULL), engine(b)
{
    b->registerClassEvent("history", LogWidgetModel::updateHistory);
    mode = initialMode;
    self = this;
    history << QVariant() << QVariant() << QVariant();
}

void LogWidgetModel::sort(int column, Qt::SortOrder order)
{
    QList<QVariant> tosort = history[mode].toList();

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

    history[mode] = tosort;
    reset();
}

int LogWidgetModel::rowCount(const QModelIndex &a) const
{
    int row, column; row = a.row(); column = a.column();
    int nbrow = 0;

    if (((history[mode].toList().count())))
        nbrow = (history[mode].toList()).count();

    return nbrow;
}

int LogWidgetModel::columnCount(const QModelIndex&) const
{
    if ((mode==0) || (mode==1))
        return 3;
    else if (mode==2)
        return 2;

    return 0;
}

QVariant LogWidgetModel::data(const QModelIndex &a, int role) const
{
    int row, column; row = a.row(); column = a.column();

    if (role==Qt::DisplayRole) {
        if (((history[mode].toList().count()) &&
             ((history[mode].toList()).value(row).toMap().count()))) {
            if (column == 0) {
                return ((history[mode].toList()).value(row).toMap())["fullname"]; 
            } else if (column == 1) {
                return ((history[mode].toList()).value(row).toMap())["ts"]; 
            } else if (column == 2) {
                int duration = ((history[mode].toList()).value(row).toMap())["duration"].toInt();
                int sec =   ( duration % 60);
                int min =   ( duration - sec ) / 60 % 60;
                int hou = ( ( duration - sec - min * 60 ) / 60 ) / 60;
                if (hou)
                    return QString("%0 hou %1 min %2 sec").arg(hou).arg(min).arg(sec);
                else if (min) 
                  return tr("%0 min %1 sec").arg(min).arg(sec);
                else
                  return tr("%0 sec").arg(sec);
            }
        }
    } 

    return QVariant();
}

/*! \brief parse history command response
 */
void LogWidgetModel::updateHistory(QVariantMap p)
{
    QVariant payload = p["payload"] ;
    
    self->history[self->mode] = payload;
    self->reset();
}

Qt::ItemFlags LogWidgetModel::flags(const QModelIndex &) const
{
    return Qt::NoItemFlags;
}

/*! \brief ask history for an extension */
void LogWidgetModel::requestHistory(QString peer, int mode, QDateTime moreRecent, int forceEntry)
{
    /* mode = 0 : Out calls
     * mode = 1 : In calls
     * mode = 2 : Missed calls */
    if(mode >= 0) {
        QVariantMap command;
        command["class"] = "history";
        command["direction"] = "xivoserver";
        command["peer"] = peer;
        command["size"] = QString::number(engine->historySize());
        command["mode"] = QString::number(mode);
        if(moreRecent.isValid()) {
            command["morerecentthan"] = moreRecent.toString(Qt::ISODate);
        }
        engine->sendJsonCommand(command);
    }
}

void LogWidgetModel::changeMode(bool active)
{
    if (active) {
        mode = qobject_cast<QRadioButton *>(sender())->property("mode").toInt();
        requestHistory(engine->getFullId(), mode);
        emit headerDataChanged(Qt::Horizontal, 0, 3);
        reset();
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

        if ((section==2) && ((mode==0) || (mode==1)))
            return QVariant(tr("Duration"));
    }

    return QVariant();
}





static inline QRadioButton* buildRadioButton(QString text, QString icon, int mode, QGroupBox *groupBox, QHBoxLayout *hbox, LogWidgetModel *m_viewmodel)
{
    QRadioButton *build = new QRadioButton(text, groupBox);

    build->setIcon(QIcon(QString(":/images/%0").arg(icon)));
    build->setProperty("mode", mode);
    build->setToolTip(build->text());
    hbox->addWidget(build);

    QObject::connect(build, SIGNAL(toggled(bool)), m_viewmodel, SLOT(changeMode(bool)));

    return build;
}

static inline void layoutMarginSpacingTo0(QBoxLayout *l)
{
    l->setMargin(0);
    l->setSpacing(0);
}


LogWidget::LogWidget(BaseEngine * engine, QWidget * parent)
    : XLet(engine, parent)
{
    setTitle(tr("History"));

    QGroupBox *groupBox = new QGroupBox(this);
    groupBox->setAlignment(Qt::AlignHCenter);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *hBox = new QHBoxLayout(groupBox);
    QHBoxLayout *hBox2 = new QHBoxLayout(groupBox);
    layoutMarginSpacingTo0(layout);
    layoutMarginSpacingTo0(hBox);

    LogWidgetModel *viewmodel = new LogWidgetModel(engine, 0);

    m_view = new QTableView(this);
    m_view->setSortingEnabled(true);
    m_view->setModel(viewmodel);
    m_view->verticalHeader()->hide();
    m_view->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
    m_view->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
    m_view->horizontalHeader()->setResizeMode(2,QHeaderView::Stretch);
    //m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->setStyleSheet("QTableView { border: none; background:transparent; color:black; }");

    hBox->addStretch(1);

    buildRadioButton(tr("Outgoing calls"), "green_up.png",   0, groupBox, hBox, viewmodel)->setChecked(true);
    buildRadioButton(tr("Incoming calls"), "green_down.png", 1, groupBox, hBox, viewmodel);
    buildRadioButton(tr("Missed calls"),   "red_down.png",   2, groupBox, hBox, viewmodel);

    hBox->addStretch(1);

    layout->addWidget(groupBox);
    hBox2->addStretch(1);
    hBox2->addWidget(m_view, 4);
    hBox2->addStretch(1);
    layout->addLayout(hBox2);
}

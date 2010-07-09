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

#include <baseengine.h>
#include <logwidget.h>

LogWidgetModel::LogWidgetModel(int initialMode)
    : QAbstractTableModel(NULL)
{
    b_engine->registerClassEvent("history", LogWidgetModel::updateHistory_t, this);
    mode = initialMode;
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
    QVariant payload = p["payload"] ;
    
    history[mode] = payload;
    reset();
}

Qt::ItemFlags LogWidgetModel::flags(const QModelIndex &) const
{
    return Qt::NoItemFlags;
}

/*! \brief ask history for an extension */
void LogWidgetModel::requestHistory(const QString &peer,
                                    int mode,
                                    const QDateTime &moreRecent,
                                    int)
{
    /* mode = 0 : Out calls
     * mode = 1 : In calls
     * mode = 2 : Missed calls */
    if(mode >= 0) {
        QVariantMap command;
        command["class"] = "history";
        command["direction"] = "xivoserver";
        command["peer"] = peer;
        command["size"] = QString::number(b_engine->historySize());
        command["mode"] = QString::number(mode);
        if(moreRecent.isValid()) {
            command["morerecentthan"] = moreRecent.toString(Qt::ISODate);
        }
        b_engine->sendJsonCommand(command);
    }
}

void LogWidgetModel::changeMode(bool active)
{
    if (active) {
        mode = qobject_cast<QRadioButton *>(sender())->property("mode").toInt();
        requestHistory(b_engine->getFullId(), mode);
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




static inline 
QRadioButton* buildRadioButton(QString text,
                               QString icon,
                               int mode,
                               QGroupBox *groupBox,
                               QHBoxLayout *hbox,
                               LogWidgetModel *m_viewmodel)
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


LogWidget::LogWidget(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("History"));

    QGroupBox *groupBox = new QGroupBox(this);
    groupBox->setAlignment(Qt::AlignHCenter);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *hBox = new QHBoxLayout(groupBox);
    QHBoxLayout *hBox2 = new QHBoxLayout;
    layoutMarginSpacingTo0(layout);
    layoutMarginSpacingTo0(hBox);

    LogWidgetModel *viewmodel = new LogWidgetModel(0);

    m_view = new LogTableView(this, viewmodel);
    m_view->installEventFilter(this);

    
    hBox->addStretch(1);

    buildRadioButton(tr("Sent calls"), "green_up.png",   0, groupBox, hBox, viewmodel)
                    ->setChecked(true);
    buildRadioButton(tr("Received calls"), "green_down.png", 1, groupBox, hBox, viewmodel);
    buildRadioButton(tr("Missed calls"),   "red_down.png",   2, groupBox, hBox, viewmodel);

    hBox->addStretch(1);

    layout->addWidget(groupBox);
    hBox2->addStretch(1);
    hBox2->addWidget(m_view, 4);
    hBox2->addStretch(1);
    layout->addLayout(hBox2);

}

LogTableView::LogTableView(QWidget *parent, LogWidgetModel *model)
    : QTableView(parent)
{
    setSortingEnabled(true);
    setModel(model);
    verticalHeader()->hide();
    horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
    horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
    horizontalHeader()->setResizeMode(2,QHeaderView::Stretch);
    setStyleSheet("QTableView { border: none; background:transparent; color:black; }");

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
    lastPressed = event->button();
    QTableView::mousePressEvent(event);
}

void LogTableView::callOnClick(bool)
{
    QAction *calling_action = qobject_cast<QAction *>(sender());
    QString num_to_call = calling_action->property("num_to_call").toString();
    b_engine->actionCall("originate", "user:special:me", "ext:" + num_to_call);
}

void LogTableView::onViewClick(const QModelIndex &model)
{
    QString caller = model.sibling(model.row(), 0).data().toString();

    if (caller.indexOf("<") != -1) {
        caller.remove(QRegExp("[^<]*<"));
        caller.remove(">");
    }
    caller.remove(QRegExp("[^0-9]"));

    if (caller != "") {
        if (lastPressed&Qt::LeftButton) {
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

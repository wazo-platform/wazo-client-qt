/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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
#include <QLabel>
#include <QPainter>

#include "agentstatusdelegate.h"

AgentStatusDelegate::AgentStatusDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{
    m_gui_buttonsize = 32;
}

QWidget *AgentStatusDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option,
                        const QModelIndex & index) const
{
    qDebug() << Q_FUNC_INFO;

    QPixmap pixmap =  QPixmap(m_gui_buttonsize, m_gui_buttonsize);
    pixmap.fill(Qt::gray);


    QLabel *label = new QLabel();
    label->setPixmap(pixmap);

    return label;
}

QSize AgentStatusDelegate::sizeHint(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    return QSize(10,32);
}

void AgentStatusDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    qDebug() << Q_FUNC_INFO << index.data();

    QPixmap pixmap =  QPixmap(m_gui_buttonsize, m_gui_buttonsize);
    QString agentStatus = index.data().toString();
    if (agentStatus == "AGENT_IDLE") {
            pixmap.fill(Qt::green);
    }
    else if (agentStatus == "AGENT_ONCALL") {
            pixmap.fill(Qt::green);
    } else if (agentStatus == "AGENT_LOGGEDOFF") {
            pixmap.fill(Qt::red);
    } else {
            pixmap.fill(Qt::gray);
    }

    QRect drawingArea = option.rect;
    drawingArea.adjust(2,2,-2,-2);
    painter->drawPixmap(drawingArea,pixmap);

}

void AgentStatusDelegate::commitAndCloseEditor()
 {
 }

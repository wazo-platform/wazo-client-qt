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

/* $Revision: 8992 $
 * $Date: 2010-08-24 05:59:38 -0400 (mar 24 aoû 2010) $
 */

#include "group.h"
#include <QDebug>

Group::Group(QObject * parent)
    : QObject(parent)
{
}

int Group::move(QList<Group *> &groups, PeersLayout *layout_on, QPoint &deltaGrid)
{
    QRect r = rect().adjusted(0, 0, -1, -1);
    QRect nr = r;
    nr.translate(deltaGrid);

    if((nr.left() < 0) || (nr.top() < 0))
        return 0;

    /* is there any group at the place where we intend to move ? */
    bool group_collision = false;
    foreach(Group *group, groups){
        if(group != this){
            if(group->rect().adjusted(0, 0, -1, -1).intersects(nr)){
                group_collision = true;
                break;
            }
        }
    }
    if(group_collision)
        return 0;

    /* is there any item that would collide with our, where we intend to move ? */
    bool item_collision = false;
    int i;

    for(i=0;i<layout_on->count();i++){
        QPoint itemPos = layout_on->getItemPosition(i);
        if((!r.contains(itemPos))&&(nr.contains(itemPos))){
            /* An item is not contained by the current group, but he will be contained 
             * if the group is moved. */
            if(layout_on->indexOfItemAtPosition(itemPos - deltaGrid) != -1){
                item_collision = true;
                break;
            }
        }
    }
    if(item_collision)
        return 0;

    for(i=0;i<layout_on->count();i++){
        /* move items inside the group with the group */
        if(r.contains(layout_on->getItemPosition(i))){
            layout_on->setItemPosition(i, layout_on->getItemPosition(i) + deltaGrid);
        }
    }

    m_rect.translate(deltaGrid);

    return 1;
}

int Group::resize(QList<Group *> & /*groups*/,
                  PeersLayout */*on*/,
                  QPoint &deltaGrid,
                  int resize_direction)
{
    int update = 0;

    if(resize_direction&ETop){
        if(rect().height() - deltaGrid.y() > 1){
            m_rect.adjust(0, deltaGrid.y(), 0, 0);
            update |= 1;
        }
    }else if(resize_direction&EBottom){
        if(rect().height() + deltaGrid.y() > 1){
            m_rect.adjust(0, 0, 0, deltaGrid.y());
            update |= 1;
        }
    }

    if(resize_direction&ELeft){
        if(rect().width() - deltaGrid.x() > 1){
            m_rect.adjust(deltaGrid.x(), 0, 0, 0);
            update |= 1;
        }
    }else if(resize_direction&ERight){
        if(rect().width() + deltaGrid.x() > 1){
            m_rect.adjust(0, 0, deltaGrid.x(), 0);
            update |= 1;
        }
    }

    return update;
}

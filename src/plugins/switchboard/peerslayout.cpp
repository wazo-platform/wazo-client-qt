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

#include <QDebug>
#include <QWidget>

#include "peerslayout.h"

PeersLayout::PeersLayout(QWidget * parent)
    : QLayout(parent), m_nb_rows(0), m_nb_columns(0)
{
    //qDebug() << Q_FUNC_INFO << parent;
    //qDebug() << Q_FUNC_INFO << spacing();
}

PeersLayout::~PeersLayout()
{
    while(!m_list.isEmpty()) {
        delete m_list.takeFirst();
    }
}

/*! \brief same as size()
 */
QSize PeersLayout::sizeHint() const
{
    //qDebug() << Q_FUNC_INFO;
    return size();
}

/*! \brief same as size()
 */
QSize PeersLayout::minimumSize() const
{
    //qDebug() << Q_FUNC_INFO;
    return size();
}

/*! \brief same as size()
 */
QSize PeersLayout::maximumSize() const
{
    //qDebug() << Q_FUNC_INFO;
    return size();
}

/*! \brief return the size in pixels
 */
QSize PeersLayout::size() const
{
    QSize itemSize = maxItemSize();
    return QSize( (itemSize.width() + 2 ) * m_nb_columns,
                  (itemSize.height() + 2 ) * m_nb_rows );
}

/*! \brief add a widget at position
 */
void PeersLayout::addWidget(QWidget *w, QPoint pos)
{
    // qDebug() << Q_FUNC_INFO;
    addChildWidget(w);
    QWidgetItem * wi = new QWidgetItem(w);
    // wi->setAlignment(Qt::AlignCenter);
    addItem(wi, pos);
}

/*! \brief add a layout item at position
 */
void PeersLayout::addItem(QLayoutItem * item, QPoint pos)
{
    m_list.append(item);
    if(pos.x() >= 0 && pos.y() >= 0) {
        if(m_listPos.contains(pos))
            pos = freePosition();
        if(pos.x() >= m_nb_columns)
            m_nb_columns = pos.x() + 1;
        if(pos.y() >= m_nb_rows)
            m_nb_rows = pos.y() + 1;
    }
    m_listPos.append(pos);
}

/*! \brief add a layout item at a free position
 */
void PeersLayout::addItem(QLayoutItem * item)
{
    m_list.append(item);
    QPoint pos = freePosition();
    if(pos.x() >= m_nb_columns)
        m_nb_columns = pos.x() + 1;
    if(pos.y() >= m_nb_rows)
        m_nb_rows = pos.y() + 1;
    m_listPos.append(pos);
}

/*! \brief set geometry of contained layout items
 */
void PeersLayout::setGeometry(const QRect &/* r*/)
{
    //qDebug() << Q_FUNC_INFO << r;
    QSize itemSize = maxItemSize();
    int i, x, y;
    for(i = 0; i<m_list.size(); i++) {
        x = m_listPos[i].x();
        y = m_listPos[i].y();
        if((x >= 0) && (y >= 0)) {
            m_list[i]->setGeometry(QRect( x*(itemSize.width()+2)+1/*left*/,
                                          y*(itemSize.height()+2)+1/*top*/,
                                          itemSize.width()/*width*/,
                                          itemSize.height()/*height*/ ) );
        }
    }
}

/*! \brief return item at index
 */
QLayoutItem* PeersLayout::itemAt(int i) const
{
    //qDebug() << Q_FUNC_INFO << i;
    return m_list.value(i);
}

/*! \brief take item at index
 */
QLayoutItem* PeersLayout::takeAt(int i)
{
    //qDebug() << Q_FUNC_INFO << i;
    m_listPos.takeAt(i);
    return m_list.takeAt(i);
}

/*! \brief return number of items
 */
int PeersLayout::count() const
{
    //qDebug() << Q_FUNC_INFO;
    return m_list.size();
}

/*! \brief return the maximal size of contained items
 */
QSize PeersLayout::maxItemSize() const
{
    int max_w = 40; //150;
    int max_h = 10; //20;
    int i;
    for(i = 0; i < m_list.size(); i++) {
        QSize size = m_list[i]->minimumSize();
        if(size.width() > max_w)
            max_w = size.width();
        if(size.height() > max_h)
            max_h = size.height();
    }
    return QSize(max_w, max_h);
}

/*! \brief find and return a free position in the grid
 */
QPoint PeersLayout::freePosition() const
{
    // TODO : change this...
    QPoint pos(0, 0);
    while(m_listPos.contains(pos)) {
        pos.ry()++;
        if(pos.y() > 6) {
            pos.ry() = 0;
            pos.rx()++;
        }
    }
    return pos;
}

/*! \brief return in which case of the cell is the pixel
 */
QPoint PeersLayout::getPosInGrid(const QPoint & pos) const
{
    QSize itemSize = maxItemSize();
    return QPoint( pos.x() / (itemSize.width() + 2),
                   pos.y() / (itemSize.height() + 2) );
}

QPoint PeersLayout::getPosFromGrid(const QPoint & pos) const
{
    QSize itemSize = maxItemSize();
    return QPoint( pos.x() * (itemSize.width() + 2),
                   pos.y() * (itemSize.height() + 2) );
}

/*! \brief set item position
 */
void PeersLayout::setItemPosition(int i, QPoint pos)
{
    if(i >= 0 && i < m_listPos.size()) {
        m_listPos[i] = pos;
        if(pos.x() >= m_nb_columns)
            m_nb_columns = pos.x() + 1;
        if(pos.y() >= m_nb_rows)
            m_nb_rows = pos.y() + 1;
        if(pos.x() >= 0)
            m_list[i]->widget()->show();
        else
            m_list[i]->widget()->hide();
        setGeometry(QRect());
    }
}

/*! \brief set item position
 */
void PeersLayout::setItemPosition(QWidget * widget, QPoint pos)
{
    setItemPosition(itemIndex(widget), pos);
}

/*! \brief get item position
 */
QPoint PeersLayout::getItemPosition(int i) const
{
    QPoint reply = QPoint(-1, -1);
    if(i >= 0 && i < m_listPos.size())
        reply = m_listPos[i];
    return reply;
}

/*! \brief get item position
 */
QPoint PeersLayout::getItemPosition(QWidget * widget) const
{
    return getItemPosition( itemIndex( widget ) );
}

/*! \brief get item index
 */
int PeersLayout::itemIndex(QWidget * widget) const
{
    int i = m_list.size();
    while(--i >= 0)
        {
            if(widget == m_list[i]->widget())
                break;
        }
    return i;
}

/*! \brief get a "grid" rect around the given rectangle
 *
 * used for Group layout
 */
QRect PeersLayout::getGridRect( const QRect & rect ) const
{
    QSize itemSize = maxItemSize();
    QPoint add = QPoint( itemSize.width() >> 1, itemSize.height() >> 1 );
    return QRect( getPosInGrid(rect.topLeft() + add),
                  getPosInGrid(rect.bottomRight() + add) );

}

int PeersLayout::indexOfItemAtPosition(const QPoint &pos)
{
    int i;
    for(i=0;i<count();i++){
        if(getItemPosition(i) == pos){
            return i;
        }
    }
    return -1;
}

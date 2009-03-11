/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
 */

/* $Revision$
 * $Date$
 */

#include <QDebug>
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>
#include <QPushButton>
#include <QSettings>
#include <QToolTip>
#include <QPainter>
#include <QInputDialog>
#include <QColorDialog>
#include <QMenu>

#include "baseengine.h"
#include "peerwidgetfactory.h"
#include "peerslayout.h"
#include "peeritem.h"
#include "switchboardwindow.h"
#include "userinfo.h"
#include "xivoconsts.h"
#include "externalphonedialog.h"
#include "group.h"

/*! \brief constructor
 *
 * initialize layout, attributes, etc.
 */
SwitchBoardWindow::SwitchBoardWindow(BaseEngine * engine,
                                     QWidget * parent)
    : QWidget(parent), m_engine(engine),
      m_trace_box(false), m_group_to_resize(0)
{
    m_peerwidgetfactory = new PeerWidgetFactory(m_engine, this),
    m_layout = new PeersLayout(this);
    setObjectName("scroller"); // in order for the style settings to be set accordingly
    setAcceptDrops(true);
    reloadGroups();
    reloadExternalPhones();
}

/*! 
 * Save the positions in the grid of the peer widgets.
 */
SwitchBoardWindow::~SwitchBoardWindow()
{
    savePositions();
    saveGroups();
    removePeers();
}

/*!
 * Does nothing
 */
void SwitchBoardWindow::setGuiOptions(const QVariant &)
{
}

/*!
 * Does nothing
 */
void SwitchBoardWindow::setUserInfo(const UserInfo *)
{
}

/*! \brief update display of user informations
 *
 * Update all what need to be displayed about this user.
 * PeerItem is created if needed.
 * Display the PeerWidget if needed.
 */
void SwitchBoardWindow::updateUser(UserInfo * ui)
{
    QString userid = ui->userid();
    PeerItem * peeritem = NULL;
    if(m_peerhash.contains(userid))
        {
            peeritem = m_peerhash.value(userid);
        }
    else
        {
            peeritem = new PeerItem(ui);
            m_peerhash.insert(userid, peeritem);
            QSettings * settings = m_engine->getSettings();
            settings->beginGroup("layout");
            QPoint pos = settings->value(userid, QPoint(-1, -1) ).toPoint();
            settings->endGroup();
            if(pos.x() >= 0) {
                /*BasePeerWidget * peerwidget = */addPeerWidget(peeritem, pos);
            }
        }
    peeritem->updateStatus();
    update();
}

/*! \brief update agent status
 *
 */
void SwitchBoardWindow::updatePeerAgent(double,
                                        const QString & id,
                                        const QString & what,
                                        const QVariant & statuslist)
{
    // qDebug() << "SwitchBoardWindow::updatePeerAgent()";
    if(m_peerhash.contains(id)) {
        if(what == "agentstatus")
            m_peerhash.value(id)->updateAgentStatus(statuslist);
        else if(what == "imstatus")
            m_peerhash.value(id)->updateStatus();
    }
}


/*! \brief remove the peer from the layout
 *
 * the peer is moved to position (-1, -1) so it
 * wont be displayed anymore
 *
 * \sa dropEvent()
 */
void SwitchBoardWindow::removePeerFromLayout()
{
    const QString userid = sender()->property( "userid" ).toString();
    qDebug() << "SwitchBoardWindow::removePeerFromLayout" << userid << m_peerhash.keys();
    if( m_peerhash.contains(userid) )
        {
            PeerItem * peeritem = m_peerhash[userid];
            BasePeerWidget * peerwidget = peeritem->getWidget();
            m_layout->removeWidget( peerwidget );

            m_layout->setItemPosition(peerwidget, QPoint(-1, -1));
            //savePositions();
            // this disconnect() step takes time, whether explicitly or implicitly,
            // so we should be careful to avoid too much connect's (anyway)
            disconnect( peerwidget, SIGNAL(actionCall(const QString &,
                                                      const QString &,
                                                      const QString &)),
                        m_engine, SLOT(actionCall(const QString &,
                                                  const QString &,
                                                  const QString &)) );
            disconnect( peerwidget, SIGNAL(removeFromPanel()),
                        this, SLOT(removePeerFromLayout()) );
            peerwidget->deleteLater();
            peeritem->setWidget(NULL);
            update();
        }
    else
        {
            m_layout->removeWidget( static_cast<QWidget *>(sender()) );
            sender()->deleteLater();
        }
}

/*! \brief remove a group
 */
void SwitchBoardWindow::removeGroup()
{
    qDebug() << "removeGroup" << sender()->property( "group" );
    Group * group = (Group *)(sender()->property( "group" ).value<void *>());
    if( group )
        {
            m_group_list.removeAll( group );
            group->deleteLater();
            update();
            //saveGroups();
        }
}

/*! \brief change a group color
 */
void SwitchBoardWindow::changeGroupColor()
{
    Group * group = (Group *)(sender()->property( "group" ).value<void *>());
    if( group )
        {
            QColor newColor = QColorDialog::getColor( group->color(), this );
            if( newColor.isValid() )
                {
                    group->setColor( newColor );
                    update();
                    //saveGroups();
                }
        }
}

/*! \brief change group name
 */
void SwitchBoardWindow::changeGroupName()
{    
    Group * group = (Group *)(sender()->property( "group" ).value<void *>());
    if( group )
        {
            QString newName
                = QInputDialog::getText( this, tr("Edit group color"),
                                         tr("Name for group"), QLineEdit::Normal,
                                         group->name() );
            if(!newName.isEmpty())
                {
                    group->setName( newName );
                    update();
                }
        }
}

/*! \brief remove a PeerItem
 *
 * Find the peer and remove it from the list
 * and the widget.
 *
 * \sa updatePeer
 * \sa removePeers
 */
void SwitchBoardWindow::removePeer(const QString & userid)
{
    // qDebug() << "SwitchBoardWindow::removePeer()" << userid;
    if(m_peerhash.contains(userid)) {
        PeerItem * peeritem = m_peerhash.value(userid);
        BasePeerWidget * peerwidget = peeritem->getWidget();
        m_layout->removeWidget( peerwidget );
        peerwidget->deleteLater();
        delete m_peerhash.take(userid);
    }
}


/*! \brief remove all peers
 *
 * remove all peers and widget.
 *
 * \sa removePeer
 */
void SwitchBoardWindow::removePeers(void)
{
    // qDebug() << "SwitchBoardWindow::removePeers()";
    QHashIterator<QString, PeerItem *> peeriter(m_peerhash);
    while(peeriter.hasNext()) {
        peeriter.next();
        PeerItem * peeritem = peeriter.value();
        //PeerWidget * peerwidget = peeritem->getWidget();
        BasePeerWidget * peerwidget = peeritem->getWidget();
        if(peerwidget != NULL) {
            m_layout->removeWidget( peerwidget );
            // this disconnect() step takes time, whether explicitly or implicitly,
            // so we should be careful to avoid too much connect's (anyway)
            disconnect( peerwidget, SIGNAL(actionCall(const QString &,
                                                      const QString &,
                                                      const QString &)),
                        m_engine, SLOT(actionCall(const QString &,
                                                  const QString &,
                                                  const QString &)) );
            disconnect( peerwidget, SIGNAL(removeFromPanel()),
                        this, SLOT(removePeerFromLayout()) );
            peerwidget->deleteLater();
        }
        delete peeritem;
    }
    m_peerhash.clear();
}

void SwitchBoardWindow::dragMoveEvent(QDragMoveEvent * event)
{
    event->acceptProposedAction();
}

/*!
 * This method accept or reject the drag event.
 *
 * \sa dropEvent()
 */
void SwitchBoardWindow::dragEnterEvent(QDragEnterEvent * event)
{
    // qDebug() << "SwitchBoardWindow::dragEnterEvent()" << event->mimeData()->formats();
    if( event->mimeData()->hasFormat(USERID_MIMETYPE) ||
        event->mimeData()->hasFormat(NUMBER_MIMETYPE) )
        event->acceptProposedAction();
}

/*! \brief Receives drop events
 * 
 * This method recieve drop events. It is currently used to 
 * move PeerWidgets arount :)
 *
 * \sa dragEnterEvent()
 */
void SwitchBoardWindow::dropEvent(QDropEvent * event)
{
    if( event->mimeData()->hasFormat(USERID_MIMETYPE) )
    {
        QString userid = event->mimeData()->data(USERID_MIMETYPE);
        if( m_peerhash.contains(userid) )
        {
            PeerItem * peeritem = m_peerhash[userid];
            BasePeerWidget * peerwidget = peeritem->getWidget();
            if( peerwidget )
            {
                m_layout->setItemPosition(peerwidget, m_layout->getPosInGrid(event->pos()));
            }
            else
            {
                peerwidget = addPeerWidget( peeritem, m_layout->getPosInGrid(event->pos()) );
            }
            updateGeometry();
            event->acceptProposedAction();
            savePositions();
            update();
        }
    }
    else if( event->mimeData()->hasFormat(NUMBER_MIMETYPE) )
    {
        QString number = event->mimeData()->data(NUMBER_MIMETYPE);
        for(int i = 0; i < m_layout->count(); i++)
        {
            QLayoutItem * item = m_layout->itemAt(i);
            if( item && item->widget()
                && (item->widget()->inherits("ExternalPhonePeerWidget")
                    || item->widget()->inherits("DetailedExternalPhonePeerWidget") ) )
//                && item->widget()->inherits("ExternalPhonePeerWidget") )
            {
                BasePeerWidget * w = static_cast<BasePeerWidget *>( item->widget() );
                if( number == w->number() )
                {
                    m_layout->setItemPosition(w, m_layout->getPosInGrid(event->pos()));
                    updateGeometry();
                    event->acceptProposedAction();
                    savePositions();
                    update();
                    break;
                }
            }
        }
    }
}


/*! \brief Save the positions of Peer Widgets
 *
 * Save the positions of all Peer Widgets to the settings.
 */
void SwitchBoardWindow::savePositions() const
{
    // qDebug() << "SwitchBoardWindow::savePositions()";
    QSettings * settings = m_engine->getSettings();
    settings->beginGroup("layout");
    QHashIterator<QString, PeerItem *> it( m_peerhash );
    while( it.hasNext() )
    {
        it.next();
        const QString userid = it.key();
        PeerItem * peeritem = it.value();
        if( peeritem->getWidget() )
        {
            settings->setValue( userid,
                                m_layout->getItemPosition( peeritem->getWidget() ) );
        }
        else
        {
            settings->remove( userid );
        }
    }
    settings->beginWriteArray("externalphone");
    int index = 0;
    for(int i = 0; i < m_layout->count(); i++)
    {
        QLayoutItem * item = m_layout->itemAt(i);
        if( item && item->widget()
            && (item->widget()->inherits("ExternalPhonePeerWidget")
                || item->widget()->inherits("DetailedExternalPhonePeerWidget") ) )
//            && item->widget()->inherits("ExternalPhonePeerWidget") )
        {
            BasePeerWidget * w = static_cast<BasePeerWidget *>( item->widget() );
            settings->setArrayIndex( index++ );
            settings->setValue( "position", m_layout->getItemPosition( w ) );
            settings->setValue( "name", w->name() );
            settings->setValue( "number", w->number() );
        }
    }
    settings->endArray();
    settings->endGroup();
}

/*! \brief reload existings "external phones" widget from config
 */
void SwitchBoardWindow::reloadExternalPhones()
{
    QSettings * settings = m_engine->getSettings();
    settings->beginGroup("layout");
    int size = settings->beginReadArray("externalphone");
    for(int i = 0; i < size; i++)
    {
        settings->setArrayIndex( i );
        BasePeerWidget * peerwidget
            = m_peerwidgetfactory->newExternalPhonePeerWidget( settings->value("name").toString(),
                                                               settings->value("number").toString() );
        connect( peerwidget, SIGNAL(removeFromPanel()),
                 this, SLOT(removePeerFromLayout()) );
        m_layout->addWidget( peerwidget, settings->value("position").toPoint() );
    }
    settings->endArray();
    settings->endGroup();
}

/*!
 * update m_second_corner if needed
 */
void SwitchBoardWindow::mouseMoveEvent ( QMouseEvent * event )
{
    //if( event->buttons() == Qt::LeftButton)
    //{
    //}
    if(m_trace_box)
    {
        // set second box corner
        m_second_corner = event->pos();
        // request redraw
        update();
    }
    if(m_group_to_resize)
    {
        QPoint delta = event->pos() - m_first_corner;
        QPoint deltaGrid = m_layout->getPosInGrid( delta );
        //qDebug() << delta << deltaGrid;
        switch(m_group_resize_mode)
        {
        case EMove:
            if(deltaGrid != QPoint(0, 0))
            {
                QRect rect = m_group_to_resize->rect().adjusted(0, 0, -1, -1);
                bool intersects = false;
                foreach(Group * group, m_group_list)
                {
                    if(group != m_group_to_resize)
                        intersects = intersects || group->rect().adjusted(0, 0, -1, -1).intersects( rect.translated(deltaGrid) );
                }
                if(!intersects)
                {
                    for(int i = 0; i < m_layout->count(); i++)
                    {
                        if( rect.contains( m_layout->getItemPosition( i ) ) )
                        {
                            m_layout->setItemPosition( i, m_layout->getItemPosition( i ) + deltaGrid );
                        }
                    }
                    m_group_to_resize->rect().translate(deltaGrid);
                    update();
                    m_first_corner += m_layout->getPosFromGrid(deltaGrid);
                }
            }
            break;
        case ETop:
            if(deltaGrid.y() != 0 && (m_group_to_resize->rect().height() - deltaGrid.y() > 1))
            {
                m_group_to_resize->rect().adjust(0, deltaGrid.y(), 0, 0);
                update();
                m_first_corner += m_layout->getPosFromGrid(deltaGrid);
            }
            break;
        case EBottom:
            if(deltaGrid.y() != 0 && (m_group_to_resize->rect().height() + deltaGrid.y() > 1))
            {
                m_group_to_resize->rect().adjust(0, 0, 0, deltaGrid.y());
                update();
                m_first_corner += m_layout->getPosFromGrid(deltaGrid);
            }
            break;
        case ELeft:
            if(deltaGrid.x() != 0 && (m_group_to_resize->rect().width() - deltaGrid.x() > 1))
            {
                m_group_to_resize->rect().adjust(deltaGrid.x(), 0, 0, 0);
                update();
                m_first_corner += m_layout->getPosFromGrid(deltaGrid);
            }
            break;
        case ERight:
            if(deltaGrid.x() != 0 && (m_group_to_resize->rect().width() + deltaGrid.x() > 1))
            {
                m_group_to_resize->rect().adjust(0, 0, deltaGrid.x(), 0);
                update();
                m_first_corner += m_layout->getPosFromGrid(deltaGrid);
            }
            break;
        default:
            qDebug() << "case not handled";
        }
    }
}

/*!
 * starts to draw a new group
 * or to move/resize an existing group
 */
void SwitchBoardWindow::mousePressEvent( QMouseEvent * event )
{
    // check which button was pressed
    if( event->button() == Qt::LeftButton)
        {
            // check if there is a group under the cursor
            Group * group = getGroup( m_layout->getPosInGrid( event->pos() ) );
            if( !group )
                {
                    // start drawing a box for a new group
                    m_first_corner = event->pos();
                    m_second_corner = event->pos();
                    m_trace_box = true;
                }
            else
                {
                    QPoint topLeft = m_layout->getPosFromGrid(group->rect().topLeft());
                    QPoint bottomRight = m_layout->getPosFromGrid(group->rect().bottomRight());
                    if( event->pos().y() - topLeft.y() < 10 )
                        {
                            setCursor( QCursor( Qt::SizeVerCursor ) );
                            m_group_resize_mode = ETop;
                        }
                    else if( bottomRight.y() - event->pos().y() < 10 )
                        {
                            setCursor( QCursor( Qt::SizeVerCursor ) );
                            m_group_resize_mode = EBottom;
                        }
                    else if( event->pos().x() - topLeft.x() < 10 )
                        {
                            setCursor( QCursor( Qt::SizeHorCursor ) );
                            m_group_resize_mode = ELeft;
                        }
                    else if( bottomRight.x() - event->pos().x() < 10 )
                        {
                            setCursor( QCursor( Qt::SizeHorCursor ) );
                            m_group_resize_mode = ERight;
                        }
                    else
                        {
                            setCursor( QCursor( Qt::SizeAllCursor ) );
                            m_group_resize_mode = EMove;
                        }
                    m_group_to_resize = group;
                    m_first_corner = event->pos();  // storing "start" position
                }
        }
}

void SwitchBoardWindow::mouseReleaseEvent( QMouseEvent * event )
{
    //if( event->button() == Qt::LeftButton)
    if( m_trace_box )
        {
            QRect rect = QRect( m_first_corner, m_second_corner ).normalized();
            //        QRect gridRect = QRect( m_layout->getPosInGrid(rect.topLeft()),
            //                                m_layout->getPosInGrid(rect.bottomRight()) );
            QRect gridRect = m_layout->getGridRect( rect );
            qDebug() << rect << gridRect;
            if( (gridRect.top() != gridRect.bottom())
                && (gridRect.right() != gridRect.left()) )
                {
                    QString name = 
                        QInputDialog::getText( this, tr("Name of the group"),
                                               tr("Please enter a name for the new group") );
                    if( !name.isEmpty() ) 
                        {
                            Group * group = new Group( this );
                            group->setRect( gridRect );
                            group->setName( name );
                            group->setColor( QColor(63, 63, 255) );
                            m_group_list << group;
                            saveGroups();
                        }
                }
            m_trace_box = false;
            update();
        }
    if( m_group_to_resize )
        {
            unsetCursor();
            m_group_to_resize = NULL;
        }
}

/*! \brief handle paint event
 *
 * Draw graphical representation of groups
 */
void SwitchBoardWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter( this );
    for(int i = 0; i < m_group_list.size(); i++)
    {
        // draw the color rectangle
        const QRect & gridRect = m_group_list[i]->rect();
        QRect rect = QRect( m_layout->getPosFromGrid(gridRect.topLeft()),
                            m_layout->getPosFromGrid(gridRect.bottomRight()) - QPoint(1, 1) );
        // skip useless groups !
        if(!event->region().contains(rect))
            continue;
        painter.setBrush( m_group_list[i]->color() );
        painter.setPen(Qt::NoPen);
        painter.drawRect( rect );
        // write the text
        painter.setPen(Qt::SolidLine);
        QFont font = this->font();
        font.setBold( true );
        if(m_group_list[i]->color().value() < 128)
            painter.setPen( QColor(0xcc, 0xcc, 0xcc) );
        painter.setFont( font );
        painter.drawText( rect, Qt::AlignTop | Qt::AlignHCenter, m_group_list[i]->name() );
    }
    if( m_trace_box )
    {
        painter.setBrush( Qt::NoBrush );
        painter.setPen( Qt::DashLine );
        painter.drawRect( QRect( m_first_corner, m_second_corner ) );
    }
}

/*! \brief show the context menu
 */
void SwitchBoardWindow::contextMenuEvent(QContextMenuEvent * event)
{
    //    qDebug() << "SwitchBoardWindow::contextMenuEvent";
    QMenu contextMenu( this );
    // check if there is a group under the cursor
    Group * group = getGroup( m_layout->getPosInGrid( event->pos() ) );
    if( group )
        {
            QAction * removeGroupAction
                = contextMenu.addAction( tr("Remove group %1").arg( group->name() ),
                                         this, SLOT(removeGroup()) );
            removeGroupAction->setProperty( "group", QVariant::fromValue( (void *)group ) );
            QAction * changeColorAction
                = contextMenu.addAction( tr("Change color of group %1").arg( group->name() ),
                                         this, SLOT(changeGroupColor()) );
            changeColorAction->setProperty( "group", QVariant::fromValue( (void *)group ) );
            QAction * changeNameAction
                = contextMenu.addAction( tr("Rename group %1").arg( group->name() ),
                                         this, SLOT(changeGroupName()) ); 
            changeNameAction->setProperty( "group", QVariant::fromValue( (void *)group ) );
        }
    QAction * addPhoneAction
        = contextMenu.addAction( tr("Add Phone number entry"),
                                 this, SLOT(addPhoneNumberEntry()) );
    addPhoneAction->setProperty( "pos", m_layout->getPosInGrid( event->pos() ) );
    contextMenu.exec( event->globalPos() );
}

/*! \brief save groups and their position
 *
 * Groups are saved into the settings "groups"
 */
void SwitchBoardWindow::saveGroups() const
{
    QSettings * settings = m_engine->getSettings();
    settings->beginGroup("groups");
    settings->beginWriteArray("groups");
    for(int i = 0; i < m_group_list.size(); i++)
    {
        settings->setArrayIndex( i );
        settings->setValue( "name", m_group_list[i]->name() );
        settings->setValue( "rect", m_group_list[i]->rect() );
        settings->setValue( "color", m_group_list[i]->color() );
    }
    settings->endArray();
    settings->endGroup();
}

/*! \brief load groups and their positions
 *
 * Groups are loaded from the settings "groups"
 */
void SwitchBoardWindow::reloadGroups()
{
    m_group_list.clear();
    QSettings * settings = m_engine->getSettings();
    settings->beginGroup("groups");
    int size = settings->beginReadArray("groups");
    for(int i = 0; i < size; i++)
    {
        settings->setArrayIndex( i );
        Group * group = new Group( this );
        group->setName( settings->value( "name" ).toString() );
        group->setRect( settings->value( "rect" ).toRect() );
        group->setColor( settings->value( "color" ).value<QColor>() );
        m_group_list.insert( i, group );
    }
    settings->endArray();
    settings->endGroup();
}

/*! \brief return the group under this position in the grid
 */
Group * SwitchBoardWindow::getGroup( const QPoint & position ) const
{
    foreach(Group * group, m_group_list)
    {
        if( group->rect().contains( position ) 
            && (group->rect().right() != position.x())
            && (group->rect().bottom() != position.y()) )
            return group;
    }
    return NULL;
}

/*! \brief add a widget for user display
 *
 * create the object, connect signal/slots,
 * put in layout and update display
 */
BasePeerWidget * SwitchBoardWindow::addPeerWidget(PeerItem * peeritem, const QPoint & pos)
{
    BasePeerWidget * peerwidget
        = m_peerwidgetfactory->newPeerWidget( peeritem->userinfo() );
    connect( peerwidget, SIGNAL(removeFromPanel()),
             this, SLOT(removePeerFromLayout()) );

    peeritem->setWidget(peerwidget);
    m_layout->addWidget( peerwidget, pos );
    peeritem->updateDisplayedStatus();
    peeritem->updateDisplayedName();
    return peerwidget;
}

/*! \brief Add an "external" user widget used to call
 */
void SwitchBoardWindow::addPhoneNumberEntry()
{
    qDebug() << "SwitchBoardWindow::addPhoneNumberEntry()";
    ExternalPhoneDialog dialog;
    int rDialog = dialog.exec();
    QPoint pos = sender()->property( "pos" ).toPoint();
    if( rDialog && !dialog.number().isEmpty() )
    {
        QString label = dialog.label();
        QString number = dialog.number();
        if( label.isEmpty() )
            label = number;
        BasePeerWidget * peerwidget
            = m_peerwidgetfactory->newExternalPhonePeerWidget( label, number );
        connect( peerwidget, SIGNAL(removeFromPanel()),
                 this, SLOT(removePeerFromLayout()) );
        m_layout->addWidget( peerwidget, pos );
    }
}

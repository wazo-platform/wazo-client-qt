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

#include <switchboardwindow.h>
#include <QtColorPicker>

#include <baseengine.h>
#include <peerwidgetfactory.h>
#include <peerslayout.h>
#include <peeritem.h>
#include <xivoconsts.h>
#include <externalphonedialog.h>
#include <group.h>

Q_EXPORT_PLUGIN2(xletswitchboardplugin, XLetSwitchboardPlugin);

XLet* XLetSwitchboardPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/switchboard_%1");
    return new XletSwitchBoard(parent);
}

/*! \brief constructor
 *
 * initialize layout, attributes, etc.
 */
XletSwitchBoard::XletSwitchBoard(QWidget *parent)
    : XLet(parent), m_drawGrid(false),
      m_trace_box(false), m_group_to_resize(0)
{
    setTitle(tr("Switchboard"));
    m_layout = new PeersLayout(this);
    setObjectName("scroller"); // in order for the style settings to be set accordingly
    setAccessibleName(tr("Switchboard"));
    setAcceptDrops(true);
    reloadGroups();
    reloadExternalPhones();

    connect(b_engine, SIGNAL(updateUserConfig(const QString &)),
            this, SLOT(updateUserConfig(const QString &)));
    connect(b_engine, SIGNAL(updateUserStatus(const QString &)),
            this, SLOT(updateUserStatus(const QString &)));
    connect(b_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QVariant &)),
            this, SLOT(updatePeerAgent(const QString &, const QString &, const QVariant &)));
    // savePositions() needs m_peerhash to be non-empty in order to be useful,
    // thus we call it before removePeers()
    connect(b_engine, SIGNAL(delogged()),
            this, SLOT(savePositions()));
    connect(b_engine, SIGNAL(delogged()),
            this, SLOT(removePeers()));

    setMouseTracking(true);
}

/*!
 * Save the positions in the grid of the peer widgets.
 */
XletSwitchBoard::~XletSwitchBoard()
{
    savePositions();
    saveGroups();
    removePeers();
}

/*! \brief update display of user informations
 *
 * Update all what need to be displayed about this user.
 * PeerItem is created if needed.
 * Display the PeerWidget if needed.
 */
void XletSwitchBoard::updateUserConfig(const QString & xuserid)
{
    qDebug() << Q_FUNC_INFO << xuserid;
    PeerItem * peeritem = NULL;
    if (m_peerhash.contains(xuserid)) {
        peeritem = m_peerhash.value(xuserid);
    } else {
        UserInfo * ui = b_engine->users()[xuserid];
        peeritem = new PeerItem(ui);
        m_peerhash.insert(xuserid, peeritem);
        QSettings *settings = b_engine->getSettings();
        settings->beginGroup("layout");
        QPoint pos = settings->value(xuserid, QPoint(-1, -1)).toPoint();
        settings->endGroup();
        if (pos.x() >= 0) {
            addPeerWidget(peeritem, pos);
        }
    }
    peeritem->updateStatus();
    update();
}

void XletSwitchBoard::updateUserStatus(const QString & xuserid)
{
    PeerItem * peeritem = NULL;
    if (m_peerhash.contains(xuserid)) {
        peeritem = m_peerhash.value(xuserid);
        peeritem->updateStatus();
        update();
    }
}

/*! \brief update agent status
 *
 */
void XletSwitchBoard::updatePeerAgent(const QString &id,
                                      const QString &what,
                                      const QVariant &statuslist)
{
    // qDebug() << Q_FUNC_INFO;
    if (m_peerhash.contains(id)) {
        if (what == "agentstatus") {
            m_peerhash.value(id)->updateAgentStatus(statuslist);
        } else if (what == "imstatus") {
            m_peerhash.value(id)->updateStatus();
        }
    }
}


/*! \brief remove the peer from the layout
 *
 * the peer is moved to position (-1, -1) so it
 * wont be displayed anymore
 *
 * \sa dropEvent()
 */
void XletSwitchBoard::removePeerFromLayout()
{
    const QString xuserid = sender()->property("xuserid").toString();
    // qDebug() << Q_FUNC_INFO << xuserid << m_peerhash.keys();
    if (m_peerhash.contains(xuserid)) {
        PeerItem *peeritem = m_peerhash[xuserid];
        BasePeerWidget *peerwidget = peeritem->getWidget();
        m_layout->removeWidget(peerwidget);

        m_layout->setItemPosition(peerwidget, QPoint(-1, -1));
        //savePositions();
        // this disconnect() step takes time, whether explicitly or implicitly,
        // so we should be careful to avoid too much connect's (anyway)
        disconnect(peerwidget, SIGNAL(removeFromPanel()),
                   this, SLOT(removePeerFromLayout()));
        peerwidget->deleteLater();
        peeritem->setWidget(NULL);
        update();
    } else {
        m_layout->removeWidget(static_cast<QWidget *>(sender()));
        sender()->deleteLater();
    }
}

/*! \brief remove a group
 */
void XletSwitchBoard::removeGroup()
{
    int ret;
    // qDebug() << "removeGroup" << sender()->property("group");
    Group *group = (Group *)(sender()->property("group").value<void *>());
    if (group) {
        ret = QMessageBox::question(this,
                tr("Removing group %1").arg(group->name()),
                tr("Removing group %1.\nAre you sure ?").arg(group->name()),
                QMessageBox::Yes | QMessageBox::Cancel,
                QMessageBox::Cancel);
        if (ret == QMessageBox::Yes) {
            m_group_list.removeAll(group);
            group->deleteLater();
            update();
            //saveGroups();
        }
    }
}

/*! \brief change a group color
 */
void XletSwitchBoard::changeGroupColor()
{
    Group *group = (Group *)(sender()->property("group").value<void *>());
    if (group) {
        QColor newColor = QColorDialog::getColor(group->color(), this);
        if (newColor.isValid()) {
            group->setColor(newColor);
            update();
            //saveGroups();
        }
    }
}

/*! \brief change group name
 */
void XletSwitchBoard::changeGroupName()
{
    Group *group = (Group *)(sender()->property("group" ).value<void *>());
    if (group) {
        QString newName
            = QInputDialog::getText(this, tr("Edit group name"),
                                    tr("Name for group"), QLineEdit::Normal,
                                    group->name());
        if (!newName.isEmpty()) {
            group->setName(newName);
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
void XletSwitchBoard::removePeer(const QString & xuserid)
{
    // qDebug() << Q_FUNC_INFO << xuserid;
    if (m_peerhash.contains(xuserid)) {
        PeerItem * peeritem = m_peerhash.value(xuserid);
        BasePeerWidget * peerwidget = peeritem->getWidget();
        m_layout->removeWidget(peerwidget);
        peerwidget->deleteLater();
        delete m_peerhash.take(xuserid);
        update();
    }
}

/*! \brief remove all peers
 *
 * remove all peers and widget.
 *
 * \sa removePeer
 */
void XletSwitchBoard::removePeers(void)
{
    // qDebug() << Q_FUNC_INFO;
    QHashIterator<QString, PeerItem *> peeriter(m_peerhash);
    while (peeriter.hasNext()) {
        peeriter.next();
        PeerItem *peeritem = peeriter.value();
        //PeerWidget * peerwidget = peeritem->getWidget();
        BasePeerWidget *peerwidget = peeritem->getWidget();
        if (peerwidget != NULL) {
            m_layout->removeWidget(peerwidget);
            // this disconnect() step takes time, whether explicitly or implicitly,
            // so we should be careful to avoid too much connect's (anyway)
            disconnect(peerwidget, SIGNAL(removeFromPanel()),
                       this, SLOT(removePeerFromLayout()) );
            peerwidget->deleteLater();
        }
        delete peeritem;
    }
    m_peerhash.clear();
    update();
}

void XletSwitchBoard::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

/*!
 * This method accept or reject the drag event.
 *
 * \sa dropEvent()
 */
void XletSwitchBoard::dragEnterEvent(QDragEnterEvent *event)
{
    // qDebug() << Q_FUNC_INFO << event->mimeData()->formats();
    if (event->mimeData()->hasFormat(XUSERID_MIMETYPE)  ||
        event->mimeData()->hasFormat(XPHONEID_MIMETYPE) ||
        event->mimeData()->hasFormat(NUMBER_MIMETYPE)) {
        event->acceptProposedAction();
    }
}

/*! \brief Receives drop events
 *
 * This method recieve drop events. It is currently used to
 * move PeerWidgets arount :)
 *
 * \sa dragEnterEvent()
 */
void XletSwitchBoard::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(XUSERID_MIMETYPE)) {
        qDebug() << Q_FUNC_INFO << event->mimeData()->data(XUSERID_MIMETYPE);
        QString xuserid = event->mimeData()->data(XUSERID_MIMETYPE);
        if (m_peerhash.contains(xuserid)) {
            PeerItem *peeritem = m_peerhash[xuserid];
            BasePeerWidget *peerwidget = peeritem->getWidget();
            if (peerwidget) {
                m_layout->setItemPosition(peerwidget, m_layout->getPosInGrid(event->pos()));
            } else {
                peerwidget = addPeerWidget(peeritem, m_layout->getPosInGrid(event->pos()));
            }
            updateGeometry();
            event->acceptProposedAction();
            savePositions();
            update();
        }
    } else if (event->mimeData()->hasFormat(NUMBER_MIMETYPE)) {
        QString number = event->mimeData()->data(NUMBER_MIMETYPE);
        BasePeerWidget *w = getExternalPhonePeerWidget(number);
        if (w) {
            m_layout->setItemPosition(w, m_layout->getPosInGrid(event->pos()));
            updateGeometry();
            event->acceptProposedAction();
            savePositions();
            update();
        }
    }
}

/** Return the "ExternalPhonePeerWidget" with a number if it already exists
 */
BasePeerWidget* XletSwitchBoard::getExternalPhonePeerWidget(const QString &number)
{
    int i;
    for (i=0;i<m_layout->count();i++) {
        QLayoutItem *item = m_layout->itemAt(i);
        if ((item) && (item->widget()) &&
           ((item->widget()->inherits("ExternalPhonePeerWidget")) ||
            (item->widget()->inherits("DetailedExternalPhonePeerWidget")))) {
            BasePeerWidget *w = static_cast<BasePeerWidget *>(item->widget());
            if (number == w->number()) {
                return w;
            }
        }
    }
    return NULL;
}

/*! \brief Save the positions of Peer Widgets
 *
 * Save the positions of all Peer Widgets to the settings.
 */
void XletSwitchBoard::savePositions() const
{
    // qDebug() << Q_FUNC_INFO << m_peerhash.count();
    QSettings * settings = b_engine->getSettings();
    settings->beginGroup("layout");
    QHashIterator<QString, PeerItem *> it(m_peerhash);
    while (it.hasNext()) {
        it.next();
        const QString userid = it.key();
        PeerItem *peeritem = it.value();
        if (peeritem->getWidget()) {
            settings->setValue(userid,
                               m_layout->getItemPosition(peeritem->getWidget()));
        } else {
            settings->remove(userid);
        }
    }

    settings->beginWriteArray("externalphone");
    int i, index = 0;
    for (i=0;i<m_layout->count();i++) {
        QLayoutItem *item = m_layout->itemAt(i);
        if ((item) && (item->widget()) &&
            ((item->widget()->inherits("ExternalPhonePeerWidget")) ||
             (item->widget()->inherits("DetailedExternalPhonePeerWidget")))) {
            BasePeerWidget *w = static_cast<BasePeerWidget *>(item->widget());
            settings->setArrayIndex(index++);
            settings->setValue("position", m_layout->getItemPosition(w));
            settings->setValue("name", w->name());
            settings->setValue("number", w->number());
        }
    }
    settings->endArray();
    settings->endGroup();
}

/*! \brief reload existings "external phones" widget from config
 */
void XletSwitchBoard::reloadExternalPhones()
{
    QSettings *settings = b_engine->getSettings();
    settings->beginGroup("layout");
    int i, size = settings->beginReadArray("externalphone");
    for (i=0;i<size;i++) {
        settings->setArrayIndex(i);
        BasePeerWidget *pw
            = PeerWidgetFactory::newExternalPhonePeerWidget(settings->value("name").toString(),
                                                            settings->value("number").toString());
        connect(pw, SIGNAL(removeFromPanel()),
                this, SLOT(removePeerFromLayout()));

        QPoint  pos = settings->value("position", QPoint(-1, -1)).toPoint();
        if (pos.x()>=0)
            m_layout->addWidget(pw, pos);
    }
    settings->endArray();
    settings->endGroup();
}

/*!
 * update m_second_corner if needed
 */
void XletSwitchBoard::mouseMoveEvent(QMouseEvent *event)
{
    if (m_trace_box) {
        // set second box corner
        m_second_corner = event->pos();
        // request redraw
        update();
    } else if (m_group_to_resize) {
        QPoint delta = event->pos() - m_first_corner;
        QPoint deltaGrid = m_layout->getPosInGrid(delta);

        if (deltaGrid == QPoint(0,0))
            return ;

        int updateNeeded = 0;
        if (m_group_resize_mode == EMove) {
            if (m_group_to_resize->move(m_group_list, m_layout, deltaGrid)) {
                updateNeeded = 1;
            }
        } else if (m_group_resize_mode&(ETop|EBottom|ELeft|ERight)) {
            if (m_group_to_resize->resize(m_group_list, m_layout, deltaGrid, m_group_resize_mode)) {
                updateNeeded = 1;
            }
        }

        if (updateNeeded) {
            update();
            m_first_corner += m_layout->getPosFromGrid(deltaGrid);
        }
    } else {
        // rien de special, mais on change le curseur si est sur une zone
        // pour resizer un groupe
        Group *group = getGroup(m_layout->getPosInGrid(event->pos()));
        if (group) {
            QPoint topLeft = m_layout->getPosFromGrid(group->rect().topLeft());
            QPoint bottomRight = m_layout->getPosFromGrid(group->rect().bottomRight());

            int direction = 0;
            if (event->pos().y() - topLeft.y() < 10) {
                setCursor(QCursor(Qt::SizeVerCursor));
                direction = ETop;
            } else if (bottomRight.y() - event->pos().y() < 10) {
                setCursor(QCursor(Qt::SizeVerCursor));
                direction = EBottom;
            }

            if (event->pos().x() - topLeft.x() < 10) {
                if (direction&ETop) {
                    setCursor(QCursor(Qt::SizeFDiagCursor));
                } else if (direction&EBottom) {
                    setCursor(QCursor(Qt::SizeBDiagCursor));
                } else {
                    setCursor(QCursor(Qt::SizeHorCursor));
                }
                direction |= ELeft;
            } else if (bottomRight.x() - event->pos().x() < 10) {
                if (direction&ETop) {
                    setCursor(QCursor(Qt::SizeBDiagCursor));
                } else if (direction&EBottom) {
                    setCursor(QCursor(Qt::SizeFDiagCursor));
                } else {
                    setCursor(QCursor(Qt::SizeHorCursor));
                }
                direction |= ERight;
            }
            if (!direction) {
                setCursor(QCursor(Qt::ArrowCursor));
            }
        } else {
            setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}

/*!
 * starts to draw a new group
 * or to move/resize an existing group
 */
void XletSwitchBoard::mousePressEvent(QMouseEvent *event)
{
    // check which button was pressed
    if (event->button() == Qt::LeftButton) {
        // check if there is a group under the cursor
        Group *group = getGroup(m_layout->getPosInGrid(event->pos()));
        if (!group) {
            // start drawing a box for a new group
            m_first_corner = event->pos();
            m_second_corner = event->pos();
            m_trace_box = true;
        } else {
            QPoint topLeft = m_layout->getPosFromGrid(group->rect().topLeft());
            QPoint bottomRight = m_layout->getPosFromGrid(group->rect().bottomRight());
            m_group_resize_mode = 0;

            if (event->pos().y() - topLeft.y() < 10) {
                setCursor(QCursor(Qt::SizeVerCursor));
                m_group_resize_mode |= ETop;
            } else if (bottomRight.y() - event->pos().y() < 10) {
                setCursor(QCursor(Qt::SizeVerCursor));
                m_group_resize_mode |= EBottom;
            }


            if (event->pos().x() - topLeft.x() < 10) {
                if (m_group_resize_mode&ETop) {
                    setCursor(QCursor(Qt::SizeFDiagCursor));
                } else if (m_group_resize_mode&EBottom) {
                    setCursor(QCursor(Qt::SizeBDiagCursor));
                } else {
                    setCursor(QCursor(Qt::SizeHorCursor));
                }
                m_group_resize_mode |= ELeft;
            } else if (bottomRight.x() - event->pos().x() < 10) {
                if (m_group_resize_mode&ETop) {
                    setCursor(QCursor(Qt::SizeBDiagCursor));
                } else if (m_group_resize_mode&EBottom) {
                    setCursor(QCursor(Qt::SizeFDiagCursor));
                } else {
                    setCursor(QCursor(Qt::SizeHorCursor));
                }
                m_group_resize_mode |= ERight;
            }

            if (!m_group_resize_mode) {
                setCursor(QCursor(Qt::SizeAllCursor));
                m_group_resize_mode = EMove;
            }

            m_group_to_resize = group;
            m_first_corner = event->pos();  // storing "start" position
        }
    }
}

void XletSwitchBoard::mouseReleaseEvent(QMouseEvent *)
{
    // qDebug() << Q_FUNC_INFO;
    if (m_trace_box) {
        QRect rect = QRect(m_first_corner, m_second_corner).normalized();

        QRect gridRect = m_layout->getGridRect(rect);
        if ((gridRect.top() != gridRect.bottom()) &&
           (gridRect.right() != gridRect.left())) {
            QString name;
            QDialog dialog;
            dialog.setWindowTitle(tr("New group"));
            QVBoxLayout *layout = new QVBoxLayout(&dialog);
            layout->addWidget(new QLabel(tr("Please enter a name for the new group"), &dialog));
            QLineEdit *lineedit = new QLineEdit(&dialog);
            layout->addWidget(lineedit);
            layout->addWidget(new QLabel(tr("Please choose a color for the new group"), &dialog));
            QtColorPicker *colorpicker = new QtColorPicker(&dialog);
            colorpicker->setStandardColors();
            colorpicker->setCurrentColor(QColor(63, 63, 255));
            layout->addWidget(colorpicker);
            QDialogButtonBox *buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                               Qt::Horizontal, &dialog);
            layout->addWidget(buttonbox);
            connect(buttonbox, SIGNAL(accepted()),
                    &dialog, SLOT(accept()) );
            connect(buttonbox, SIGNAL(rejected()),
                    &dialog, SLOT(reject()) );
            dialog.exec();
            if (dialog.result() == QDialog::Accepted)
                name = lineedit->text();
            if (!name.isEmpty()) {
                Group *group = new Group(this);
                group->setRect(gridRect);
                group->setName(name);
                group->setColor(colorpicker->currentColor());
                m_group_list.append(group);
                saveGroups();
            }
        }
        m_trace_box = false;
        update();
    }
    if (m_group_to_resize) {
        unsetCursor();
        m_group_to_resize = NULL;
    }
}

/*! \brief handle paint event
 *
 * Draw graphical representation of groups
 */
void XletSwitchBoard::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int i;
    if (m_drawGrid) {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::SolidLine);
        int widgetWidth = width(), widgetHeight = height();
        int rowWidth = m_layout->maxItemSize().width() + 2;
        int rowHeight = m_layout->maxItemSize().height() + 2;

        for (i=rowWidth;i<widgetWidth;i+=rowWidth) {
            painter.drawLine(i, 0, i, widgetHeight);
        }
        for (i=rowHeight;i<widgetHeight;i+=rowHeight) {
            painter.drawLine(0, i, widgetWidth, i);
        }
    }
    for (i=0;i<m_group_list.size();i++) {
        // draw the color rectangle
        const QRect &gridRect = m_group_list[i]->rect();
        QRect rect = QRect(m_layout->getPosFromGrid(gridRect.topLeft()),
                           m_layout->getPosFromGrid(gridRect.bottomRight()) - QPoint(1, 1) );
        // skip useless groups !
        if (!event->region().contains(rect))
            continue;
        painter.setBrush(m_group_list[i]->color() );
        painter.setPen(Qt::NoPen);
        painter.drawRect(rect.adjusted(1, 0, -1, 0));
        // to make rounded border
        painter.drawRect(rect.x(), rect.y() + 1, 1, rect.height() - 2);
        painter.drawRect(rect.right(), rect.y() + 1, 1, rect.height() - 2);
        // write the text
        painter.setPen(Qt::SolidLine);
        QFont font = this->font();
        font.setBold(true);
        if (m_group_list[i]->color().value() < 128) {
            painter.setPen(QColor(0xcc, 0xcc, 0xcc));
        }
        painter.setFont(font);
        painter.drawText(rect, Qt::AlignTop | Qt::AlignHCenter, m_group_list[i]->name());
    }
    if (m_trace_box) {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::DashLine);
        painter.drawRect(QRect(m_first_corner, m_second_corner));
    }
}

/*! \brief show the context menu
 */
void XletSwitchBoard::contextMenuEvent(QContextMenuEvent *event)
{
    // check if there is a group under the cursor
    Group *group = getGroup(m_layout->getPosInGrid(event->pos()));

    QMenu contextMenu(this);
    QAction *action;
    if (group) {
        action = contextMenu.addAction(tr("Remove group %1").arg(group->name()),
                                       this, SLOT(removeGroup()));
        action->setProperty("group", QVariant::fromValue((void *)group));

        action = contextMenu.addAction(tr("Change color of group %1").arg(group->name()),
                                       this, SLOT(changeGroupColor()));
        action->setProperty("group", QVariant::fromValue((void *)group));

        action = contextMenu.addAction(tr("Rename group %1").arg(group->name()),
                                       this, SLOT(changeGroupName()));
        action->setProperty("group", QVariant::fromValue((void *)group));
    }

    action = contextMenu.addAction(tr("Add Phone number entry"),
                                   this, SLOT(addPhoneNumberEntry()));
    action->setProperty("pos", m_layout->getPosInGrid(event->pos()));

    action = new QAction(tr("Draw the grid"), this);
    action->setCheckable(true);
    action->setChecked(m_drawGrid);
    connect(action, SIGNAL(toggled(bool)),
            this, SLOT(drawTheGrid(bool)));
    contextMenu.addAction(action);

    contextMenu.exec(event->globalPos());
}

void XletSwitchBoard::drawTheGrid(bool drawGrid)
{
    m_drawGrid = drawGrid;
    update();
}

/*! \brief save groups and their position
 *
 * Groups are saved into the settings "groups"
 */
void XletSwitchBoard::saveGroups() const
{
    // qDebug() << Q_FUNC_INFO;
    QSettings *settings = b_engine->getSettings();
    settings->beginGroup("groups");
    settings->beginWriteArray("groups");
    int i;
    for (i= 0;i < m_group_list.size();i++) {
        settings->setArrayIndex(i);
        settings->setValue("name", m_group_list[i]->name());
        settings->setValue("rect", m_group_list[i]->rect());
        settings->setValue("color", m_group_list[i]->color());
    }
    settings->endArray();
    settings->endGroup();
}

/*! \brief load groups and their positions
 *
 * Groups are loaded from the settings "groups"
 */
void XletSwitchBoard::reloadGroups()
{
    m_group_list.clear();
    QSettings *settings = b_engine->getSettings();
    settings->beginGroup("groups");
    Group *group;
    int i, size = settings->beginReadArray("groups");
    for (i=0;i<size;i++) {
        settings->setArrayIndex(i);
        group = new Group(this);
        group->setName(settings->value("name").toString());
        group->setRect(settings->value("rect").toRect());
        group->setColor(settings->value("color").value<QColor>());
        m_group_list.insert(i, group);
    }
    settings->endArray();
    settings->endGroup();
}

/*! \brief return the group under this position in the grid
 */
Group* XletSwitchBoard::getGroup(const QPoint &position) const
{
    foreach(Group *group, m_group_list) {
        if ((group->rect().contains(position)) &&
           (group->rect().right() != position.x()) &&
           (group->rect().bottom() != position.y())) {
            return group;
        }
    }
    return NULL;
}

/*! \brief add a widget for user display
 *
 * create the object, connect signal/slots,
 * put in layout and update display
 */
BasePeerWidget* XletSwitchBoard::addPeerWidget(PeerItem *peeritem, const QPoint &pos)
{
    BasePeerWidget *peerwidget
        = PeerWidgetFactory::newPeerWidget(peeritem->userinfo());
    connect(peerwidget, SIGNAL(removeFromPanel()),
            this, SLOT(removePeerFromLayout()));

    peeritem->setWidget(peerwidget);
    m_layout->addWidget(peerwidget, pos);
    peeritem->updateDisplayedStatus();
    return peerwidget;
}

/*! \brief Add an "external" user widget used to call
 */
void XletSwitchBoard::addPhoneNumberEntry()
{
    QPoint pos = sender()->property("pos" ).toPoint();
    // qDebug() << Q_FUNC_INFO;
    ExternalPhoneDialog dialog;
    int rDialog = dialog.exec();
    if (rDialog && !dialog.number().isEmpty()) {
        QString label = dialog.label();
        QString number = dialog.number();
        if (label.isEmpty()) {
            label = number;
        }
        // verifier tout d'abord qu'il n'y a pas deja une etiquette avec
        // le meme numero
        if (getExternalPhonePeerWidget(number)) {
            QMessageBox::information(this, tr("Duplicate number"),
                tr("Phone number %1 is already present in the switchboard panel.").arg(number));
        } else {
            BasePeerWidget *peerwidget
                = PeerWidgetFactory::newExternalPhonePeerWidget(label, number);
            connect(peerwidget, SIGNAL(removeFromPanel()),
                    this, SLOT(removePeerFromLayout()));
            m_layout->addWidget(peerwidget, pos);
            update();
        }
    }
}

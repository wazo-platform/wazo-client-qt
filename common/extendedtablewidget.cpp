/* XIVO CTI clients
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Revision$
 * $Date$
 */

#include <QContextMenuEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QHeaderView>
#include <QMenu>
#include <QUrl>

#include "extendedtablewidget.h"
#include "xivoconsts.h"

/*! \brief Constructor
 */
ExtendedTableWidget::ExtendedTableWidget(QWidget * parent)
        : QTableWidget(parent)
{
	setAcceptDrops(true);
        setAlternatingRowColors(true);
        this->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
}

/*! \brief Constructor
 */
ExtendedTableWidget::ExtendedTableWidget(int rows, int columns, QWidget * parent)
        : QTableWidget(rows, columns, parent)
{
	setAcceptDrops(true);
        setAlternatingRowColors(true);
}

/*! \brief display the context Menu
 */
void ExtendedTableWidget::contextMenuEvent(QContextMenuEvent * event)
{
	qDebug() << "ExtendedTableWidget::contextMenuEvent()";
	qDebug() << event->pos();
	QTableWidgetItem * item = itemAt( event->pos() );
	QRegExp re_number("\\+?[0-9\\s\\.]+");
	if(item && re_number.exactMatch( item->text() )) {
		m_numberToDial = item->text();
		qDebug() << "preparing to dial :" << m_numberToDial;
		QMenu contextMenu(this);
		contextMenu.addAction( tr("&Dial"), this, SLOT(dialNumber()) );
		if(!m_mychannels.empty()) {
			QMenu * transferMenu = new QMenu(tr("&Transfer"), &contextMenu);
			QListIterator<PeerChannel *> i(m_mychannels);
			while(i.hasNext())
			{
				const PeerChannel * channel = i.next();
				transferMenu->addAction(channel->otherPeer(),
				                        channel, SLOT(transfer()));
			}
			contextMenu.addMenu(transferMenu);
		}
		contextMenu.exec( event->globalPos() );
	}

 	if(item && item->text().contains("@")) {
                m_mailAddr = item->text();
                qDebug() << "email addr detection :" << m_mailAddr;
 		QMenu emailContextMenu(this);
                emailContextMenu.addAction( tr("Send an E-mail"), this, SLOT(sendMail()) );
                emailContextMenu.exec( event->globalPos() );
 	}
}

/*! \brief dial the number (when context menu item is toggled)
 */
void ExtendedTableWidget::dialNumber()
{
	if(m_numberToDial.length() > 0)
	{
		emitDial( m_numberToDial );
	}
}

/*! \brief dial the number (when context menu item is toggled)
 */
void ExtendedTableWidget::sendMail()
{
        if(m_mailAddr.length() > 0) {
                qDebug() << "ExtendedTableWidget::sendMail()" << m_mailAddr;
                QDesktopServices::openUrl(QUrl("mailto:" + m_mailAddr));
        }
}

/*! \brief update call list for transfer
 */
void ExtendedTableWidget::updateMyCalls(const QStringList & chanIds,
                                        const QStringList & chanStates,
                                        const QStringList & chanOthers)
{
	while(!m_mychannels.isEmpty())
		delete m_mychannels.takeFirst();
	for(int i = 0; i<chanIds.count(); i++)
	{
		PeerChannel * ch = new PeerChannel(chanIds[i], chanStates[i], chanOthers[i]);
		connect(ch, SIGNAL(transferChan(const QString &)),
		        this, SLOT(transferChan(const QString &)) );
		m_mychannels << ch;
	}
}

/*! \brief transfer channel to the number
 */
void ExtendedTableWidget::transferChan(const QString & chan)
{
	transferCall(chan, m_numberToDial);
}

/*! \brief filter drag events
 */
void ExtendedTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
	//qDebug() << "ExtendedTableWidget::dragEnterEvent" << event->mimeData()->formats() << event->pos();
	if(  event->mimeData()->hasFormat(PEER_MIMETYPE)
	  || event->mimeData()->hasFormat(CHANNEL_MIMETYPE) )
	{
		event->acceptProposedAction();
	}
}

/*! \brief filter drag move events
 *
 * Detect if the move is over a cell containing a phone number.
 */
void ExtendedTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
	//qDebug() << "ExtendedTableWidget::dragMoveEvent()" << event->pos();
	if(event->proposedAction() & ( Qt::CopyAction | Qt::MoveAction ))
		event->acceptProposedAction();
	QTableWidgetItem * item = itemAt( event->pos() );
	if(item)
	{
		QRegExp re("\\+?[0-9\\s\\.]+");
		if(re.exactMatch( item->text() ))
			event->accept(visualItemRect(item));
		else
			event->ignore(visualItemRect(item));
	}
	else
		event->ignore();
}

/*! \brief receive drop event
 */
void ExtendedTableWidget::dropEvent(QDropEvent *event)
{
	qDebug() << "ExtendedTableWidget::dropEvent()" << event->mimeData()->text() << event->pos();
	QTableWidgetItem * item = itemAt( event->pos() );
	QRegExp re("\\+?[0-9\\s\\.]+");
	if(item && re.exactMatch( item->text() ))
	{
		qDebug() << item->text();
		QString from = event->mimeData()->text();
		if(event->mimeData()->hasFormat(CHANNEL_MIMETYPE))
		{
			event->acceptProposedAction();
			transferCall(from, item->text());
		}
		else if(event->mimeData()->hasFormat(PEER_MIMETYPE))
		{
			event->acceptProposedAction();
			originateCall(from, item->text());
		}
		else
			event->ignore();
	}
	else
		event->ignore();
}


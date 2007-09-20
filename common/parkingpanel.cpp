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

#include <QDebug>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTime>
#include <QTimerEvent>
#include <QVBoxLayout>

#include "searchpanel.h"
#include "peerwidget.h"
#include "baseengine.h"
#include "parkingpanel.h"

/*! \brief Constructor
 *
 * Set up the layout and the table widget.
 */
ParkingPanel::ParkingPanel(QWidget * parent)
	: QWidget(parent)
{
	QVBoxLayout * vlayout = new QVBoxLayout(this);
	vlayout->setMargin(0);
	m_table = new QTableWidget( this );
        qDebug() << m_table;
        m_table->setAlternatingRowColors(true);
        m_table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	m_table->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
	m_table->setColumnCount( 5 );
	QStringList labels;
	labels << "Asterisk";
	labels << tr("Number");
	labels << tr("Time");
	labels << tr("Parked");
	labels << tr("Parker");
	m_table->setHorizontalHeaderLabels(labels);

	connect( m_table, SIGNAL(itemClicked(QTableWidgetItem *)),
	         this, SLOT(itemClicked(QTableWidgetItem *)) );
	connect( m_table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	         this, SLOT(itemDoubleClicked(QTableWidgetItem *)) );

	vlayout->addWidget( m_table, 0 );
        m_table->resizeColumnsToContents();
        m_timerid = 0;
        m_deltasec = 2;
}

ParkingPanel::~ParkingPanel()
{
        // qDebug() << "ParkingPanel::~ParkingPanel()";
        delete m_table;
}

/*! \brief add a message to the list
 *
 * add a new message.
 */
void ParkingPanel::parkingEvent(const QString & eventkind, const QString & str)
{
        QStringList newpark = str.split(";");
        QString astid, parkplacenum, seconds, parkedpeer, parkedby;
	// QTime time = QTime::currentTime();

        if(newpark.size() == 5) {
                astid        = newpark[0];
                parkplacenum = newpark[3];
                seconds      = newpark[4];
                parkedpeer   = newpark[1].split("-")[0];
                parkedby     = newpark[2].split("-")[0];
        } else
                return;

        if(eventkind == "parkedcall") {
                m_table->insertRow( 0 );
                QTableWidgetItem * item0 = new QTableWidgetItem( astid );
                item0->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
                m_table->setItem( 0, 0, item0 );
                QTableWidgetItem * item1 = new QTableWidgetItem( parkplacenum );
                item1->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
                m_table->setItem( 0, 1, item1 );
                QTableWidgetItem * item2 = new QTableWidgetItem( seconds );
                item2->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
                m_table->setItem( 0, 2, item2 );
                QTableWidgetItem * item3 = new QTableWidgetItem( parkedpeer );
                item3->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
                m_table->setItem( 0, 3, item3 );
                QTableWidgetItem * item4 = new QTableWidgetItem( parkedby );
                item4->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
                m_table->setItem( 0, 4, item4 );
                //                m_table->resizeColumnsToContents();

                // do not start another timer if there is already one running
                if(m_timerid == 0)
                        m_timerid = startTimer(m_deltasec * 1000);
        } else if ((eventkind == "unparkedcall") ||
                   (eventkind == "parkedcalltimeout") ||
                   (eventkind == "parkedcallgiveup")) {
                QList<QTableWidgetItem *> finditem = m_table->findItems(parkplacenum, Qt::MatchExactly);
                for(int i = 0; i < finditem.count(); i++) {
                        int n = m_table->column(finditem[i]);
                        int m = m_table->row(finditem[i]);
                        if (n == 1)
                                if (astid == m_table->item(m, 0)->text())
                                        if (parkedpeer == m_table->item(m, 3)->text()) {
                                                m_table->removeRow(m);
                                                // kills the timer only if there are no parked calls left
                                                if(m_table->rowCount() == 0) {
                                                        killTimer(m_timerid);
                                                        m_timerid = 0;
                                                }
                                        }
                }
        }
}

void ParkingPanel::itemClicked(QTableWidgetItem * item)
{
        int rown = m_table->row(item);
        QString astid = m_table->item(rown, 0)->text();
        QString placenum = m_table->item(rown, 1)->text();

        copyNumber(m_table->item(rown, 1)->text());
}

void ParkingPanel::itemDoubleClicked(QTableWidgetItem * item)
{
        int rown = m_table->row(item);
        QString astid = m_table->item(rown, 0)->text();
        QString placenum = m_table->item(rown, 1)->text();

        emitDial(m_table->item(rown, 1)->text());
}

void ParkingPanel::timerEvent(QTimerEvent * event)
{
	int timerId = event->timerId();
        if (timerId == m_timerid)
                for(int i = 0; i < m_table->rowCount(); i++) {
                        QTableWidgetItem * item = m_table->item(i, 2);
                        int leftsec = item->text().toInt() - m_deltasec;
                        item->setText( QString::number(leftsec) );
                        m_table->setItem( i, 2, item );
                }
}


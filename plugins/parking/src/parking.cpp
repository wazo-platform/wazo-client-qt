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


#include "parking.h"

Q_EXPORT_PLUGIN2(xletparkingplugin, XLetParkingPlugin);

XLet* XLetParkingPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/parking_%1");
    return new XletParking(parent);
}


XletParking::XletParking(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Parking"));
    QVBoxLayout * vlayout = new QVBoxLayout(this);
    vlayout->setMargin(0);
    m_table = new ExtendedTableWidget(this);
    qDebug() << m_table;
    m_table->setAlternatingRowColors(true);
    m_table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_table->setColumnCount(4);
    QStringList labels = (QStringList() << /*tr("XiVO Id") << */tr("Number") << tr("Time") << tr("Parked") << tr("Parker"));
    m_table->setHorizontalHeaderLabels(labels);
    
    connect(m_table, SIGNAL(itemClicked(QTableWidgetItem *)),
            this, SLOT(itemClicked(QTableWidgetItem *)));
    connect(m_table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
            this, SLOT(itemDoubleClicked(QTableWidgetItem *)) );
    vlayout->addWidget( m_table, 0 );
    m_table->resizeColumnsToContents();
    m_timerid = 0;
    m_deltasec = 2;
    // connect signal / slots
    connect(b_engine, SIGNAL(parkingEvent(const QString &,
                                          const QString &,
                                          const QString &,
                                          const QVariant &)),
            this, SLOT(parkingEvent(const QString &,
                                    const QString &,
                                    const QString &,
                                    const QVariant &)));
    connect(this, SIGNAL(copyNumber(const QString &)),
            b_engine, SIGNAL(pasteToDialPanel(const QString &)));
}

/*! \brief add a message to the list
 *
 * add a new message.
 */
void XletParking::parkingEvent(const QString & eventkind,
                                const QString & astid,
                                const QString & parkingbay,
                                const QVariant & subcommand)
{
    ParkingInfo * pi = b_engine->parking()[astid][parkingbay];
    QVariantMap map = subcommand.toMap();
    QString channel = map["channel"].toString();
    QString fromchannel = map["fromchannel"].toString();
    QString calleridnum = map["calleridnum"].toString();
    QString calleridname = map["calleridname"].toString();
    QString fromcalleridnum = map["fromcalleridnum"].toString();
    QString fromcalleridname = map["fromcalleridname"].toString();
    // ignore buggy events
    if(fromchannel == channel)
        qDebug() << " *** WARNING channel == fromchannel ***" << channel;
    //    return;
    
    //QString parkedpeer = channel.split("-")[0];
    QString parkedpeer = QString("%1 (%2)").arg(calleridname).arg(calleridnum);
    QString parkedby = fromchannel.split("-")[0];
    if(!fromcalleridnum.isEmpty())
        parkedby = QString("%1 (%2)").arg(fromcalleridname).arg(fromcalleridnum);
    
    if(eventkind == "parkedcall") {
        for(int m = 0; m < m_table->rowCount(); m++) {
            if ((m_table->item(m, 0)->data(Qt::UserRole+0).toString() == astid) &&
                (m_table->item(m, 0)->data(Qt::UserRole+1).toString() == parkingbay)) {
                // do not add the same entry twice !
                return;
            }
        }
        int i = 0;
        m_table->insertRow( 0 );
        //QTableWidgetItem * item0 = new QTableWidgetItem( astid );
        //item0->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        //m_table->setItem( 0, 0, item0 );
        QTableWidgetItem * item1 = new QTableWidgetItem( parkingbay );
        item1->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        item1->setData(Qt::UserRole+0, astid);
        item1->setData(Qt::UserRole+1, parkingbay);
        m_table->setItem( 0, i++, item1 );
        
        double remaining = pi->timeout() + pi->parkingtime() - QDateTime::currentDateTime().toTime_t() + b_engine->timeDeltaServerClient();
        int leftsec = int(remaining + 0.5);
        
        QTableWidgetItem * item2 = new QTableWidgetItem(QString("%1 s").arg(leftsec));
        item2->setText(QString("%1 s").arg(leftsec));
        item2->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        m_table->setItem( 0, i++, item2 );
        QTableWidgetItem * item3 = new QTableWidgetItem( parkedpeer );
        item3->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        m_table->setItem( 0, i++, item3 );
        QTableWidgetItem * item4 = new QTableWidgetItem( parkedby );
        item4->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        m_table->setItem( 0, i++, item4 );
        
        // do not start another timer if there is already one running
        if(m_timerid == 0)
            m_timerid = startTimer(m_deltasec * 1000);
    } else if ((eventkind == "unparkedcall") ||
               (eventkind == "parkedcalltimeout") ||
               (eventkind == "parkedcallgiveup")) {
        for(int m = 0; m < m_table->rowCount(); m++) {
            if ( (m_table->item(m, 0)->data(Qt::UserRole+0).toString() == astid) &&
                 (m_table->item(m, 0)->data(Qt::UserRole+1).toString() == parkingbay)) {
                m_table->removeRow(m);
                // kills the timer only if there are no parked calls left
                if(m_table->rowCount() == 0) {
                    killTimer(m_timerid);
                    m_timerid = 0;
                }
            }
        }
    }
    // make columns fit the content
    if(m_table->rowCount() > 0)
        m_table->resizeColumnsToContents();
}

void XletParking::itemClicked(QTableWidgetItem * item)
{
    int rown   = m_table->row(item);
    QString astid      = m_table->item(rown, 0)->data(Qt::UserRole+0).toString();
    QString parkingbay = m_table->item(rown, 0)->data(Qt::UserRole+1).toString();
    //qDebug() << Q_FUNC_INFO << rown << astid << placenum;
    if (((b_engine) && (b_engine->getXivoClientUser())) &&
         (astid == b_engine->getXivoClientUser()->astid())) {
        b_engine->pasteToDial(parkingbay);
    }
}

void XletParking::itemDoubleClicked(QTableWidgetItem * item)
{
    int rown   = m_table->row(item);
    QString astid      = m_table->item(rown, 0)->data(Qt::UserRole+0).toString();
    QString parkingbay = m_table->item(rown, 0)->data(Qt::UserRole+1).toString();
    //qDebug() << Q_FUNC_INFO << rown << astid << placenum;
    if(b_engine && b_engine->getXivoClientUser() &&
       (astid == b_engine->getXivoClientUser()->astid())) {
        b_engine->actionCall("originate", "user:special:me", "ext:" + parkingbay);
    }
}

void XletParking::timerEvent(QTimerEvent * event)
{
    int timerId = event->timerId();
    if (timerId == m_timerid) {
        for (int i = 0; i < m_table->rowCount(); i++) {
            QTableWidgetItem * item = m_table->takeItem(i, 1);
            QString astid      = m_table->item(i, 0)->data(Qt::UserRole+0).toString();
            QString parkingbay = m_table->item(i, 0)->data(Qt::UserRole+1).toString();
            ParkingInfo * pi = b_engine->parking()[astid][parkingbay];
            double remaining = pi->timeout() + pi->parkingtime() - QDateTime::currentDateTime().toTime_t() + b_engine->timeDeltaServerClient();
            int leftsec = int(remaining + 0.5);
            item->setText( QString::number(leftsec) + " s" );
            m_table->setItem( i, 1, item );
        }
    }
}

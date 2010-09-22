/* XiVO CTI clients
 * Copyright (C) 2007-2010  Proformatique
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

#include <QContextMenuEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QDropEvent>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>
#include <QLineEdit>

#include "baseengine.h"
#include "extendedtablewidget.h"
//#include "extendedlineedit.h"
#include "xivoconsts.h"
#include "phoneinfo.h"
#include "userinfo.h"

#include "outlook_panel.h"
#include "outlook_tools.h"
#include "outlook_engine.h"

/*! \brief Constructor
 *
 *  Build layout and child widgets, connect signals/slots.
 */
OutlookPanel::OutlookPanel(QWidget * parent)
    : XLet(parent),
      m_calllength(20), m_callprefix("")
{
    OLEngine()->start_thread();
    setTitle( tr("Outlook") );

    displayname["FirstName"] = tr("First Name");
    displayname["LastName"] = tr("Last Name");
    displayname["FullName"] = tr("Full Name");
    displayname["BusinessTelephoneNumber"] = tr("Business Phone");
    displayname["MobileTelephoneNumber"] = tr("Mobile Phone");
    displayname["HomeTelephoneNumber"] = tr("Home Phone");
    displayname["Email1Address"] = tr("Email Address");
    displayname["EntryID"] = tr("EntryID");

    QVBoxLayout * vlayout = new QVBoxLayout(this);
    vlayout->setMargin(0);
    QLabel * titleLbl = new QLabel( tr("Di&rectory"), this );
    vlayout->addWidget( titleLbl, 0, Qt::AlignCenter );
    QHBoxLayout * hlayout = new QHBoxLayout();
    //m_input = new ExtendedLineEdit(this);
    m_input = new QLineEdit(this);
    titleLbl->setBuddy(m_input);
    connect( m_input, SIGNAL(textChanged(const QString &)),
             this, SLOT(affTextChanged(const QString &)) );
    hlayout->addWidget( m_input );
    vlayout->addLayout( hlayout );
    m_table = new ExtendedTableWidget( this );
    connect( m_table, SIGNAL(itemClicked(QTableWidgetItem *)),
             this, SLOT(itemClicked(QTableWidgetItem *)) );
    connect( m_table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
             this, SLOT(itemDoubleClicked(QTableWidgetItem *)) );
    connect( m_table, SIGNAL(ContextMenuEvent(QContextMenuEvent *)),
             this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
    connect( m_table, SIGNAL(actionCall(const QString &, const QString &)),
             this, SLOT(proxyCallRequests(const QString &, const QString &)) );

    vlayout->addWidget(m_table);
    setAcceptDrops(true);
    m_numberToDial = "";

    setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(m_input);

    COLPropsDef props_def;
    OLEngine()->get_props_def(props_def);

    COLPropsDef::iterator i;

    for ( i = props_def.begin() ; i != props_def.end() ; ++i ) {
        COLCol * pCol;
        m_cols.append(pCol=new COLCol());
        pCol->m_def=i.value();
    }

    m_table->setSortingEnabled(true);
    m_table->sortByColumn(0, Qt::AscendingOrder);

    refresh_table();
    // connect signals/slots to engine
    connect( &(OLEngine()->m_OLThread), SIGNAL(contactsLoaded()),
             this, SLOT(contactsLoaded()) );
    connect( this, SIGNAL(searchOutlook(const QString &)),
             b_engine, SLOT(searchOutlook(const QString &)) );
    connect( this, SIGNAL(copyNumber(const QString &)),
             b_engine, SLOT(copyNumber(const QString &)) );
    connect( b_engine, SIGNAL(outlookResponse(const QString &)),
             this, SLOT(setSearchResponse(const QString &)) );
}

class QTableWidgetItemExt : public QTableWidgetItem {
public:
    QTableWidgetItemExt(const QString &text, int type = Type) : QTableWidgetItem(text, type) {}
    virtual ~QTableWidgetItemExt() {}

    virtual bool operator<(const QTableWidgetItem &other) const {
        QString str1=text().toLower();
        QString str2=other.text().toLower();
        return str1 < str2 ;
    }
};

void OutlookPanel::refresh_table() {
    int col_count = 0;
    QStringList labelList;
    for ( int i = 0, c = m_cols.count() ; i < c ; i++ ) {
        COLCol * pCol = m_cols[i];
        if ( pCol->m_bEnable && pCol->m_def.m_bDisplayable) {
            col_count++;
            labelList << displayname[pCol->m_def.m_strName];
        }
    }
    m_table->setColumnCount(col_count);
    m_table->setHorizontalHeaderLabels( labelList );

    COLContacts contacts;
    OLEngine()->get_contacts(contacts);

    // static int first=true;
    int sort_col = 0;
    Qt::SortOrder sort_order = Qt::AscendingOrder;
    m_table->setRowCount(contacts.count());
    QHeaderView * h = m_table->horizontalHeader();
    if ( h ) {
        sort_col = h->sortIndicatorSection();
        sort_order = h->sortIndicatorOrder();
    }

    m_table->setSortingEnabled(false);
    col_count = 0;
    for ( int j = 0, d = m_cols.count() ; j < d ; j++ ) {
        COLCol * pCol = m_cols[j];
        if ( !pCol->m_bEnable || !pCol->m_def.m_bDisplayable ) continue ;
        m_table->setColumnWidth(col_count, 100);

        for ( int i = 0, c = contacts.count() ; i < c ; i++ ) {
            const COLContact & contact = contacts.at(i);

            if ( col_count == 0 )
                m_table->setRowHeight(i, 20);

            QString val = contact.m_properties.value(pCol->m_def.m_strName);
            QTableWidgetItemExt * item = new QTableWidgetItemExt(val);
            item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled ); // Qt::ItemIsDragEnabled

            QRegExp re_number("\\+?[0-9\\s\\.]+");
            QString strTmpVal = cleanup_num(val);
            if(strTmpVal.contains("@"))
                item->setToolTip(tr("Double-click to send an E-mail to") + "\n" + val);
            else if(re_number.exactMatch(strTmpVal))
                item->setToolTip(tr("Double-click to call") + "\n" + val);
            //item->setStatusTip();
            // qDebug() << x << y << item->flags();
            m_table->setItem( i, col_count, item );
        }
        col_count++;
    }
    m_table->setSortingEnabled(true);
}

void OutlookPanel::proxyCallRequests(const QString & src, const QString & dst)
{
    b_engine->actionCall(sender()->property("action").toString(), src, dst); // Call
}

void OutlookPanel::setGuiOptions(const QVariant &)
{
}

void OutlookPanel::setUserInfo(const UserInfo * ui)
{
    m_userinfo = ui;
}

void OutlookPanel::focusInEvent(QFocusEvent * event)
{
    qDebug() << "OutlookPanel::focusInEvent()" << event;
}

void OutlookPanel::dropEvent(QDropEvent * event)
{
    qDebug() << "OutlookPanel::dropEvent()" << event;
}

void OutlookPanel::itemClicked(QTableWidgetItem * item)
{
    //qDebug() << item << item->text();
    // check if the string is a number
    QRegExp re_number("\\+?[0-9\\s\\.]+");
    QString str=callnum(item->text());
    if(re_number.exactMatch(str)) {
        // qDebug() << "OutlookPanel::itemClicked()" << "preparing to dial" << item->text();
        if(str.size() >= m_calllength)
            copyNumber(m_callprefix + str);
        else
            copyNumber(str);
    }
}

void OutlookPanel::itemDoubleClicked(QTableWidgetItem * item)
{
    //qDebug() << item << item->text();
    // check if the string is a number
    QRegExp re_number("\\+?[0-9\\s\\.]+");
    QString str;
    if ( item ) {
        str=callnum(item->text());
        /*    if ( str.length() && str[0] != '0' )
              str='0'+str; */
    }

    if(re_number.exactMatch(str)) {
        b_engine->actionCall("originate", "user:special:me", "ext:" + str); // Call

        // 0.3 actions :
        // if(item->text().size() >= m_calllength)
        // emitDial(m_callprefix + str);
        // else
        // emitDial(str);
    }

    if(item && item->text().contains("@")) {
        QString mailAddr = item->text();
        if(mailAddr.length() > 0) {
            //qDebug() << "OutlookPanel::itemDoubleClicked() : mail" << mailAddr;
            QDesktopServices::openUrl(QUrl("mailto:" + mailAddr));
        }
    }
}

/*! \brief receive and process search response
 *
 * Parses the response, sets column and row headers,
 * set table cells.
 */
void OutlookPanel::setSearchResponse(const QString & resp)
{
    int i, x, y;
    //qDebug() << "setSearchResponse()" << resp;
    QStringList items = resp.split(";");
    int ncolumns = items[0].toInt();
    if(ncolumns > 0) {
        int nrows = ((items.size() - 1) / ncolumns) - 1;
        if(nrows >= 0) {
            m_table->setColumnCount(ncolumns);
            m_table->setRowCount(nrows);
            // qDebug() << items.size() << nrows << ncolumns ;
            QStringList labelList;
            for(i = 1; i <= ncolumns; i++)
                labelList << items[i];
            m_table->setHorizontalHeaderLabels( labelList );
            for(y = 0; y < nrows; y++) {
                for(x = 0; x < ncolumns; x++) {
                    QString it = items[1+(1+y)*ncolumns+x];
                    QTableWidgetItem * item = new QTableWidgetItem(it);
                    item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled ); // Qt::ItemIsDragEnabled

                    QRegExp re_number("\\+?[0-9\\s\\.]+");
                    if(it.contains("@"))
                        item->setToolTip(tr("Double-click to send an E-mail to") + "\n" + it);
                    else if(re_number.exactMatch(it))
                        item->setToolTip(tr("Double-click to call") + "\n" + it);
                    //item->setStatusTip();
                    // qDebug() << x << y << item->flags();
                    m_table->setItem( y, x, item );
                    //qDebug() << m_table->cellWidget( y, x );
                }
            }
        }
    }
}

/*! \brief stop
 *
 * clear everything.
 */
void OutlookPanel::stop()
{
    m_table->setRowCount(0);
    m_table->setColumnCount(0);
    m_input->setText("");
}

void OutlookPanel::setCol(int col) {
    if ( col >= 0 && col < m_cols.count() ) {
        COLCol * pCol = m_cols[col];
        pCol->m_bEnable=!pCol->m_bEnable;
        refresh_table();
    }
}

#include <QSignalMapper>

void OutlookPanel::doColumnsMenu(QContextMenuEvent * event) {
    QMenu contextMenu(this);

    QSignalMapper *signalMapper = new QSignalMapper(this);

    QAction * a;
    for ( int i = 0, c = m_cols.count() ; i < c ; i++ ) {
        COLCol * pCol = m_cols[i];
        if ( !pCol->m_def.m_bDisplayable ) continue ;
        a = contextMenu.addAction(displayname[pCol->m_def.m_strName]);
        a->setCheckable(true);
        a->setChecked(pCol->m_bEnable);

        connect(a, SIGNAL(triggered()), signalMapper, SLOT(map()));
        signalMapper->setMapping(a, i);
    }

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(setCol(int)) );

    contextMenu.exec( event->globalPos() );
}

void OutlookPanel::contactsLoaded() {
    refresh_table();
    apply_filter();
}

void OutlookPanel::contextMenuEvent(QContextMenuEvent * event)
{
    QTableWidgetItem * item = m_table->itemAt( event->pos() );
    if (item == NULL) {
        doColumnsMenu(event);
        return;
    }

    QRegExp re_number("\\+?[0-9\\s\\.]+");
    QString strVal;
    if ( item )
        strVal=cleanup_num(item->text());
    if(item && re_number.exactMatch( strVal )) {
        if(item->text().size() >= m_calllength)
            m_numberToDial = cleanup_num(m_callprefix + item->text());
        else
            m_numberToDial = strVal;
        // qDebug() << "OutlookPanel::contextMenuEvent()" << "preparing to dial" << m_numberToDial;
        QMenu contextMenu(this);
        contextMenu.addAction( tr("&Dial"), this, SLOT(dialNumber()) );
        QMenu * transferMenu = new QMenu(tr("&Transfer"), &contextMenu);
        if(m_userinfo)
            {
                foreach( const QString phone, m_userinfo->phonelist() )
                    {
                        const PhoneInfo * pi = m_userinfo->getPhoneInfo( phone );
                        if( pi )
                            {
                                QMapIterator<QString, QVariant> it( pi->comms() );
                                while( it.hasNext() )
                                    {
                                        it.next();
                                        QMap<QString, QVariant> call = it.value().toMap();
                                        QString text;
                                        if( call.contains("calleridname") )
                                            {
                                                text.append( call["calleridname"].toString() );
                                                text.append( " : " );
                                            }
                                        text.append( call["calleridnum"].toString() );
                                        QAction * transferAction =
                                            transferMenu->addAction( text,
                                                                     this, SLOT(transfer()) );
                                        transferAction->setProperty( "chan", call["peerchannel"] );
                                    }
                            }
                    }
            }
#if 0
        if(false) { //!m_mychannels.empty()) {
            QListIterator<PeerChannel *> i(m_mychannels);
            while(i.hasNext()) {
                const PeerChannel * channel = i.next();
                transferMenu->addAction(channel->otherPeer(),
                                        channel, SLOT(transfer()));
            }
        }
#endif
        if( !transferMenu->isEmpty() )
            contextMenu.addMenu( transferMenu );
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

void OutlookPanel::apply_filter() {
    bool is_empty=m_strFilter.isEmpty();
    for ( int i = 0, c = m_table->rowCount() ; i < c ; i++ ) {
        bool bShow=is_empty;

        if ( !is_empty ) {
            for ( int j = 0, d = m_cols.count(), col=0 ; j < d ; j++ ) {
                COLCol * pCol = m_cols[j];
                if ( !pCol->m_bEnable || !pCol->m_def.m_bDisplayable ) continue ;

                QTableWidgetItem * item = m_table->item(i, col);
                if ( item && item->text().contains(m_strFilter, Qt::CaseInsensitive) ) {
                    bShow=true;
                    break;
                }
                col++;
            }
        }

        if ( bShow )
            m_table->showRow(i);
        else
            m_table->hideRow(i);

    }
}

void OutlookPanel::affTextChanged(const QString & searched) {
    m_strFilter=searched;
    apply_filter();
}



/*! \brief dial the number (when context menu item is toggled)
 */
void OutlookPanel::dialNumber()
{
    if(m_numberToDial.length() > 0)
        emitDial(m_numberToDial);
}

/*! \brief dial the number (when context menu item is toggled)
 */
void OutlookPanel::sendMail()
{
    if(m_mailAddr.length() > 0) {
        qDebug() << "ExtendedTableWidget::sendMail()" << m_mailAddr;
        QDesktopServices::openUrl(QUrl("mailto:" + m_mailAddr));
    }
}

/*! \brief update call list for transfer
 */
void OutlookPanel::updatePeer(UserInfo *,
                              const QString &,
                              const QVariant & chanlist)
{
#if 0
    while(!m_mychannels.isEmpty())
        delete m_mychannels.takeFirst();
    foreach(QString ref, chanlist.toMap().keys()) {
        QVariant chanprops = chanlist.toMap()[ref];
        if(chanprops.toMap()["status"].toString() != CHAN_STATUS_HANGUP) {
            PeerChannel * ch = new PeerChannel(chanprops);
            connect(ch, SIGNAL(transferChan(const QString &)),
                    this, SLOT(transferChan(const QString &)) );
            m_mychannels << ch;
        }
    }
#endif
}

/*! \brief transfer channel to the number
 */
void OutlookPanel::transfer()
{
    QString chan = sender()->property( "chan" ).toString();
    if( !chan.isEmpty() && !m_numberToDial.isEmpty() )
        b_engine->actionCall("transfer", "chan:special:me:" + chan, "ext:" + m_numberToDial);
}

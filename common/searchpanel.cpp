/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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

#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDebug>

#include "searchpanel.h"
#include "peerwidget.h"
#include "baseengine.h"
#include "extendedlineedit.h"
#include "userinfo.h"

SearchPanel::SearchPanel(BaseEngine * engine,
                         QWidget * parent)
    : XLet(engine, parent)
{
    // qDebug() << "SearchPanel::SearchPanel()" << options;
    QVariantMap optionsMap = m_engine->getGuiOptions("client_gui").toMap();
    m_maxdisplay = optionsMap["contacts-max"].toUInt();
    m_ncolumns = optionsMap["contacts-width"].toUInt();
    // m_engine->askCallerIds();
    
    QVBoxLayout * vlayout = new QVBoxLayout(this);
    vlayout->setMargin(0);
    QLabel * lbl = new QLabel( tr("N&ame or number to search :"), this );
    vlayout->addWidget(lbl, 0, Qt::AlignCenter);
    m_input = new ExtendedLineEdit( this );
    lbl->setBuddy(m_input);
    connect( m_input, SIGNAL(textChanged(const QString &)),
             this, SLOT(affTextChanged(const QString &)) );
    vlayout->addWidget( m_input );
    QScrollArea * scrollarea = new QScrollArea(this);
    scrollarea->setWidgetResizable(true);
    QWidget * widget = new QWidget(scrollarea);
    widget->setObjectName("scroller");
    scrollarea->setWidget(widget);
    QVBoxLayout * scrollarealayout = new QVBoxLayout(widget);
    m_peerlayout = new QGridLayout();
    scrollarealayout->addLayout( m_peerlayout );
    scrollarealayout->addStretch( 1 );
    vlayout->addWidget(scrollarea);
    
    m_searchpattern = "";

    // connect signal/slots
    connect( m_engine, SIGNAL(userUpdated(UserInfo *)),
             this, SLOT(updateUser(UserInfo *)) );
    connect( m_engine, SIGNAL(updatePeerAgent(double, const QString &, const QString &, const QVariant &)),
             this, SLOT(updatePeerAgent(double, const QString &, const QString &, const QVariant &)) );
    connect( m_engine, SIGNAL(peersReceived()),
             this, SLOT(updateDisplay()) );
    connect( this, SIGNAL(askCallerIds()),
             m_engine, SLOT(askCallerIds()) );
    connect( m_engine, SIGNAL(delogged()),
             this, SLOT(removePeers()) );
}

SearchPanel::~SearchPanel()
{
    // qDebug() << "SearchPanel::~SearchPanel()";
    removePeers();
}

/*! \brief apply the search
 */
void SearchPanel::affTextChanged(const QString & text)
{
    m_searchpattern = text;
    updateDisplay();
}

/*! \brief update the list of Persons displayed
 */
void SearchPanel::updateDisplay()
{
    // first hide/delete everyonedisplayed
    QHashIterator<QString, PeerItem *> i( m_peerhash );
    while(i.hasNext())
        {
            i.next();
            PeerItem * peeritem = i.value();
            BasePeerWidget * peerwidget = peeritem->getWidget();

            if ( (peerwidget != NULL)
                 && (m_peerlayout->indexOf( peerwidget ) > -1) )
                {
                    m_peerlayout->removeWidget( peerwidget );
                    peerwidget->hide();
                    // originate
                    // if switchboard : transfer, atxfer, hangup, intercept
                    disconnect( peerwidget, SIGNAL(actionCall(const QString &,
                                                              const QString &,
                                                              const QString &)),
                                m_engine, SLOT(actionCall(const QString &,
                                                          const QString &,
                                                          const QString &)) );
                    peeritem->setWidget(NULL);
                    //delete peerwidget;
                    peerwidget->deleteLater();
                }
        }
    
    // then display all users whose name match the search pattern
    int naff = 0;
    i.toFront();
    while(i.hasNext())
        {
            i.next();
            PeerItem * peeritem = i.value();
            BasePeerWidget * peerwidget = peeritem->getWidget();
            
            if( ( peeritem->userinfo()->fullname().contains(m_searchpattern, Qt::CaseInsensitive)
                  || peeritem->userinfo()->phonenumber().contains(m_searchpattern))
                && (naff < m_maxdisplay) )
                {
                    if(peerwidget == NULL)
                        {
                            // qDebug() << "SearchPanel::affTextChanged()" << peeritem->userinfo() << peeritem->userinfo()->termstatus();
                            peerwidget = new PeerWidget(m_engine, peeritem->userinfo());
                            //peerwidget->setEngine(m_engine);
                            peeritem->setWidget(peerwidget);
                            peeritem->updateDisplayedStatus();
                            peeritem->updateDisplayedName();
                                
                            m_peerlayout->addWidget(peerwidget,
                                                    naff / m_ncolumns,
                                                    naff % m_ncolumns);
                            // m_peerlayout->addWidget(peerwidget,
                            // naff / m_ncolumns,
                            // naff % m_ncolumns,
                            // Qt::AlignCenter);
                            // peerwidget->setMinimumSize(QSize(150, 60));
                                
                            naff ++;
                            peerwidget->show();
                            // if switchboard : transfer, atxfer, hangup, intercept
                            connect( peerwidget, SIGNAL(actionCall(const QString &,
                                                                   const QString &,
                                                                   const QString &)),
                                     m_engine, SLOT(actionCall(const QString &,
                                                               const QString &,
                                                               const QString &)) );
                        }
                }
        }
}

/*! \brief update display according to changes
 */
void SearchPanel::updateUser(UserInfo * ui)
{
    const QString & userid = ui->userid();
    PeerItem * peeritem = NULL;
    if(m_peerhash.contains(userid))
        {
            peeritem = m_peerhash.value(userid);
        }
    else
        {
            peeritem = new PeerItem(ui);
            m_peerhash.insert(userid, peeritem);
        }
    peeritem->updateStatus();
}

void SearchPanel::updatePeerAgent(double,
                                  const QString & id,
                                  const QString & what,
                                  const QVariant & statuslist)
{
    // qDebug() << "SearchPanel::updatePeerAgent()";
    if(m_peerhash.contains(id)) {
        if(what == "agentstatus")
            m_peerhash.value(id)->updateAgentStatus(statuslist);
        else if(what == "imstatus")
            m_peerhash.value(id)->updateStatus();
    }
    return;
}

/*! \brief remove on peer
 */
void SearchPanel::removePeer(const QString & ext)
{
    // qDebug() << "SearchPanel::removePeer()" << ext;
    if(m_peerhash.contains(ext)) {
        PeerItem * peeritem = m_peerhash.value(ext);
        BasePeerWidget * peerwidget = peeritem->getWidget();
        if (m_peerlayout->indexOf( peerwidget ) > -1)
            m_peerlayout->removeWidget( peerwidget );
        m_peerhash.remove(ext);
        delete peerwidget; // peerwidget->deleteLater();
        return;
    }
}

/*! \brief clear the widget
 */
void SearchPanel::removePeers()
{
    // qDebug() << "SearchPanel::removePeers()";
    foreach(QString peerkey, m_peerhash.keys()) {
        PeerItem * peeritem = m_peerhash[peerkey];
        BasePeerWidget * peerwidget = peeritem->getWidget();
        if( peerwidget )
            {
                // qDebug() << "span" << m_peerlayout->indexOf( peerwidget );
                if (m_peerlayout->indexOf( peerwidget ) > -1)
                    m_peerlayout->removeWidget( peerwidget );
                delete peerwidget;
                //peerwidget->deleteLater();
            }
        delete peeritem;
    }
    // qDebug() << "SearchPanel::removePeers : time elapsed" << qtime.elapsed();
    m_peerhash.clear();
}

/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
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

#include "peeritem.h"
#include "peerwidget.h"
#include "userinfo.h"
#include "xivoconsts.h"

/*! \brief Constructor
 */
PeerItem::PeerItem(UserInfo * ui)
        : m_ui(ui)
{
        m_peerwidget = NULL;
}

PeerItem::PeerItem()
{
        m_peerwidget = NULL;
}

/*! \brief Copy constructor
 */
PeerItem::PeerItem(const PeerItem & peer)
{
	m_ui = peer.m_ui;
	m_peerwidget = peer.m_peerwidget;
}

/*! \brief update status of the peer
 *
 * Change what is displayed according to new status values.
 */
void PeerItem::updateStatus()
{
        if(m_peerwidget != NULL)
                updateDisplayedStatus();
}

void PeerItem::updateAgentStatus(const QStringList & agentstatus)
{
        // qDebug() << "PeerItem::updateAgentStatus()" << agentstatus;
        m_agentstatus = agentstatus;
        if(m_peerwidget != NULL)
                updateDisplayedStatus();
}

/*! \brief update channel list
 */
void PeerItem::updateChans(const QVariant & chanlist)
{
        m_chanlist = chanlist;
        if(m_peerwidget != NULL)
                updateDisplayedChans();
}

/*! \brief update name if changed
 */
void PeerItem::updateName(const QString & newname)
{
	if(newname != m_ui->fullname())
		m_ui->setFullName(newname);
        if(m_peerwidget != NULL)
                updateDisplayedName();
}

/*! \brief update status of the peer
 *
 * Change what is displayed according to new status values.
 */
void PeerItem::updateDisplayedStatus()
{
        if(m_peerwidget == NULL)
                return;
        
        QStringList fortooltip;
        
        // qDebug() << "PeerItem::updateDisplayedStatus()";
        if(! m_ui->ctilogin().isEmpty()) {
                m_peerwidget->setColorAvail("presence",
                                            m_ui->availstate().toMap()["color"].toString(),
                                            m_ui->availstate().toMap()["longname"].toString());
                fortooltip.append(m_peerwidget->getToolTip("presence"));
        }
        
        foreach(QString term, m_ui->termstatus().keys()) {
                QVariantMap termstatusmap = m_ui->termstatus()[term].toMap();
                m_peerwidget->setColorAvail(term,
                                            termstatusmap["color"].toString(),
                                            termstatusmap["longname"].toString());
                fortooltip.append(m_peerwidget->getToolTip(term));
        }
        
        if(m_agentstatus.size() >= 4) {
                // PeerItem::updateDisplayedStatus() ("agentlogout", "xivo", "6103", "103")
                // PeerItem::updateDisplayedStatus() ("queuememberstatus", "6103", "qcb_00000", "5", "0")
                QString astid = m_agentstatus[1];
                QString agentnum = m_agentstatus[2];
                
                if(m_agentstatus[0] == "agentlogin") {
                        m_peerwidget->setAgentToolTip(agentnum, m_queuelist);
                        m_peerwidget->setColorAvail("agent", "green", "");
                } else if(m_agentstatus[0] == "agentlogout") {
                        m_peerwidget->setAgentToolTip("", m_queuelist);
                        m_peerwidget->setColorAvail("agent", "grey", "");
                } else if(m_agentstatus[0] == "joinqueue") {
                        if(! m_queuelist.contains(m_agentstatus[3]))
                                m_queuelist.append(m_agentstatus[3]);
                        m_peerwidget->setAgentToolTip(agentnum, m_queuelist);
                } else if(m_agentstatus[0] == "leavequeue") {
                        if(m_queuelist.contains(m_agentstatus[3]))
                                m_queuelist.removeAll(m_agentstatus[3]);
                        m_peerwidget->setAgentToolTip(agentnum, m_queuelist);
                } else if(m_agentstatus[0] == "queuememberstatus") {
                        if(m_agentstatus[4] == "1") {
                                m_peerwidget->setColorAvail("agent", "green", "");
                        } else if(m_agentstatus[4] == "3") {
                                m_peerwidget->setColorAvail("agent", "blue", "");
                        } else if(m_agentstatus[4] == "5") {
                                m_peerwidget->setColorAvail("agent", "grey", "");
                        } else {
                                m_peerwidget->setColorAvail("agent", "yellow", "");
                        }
                        m_peerwidget->setAgentToolTip(agentnum, m_queuelist);
                } else if(m_agentstatus[0] == "agentstatus") {
                        if(m_agentstatus[4].size() > 0)
                                m_queuelist = m_agentstatus[4].split(",");
                        else
                                m_queuelist = QStringList();
                        if (m_agentstatus[3] == "0") {
                                m_peerwidget->setAgentToolTip("", m_queuelist);
                                m_peerwidget->setColorAvail("agent", "grey", "");
                        } else {
                                m_peerwidget->setAgentToolTip(agentnum, m_queuelist);
                                m_peerwidget->setColorAvail("agent", "green", "");
                        }
                        
                        // ("agentstatus", "xivo", "6102", "0", "qcb_00003,qcb_00000")
                } else {
                        qDebug() << "UNKNOWN in updateDisplayedStatus()" << m_agentstatus;
                        // m_peerwidget->setAgentToolTip(agentnum, m_queuelist);
                        // m_peerwidget->setBlue("agent");
                }
        } else if(m_agentstatus.size() > 0) {
                qDebug() << "PeerItem::updateDisplayedStatus() / 0 < size < 4" << m_agentstatus;
        }
        
	m_peerwidget->setToolTip(fortooltip.join("\n"));
        
        return;
}

/*! \brief update channel list
 */
void PeerItem::updateDisplayedChans()
{
	if(m_peerwidget == NULL)
                return;

        m_peerwidget->clearChanList();
        foreach(QString ref, m_chanlist.toMap().keys()) {
                QVariant chanprops = m_chanlist.toMap()[ref];
                m_peerwidget->addChannel(chanprops);
        }
        return;
}

/*! \brief update name if changed
 */
void PeerItem::updateDisplayedName()
{
	if(m_peerwidget == NULL)
                return;

        m_peerwidget->setName(m_ui->fullname());
        return;
}

UserInfo * PeerItem::userinfo()
{
        return m_ui;
}

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

/*! \brief Constructor
 */
PeerItem::PeerItem(const UserInfo * ui)
        : m_ui(ui)
{
        m_ext = ui->userid();
        m_name = ui->fullname();
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
	m_ext = peer.m_ext;
	m_name = peer.m_name;
	m_peerwidget = peer.m_peerwidget;
}

/*! \brief update status of the peer
 *
 * Change what is displayed according to new status values.
 */
void PeerItem::updateStatus(const QString & sipstatus)
{
        m_sipstatus   = sipstatus;
        if(m_peerwidget != NULL)
                updateDisplayedStatus();
}

void PeerItem::updateIMStatus(const QString & imavail)
{
        m_imavail   = imavail;
        if(m_peerwidget != NULL)
                updateDisplayedStatus();
}

void PeerItem::updateAgentStatus(const QString & agentstatus)
{
        // qDebug() << "PeerItem::updateAgentStatus()" << agentstatus;
        m_agentstatus = agentstatus;
        if(m_peerwidget != NULL)
                updateDisplayedStatus();
}

/*! \brief update channel list
 */
void PeerItem::updateChans(const QStringList & chanIds,
                           const QStringList & chanStates,
                           const QStringList & chanOthers)
{
	if( (chanIds.size() != chanStates.size())
            || (chanIds.size() != chanOthers.size()) ) {
		qDebug() << "PeerItem::updateChans() : bad args";
		return;
	}

        m_chanIds    = chanIds;
        m_chanStates = chanStates;
        m_chanOthers = chanOthers;

        if(m_peerwidget != NULL)
                updateDisplayedChans();
}

/*! \brief update name if changed
 */
void PeerItem::updateName(const QString & newname)
{
	if(newname != m_name)
		m_name = newname;
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

	QString display_imavail;
	QString display_sipstatus;

	//qDebug() << imavail << sipstatus;
	if(m_imavail == "available") {
		m_peerwidget->setColor("presence", "green");
		display_imavail = PeerWidget::tr("Available");
	} else if(m_imavail == "away") {
		m_peerwidget->setColor("presence", "blue");
		display_imavail = PeerWidget::tr("Away");
	} else if(m_imavail == "donotdisturb") {
		m_peerwidget->setColor("presence", "red");
		display_imavail = PeerWidget::tr("Do not disturb");
	} else if(m_imavail == "berightback") {
		m_peerwidget->setColor("presence", "orange");
		display_imavail = PeerWidget::tr("Be Right Back");
	} else if(m_imavail == "outtolunch") {
		m_peerwidget->setColor("presence", "yellow");
		display_imavail = PeerWidget::tr("Out To Lunch");
	} else if(m_imavail == "unknown") {
		m_peerwidget->setColor("presence", "grey");
		display_imavail = PeerWidget::tr("Unknown");
	} else {
		m_peerwidget->setColor("presence", "grey");
		display_imavail = m_imavail;
	}

        foreach(QString term, m_ui->termlist()) {
                QString termstatus = m_ui->termstatus()[term];
                if(termstatus == "Ready") {
                        m_peerwidget->setColor(term, "green");
                        display_sipstatus = PeerWidget::tr("Ready");
                } else if(termstatus == "Ringing") {
                        m_peerwidget->setColor(term, "blue");
                        display_sipstatus = PeerWidget::tr("Ringing");
                } else if(termstatus == "Calling") {
                        m_peerwidget->setColor(term, "yellow");
                        display_sipstatus = PeerWidget::tr("Calling");
                } else if((termstatus == "On the phone") || (termstatus == "Up")) {
                        m_peerwidget->setColor(term, "red");
                        display_sipstatus = PeerWidget::tr("On the phone");
                } else if(termstatus == "Not online") {
                        m_peerwidget->setColor(term, "grey");
                        display_sipstatus = PeerWidget::tr("Not online");
                } else {
                        m_peerwidget->setColor(term, "grey");
                        display_sipstatus = termstatus;
                }
        }

        if(m_agentstatus.size() > 0) {
                QStringList qsl = m_agentstatus.split("/");
                // PeerItem::updateDisplayedStatus() ("agentlogout", "xivo", "6103", "103")
                // PeerItem::updateDisplayedStatus() ("queuememberstatus", "6103", "qcb_00000", "5", "0")
                if (qsl.size() >= 4) {
                        if(qsl[0] == "agentlogin") {
                                QString astid = qsl[1];
                                QString agentnum = qsl[2];
                                m_peerwidget->setAgentToolTip(agentnum, m_queuelist);
                                m_peerwidget->setColor("agent", "green");
                        } else if(qsl[0] == "agentlogout") {
                                QString astid = qsl[1];
                                QString agentnum = qsl[2];
                                m_peerwidget->setAgentToolTip("", m_queuelist);
                                m_peerwidget->setColor("agent", "grey");
                        } else if(qsl[0] == "joinqueue") {
                                QString astid = qsl[1];
                                QString agentnum = qsl[2];
                                if(! m_queuelist.contains(qsl[3]))
                                        m_queuelist.append(qsl[3]);
                                m_peerwidget->setAgentToolTip(agentnum, m_queuelist);
                        } else if(qsl[0] == "leavequeue") {
                                QString astid = qsl[1];
                                QString agentnum = qsl[2];
                                if(m_queuelist.contains(qsl[3]))
                                        m_queuelist.removeAll(qsl[3]);
                                m_peerwidget->setAgentToolTip(agentnum, m_queuelist);
                        } else if(qsl[0] == "queuememberstatus") {
                                QString astid = qsl[1];
                                QString agentnum = qsl[2];
                                if(qsl[4] == "1") {
                                        m_peerwidget->setColor("agent", "green");
                                } else if(qsl[4] == "3") {
                                        m_peerwidget->setColor("agent", "blue");
                                } else if(qsl[4] == "5") {
                                        m_peerwidget->setColor("agent", "grey");
                                } else {
                                        m_peerwidget->setColor("agent", "yellow");
                                }
                                m_peerwidget->setAgentToolTip(agentnum, m_queuelist);
                        } else if(qsl[0] == "agentstatus") {
                                QString astid = qsl[1];
                                QString agentnum = qsl[2];
                                if(qsl[4].size() > 0)
                                        m_queuelist = qsl[4].split(",");
                                else
                                        m_queuelist = QStringList();
                                if (qsl[3] == "0") {
                                        m_peerwidget->setAgentToolTip("", m_queuelist);
                                        m_peerwidget->setColor("agent", "grey");
                                } else {
                                        m_peerwidget->setAgentToolTip(agentnum, m_queuelist);
                                        m_peerwidget->setColor("agent", "green");
                                }

                                // ("agentstatus", "xivo", "6102", "0", "qcb_00003,qcb_00000")
                        } else {
                                qDebug() << "UNKNOWN in updateDisplayedStatus()" << qsl;
                                // m_peerwidget->setAgentToolTip(agentnum, m_queuelist);
                                // m_peerwidget->setBlue("agent");
                        }
                } else {
                        qDebug() << "PeerItem::updateDisplayedStatus() / size < 4" << qsl;
                }
        }

	QString fortooltip = PeerWidget::tr("SIP Presence : ") + display_sipstatus + "\n"
		+ PeerWidget::tr("Availability : ") + display_imavail;
	/* + "\n"
	   + "Voicemail Status: " + vmstatus + "\n"
	   + "Queues Status: " + queuestatus;*/

	m_peerwidget->setToolTip(fortooltip);

        return;
}

/*! \brief update channel list
 */
void PeerItem::updateDisplayedChans()
{
	if(m_peerwidget == NULL)
                return;

        m_peerwidget->clearChanList();
        for(int i = 0; i < m_chanIds.size(); i++)
                m_peerwidget->addChannel(m_chanIds[i], m_chanStates[i], m_chanOthers[i]);
        return;
}

/*! \brief update name if changed
 */
void PeerItem::updateDisplayedName()
{
	if(m_peerwidget == NULL)
                return;

        m_peerwidget->setName(m_name);
        return;
}

const QString & PeerItem::name()
{
        return m_name;
}

const UserInfo * PeerItem::userinfo()
{
        return m_ui;
}

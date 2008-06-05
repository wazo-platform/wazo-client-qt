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

#include <QCheckBox>
#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include "servicepanel.h"

ServicePanel::ServicePanel(const QStringList & capafeatures,
                           QWidget * parent)
        : QWidget(parent), m_capas(capafeatures)
{
        // qDebug() << "ServicePanel::ServicePanel()" << m_capas;
        m_capas << "enablevm" << "incallrec" << "incallfilter" << "enablednd"
                << "fwdrna" << "fwdbusy" << "fwdunc";

	int line = 0;
        m_status = new ServiceStatus();

	QGroupBox * groupBox1 = new QGroupBox( tr("Services") );
	groupBox1->setAlignment( Qt::AlignLeft );
	QGridLayout * gridlayout1 = new QGridLayout(groupBox1);

        if(m_capas.contains("enablevm")) {
                m_voicemail = new QCheckBox(tr("Voice &Mail"), this);
                m_voicemail->setObjectName("service");
                gridlayout1->addWidget(m_voicemail, line++, 0, 1, 0);
        }
        if(m_capas.contains("incallrec")) {
                m_callrecording = new QCheckBox(tr("Call &Recording"), this);
                m_callrecording->setObjectName("service");
                gridlayout1->addWidget(m_callrecording, line++, 0, 1, 0);
        }
        if(m_capas.contains("incallfilter")) {
                m_callfiltering = new QCheckBox(tr("Call &Filtering"), this);
                m_callfiltering->setObjectName("service");
                gridlayout1->addWidget(m_callfiltering, line++, 0, 1, 0);
        }
        if(m_capas.contains("enablednd")) {
                m_dnd = new QCheckBox(tr("Do Not &Disturb"), this);
                m_dnd->setObjectName("service");
                gridlayout1->addWidget(m_dnd, line++, 0, 1, 0);
        }

	QGroupBox * groupBox2 = new QGroupBox( tr("Call Forwards") );
	groupBox2->setAlignment( Qt::AlignLeft );
	QGridLayout * gridlayout2 = new QGridLayout(groupBox2);

        if(m_capas.contains("fwdrna")) {
                m_forwardonunavailable = new QCheckBox(tr("Forward on &No Reply"), this);
                m_forwardonunavailable->setObjectName("service");
                gridlayout2->addWidget(m_forwardonunavailable, line++, 0, 1, 0);
                QLabel * lblonunavailable = new QLabel(tr("Destination"), this);
                gridlayout2->addWidget(lblonunavailable, line, 0);
                m_forwardonunavailabledest = new QLineEdit(this);
                m_forwardonunavailable->setEnabled(false);
                gridlayout2->addWidget(m_forwardonunavailabledest, line++, 1);
                lblonunavailable->setObjectName("service");
        }

        if(m_capas.contains("fwdbusy")) {
                m_forwardonbusy = new QCheckBox(tr("Forward on &Busy"), this);
                m_forwardonbusy->setObjectName("service");
                gridlayout2->addWidget(m_forwardonbusy, line++, 0, 1, 0);
                QLabel * lblonbusy = new QLabel(tr("Destination"), this);
                gridlayout2->addWidget(lblonbusy, line, 0);
                m_forwardonbusydest = new QLineEdit(this);
                m_forwardonbusy->setEnabled(false);
                gridlayout2->addWidget(m_forwardonbusydest, line++, 1);
                lblonbusy->setObjectName("service");
        }

        if(m_capas.contains("fwdunc")) {
                m_uncondforward = new QCheckBox(tr("&Unconditional Forward"), this);
                m_uncondforward->setObjectName("service");
                gridlayout2->addWidget(m_uncondforward, line++, 0, 1, 0);
                QLabel * lbluncond = new QLabel(tr("Destination"), this);
                gridlayout2->addWidget(lbluncond, line, 0);
                m_uncondforwarddest = new QLineEdit(this);
                m_uncondforward->setEnabled(false);
                gridlayout2->addWidget(m_uncondforwarddest, line++, 1);
                lbluncond->setObjectName("service");
        }

        QVBoxLayout * vlayout = new QVBoxLayout(this);
        if(m_capas.contains("enablevm") || m_capas.contains("incallrec") || m_capas.contains("incallfilter") || m_capas.contains("enablednd"))
                vlayout->addWidget(groupBox1);
        if(m_capas.contains("fwdrna") || m_capas.contains("fwdbusy") || m_capas.contains("fwdunc"))
                vlayout->addWidget(groupBox2);
        vlayout->addStretch(1);

        Reset();
        if(m_capas.contains("fwdunc"))
                connect(m_uncondforwarddest, SIGNAL(textEdited(const QString &)),
                        this, SLOT(toggleUncondIfAllowed(const QString &)));
        if(m_capas.contains("fwdbusy"))
                connect(m_forwardonbusydest, SIGNAL(textEdited(const QString &)),
                        this, SLOT(toggleOnBusyIfAllowed(const QString &)));
        if(m_capas.contains("fwdrna"))
                connect(m_forwardonunavailabledest, SIGNAL(textEdited(const QString &)),
                        this, SLOT(toggleOnUnavailIfAllowed(const QString &)));
        Connect();
}

void ServicePanel::Connect()
{
        //qDebug() << "ServicePanel::Connect()";
        if(m_capas.contains("enablevm"))
                connect(m_voicemail, SIGNAL(clicked(bool)),
                        this, SIGNAL(voiceMailToggled(bool)));
        if(m_capas.contains("incallrec"))
                connect(m_callrecording, SIGNAL(clicked(bool)),
                        this, SIGNAL(callRecordingToggled(bool)));
        if(m_capas.contains("incallfilter"))
                connect(m_callfiltering, SIGNAL(clicked(bool)),
                        this, SIGNAL(callFilteringToggled(bool)));
        if(m_capas.contains("enablednd"))
                connect(m_dnd, SIGNAL(clicked(bool)),
                        this, SIGNAL(dndToggled(bool)));
        if(m_capas.contains("fwdunc"))
                connect(m_uncondforward, SIGNAL(clicked(bool)),
                        this, SLOT(uncondForwardToggled(bool)));
        if(m_capas.contains("fwdbusy"))
                connect(m_forwardonbusy, SIGNAL(clicked(bool)),
                        this, SLOT(forwardOnBusyToggled(bool)));
        if(m_capas.contains("fwdrna"))
                connect(m_forwardonunavailable, SIGNAL(clicked(bool)),
                        this, SLOT(forwardOnUnavailableToggled(bool)));
}

void ServicePanel::DisConnect()
{
        //qDebug() << "ServicePanel::DisConnect()";
        if(m_capas.contains("enablevm"))
                disconnect(m_voicemail, SIGNAL(clicked(bool)),
                           this, SIGNAL(voiceMailToggled(bool)));
        if(m_capas.contains("incallrec"))
                disconnect(m_callrecording, SIGNAL(clicked(bool)),
                           this, SIGNAL(callRecordingToggled(bool)));
        if(m_capas.contains("incallfilter"))
                disconnect(m_callfiltering, SIGNAL(clicked(bool)),
                           this, SIGNAL(callFilteringToggled(bool)));
        if(m_capas.contains("enablednd"))
                disconnect(m_dnd, SIGNAL(clicked(bool)),
                           this, SIGNAL(dndToggled(bool)));
        if(m_capas.contains("fwdunc"))
                disconnect(m_uncondforward, SIGNAL(clicked(bool)),
                           this, SLOT(uncondForwardToggled(bool)));
        if(m_capas.contains("fwdbusy"))
                disconnect(m_forwardonbusy, SIGNAL(clicked(bool)),
                           this, SLOT(forwardOnBusyToggled(bool)));
        if(m_capas.contains("fwdrna"))
                disconnect(m_forwardonunavailable, SIGNAL(clicked(bool)),
                           this, SLOT(forwardOnUnavailableToggled(bool)));

// 	disconnect(m_uncondforwarddest, SIGNAL(textEdited(const QString &)),
//                    this, SLOT(toggleUncondIfAllowed(const QString &)));
// 	disconnect(m_forwardonbusydest, SIGNAL(textEdited(const QString &)),
//                    this, SLOT(toggleOnBusyIfAllowed(const QString &)));
// 	disconnect(m_forwardonunavailabledest, SIGNAL(textEdited(const QString &)),
//                    this, SLOT(toggleOnUnavailIfAllowed(const QString &)));
}

void ServicePanel::Reset()
{
        //qDebug() << "ServicePanel::Reset()";
        if(m_capas.contains("enablevm"))
                m_voicemail->setChecked(false);
        if(m_capas.contains("incallrec"))
                m_callrecording->setChecked(false);
        if(m_capas.contains("incallfilter"))
                m_callfiltering->setChecked(false);
        if(m_capas.contains("enablednd"))
                m_dnd->setChecked(false);

        if(m_capas.contains("fwdunc")) {
                m_uncondforward->setChecked(false);
                m_uncondforwarddest->setText("");
        }
        if(m_capas.contains("fwdbusy")) {
                m_forwardonbusy->setChecked(false);
                m_forwardonbusydest->setText("");
        }
        if(m_capas.contains("fwdrna")) {
                m_forwardonunavailable->setChecked(false);
                m_forwardonunavailabledest->setText("");
        }
}

void ServicePanel::toggleUncondIfAllowed(const QString & text)
{
        bool allowed     = (text.size() > 0);
        bool was_checked = (m_uncondforward->checkState() == Qt::Checked);
        m_uncondforward->setEnabled(allowed);
        if(allowed == false) {
                m_uncondforward->setChecked(false);
                if(was_checked)
                        uncondForwardChanged(false,
                                             m_uncondforwarddest->text());
        } else if(was_checked)
                uncondForwardChanged(true,
                                     m_uncondforwarddest->text());
}

void ServicePanel::toggleOnBusyIfAllowed(const QString & text)
{
        bool allowed     = (text.size() > 0);
        bool was_checked = (m_forwardonbusy->checkState() == Qt::Checked);
        m_forwardonbusy->setEnabled(allowed);
        if(allowed == false) {
                m_forwardonbusy->setChecked(false);
                if(was_checked)
                        forwardOnBusyChanged(false,
                                             m_forwardonbusydest->text());
        } else if(was_checked)
                forwardOnBusyChanged(true,
                                     m_forwardonbusydest->text());
}

void ServicePanel::toggleOnUnavailIfAllowed(const QString & text)
{
        bool allowed     = (text.size() > 0);
        bool was_checked = (m_forwardonunavailable->checkState() == Qt::Checked);
        m_forwardonunavailable->setEnabled(allowed);
        if(allowed == false) {
                m_forwardonunavailable->setChecked(false);
                if(was_checked)
                        forwardOnUnavailableChanged(false,
                                                    m_forwardonunavailabledest->text());
        } else if(was_checked)
                forwardOnUnavailableChanged(true,
                                            m_forwardonunavailabledest->text());
}

void ServicePanel::uncondForwardToggled(bool b)
{
	uncondForwardChanged(b, m_uncondforwarddest->text());
}

void ServicePanel::forwardOnBusyToggled(bool b)
{
	forwardOnBusyChanged(b, m_forwardonbusydest->text());
}

void ServicePanel::forwardOnUnavailableToggled(bool b)
{
	forwardOnUnavailableChanged(b, m_forwardonunavailabledest->text());
}


// The following actions are entered in when the status is received from the server (init or update)

void ServicePanel::setVoiceMail(bool b)
{
        m_status->setVoiceMail(b);
        if(m_capas.contains("enablevm"))
                m_voicemail->setChecked(b);
}

void ServicePanel::setCallRecording(bool b)
{
        m_status->setCallRecording(b);
        if(m_capas.contains("incallrec"))
                m_callrecording->setChecked(b);
}

void ServicePanel::setCallFiltering(bool b)
{
        m_status->setCallFiltering(b);
        if(m_capas.contains("incallfilter"))
                m_callfiltering->setChecked(b);
}

void ServicePanel::setDnd(bool b)
{
        if(m_capas.contains("enablednd")) {
                m_status->setDnd(b);
                m_dnd->setChecked(b);
        }
}


void ServicePanel::setUncondForward(bool b, const QString & dest)
{
        if(m_capas.contains("fwdunc")) {
                m_status->setUncondForward(b, dest);
                m_uncondforwarddest->setText(dest);
                m_uncondforward->setChecked(b);
                m_uncondforward->setEnabled(dest.size() > 0);
        }
}

void ServicePanel::setUncondForward(bool b)
{
        if(m_capas.contains("fwdunc")) {
                m_status->setUncondForward(b);
                m_uncondforward->setChecked(b);
        }
}

void ServicePanel::setUncondForward(const QString & dest)
{
        if(m_capas.contains("fwdunc")) {
                m_status->setUncondForward(dest);
                m_uncondforwarddest->setText(dest);
                m_uncondforward->setEnabled(dest.size() > 0);
        }
}


void ServicePanel::setForwardOnBusy(bool b, const QString & dest)
{
        if(m_capas.contains("fwdbusy")) {
                m_status->setForwardOnBusy(b, dest);
                m_forwardonbusydest->setText(dest);
                m_forwardonbusy->setChecked(b);
                m_forwardonbusy->setEnabled(dest.size() > 0);
        }
}

void ServicePanel::setForwardOnBusy(bool b)
{
        if(m_capas.contains("fwdbusy")) {
                m_status->setForwardOnBusy(b);
                m_forwardonbusy->setChecked(b);
        }
}

void ServicePanel::setForwardOnBusy(const QString & dest)
{
        if(m_capas.contains("fwdbusy")) {
                m_status->setForwardOnBusy(dest);
                m_forwardonbusydest->setText(dest);
                m_forwardonbusy->setEnabled(dest.size() > 0);
        }
}

void ServicePanel::setForwardOnUnavailable(bool b, const QString & dest)
{
        if(m_capas.contains("fwdrna")) {
                m_status->setForwardOnUnavailable(b, dest);
                m_forwardonunavailabledest->setText(dest);
                m_forwardonunavailable->setChecked(b);
                m_forwardonunavailable->setEnabled(dest.size() > 0);
        }
}

void ServicePanel::setForwardOnUnavailable(bool b)
{
        if(m_capas.contains("fwdrna")) {
                m_status->setForwardOnUnavailable(b);
                m_forwardonunavailable->setChecked(b);
        }
}

void ServicePanel::setForwardOnUnavailable(const QString & dest)
{
        if(m_capas.contains("fwdrna")) {
                m_status->setForwardOnUnavailable(dest);
                m_forwardonunavailabledest->setText(dest);
                m_forwardonunavailable->setEnabled(dest.size() > 0);
        }
}



/*! \brief change the monitored peer
 */
void ServicePanel::setPeerToDisplay(const QString & peer)
{
        // qDebug() << "ServicePanel::setPeerToDisplay()" << peer;
	m_peer = peer;
        askFeatures();
}

void ServicePanel::setRecordedStatus()
{
        // qDebug() << "ServicePanel::setRecordedStatus()";
        if(m_capas.contains("enablevm"))
                m_status->m_voicemail = m_voicemail->isChecked();
        if(m_capas.contains("incallrec"))
                m_status->m_callrecording = m_callrecording->isChecked();
        if(m_capas.contains("incallfilter"))
                m_status->m_callfiltering = m_callfiltering->isChecked();
        if(m_capas.contains("enablednd"))
                m_status->m_dnd = m_dnd->isChecked();

        if(m_capas.contains("fwdunc"))
                m_status->m_uncondforward = m_uncondforward->isChecked();
        if(m_capas.contains("fwdbusy"))
                m_status->m_forwardonbusy = m_forwardonbusy->isChecked();
        if(m_capas.contains("fwdrna"))
                m_status->m_forwardonunavailable = m_forwardonunavailable->isChecked();
}

void ServicePanel::getRecordedStatus()
{
        // qDebug() << "ServicePanel::getRecordedStatus()";
        if(m_capas.contains("enablevm"))
                m_voicemail->setChecked(m_status->m_voicemail);
        if(m_capas.contains("incallrec"))
                m_callrecording->setChecked(m_status->m_callrecording);
        if(m_capas.contains("incallfilter"))
                m_callfiltering->setChecked(m_status->m_callfiltering);
        if(m_capas.contains("enablednd"))
                m_dnd->setChecked(m_status->m_dnd);

        if(m_capas.contains("fwdunc")) {
                m_uncondforwarddest->setText(m_status->m_uncondforwarddest);
                m_uncondforward->setChecked(m_status->m_uncondforward);
        }
        if(m_capas.contains("fwdbusy")) {
                m_forwardonbusydest->setText(m_status->m_forwardonbusydest);
                m_forwardonbusy->setChecked(m_status->m_forwardonbusy);
        }
        if(m_capas.contains("fwdrna")) {
                m_forwardonunavailabledest->setText(m_status->m_forwardonunavailabledest);
                m_forwardonunavailable->setChecked(m_status->m_forwardonunavailable);
        }
}


////////////////////////////////////////////////:::

ServiceStatus::ServiceStatus()
{
        m_voicemail = false;
        m_callrecording = false;
        m_callfiltering = false;
        m_dnd = false;
        m_uncondforward = false;
        m_uncondforwarddest = "";
        m_forwardonbusy = false;
        m_forwardonbusydest = "";
        m_forwardonunavailable = false;
        m_forwardonunavailabledest = "";
}

void ServiceStatus::setVoiceMail(bool b)
{
	m_voicemail = b;
}

void ServiceStatus::setCallRecording(bool b)
{
	m_callrecording = b;
}

void ServiceStatus::setCallFiltering(bool b)
{
	m_callfiltering = b;
}

void ServiceStatus::setDnd(bool b)
{
	m_dnd = b;
}

void ServiceStatus::setUncondForward(bool b, const QString & dest)
{
        m_uncondforward = b;
        m_uncondforwarddest = dest;
}

void ServiceStatus::setUncondForward(bool b)
{
        m_uncondforward = b;
}

void ServiceStatus::setUncondForward(const QString & dest)
{
        m_uncondforwarddest = dest;
}

void ServiceStatus::setForwardOnBusy(bool b, const QString & dest)
{
        m_forwardonbusy = b;
        m_forwardonbusydest = dest;
}

void ServiceStatus::setForwardOnBusy(bool b)
{
        m_forwardonbusy = b;
}

void ServiceStatus::setForwardOnBusy(const QString & dest)
{
        m_forwardonbusydest = dest;
}

void ServiceStatus::setForwardOnUnavailable(bool b, const QString & dest)
{
        m_forwardonunavailable = b;
        m_forwardonunavailabledest = dest;
}

void ServiceStatus::setForwardOnUnavailable(bool b)
{
        m_forwardonunavailable = b;
}

void ServiceStatus::setForwardOnUnavailable(const QString & dest)
{
        m_forwardonunavailabledest = dest;
}

void ServiceStatus::display()
{
        qDebug() << m_voicemail << m_callrecording << m_callfiltering << m_dnd << "/"
                 << m_uncondforward << m_uncondforwarddest
                 << m_forwardonbusy << m_forwardonbusydest
                 << m_forwardonunavailable << m_forwardonunavailabledest;
}

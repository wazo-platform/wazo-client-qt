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

#include <QCheckBox>
#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVariant>
#include <QVBoxLayout>

#include "servicepanel.h"
#include "userinfo.h"

const QStringList fwdcapas = (QStringList() << "fwdrna" << "fwdbusy" << "fwdunc");
const QStringList chkcapas = (QStringList() << "enablevm" << "incallrec" << "incallfilter" << "enablednd");

ServicePanel::ServicePanel(const QVariant & options,
                           QWidget * parent)
    : QWidget(parent)
{
    m_capalegend["enablevm"]     = tr("Voice &Mail");
    m_capalegend["incallrec"]    = tr("Call &Recording");
    m_capalegend["incallfilter"] = tr("Call &Filtering");
    m_capalegend["enablednd"]    = tr("Do Not &Disturb");
    m_capalegend["fwdrna"]  = tr("Forward on &No Reply");
    m_capalegend["fwdbusy"] = tr("Forward on &Busy");
    m_capalegend["fwdunc"]  = tr("&Unconditional Forward");
        
    m_capas = options.toMap()["services"].toStringList();
        
    int line = 0;
    m_status = new ServiceStatus();
        
    QGroupBox * groupBox1 = new QGroupBox( tr("Services") );
    groupBox1->setAlignment( Qt::AlignLeft );
    QGridLayout * gridlayout1 = new QGridLayout(groupBox1);
        
    foreach(QString capa, chkcapas)
        if(m_capas.contains(capa)) {
            m_chkopt[capa] = new QCheckBox(m_capalegend[capa], this);
            m_chkopt[capa]->setObjectName("service");
            m_chkopt[capa]->setProperty("capa", capa);
            gridlayout1->addWidget(m_chkopt[capa], line++, 0, 1, 0);
        }
        
    QGroupBox * groupBox2 = new QGroupBox( tr("Call Forwards") );
    groupBox2->setAlignment( Qt::AlignLeft );
    QGridLayout * gridlayout2 = new QGridLayout(groupBox2);
    QHash<QString, QLabel *> label;
        
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa)) {
            m_forward[capa] = new QCheckBox(m_capalegend[capa], this);
            m_forward[capa]->setObjectName("service");
            m_forward[capa]->setProperty("capa", capa);
            gridlayout2->addWidget(m_forward[capa], line++, 0, 1, 0);
            label[capa] = new QLabel(tr("Destination"), this);
            gridlayout2->addWidget(label[capa], line, 0);
            m_forwarddest[capa] = new QLineEdit(this);
            m_forwarddest[capa]->setProperty("capa", capa);
            m_forward[capa]->setEnabled(false);
            gridlayout2->addWidget(m_forwarddest[capa], line++, 1);
            label[capa]->setObjectName("service");
        }
        
    QVBoxLayout * vlayout = new QVBoxLayout(this);
    if(m_capas.contains("enablevm") || m_capas.contains("incallrec") || m_capas.contains("incallfilter") || m_capas.contains("enablednd"))
        vlayout->addWidget(groupBox1);
    if(m_capas.contains("fwdrna") || m_capas.contains("fwdbusy") || m_capas.contains("fwdunc"))
        vlayout->addWidget(groupBox2);
    vlayout->addStretch(1);

    Reset();
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa)) {
            connect(m_forwarddest[capa], SIGNAL(textEdited(const QString &)),
                    this, SLOT(toggleIfAllowed(const QString &)));
        }
    Connect();
}

ServicePanel::~ServicePanel()
{
}

void ServicePanel::setGuiOptions(const QVariant &)
{
}

void ServicePanel::setUserInfo(const UserInfo *)
{
}

void ServicePanel::Connect()
{
    //qDebug() << "ServicePanel::Connect()";
    foreach(QString capa, chkcapas)
        if(m_capas.contains(capa))
            connect(m_chkopt[capa], SIGNAL(clicked(bool)),
                    this, SLOT(chkoptToggled(bool)));
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa))
            connect(m_forward[capa], SIGNAL(clicked(bool)),
                    this, SLOT(Toggled(bool)));
}

void ServicePanel::DisConnect()
{
    //qDebug() << "ServicePanel::DisConnect()";
    foreach(QString capa, chkcapas)
        if(m_capas.contains(capa))
            disconnect(m_chkopt[capa], SIGNAL(clicked(bool)),
                       this, SLOT(chkoptToggled(bool)));
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa))
            disconnect(m_forward[capa], SIGNAL(clicked(bool)),
                       this, SLOT(Toggled(bool)));
    // foreach(QString capa, fwdcapas)
    // if(m_capas.contains(capa))
    // disconnect(m_forward[capa], SIGNAL(textEdited(const QString &)),
    // this, SLOT(toggleIfAllowed(const QString &)));
}

void ServicePanel::Reset()
{
    //qDebug() << "ServicePanel::Reset()";
    foreach(QString capa, chkcapas)
        if(m_capas.contains(capa))
            m_chkopt[capa]->setChecked(false);
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa)) {
            m_forward[capa]->setChecked(false);
            m_forwarddest[capa]->setText("");
        }
}

void ServicePanel::toggleIfAllowed(const QString & text)
{
    QString capa = sender()->property("capa").toString();
    bool allowed     = (text.size() > 0);
    bool was_checked = (m_forward[capa]->checkState() == Qt::Checked);
    m_forward[capa]->setEnabled(allowed);
    if(allowed == false) {
        m_forward[capa]->setChecked(false);
        if(was_checked)
            forwardChanged(capa, false,
                           m_forwarddest[capa]->text());
    } else if(was_checked)
        forwardChanged(capa, true,
                       m_forwarddest[capa]->text());
}

void ServicePanel::chkoptToggled(bool b)
{
    QString capa = sender()->property("capa").toString();
    chkoptChanged(capa, b);
}

void ServicePanel::Toggled(bool b)
{
    QString capa = sender()->property("capa").toString();
    forwardChanged(capa, b, m_forwarddest[capa]->text());
}

// The following actions are entered in when the status is received from the server (init or update)

void ServicePanel::setOpt(const QString & capa, bool b)
{
    m_status->setOpt(capa, b);
    if(m_capas.contains(capa))
        m_chkopt[capa]->setChecked(b);
}

void ServicePanel::setForward(const QString & capa, const QVariant & value)
{
    bool b = value.toMap()["enabled"].toBool();
    QString thiscapa = "fwd" + capa;
    if(m_capas.contains(thiscapa)) {
        if(value.toMap().keys().contains("number")) {
            QString dest = value.toMap()["number"].toString();
            m_status->setForward(thiscapa, b, dest);
            m_forward[thiscapa]->setChecked(b);
            m_forwarddest[thiscapa]->setText(dest);
            m_forward[thiscapa]->setEnabled(dest.size() > 0);
        } else {
            // m_status->setForward(thiscapa, b, dest);
            m_forward[thiscapa]->setChecked(b);
        }
    }
}


/*! \brief change the monitored peer
 */
void ServicePanel::monitorPeer(UserInfo * /*ui*/)
{
    // qDebug() << "ServicePanel::monitorPeer()" << peer;
    askFeatures();
}

void ServicePanel::setRecordedStatus()
{
    // qDebug() << "ServicePanel::setRecordedStatus()";
    foreach(QString capa, chkcapas)
        if(m_capas.contains(capa))
            m_status->m_chkopt[capa] = m_chkopt[capa]->isChecked();
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa))
            m_status->m_forward[capa] = m_forward[capa]->isChecked();
}

void ServicePanel::getRecordedStatus()
{
    // qDebug() << "ServicePanel::getRecordedStatus()";
    foreach(QString capa, chkcapas)
        if(m_capas.contains(capa))
            m_chkopt[capa]->setChecked(m_status->m_chkopt[capa]);
    foreach(QString capa, fwdcapas)
        if(m_capas.contains(capa)) {
            m_forwarddest[capa]->setText(m_status->m_forwarddest[capa]);
            m_forward[capa]->setChecked(m_status->m_forward[capa]);
        }
}


ServiceStatus::ServiceStatus()
{
    foreach(QString capa, chkcapas)
        m_chkopt[capa] = false;
    foreach(QString capa, fwdcapas) {
        m_forward[capa] = false;
        m_forwarddest[capa] = "";
    }
}

void ServiceStatus::setOpt(const QString & capa, bool b)
{
    m_chkopt[capa] = b;
}

void ServiceStatus::setForward(const QString & capa, bool b, const QString & dest)
{
    m_forward[capa] = b;
    m_forwarddest[capa] = dest;
}

void ServiceStatus::display()
{
    qDebug() << m_chkopt << "/" << m_forward << m_forwarddest;
}

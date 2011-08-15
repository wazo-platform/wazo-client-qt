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

#include "baseengine.h"
#include "servicepanel.h"

const QStringList chkcapas = (QStringList() << "enablevoicemail" << "callrecord" << "incallfilter" << "enablednd");
const QStringList fwdcapas = (QStringList() << "fwdrna" << "fwdbusy" << "fwdunc");

Q_EXPORT_PLUGIN2(xletfeatureplugin, XLetFeaturePlugin);

XLet* XLetFeaturePlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/features_%1");
    return new ServicePanel(parent);
}



ServicePanel::ServicePanel(QWidget * parent)
    : XLet(parent)
{
    setTitle(tr("Services"));
    m_capalegend["enablevoicemail"] = tr("Voice &Mail");
    m_capalegend["callrecord"]      = tr("Call &Recording");
    // m_capalegend["incallrec"]    = tr("");
    m_capalegend["incallfilter"]    = tr("Call &Filtering");
    m_capalegend["enablednd"]       = tr("Do Not &Disturb");
    m_capalegend["fwdrna"]          = tr("Forward on &No Reply");
    m_capalegend["fwdbusy"]         = tr("Forward on &Busy");
    m_capalegend["fwdunc"]          = tr("&Unconditional Forward");

    m_capas = b_engine->getConfig("guioptions.services").toStringList();

    int line = 0;

    QGroupBox *groupBox1 = new QGroupBox(tr("Services"), this);
    groupBox1->setAlignment( Qt::AlignLeft );
    groupBox1->hide();
    QGridLayout *gridlayout1 = new QGridLayout(groupBox1);

    foreach (QString capa, chkcapas) {
        if (m_capas.contains(capa)) {
            m_chkopt[capa] = new QCheckBox(m_capalegend[capa], this);
            m_chkopt[capa]->setObjectName("service");
            m_chkopt[capa]->setProperty("capa", capa);
            gridlayout1->addWidget(m_chkopt[capa], line++, 0, 1, 0);
        }
    }

    QGroupBox *groupBox2 = new QGroupBox(tr("Call Forwards"), this);
    groupBox2->setAlignment(Qt::AlignLeft);
    groupBox2->hide();
    QGridLayout *gridlayout2 = new QGridLayout(groupBox2);
    QHash<QString, QLabel *> label;

    foreach (QString capa, fwdcapas) {
        if (m_capas.contains(capa)) {
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
    }

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    if (m_capas.contains("enablevoicemail") ||
        m_capas.contains("callrecord") ||
        m_capas.contains("incallfilter") ||
        m_capas.contains("enablednd")) {
        groupBox1->show();
        vlayout->addWidget(groupBox1);
    }
    if (m_capas.contains("fwdrna") ||
        m_capas.contains("fwdbusy") ||
        m_capas.contains("fwdunc")) {
        groupBox2->show();
        vlayout->addWidget(groupBox2);
    }
    vlayout->addStretch(1);

    Reset();
    foreach (QString capa, fwdcapas)
        if (m_capas.contains(capa)) {
            connect(m_forwarddest[capa], SIGNAL(textEdited(const QString &)),
                    this, SLOT(toggleIfAllowed(const QString &)));
        }
    Connect();

    // connect signals/slots

    connect(b_engine, SIGNAL(disconnectFeatures()),
            this, SLOT(DisConnect()));
    connect(b_engine, SIGNAL(connectFeatures()),
            this, SLOT(Connect()));
    connect(b_engine, SIGNAL(resetFeatures()),
            this, SLOT(Reset()));

    connect(b_engine, SIGNAL(optChanged(const QString &)),
            this, SLOT(setOpt(const QString &)));
    connect(b_engine, SIGNAL(forwardUpdated(const QString &)),
            this, SLOT(setForward(const QString &)) );

    connect(b_engine, SIGNAL(updateUserConfig(const QString &)),
            this, SLOT(updateUserConfig(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
            this, SLOT(updatePhoneConfig(const QString &)));

    b_engine->askFeatures();
}

void ServicePanel::updateUserConfig(const QString & xuserid)
{
    if (xuserid == m_xuserid) {
        foreach (QString capa, chkcapas)
            if (m_capas.contains(capa))
                setOpt(capa);
        foreach (QString capa, fwdcapas)
            if (m_capas.contains(capa)) {
                setForward("enable" + capa.mid(3));
                setForward("dest" + capa.mid(3));
            }
    }
}

void ServicePanel::updatePhoneConfig(const QString &)
{
}

void ServicePanel::Connect()
{
    foreach (QString capa, chkcapas) {
        if (m_capas.contains(capa)) {
            connect(m_chkopt[capa], SIGNAL(clicked(bool)),
                    this, SLOT(chkoptToggled(bool)));
        }
    }
    foreach (QString capa, fwdcapas) {
        if (m_capas.contains(capa)) {
            connect(m_forward[capa], SIGNAL(clicked(bool)),
                    this, SLOT(Toggled(bool)));
        }
    }
}

void ServicePanel::DisConnect()
{
    foreach (QString capa, chkcapas) {
        if (m_capas.contains(capa)) {
            disconnect(m_chkopt[capa], SIGNAL(clicked(bool)),
                       this, SLOT(chkoptToggled(bool)));
        }
    }
    foreach (QString capa, fwdcapas) {
        if (m_capas.contains(capa)) {
            disconnect(m_forward[capa], SIGNAL(clicked(bool)),
                       this, SLOT(Toggled(bool)));
        }
    }
    // foreach(QString capa, fwdcapas)
    // if(m_capas.contains(capa))
    // disconnect(m_forward[capa], SIGNAL(textEdited(const QString &)),
    // this, SLOT(toggleIfAllowed(const QString &)));
}

void ServicePanel::Reset()
{
    //qDebug() << Q_FUNC_INFO;
    foreach (QString capa, chkcapas) {
        if (m_capas.contains(capa))
            m_chkopt[capa]->setChecked(false);
    }
    foreach (QString capa, fwdcapas) {
        if (m_capas.contains(capa)) {
            m_forward[capa]->setChecked(false);
            m_forwarddest[capa]->setText("");
        }
    }
}

void ServicePanel::toggleIfAllowed(const QString & text)
{
    QString capa = sender()->property("capa").toString();
    bool allowed     = (text.size() > 0);
    bool was_checked = (m_forward[capa]->checkState() == Qt::Checked);
    m_forward[capa]->setEnabled(allowed);
    if (allowed == false) {
        m_forward[capa]->setChecked(false);
        if (was_checked) {
            b_engine->featurePutForward(capa, false, m_forwarddest[capa]->text());
        }
    } else if (was_checked) {
        b_engine->featurePutForward(capa, true, m_forwarddest[capa]->text());
    }
}

void ServicePanel::chkoptToggled(bool b)
{
    QString capa = sender()->property("capa").toString();
    b_engine->featurePutOpt(capa, b);
}

void ServicePanel::Toggled(bool b)
{
    QString capa = sender()->property("capa").toString();
    b_engine->featurePutForward(capa, b, m_forwarddest[capa]->text());
}

// The following actions are entered in when the status is received from the server (init or update)

void ServicePanel::setOpt(const QString & capa)
{
    if (m_ui == NULL)
        return;
    if (m_capas.contains(capa)) {
        if (capa == "enablednd")
            m_chkopt[capa]->setChecked(m_ui->enablednd());
        if (capa == "incallfilter")
            m_chkopt[capa]->setChecked(m_ui->incallfilter());
        if (capa == "enablevoicemail")
            m_chkopt[capa]->setChecked(m_ui->enablevoicemail());
        if (capa == "callrecord")
            m_chkopt[capa]->setChecked(m_ui->callrecord());
    }
}

void ServicePanel::setForward(const QString & capa)
{
    if (m_ui == NULL)
        return;
    if (capa.startsWith("enable")) {
        QString thiscapa = "fwd" + capa.mid(6);
        if (m_capas.contains(thiscapa)) {
            if (capa == "enablebusy") {
                m_forward[thiscapa]->setChecked(m_ui->enablebusy());
                m_forward[thiscapa]->setEnabled(m_ui->destbusy().size() > 0);
            } else if (capa == "enablerna") {
                m_forward[thiscapa]->setChecked(m_ui->enablerna());
                m_forward[thiscapa]->setEnabled(m_ui->destrna().size() > 0);
            } else if (capa == "enableunc") {
                m_forward[thiscapa]->setChecked(m_ui->enableunc());
                m_forward[thiscapa]->setEnabled(m_ui->destunc().size() > 0);
            }
        }
    } else if (capa.startsWith("dest")) {
        QString thiscapa = "fwd" + capa.mid(4);
        if (m_capas.contains(thiscapa)) {
            if (capa == "destbusy") {
                m_forwarddest[thiscapa]->setText(m_ui->destbusy());
                m_forwarddest[thiscapa]->setEnabled(m_ui->destbusy().size() > 0);
            } else if (capa == "destrna") {
                m_forwarddest[thiscapa]->setText(m_ui->destrna());
                m_forwarddest[thiscapa]->setEnabled(m_ui->destrna().size() > 0);
            } else if (capa == "destunc") {
                m_forwarddest[thiscapa]->setText(m_ui->destunc());
                m_forwarddest[thiscapa]->setEnabled(m_ui->destunc().size() > 0);
            }
        }
    }
}

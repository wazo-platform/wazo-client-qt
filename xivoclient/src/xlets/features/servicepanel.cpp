/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

#include <baseengine.h>
#include <warningwidget.h>

#include "servicepanel.h"

const QStringList chkcapas = (QStringList() << "enablevoicemail" << "incallfilter" << "enablednd");
const QStringList fwdcapas = (QStringList() << "fwdrna" << "fwdbusy" << "fwdunc");

Q_EXPORT_PLUGIN2(xletfeatureplugin, XLetFeaturePlugin);

XLet* XLetFeaturePlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/features_%1");
    return new ServicePanel(parent);
}

ServicePanel::ServicePanel(QWidget * parent)
    : XLet(parent)
{
    setTitle(tr("Services"));
    m_capalegend["enablevoicemail"] = tr("Voice &Mail");
    m_capalegend["incallfilter"]    = tr("Call &Filtering");
    m_capalegend["enablednd"]       = tr("Do Not &Disturb");
    m_capalegend["fwdrna"]          = tr("Forward on &No Answer");
    m_capalegend["fwdbusy"]         = tr("Forward on &Busy");
    m_capalegend["fwdunc"]          = tr("&Unconditional Forward");

    m_capas = b_engine->getConfig("services").toStringList();

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
            m_forward[capa] = new WarningWidget<QCheckBox>(
                 new QCheckBox(m_capalegend[capa], this),
                 tr("This change has not been submitted yet. Please fill the "
                    "destination."), false);
            m_forward[capa]->widget()->setObjectName("service");
            m_forward[capa]->widget()->setProperty("capa", capa);
            gridlayout2->addWidget(m_forward[capa], line++, 0, 1, 0);

            label[capa] = new QLabel(tr("Destination"), this);
            gridlayout2->addWidget(label[capa], line, 0);
            m_forwarddest[capa] = new QLineEdit(this);
            m_forwarddest[capa]->setProperty("capa", capa);
            gridlayout2->addWidget(m_forwarddest[capa], line++, 1);
            label[capa]->setObjectName("service");
        }
    }

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    if (m_capas.contains("enablevoicemail") ||
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
            connect(m_forward[capa]->widget(), SIGNAL(clicked(bool)),
                    this, SLOT(Toggled(bool)));
            connect(m_forwarddest[capa], SIGNAL(lostFocus()),
                    this, SLOT(forwardLostFocus()));
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
            disconnect(m_forward[capa]->widget(), SIGNAL(clicked(bool)),
                       this, SLOT(Toggled(bool)));
        }
    }
    // foreach(QString capa, fwdcapas)
    // if(m_capas.contains(capa))
    // disconnect(m_forward[capa]->widget(), SIGNAL(textEdited(const QString &)),
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
            m_forward[capa]->widget()->setChecked(false);
            m_forwarddest[capa]->setText("");
        }
    }
}

void ServicePanel::chkoptToggled(bool b)
{
    QString capa = sender()->property("capa").toString();
    b_engine->featurePutOpt(capa, b);
}

/**
 * Called on call forward checkbox toggle
 */
void ServicePanel::Toggled(bool b)
{
    QString capa  = sender()->property("capa").toString();
    QString fdest = m_forwarddest[capa]->text();

    if(! (b && fdest.isEmpty())) {
        b_engine->featurePutForward(capa, b, m_forwarddest[capa]->text());
        m_forward[capa]->hideWarning();
    } else {
        m_forward[capa]->showWarning();
    }

    m_forwarddest[capa]->setFocus();
}

void ServicePanel::forwardLostFocus()
{
    QString capa = sender()->property("capa").toString();
    //qDebug() << Q_FUNC_INFO << capa;

    if (m_forward[capa]->widget()->isChecked() && m_forwarddest[capa]->text().isEmpty()) {
        m_forward[capa]->showWarning();
    } else {
        m_forward[capa]->hideWarning();
        b_engine->featurePutForward(capa, m_forward[capa]->widget()->isChecked(), m_forwarddest[capa]->text());
    }
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
                m_forward[thiscapa]->widget()->setChecked(m_ui->enablebusy());
            } else if (capa == "enablerna") {
                m_forward[thiscapa]->widget()->setChecked(m_ui->enablerna());
            } else if (capa == "enableunc") {
                m_forward[thiscapa]->widget()->setChecked(m_ui->enableunc());
            }
        }
    } else if (capa.startsWith("dest")) {
        QString thiscapa = "fwd" + capa.mid(4);
        if (m_capas.contains(thiscapa)) {
            if (capa == "destbusy") {
                m_forwarddest[thiscapa]->setText(m_ui->destbusy());
            } else if (capa == "destrna") {
                m_forwarddest[thiscapa]->setText(m_ui->destrna());
            } else if (capa == "destunc") {
                m_forwarddest[thiscapa]->setText(m_ui->destunc());
            }
        }
    }
}

void ServicePanel::toggleIfAllowed(const QString &) {
}

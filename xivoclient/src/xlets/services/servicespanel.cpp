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
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QAbstractButton>

#include <baseengine.h>

#include "servicespanel.h"

const QStringList chkcapas = (QStringList() << "enablevoicemail" << "incallfilter" << "enablednd");
const QStringList fwdcapas = (QStringList() << "fwdunc" << "fwdrna" << "fwdbusy");

Q_EXPORT_PLUGIN2(xletservicesplugin, XLetServicesPlugin);

XLet* XLetServicesPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/services_%1");
    return new ServicesPanel(parent);
}

ServicesPanel::ServicesPanel(QWidget * parent)
    : XLet(parent)
{
    setTitle(tr("Services"));
    m_capalegend["enablevoicemail"] = tr("Voice &Mail");
    m_capalegend["incallfilter"]    = tr("Call &Filtering");
    m_capalegend["enablednd"]       = tr("Do Not &Disturb");
    m_capalegend["fwdrna"]          = tr("Forward on &No Answer to");
    m_capalegend["fwdbusy"]         = tr("Forward on &Busy to");
    m_capalegend["fwdunc"]          = tr("&Unconditional Forward to");

    m_capas = b_engine->getConfig("services").toStringList();

    int line = 0;

    QGroupBox *groupBox1 = new QGroupBox(tr("Services"), this);
    groupBox1->setAlignment( Qt::AlignLeft );
    groupBox1->hide();
    QGridLayout *gridlayout1 = new QGridLayout(groupBox1);

    foreach (QString capa, chkcapas) {
        if (m_capas.contains(capa)) {
            m_chkopt[capa] = new WaitingWidget<QCheckBox>(
                new QCheckBox(m_capalegend[capa], this));
            m_chkopt[capa]->widget()->setObjectName("service");
            m_chkopt[capa]->widget()->setProperty("capa", capa);
            gridlayout1->addWidget(m_chkopt[capa], line++, 0, 1, 0);
        }
    }

    QGroupBox *groupBox2 = new QGroupBox(tr("Call Forwards"), this);
    groupBox2->setAlignment(Qt::AlignLeft);
    groupBox2->hide();
    QGridLayout *gridlayout2 = new QGridLayout(groupBox2);

    QHBoxLayout * helplayout = new QHBoxLayout();
    QLabel * helpicon = new QLabel(this);
    helpicon->setPixmap(QPixmap(":/images/services/help.png").scaledToHeight(18, Qt::SmoothTransformation));
    helplayout->addWidget(helpicon);
    QLabel * helplabel = new QLabel(
        tr("Please enter a destination to activate the checkboxes"));
    helplayout->addWidget(helplabel,Qt::AlignLeft);
    gridlayout2->addLayout(helplayout, line++, 0, 1, -1);

    gridlayout2->addItem(new QSpacerItem(0, 18, QSizePolicy::Fixed), line++, 0);

    m_fwdmode = new QButtonGroup(this);
    m_fwdmode->setExclusive(true);

    m_nofwd = new QRadioButton(tr("No call forward"), this);
    m_fwdmode->addButton(m_nofwd);
    gridlayout2->addWidget(m_nofwd, line++, 0, 1, 2);

    foreach (QString capa, fwdcapas) {
        if (m_capas.contains(capa)) {
            if (capa == "fwdunc") {
                m_uncfwd = new QRadioButton(m_capalegend["fwdunc"], this);
                m_fwdmode->addButton(m_uncfwd);
                /* m_forward will have pointers (through WaitingWidget) to one
                 * QRadioButton and some QCheckBoxes.
                 */
                m_forward[capa] = new WaitingWidget<QAbstractButton>(m_uncfwd, false);
                gridlayout2->addWidget(m_uncfwd, line, 0, 1, 2);
            } else {
                // If the "simple forwarding calls" is not there yet
                if (! m_fwdmode->buttons().contains(m_otherfwd)) {
                    m_otherfwd = new QRadioButton(tr("Simple call forwards"), this);
                    m_fwdmode->addButton(m_otherfwd);
                    gridlayout2->addWidget(m_otherfwd, line++, 0, 1, 2);
                }
                m_forward[capa] = new WaitingWidget<QAbstractButton>(
                    new QCheckBox(m_capalegend[capa], this), false);
                gridlayout2->addItem(new QSpacerItem(25, 0, QSizePolicy::Fixed), line, 0);
                gridlayout2->addWidget(m_forward[capa], line, 1);
            }
            m_forward[capa]->widget()->setObjectName("service");
            m_forward[capa]->widget()->setProperty("capa", capa);

            m_forwarddest[capa] = new WaitingWidget<QLineEdit>(
                 new QLineEdit(this));
            m_forwarddest[capa]->widget()->setProperty("capa", capa);
            m_forwarddest[capa]->widget()->setToolTip(
                 tr("Please enter a destination to activate the checkboxes"));
            gridlayout2->addWidget(m_forwarddest[capa], line++, 2);
        }
    }

    m_nofwd->setChecked(true);
    forwardModeChanged();

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    groupBox1->show();
    vlayout->addWidget(groupBox1);

    groupBox2->show();
    vlayout->addWidget(groupBox2);

    vlayout->addStretch(1);

    // connect signals/slots

    foreach (QString capa, chkcapas) {
        if (m_capas.contains(capa)) {
            connect(m_chkopt[capa]->widget(), SIGNAL(clicked(bool)),
                    this, SLOT(chkoptToggled(bool)));
        }
    }
    foreach (QString capa, fwdcapas) {
        if (m_capas.contains(capa)) {
            connect(m_forward[capa]->widget(), SIGNAL(toggled(bool)),
                    this, SLOT(forwardToggled(bool)));
            connect(m_forwarddest[capa]->widget(), SIGNAL(lostFocus()),
                    this, SLOT(forwardLostFocus()));
        }
    }

    connect(b_engine, SIGNAL(servicePutIsOK(const QString &, const QString &)),
            this, SLOT(servicePutIsOK(const QString &, const QString &)));

    connect(b_engine, SIGNAL(updateUserConfig(const QString &, const QVariantMap &)),
            this, SLOT(updateUserConfig(const QString &, const QVariantMap &)));
    connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
            this, SLOT(updatePhoneConfig(const QString &)));
    connect(m_fwdmode, SIGNAL(buttonReleased(QAbstractButton *)),
            this, SLOT(forwardModeChanged()));
}

void ServicesPanel::updateUserConfig(const QString & xuserid, const QVariantMap & datamap)
{
    if (xuserid == m_xuserid) {
        QVariantMap deltaConfig = datamap["config"].toMap();
        /* We already are fetching infos from the server (though indirectly).
         * We don't need to send changes.
         */
        foreach (QString capa, fwdcapas) {
            if (m_capas.contains(capa)) {
                 disconnect(m_forward[capa]->widget(), SIGNAL(toggled(bool)),
                            this, SLOT(forwardToggled(bool)));
            }
        }
        // Check options
        foreach (QVariant config_var, deltaConfig.keys()) {
            QString config_str = config_var.toString();
            if (chkcapas.contains(config_str)) {
                syncOpt(config_str);
            }
        }
        // Call forwards
        foreach (QString capa, fwdcapas) {
            QString enablecapa = "enable" + capa.mid(3);
            QString destcapa   = "dest"   + capa.mid(3);
            if (m_capas.contains(capa)
                && (deltaConfig.keys().contains(enablecapa)
                    || deltaConfig.keys().contains(destcapa))) {

                CallForwardStruct callforward = localCallForward(capa);
                m_forward[capa]->widget()->setChecked(callforward.enabled);
                m_forwarddest[capa]->widget()->setText(callforward.destination);

                m_forward[capa]->unlock();
                m_forwarddest[capa]->unlock();

                updateCheckboxEnabled(capa);
                updateTextboxEnabled(capa);

                // Automatic change of mode
                if (capa == "fwdunc") {
                    if (callforward.enabled) {
                        m_uncfwd->setChecked(true);
                    } else if (m_fwdmode->checkedButton() == m_uncfwd) {
                        // Unconditional forward was disabled -> change fwdmode
                        if (m_ui->enablerna() || m_ui->enablebusy()) {
                            m_otherfwd->setChecked(true);
                        } else {
                            m_nofwd->setChecked(true);
                        }
                    }
                    forwardModeChanged();
                } else if (m_fwdmode->checkedButton() == m_nofwd
                           && callforward.enabled) {
                    m_otherfwd->setChecked(true);
                    forwardModeChanged();
                }
            }
        }

        // Reconnect signals
        foreach (QString capa, fwdcapas) {
            if (m_capas.contains(capa)) {
                 connect(m_forward[capa]->widget(), SIGNAL(toggled(bool)),
                         this, SLOT(forwardToggled(bool)));
            }
        }
    }
}

void ServicesPanel::updatePhoneConfig(const QString &)
{
}

/*!
 * Textbox for unconditional call forward is always active.
 * Other textboxes are only enabled when the "Simple call forward" is checked.
 * Exceptions happen when widgets are waiting server answer. They then are disabled.
 */
void ServicesPanel::updateTextboxEnabled(const QString & capa)
{
    if (m_forwarddest[capa]->locked()) { // Waiting server
        m_forwarddest[capa]->widget()->setEnabled(false);
    } else if (capa == "fwdunc") { // Unconditional call forward
        m_forwarddest[capa]->widget()->setEnabled(true);
    } else if (m_fwdmode->checkedButton() == m_otherfwd) {
        m_forwarddest[capa]->widget()->setEnabled(true);
    } else {
        m_forwarddest[capa]->widget()->setEnabled(false);
    }
}

/*!
 * Checkboxes are only enabled when the associated destination is not empty.
 * Checkboxes of simple call forwards are only enabled when the "Simple call
 * forward" is checked.
 * Exceptions happen when widgets are waiting server answer. They then are disabled.
 */
void ServicesPanel::updateCheckboxEnabled(const QString & capa)
{
    if (m_forwarddest[capa]->locked()) { // Waiting server
        m_forwarddest[capa]->widget()->setEnabled(false);
    } else if (localCallForward(capa).destination.isEmpty()) { // No destination
        m_forward[capa]->widget()->setChecked(false);
        m_forward[capa]->widget()->setEnabled(false);
    } else if (capa == "fwdunc") { // Unconditional call forward
        m_forward[capa]->widget()->setEnabled(true);
    } else if (m_fwdmode->checkedButton() == m_otherfwd) {
        m_forward[capa]->widget()->setEnabled(true);
    } else {
        if (m_fwdmode->checkedButton() == m_nofwd) {
            m_forward[capa]->widget()->setChecked(false);
        }
        m_forward[capa]->widget()->setEnabled(false);
    }
}

/*! \brief Returns UserInfo data in a more convenient form
 */
CallForwardStruct ServicesPanel::localCallForward(const QString &capa)
{
    CallForwardStruct ret;
    if (m_ui == NULL) {
        return ret;
    }
    if (capa == "fwdunc") {
        ret.enabled = m_ui->enableunc();
        ret.destination = m_ui->destunc();
    } else if (capa == "fwdrna") {
        ret.enabled = m_ui->enablerna();
        ret.destination = m_ui->destrna();
    } else if (capa == "fwdbusy") {
        ret.enabled = m_ui->enablebusy();
        ret.destination = m_ui->destbusy();
    }
    return ret;
}

void ServicesPanel::chkoptToggled(bool b)
{
    QString capa = sender()->property("capa").toString();
    m_chkopt[capa]->lock();
    b_engine->servicePutOpt(capa, b);
}

void ServicesPanel::forwardModeChanged()
{
    foreach (QString capa, fwdcapas) {
        if (m_capas.contains(capa)) {
            updateCheckboxEnabled(capa);
            updateTextboxEnabled(capa);
        }
    }
}

void ServicesPanel::forwardToggled(bool b)
{
    QString capa  = sender()->property("capa").toString();
    QString fdest = m_forwarddest[capa]->widget()->text();

    if(! (b && fdest.isEmpty())) {
        m_forward[capa]->lock();
        m_forwarddest[capa]->lock();
        m_replyids[capa] = b_engine->servicePutForward(capa, b, fdest);
    } else {
        qDebug() << "Forwarding calls to nothing ... This should not happen !";
    }
}

void ServicesPanel::forwardLostFocus()
{
    QString capa = sender()->property("capa").toString();
    QString fdest = m_forwarddest[capa]->widget()->text();
    //qDebug() << Q_FUNC_INFO << capa;

    /* Disconnect signal to avoid sending twice the same info :
     * - first time because the checkbox is unchecked
     * - second time here
     */
    disconnect(m_forward[capa]->widget(), SIGNAL(toggled(bool)),
               this, SLOT(forwardToggled(bool)));
    updateCheckboxEnabled(capa);
    // Reconnect the signal
    connect(m_forward[capa]->widget(), SIGNAL(toggled(bool)),
            this, SLOT(forwardToggled(bool)));

    m_forward[capa]->lock();
    m_forwarddest[capa]->lock();
    bool status = fdest.isEmpty() ? false : m_forward[capa]->widget()->isChecked();
    m_replyids[capa] = b_engine->servicePutForward(capa, status, fdest);
}

/*!
 * The main reason of this function is to catch reply of the CTI server in case
 * no changes have been sent, for example when entering and exiting focus of a
 * text field without changing it. It prevents the WaitingWidgets to wait for an
 * update that will never come, because there is no new data, then nothing to
 * update.
 */
void ServicesPanel::servicePutIsOK(const QString & replyid, const QString & warning_string)
{
    if (warning_string == "no changes") {
        foreach(QString capa, m_replyids.keys()) {
            if (m_replyids[capa] == replyid) {
                m_forward[capa]->unlock();
                m_forwarddest[capa]->unlock();
                updateCheckboxEnabled(capa);
                updateTextboxEnabled(capa);
            }
        }
    }
}

/*! \brief sync widgets with userinfo
 */
void ServicesPanel::syncOpt(const QString & capa)
{
    if (m_ui == NULL)
        return;

    if (m_capas.contains(capa)) {
        if (capa == "enablednd")
            m_chkopt[capa]->widget()->setChecked(m_ui->enablednd());
        if (capa == "incallfilter")
            m_chkopt[capa]->widget()->setChecked(m_ui->incallfilter());
        if (capa == "enablevoicemail")
            m_chkopt[capa]->widget()->setChecked(m_ui->enablevoicemail());
        m_chkopt[capa]->unlock();
    }
}

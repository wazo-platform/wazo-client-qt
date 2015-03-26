/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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
#include <QDebug>

#include <baseengine.h>

#include "servicespanel.h"

const QStringList chkcapas = (QStringList() << "enablevoicemail" << "incallfilter" << "enablednd");
const QStringList fwdcapas = (QStringList() << "fwdunc" << "fwdrna" << "fwdbusy");

XLet* XLetServicesPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/services_%1");
    return new ServicesPanel(parent);
}

ServicesPanel::ServicesPanel(QWidget * parent)
    : XLet(parent, tr("Services"), ":/images/tab-services.svg")
//      m_nofwd(NULL),
//      m_uncfwd(NULL),
//      m_otherfwd(NULL),
//      m_fwdmode(NULL)
{
    this->ui.setupUi(this);

    QFile qssFile(QString(":/default.qss"));
    if(qssFile.open(QIODevice::ReadOnly)) {
        this->setStyleSheet(qssFile.readAll());
    }

    //m_capalegend["enablevoicemail"] = tr("Voice &Mail");
    //m_capalegend["incallfilter"]    = tr("Call &Filtering");
    //m_capalegend["enablednd"]       = tr("Do Not &Disturb");
    //m_capalegend["fwdrna"]          = tr("Forward on &No Answer to");
    //m_capalegend["fwdbusy"]         = tr("Forward on &Busy to");
    //m_capalegend["fwdunc"]          = tr("&Unconditional Forward to");

//    m_capas = b_engine->getConfig("services").toStringList();

//    int line = 0;

   // QGroupBox *groupBox1 = new QGroupBox(tr("Services"), this);
   // groupBox1->setAlignment( Qt::AlignLeft );
   // groupBox1->hide();
   // QGridLayout *gridlayout1 = new QGridLayout(groupBox1);

   // foreach (QString capa, chkcapas) {
     //   if (m_capas.contains(capa)) {
     //       //m_chkopt[capa] = new WaitingWidget<QCheckBox>(
     //       //    new QCheckBox(m_capalegend[capa], this));
     //       m_chkopt[capa]->widget()->setObjectName("service");
     //       m_chkopt[capa]->widget()->setProperty("capa", capa);
     //       gridlayout1->addWidget(m_chkopt[capa], line++, 0, 1, 0);
     //   }
   // }

   // QGroupBox *groupBox2 = new QGroupBox(tr("Call Forwards"), this);
   // groupBox2->setAlignment(Qt::AlignLeft);
   // groupBox2->hide();
   // QGridLayout *gridlayout2 = new QGridLayout(groupBox2);

   // QHBoxLayout * helplayout = new QHBoxLayout();
   // QLabel * helpicon = new QLabel(this);
   // helpicon->setPixmap(QPixmap(":/images/services/help.png").scaledToHeight(18, Qt::SmoothTransformation));
   // helplayout->addWidget(helpicon);
   // QLabel * helplabel = new QLabel(
   //     tr("Please enter a destination to activate the checkboxes"));
   // helplayout->addWidget(helplabel,Qt::AlignLeft);
   // gridlayout2->addLayout(helplayout, line++, 0, 1, -1);

   // gridlayout2->addItem(new QSpacerItem(0, 18, QSizePolicy::Fixed), line++, 0);

   // m_fwdmode = new QButtonGroup(this);
   // m_fwdmode->setExclusive(true);

   // m_nofwd = new QRadioButton(tr("No call forward"), this);
   // m_fwdmode->addButton(m_nofwd);
   // gridlayout2->addWidget(m_nofwd, line++, 0, 1, 2);
    /*this->ui.call_filtering_checkbox->hide();
    this->ui.dnd_checkbox->hide();
    this->ui.fwdunc_radiobutton->hide();
    this->ui.fwdna_checkbock->hide();
    this->ui.fwdbusy_checkbox->hide();
*/
    //hide search button inside input
    this->ui.fwdna_button->hide();
    this->ui.fwdbusy_button->hide();
    this->ui.fwdunc_button->hide();

//    foreach (QString capa, fwdcapas) {
//        if (m_capas.contains(capa)) {
//            if (capa == "fwdunc") {
                //m_uncfwd = new QRadioButton(m_capalegend["fwdunc"], this);
                //m_fwdmode->addButton(m_uncfwd);
                //this->ui.fwdunc_radiobutton->show();
                /* m_forward will have pointers (through WaitingWidget) to one
                 * QRadioButton and some QCheckBoxes.
                 */
                //m_forward[capa] = new WaitingWidget<QAbstractButton>(m_uncfwd, false);
                //gridlayout2->addWidget(m_forward[capa], line, 0, 1, 2);
//            } else {
                // If the "simple forwarding calls" is not there yet
                //if (m_otherfwd == NULL) {
                //    m_otherfwd = new QRadioButton(tr("Simple call forwards"), this);
                //    m_fwdmode->addButton(m_otherfwd);
                //    gridlayout2->addWidget(m_otherfwd, line++, 0, 1, 2);
                //}
                //m_forward[capa] = new WaitingWidget<QAbstractButton>(
                //    new QCheckBox(m_capalegend[capa], this), false);
                //gridlayout2->addItem(new QSpacerItem(25, 0, QSizePolicy::Fixed), line, 0);
                //gridlayout2->addWidget(m_forward[capa], line, 1);
//            }
            //m_forward[capa]->widget()->setObjectName("service");
            //m_forward[capa]->widget()->setProperty("capa", capa);

            //m_forwarddest[capa] = new WaitingWidget<QLineEdit>(
            //     new QLineEdit(this));
            //m_forwarddest[capa]->widget()->setProperty("capa", capa);
            //m_forwarddest[capa]->widget()->setToolTip(
            //     tr("Please enter a destination to activate the checkboxes"));
            //gridlayout2->addWidget(m_forwarddest[capa], line++, 2);
 //       }
 //   }

    //m_nofwd->setChecked(true);
    //forwardModeChanged();

    //QVBoxLayout *vlayout = new QVBoxLayout(this);
    //groupBox1->hide();
    //vlayout->addWidget(groupBox1);

    //groupBox2->hide();
    //vlayout->addWidget(groupBox2);

    //vlayout->addStretch(1);

    // connect signals/slots


   // foreach (QString capa, chkcapas) {
   //     if (m_capas.contains(capa)) {
   //         connect(m_chkopt[capa]->widget(), SIGNAL(clicked(bool)),
   //                 this, SLOT(chkoptToggled(bool)));
   //     }
   // }
   // foreach (QString capa, fwdcapas) {
   //     if (m_capas.contains(capa)) {
   //         connect(m_forward[capa]->widget(), SIGNAL(toggled(bool)),
   //                 this, SLOT(forwardToggled(bool)));
   //         connect(m_forwarddest[capa]->widget(), SIGNAL(editingFinished()),
   //                 this, SLOT(forwardLostFocus()));
   //     }
   // }

   /* connect(b_engine, SIGNAL(servicePutIsOK(const QString &, const QString &)),
            this, SLOT(servicePutIsOK(const QString &, const QString &)));
*/
    connect(b_engine, SIGNAL(updateUserConfig(const QString &, const QVariantMap &)),
            this, SLOT(updateUserConfig(const QString &, const QVariantMap &)));
/*    connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
            this, SLOT(updatePhoneConfig(const QString &)));
    connect(m_fwdmode, SIGNAL(buttonReleased(QAbstractButton *)),
            this, SLOT(forwardModeChanged()));*/
}

void ServicesPanel::on_call_filtering_checkbox_stateChanged(int state)
{
    qDebug() << "Je suis changé d'etat: " << state;


}

void ServicesPanel::on_dnd_checkbox_stateChanged(int state)
{

}

void ServicesPanel::on_fwdna_checkbox_stateChanged(int state)
{
    QString fwd_dest = this->ui.fwdna_input->text();

    if( ! fwd_dest.isEmpty() ) {
        //m_replyids[fwd_type] = b_engine->servicePutForward(fwd_type, checked, fwd_dest);
        b_engine->sendJsonCommand(MessageFactory::setForwardNoAnswer(fwd_dest, (bool)state));
        qDebug() << "Send: FWDNA" " checked:" << (bool)state << " fwd_dest:" << fwd_dest;
    }
}

void ServicesPanel::on_fwdbusy_checkbox_stateChanged(int state)
{
    QString fwd_dest = this->ui.fwdbusy_input->text();

    if( ! fwd_dest.isEmpty() ) {
        //m_replyids[fwd_type] = b_engine->servicePutForward(fwd_type, checked, fwd_dest);
        b_engine->sendJsonCommand(MessageFactory::setForwardBusy(fwd_dest, (bool)state));
        qDebug() << "Send: FWDBUSY" " checked:" << (bool)state << " fwd_dest:" << fwd_dest;
    }
}

void ServicesPanel::on_nofwd_radiobutton_toggled(bool checked)
{
    m_nofwd_sended = checked;

    if( checked ) {
        this->ui.fwdna_checkbox->setChecked(false);
        this->ui.fwdbusy_checkbox->setChecked(false);
    }
}

void ServicesPanel::on_fwdunc_radiobutton_toggled(bool checked)
{
    QString fwd_dest = this->ui.fwdunc_input->text();

    if( ! fwd_dest.isEmpty() ) {
        //m_replyids[fwd_type] = b_engine->servicePutForward(fwd_type, checked, fwd_dest);
        b_engine->sendJsonCommand(MessageFactory::setUnconditionalForward(fwd_dest, checked));
        qDebug() << "Send: FWDUNC" " checked:" << checked << " fwd_dest:" << fwd_dest;
    }
}

void ServicesPanel::on_fwdsimple_radiobutton_toggled(bool checked)
{
    this->toggledSimpleFwd(checked);
}

void ServicesPanel::on_fwdunc_input_returnPressed()
{
    this->toggledSimpleFwd(false);

    if( this->ui.fwdunc_radiobutton->isChecked() ) {
        emit on_fwdunc_radiobutton_toggled(true);
    } else {
        this->ui.fwdunc_radiobutton->setChecked(true);
    }
}

void ServicesPanel::on_fwdna_input_returnPressed()
{
    this->toggledSimpleFwd(true);

    if( this->ui.fwdna_checkbox->isChecked() ) {
        emit on_fwdna_checkbox_stateChanged(Qt::Checked);
    } else {
        this->ui.fwdna_checkbox->setChecked(true);
    }
}

void ServicesPanel::on_fwdbusy_input_returnPressed()
{
    this->toggledSimpleFwd(true);

    if( this->ui.fwdna_checkbox->isChecked() ) {
        emit on_fwdbusy_checkbox_stateChanged(Qt::Checked);
    } else {
        this->ui.fwdbusy_checkbox->setChecked(true);
    }
}

void ServicesPanel::toggledSimpleFwd(bool checked)
{
    this->ui.fwdna_checkbox->setEnabled(checked);
    this->ui.fwdbusy_checkbox->setEnabled(checked);
    this->ui.fwdsimple_radiobutton->setChecked(checked);
}

//
void ServicesPanel::updateUserConfig(const QString & xuserid, const QVariantMap & datamap)
{
    if (xuserid == m_xuserid) {
        QVariantMap deltaConfig = datamap["config"].toMap();
        /* We already are fetching infos from the server (though indirectly).
         * We don't need to send changes.
         */
//        foreach (QString capa, fwdcapas) {
//            if (m_capas.contains(capa)) {
//                 disconnect(m_forward[capa]->widget(), SIGNAL(toggled(bool)),
//                            this, SLOT(forwardToggled(bool)));
//            }
//        }
//        // Check options
//        foreach (QVariant config_var, deltaConfig.keys()) {
//            QString config_str = config_var.toString();
//            if (chkcapas.contains(config_str)) {
//                syncOpt(config_str);
//            }
//        }
//        // Call forwards
//
        qDebug() << "run updateUserConfig: " ;
        this->ui.nofwd_radiobutton->blockSignals(true);
        this->ui.fwdunc_radiobutton->blockSignals(true);
        this->ui.fwdsimple_radiobutton->blockSignals(true);
        this->ui.fwdna_checkbox->blockSignals(true);
        this->ui.fwdbusy_checkbox->blockSignals(true);

        CallForwardStruct call_fwdunc = localCallForward("fwdunc");
        CallForwardStruct call_fwdna = localCallForward("fwdna");
        CallForwardStruct call_fwdbusy = localCallForward("fwdbusy");

        if (deltaConfig.keys().contains("enableunc")
            || deltaConfig.keys().contains("destunc")) {

            this->ui.fwdunc_input->setText(call_fwdunc.destination);
        }

        if (deltaConfig.keys().contains("enablerna")
            || deltaConfig.keys().contains("destrna")) {

            this->ui.fwdna_input->setText(call_fwdna.destination);
            this->ui.fwdna_checkbox->setChecked(call_fwdna.enabled);
        }

        if (deltaConfig.keys().contains("enablebusy")
            || deltaConfig.keys().contains("destbusy")) {

            this->ui.fwdbusy_input->setText(call_fwdbusy.destination);
            this->ui.fwdbusy_checkbox->setChecked(call_fwdbusy.enabled);
        }

        // Activate the right radiobutton
        if (m_nofwd_sended) {
            this->ui.nofwd_radiobutton->setChecked(true);
        } else if (call_fwdunc.enabled) {
            this->ui.fwdunc_radiobutton->setChecked(true);
        } else if (call_fwdna.enabled || call_fwdbusy.enabled) {
                this->toggledSimpleFwd(true);
        } else if (this->ui.fwdunc_radiobutton->isChecked()) {
            this->ui.nofwd_radiobutton->setChecked(true);
        }

        if (m_nofwd_sended && ! call_fwdunc.enabled && ! call_fwdna.enabled
                && ! call_fwdbusy.enabled) {
            m_nofwd_sended = false;
        }

        this->ui.nofwd_radiobutton->blockSignals(false);
        this->ui.fwdunc_radiobutton->blockSignals(false);
        this->ui.fwdsimple_radiobutton->blockSignals(false);
        this->ui.fwdna_checkbox->blockSignals(false);
        this->ui.fwdbusy_checkbox->blockSignals(false);
    }
}
//
//void ServicesPanel::updatePhoneConfig(const QString &)
//{
//}
//
///*!
// * Textbox for unconditional call forward is always active.
// * Other textboxes are only enabled when the "Simple call forward" is checked.
// * Exceptions happen when widgets are waiting server answer. They then are disabled.
// */
//void ServicesPanel::updateTextboxEnabled(const QString & capa)
//{
//    /*if (m_capas.contains(capa)) {
//        if (m_forwarddest[capa]->locked()) { // Waiting server
//            m_forwarddest[capa]->widget()->setEnabled(false);
//        } else if (capa == "fwdunc") { // Unconditional call forward
//            m_forwarddest[capa]->widget()->setEnabled(true);
//        } else if (m_fwdmode->checkedButton() == m_otherfwd) {
//            m_forwarddest[capa]->widget()->setEnabled(true);
//        } else {
//            m_forwarddest[capa]->widget()->setEnabled(false);
//        }
//    }*/
//}
//
///*!
// * Checkboxes are only enabled when the associated destination is not empty.
// * Checkboxes of simple call forwards are only enabled when the "Simple call
// * forward" is checked.
// * Exceptions happen when widgets are waiting server answer. They then are disabled.
// */
//void ServicesPanel::updateCheckboxEnabled(const QString & capa)
//{
///*    if (m_capas.contains(capa)) {
//        if (m_forwarddest[capa]->locked()) { // Waiting server
//            m_forwarddest[capa]->widget()->setEnabled(false);
//        } else if (m_forwarddest[capa]->widget()->text().isEmpty()) { // No destination
//            m_forward[capa]->widget()->setChecked(false);
//            m_forward[capa]->widget()->setEnabled(false);
//        } else if (capa == "fwdunc") { // Unconditional call forward
//            m_forward[capa]->widget()->setEnabled(true);
//        } else if (m_fwdmode->checkedButton() == m_otherfwd) {
//            m_forward[capa]->widget()->setEnabled(true);
//        } else {
//            if (m_fwdmode->checkedButton() == m_nofwd) {
//                m_forward[capa]->widget()->setChecked(false);
//            }
//            m_forward[capa]->widget()->setEnabled(false);
//        }
//    }*/
//}
//
///*! \brief Returns UserInfo data in a more convenient form
// */
CallForwardStruct ServicesPanel::localCallForward(const QString &capa)
{
    CallForwardStruct ret;
    if (capa == "fwdunc") {
        ret.enabled = m_ui->enableunc();
        ret.destination = m_ui->destunc();
    } else if (capa == "fwdna") {
        ret.enabled = m_ui->enablerna();
        ret.destination = m_ui->destrna();
    } else if (capa == "fwdbusy") {
        ret.enabled = m_ui->enablebusy();
        ret.destination = m_ui->destbusy();
    }
    return ret;
}
//
//void ServicesPanel::chkoptToggled(int state)
//{
//    QString capa = sender()->property("capa").toString();
//    m_chkopt[capa]->lock();
//    b_engine->servicePutOpt(capa, b);
//}
//
//void ServicesPanel::forwardModeChanged()
//{
//    foreach (QString capa, fwdcapas) {
//        if (m_capas.contains(capa)) {
//            updateCheckboxEnabled(capa);
//            updateTextboxEnabled(capa);
//        }
//    }
//}
//
//void ServicesPanel::forwardToggled(bool b)
//{
//    QString capa  = sender()->property("capa").toString();
//    QString fdest = m_forwarddest[capa]->widget()->text();
//
//    if(! (b && fdest.isEmpty())) {
//        m_forward[capa]->lock();
//        m_forwarddest[capa]->lock();
//        m_replyids[capa] = b_engine->servicePutForward(capa, b, fdest);
//    } else {
//        qDebug() << "Forwarding calls to nothing ... This should not happen !";
//    }
//}
//
//void ServicesPanel::forwardLostFocus()
//{
//    QString capa = sender()->property("capa").toString();
//    QString fdest = m_forwarddest[capa]->widget()->text();
//    //qDebug() << Q_FUNC_INFO << capa;
//
//    /* Disconnect signal to avoid sending twice the same info :
//     * - first time because the checkbox is unchecked
//     * - second time here
//     */
//    disconnect(m_forward[capa]->widget(), SIGNAL(toggled(bool)),
//               this, SLOT(forwardToggled(bool)));
//    updateCheckboxEnabled(capa);
//    // Reconnect the signal
//    connect(m_forward[capa]->widget(), SIGNAL(toggled(bool)),
//            this, SLOT(forwardToggled(bool)));
//
//    m_forward[capa]->lock();
//    m_forwarddest[capa]->lock();
//    bool status = m_forward[capa]->widget()->isChecked();
//    m_replyids[capa] = b_engine->servicePutForward(capa, status, fdest);
//}
//
///*!
// * The main reason of this function is to catch reply of the CTI server in case
// * no changes have been sent, for example when entering and exiting focus of a
// * text field without changing it. It prevents the WaitingWidgets to wait for an
// * update that will never come, because there is no new data, then nothing to
// * update.
// */
//void ServicesPanel::servicePutIsOK(const QString & replyid, const QString & warning_string)
//{
//    if (warning_string == "no changes") {
//        foreach(QString capa, m_replyids.keys()) {
//            if (m_replyids[capa] == replyid) {
//                m_forward[capa]->unlock();
//                m_forwarddest[capa]->unlock();
//                updateCheckboxEnabled(capa);
//                updateTextboxEnabled(capa);
//            }
//        }
//    }
//}
//
///*! \brief sync widgets with userinfo
// */
//void ServicesPanel::syncOpt(const QString & capa)
//{
//    if (m_ui == NULL)
//        return;
//
//    if (m_capas.contains(capa)) {
//        if (capa == "enablednd")
//            m_chkopt[capa]->widget()->setChecked(m_ui->enablednd());
//        if (capa == "incallfilter")
//            m_chkopt[capa]->widget()->setChecked(m_ui->incallfilter());
//        if (capa == "enablevoicemail")
//            m_chkopt[capa]->widget()->setChecked(m_ui->enablevoicemail());
//        m_chkopt[capa]->unlock();
//    }
//}

/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <baseengine.h>

#include "servicespanel.h"

XLet* XLetServicesPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/services_%1");
    return new ServicesPanel(parent);
}

ServicesPanel::ServicesPanel(QWidget * parent)
    : XLet(parent, tr("Services"), ":/images/tab-services.svg"),
      m_nofwd_sent(false)
{
    this->ui.setupUi(this);

    QVariantList features = b_engine->getConfig("services").toList();

    if (! features.contains("incallfilter")) {
        this->ui.call_filtering_checkbox->hide();
    }

    if (! features.contains("enablednd")) {
        this->ui.dnd_checkbox->hide();
    }

    if (! features.contains("fwdunc")) {
        this->ui.fwdunc_input->hide();
        this->ui.fwdunc_button->hide();
        this->ui.fwdunc_radiobutton->hide();
    }

    if (! features.contains("fwdrna")) {
        this->ui.fwdna_input->hide();
        this->ui.fwdna_button->hide();
        this->ui.fwdna_checkbox->hide();
    }

    if (! features.contains("fwdbusy")) {
        this->ui.fwdbusy_input->hide();
        this->ui.fwdbusy_button->hide();
        this->ui.fwdbusy_checkbox->hide();
    }

    if (! features.contains("fwdrna") && ! features.contains("fwdbusy")) {
        this->ui.fwdsimple_radiobutton->hide();
    }

    //hide search button inside input
    this->ui.fwdna_button->hide();
    this->ui.fwdbusy_button->hide();
    this->ui.fwdunc_button->hide();

    connect(b_engine, SIGNAL(updateUserConfig(const QString &, const QVariantMap &)),
            this, SLOT(updateUserConfig(const QString &, const QVariantMap &)));
}

void ServicesPanel::on_call_filtering_checkbox_stateChanged(int state)
{
    b_engine->sendJsonCommand(MessageFactory::setCallFiltering((bool)state));
}

void ServicesPanel::on_dnd_checkbox_stateChanged(int state)
{
    b_engine->sendJsonCommand(MessageFactory::setDoNotDisturb((bool)state));
}

void ServicesPanel::on_fwdna_checkbox_stateChanged(int state)
{
    if (! this->ui.fwdna_input->text().isEmpty()) {
        this->sendSetForwardNoAnswer((bool)state);
    }
}

void ServicesPanel::on_fwdbusy_checkbox_stateChanged(int state)
{
    if (! this->ui.fwdbusy_input->text().isEmpty()) {
        this->sendSetForwardBusy((bool)state);
    }
}

void ServicesPanel::on_nofwd_radiobutton_toggled(bool checked)
{
    m_nofwd_sent = checked;

    if (checked) {
        this->ui.fwdna_checkbox->setChecked(false);
        this->ui.fwdbusy_checkbox->setChecked(false);
    }
}

void ServicesPanel::on_fwdunc_radiobutton_toggled(bool checked)
{
    if (! this->ui.fwdunc_input->text().isEmpty()) {
        this->sendSetUnconditionalForward(checked);
    }
}

void ServicesPanel::on_fwdsimple_radiobutton_toggled(bool checked)
{
    this->toggledSimpleFwd(checked);
}

void ServicesPanel::on_fwdunc_input_returnPressed()
{
    this->toggledSimpleFwd(false);

    if (this->ui.fwdunc_radiobutton->isChecked()) {
        this->sendSetUnconditionalForward(true);
    } else {
        this->ui.fwdunc_radiobutton->setChecked(true);
    }
}

void ServicesPanel::on_fwdna_input_returnPressed()
{
    this->toggledSimpleFwd(true);

    if (this->ui.fwdna_checkbox->isChecked()) {
        this->sendSetForwardNoAnswer(true);
    } else {
        this->ui.fwdna_checkbox->setChecked(true);
    }
}

void ServicesPanel::on_fwdbusy_input_returnPressed()
{
    this->toggledSimpleFwd(true);

    if (this->ui.fwdbusy_checkbox->isChecked()) {
        this->sendSetForwardBusy(true);
    } else {
        this->ui.fwdbusy_checkbox->setChecked(true);
    }
}

void ServicesPanel::on_fwdunc_input_editingFinished()
{
   if (this->ui.fwdunc_input->isModified() && this->ui.fwdunc_radiobutton->isChecked()) {
       if (this->ui.fwdunc_input->text().isEmpty()) {
           this->sendSetUnconditionalForward(false);
       } else {
           this->sendSetUnconditionalForward(true);
       }
   }
   this->ui.fwdunc_input->setModified(false);
}

void ServicesPanel::on_fwdna_input_editingFinished()
{
   if (this->ui.fwdna_input->isModified() && this->ui.fwdna_checkbox->isChecked()) {
       if (this->ui.fwdna_input->text().isEmpty()) {
            this->sendSetForwardNoAnswer(false);
       } else {
            this->sendSetForwardNoAnswer(true);
       }
   }
   this->ui.fwdna_input->setModified(false);
}

void ServicesPanel::on_fwdbusy_input_editingFinished()
{
   if (this->ui.fwdbusy_input->isModified() && this->ui.fwdbusy_checkbox->isChecked()) {
       if (this->ui.fwdbusy_input->text().isEmpty()) {
            this->sendSetForwardBusy(false);
       } else {
            this->sendSetForwardBusy(true);
       }
   }
   this->ui.fwdbusy_input->setModified(false);
}

void ServicesPanel::sendSetUnconditionalForward(bool checked)
{
    b_engine->sendJsonCommand(MessageFactory::setUnconditionalForward(checked, this->ui.fwdunc_input->text()));
}

void ServicesPanel::sendSetForwardNoAnswer(bool checked)
{
    b_engine->sendJsonCommand(MessageFactory::setForwardNoAnswer(checked, this->ui.fwdna_input->text()));
}

void ServicesPanel::sendSetForwardBusy(bool checked)
{
    b_engine->sendJsonCommand(MessageFactory::setForwardBusy(checked, this->ui.fwdbusy_input->text()));
}

void ServicesPanel::toggledSimpleFwd(bool checked)
{
    this->ui.fwdna_checkbox->setEnabled(checked);
    this->ui.fwdbusy_checkbox->setEnabled(checked);
    this->ui.fwdsimple_radiobutton->setChecked(checked);
}

void ServicesPanel::updateUserConfig(const QString & xuserid, const QVariantMap & datamap)
{
    if (xuserid == m_xuserid) {
        QVariantMap deltaConfig = datamap["config"].toMap();

        this->ui.call_filtering_checkbox->blockSignals(true);
        this->ui.dnd_checkbox->blockSignals(true);
        this->ui.nofwd_radiobutton->blockSignals(true);
        this->ui.fwdunc_radiobutton->blockSignals(true);
        this->ui.fwdsimple_radiobutton->blockSignals(true);
        this->ui.fwdna_checkbox->blockSignals(true);
        this->ui.fwdbusy_checkbox->blockSignals(true);

        if (deltaConfig.keys().contains("incallfilter")) {
            this->ui.call_filtering_checkbox->setChecked(m_ui->incallfilter());
        }

        if (deltaConfig.keys().contains("enablednd")) {
            this->ui.dnd_checkbox->setChecked(m_ui->enablednd());
        }

        CallForwardStruct call_fwdunc = {m_ui->enableunc(), m_ui->destunc()};
        CallForwardStruct call_fwdna = {m_ui->enablerna(), m_ui->destrna()};
        CallForwardStruct call_fwdbusy = {m_ui->enablebusy(), m_ui->destbusy()};

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
        if (m_nofwd_sent) {
            this->ui.nofwd_radiobutton->setChecked(true);
        } else if (call_fwdunc.enabled) {
            this->toggledSimpleFwd(false);
            this->ui.fwdunc_radiobutton->setChecked(true);
        } else if (call_fwdna.enabled || call_fwdbusy.enabled) {
            this->toggledSimpleFwd(true);
        } else if (this->ui.fwdunc_radiobutton->isChecked()) {
            this->ui.nofwd_radiobutton->setChecked(true);
        }

        if (m_nofwd_sent && ! call_fwdunc.enabled && ! call_fwdna.enabled
            && ! call_fwdbusy.enabled) {
            m_nofwd_sent = false;
        }

        this->ui.call_filtering_checkbox->blockSignals(false);
        this->ui.dnd_checkbox->blockSignals(false);
        this->ui.nofwd_radiobutton->blockSignals(false);
        this->ui.fwdunc_radiobutton->blockSignals(false);
        this->ui.fwdsimple_radiobutton->blockSignals(false);
        this->ui.fwdna_checkbox->blockSignals(false);
        this->ui.fwdbusy_checkbox->blockSignals(false);
    }
}

/* XiVO Client
 * Copyright (C) 2016 Avencall
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

#ifdef FUNCTESTS

#include <xlets/services/servicespanel.h>
#include "remote_control.h"

void RemoteControl::set_dnd(const QVariantList & args)
{
    bool enabled = args[0].toBool();

    ServicesPanel *xlet = this->get_xlet<ServicesPanel>("features");
    xlet->findChild<QCheckBox*>(QString("dnd_checkbox"))->setChecked(enabled);
}

void RemoteControl::set_incallfilter(const QVariantList & args)
{
    bool enabled = args[0].toBool();

    ServicesPanel *xlet = this->get_xlet<ServicesPanel>("features");
    xlet->findChild<QCheckBox*>(QString("call_filtering_checkbox"))->setChecked(enabled);
}

void RemoteControl::set_noanswer(const QVariantList & args)
{
    bool enabled = args[0].toBool();

    ServicesPanel *xlet = this->get_xlet<ServicesPanel>("features");
    if (args.size() > 1) {
        QString destination = args[1].toString();
        xlet->findChild<QLineEdit*>(QString("fwdna_input"))->setText(destination);
    }
    xlet->findChild<QRadioButton*>(QString("fwdsimple_radiobutton"))->setChecked(enabled);
    xlet->findChild<QCheckBox*>(QString("fwdna_checkbox"))->setChecked(enabled);
}

void RemoteControl::set_busy(const QVariantList & args)
{
    bool enabled = args[0].toBool();

    ServicesPanel *xlet = this->get_xlet<ServicesPanel>("features");
    if (args.size() > 1) {
        QString destination = args[1].toString();
        xlet->findChild<QLineEdit*>(QString("fwdbusy_input"))->setText(destination);
    }
    xlet->findChild<QRadioButton*>(QString("fwdsimple_radiobutton"))->setChecked(enabled);
    xlet->findChild<QCheckBox*>(QString("fwdbusy_checkbox"))->setChecked(enabled);
}

void RemoteControl::set_unconditional(const QVariantList & args)
{
    bool enabled = args[0].toBool();

    ServicesPanel *xlet = this->get_xlet<ServicesPanel>("features");
    if (args.size() > 1) {
        QString destination = args[1].toString();
        xlet->findChild<QLineEdit*>(QString("fwdunc_input"))->setText(destination);
    }
    xlet->findChild<QRadioButton*>(QString("fwdunc_radiobutton"))->setChecked(enabled);
}

void RemoteControl::disable_all_forwards()
{
    ServicesPanel *xlet = this->get_xlet<ServicesPanel>("features");
    xlet->findChild<QRadioButton*>(QString("nofwd_radiobutton"))->setChecked(true);
}

#endif

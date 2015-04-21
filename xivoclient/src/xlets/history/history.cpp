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

#include <baseengine.h>
#include <phonenumber.h>

#include "history.h"
#include "history_model.h"
#include "history_view.h"

XLet* XLetHistoryPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/history_%1");
    return new History(parent);
}


static inline
QRadioButton* buildRadioButton(QString text,
                               QString icon,
                               int mode,
                               QGroupBox *groupBox,
                               QHBoxLayout *hbox,
                               HistoryModel *m_viewmodel)
{
    QRadioButton *build = new QRadioButton(text, groupBox);

    build->setIcon(QIcon(QString(":/images/history/%0").arg(icon)));
    build->setProperty("mode", mode);
    build->setToolTip(build->text());
    hbox->addWidget(build);

    QObject::connect(build, SIGNAL(toggled(bool)), m_viewmodel, SLOT(changeMode(bool)));

    return build;
}

History::History(QWidget *parent)
    : XLet(parent, tr("History"), ":/images/tab-history.svg"),
      m_view(0),
      m_history_model(0)
{
    QGroupBox *groupBox = new QGroupBox(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *hBox = new QHBoxLayout(groupBox);

    m_history_model = new HistoryModel(0, this);

    hBox->addStretch(1);
    buildRadioButton(tr("Sent calls"), "sent_call.png", OUTCALLS, groupBox, hBox, m_history_model)->setChecked(true);
    buildRadioButton(tr("Received calls"), "received_call.png", INCALLS, groupBox, hBox, m_history_model);
    buildRadioButton(tr("Missed calls"), "missed_call.png", MISSEDCALLS, groupBox, hBox, m_history_model);
    hBox->addStretch(1);

    m_view = new HistoryView(this, m_history_model);
    m_view->installEventFilter(this);

    layout->addWidget(groupBox);
    layout->addWidget(m_view);
}

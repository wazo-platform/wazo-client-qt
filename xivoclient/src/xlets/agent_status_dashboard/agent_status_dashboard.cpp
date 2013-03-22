/* Copyright (C) 2007-2013, Avencall
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

#include <QVBoxLayout>
#include <QTimer>

#include <baseengine.h>

#include "agent_status_dashboard.h"

Q_EXPORT_PLUGIN2(xletagentstatusdashboardplugin, XLetAgentStatusDashboardPlugin);

XLet* XLetAgentStatusDashboardPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/agent_status_dashboard_%1");
    return new XletAgentStatusDashboard(parent);
}

XletAgentStatusDashboard::XletAgentStatusDashboard(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Agent status dashboard"));
}

/* Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
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

#include <QStyle>
#include <QStylePainter>
#include <QStyleOption>
#include <QLabel>

#include "agent_status_widget.h"

AgentStatusWidget::AgentStatusWidget(QWidget * parent)
    : QWidget(parent)
{
}

AgentStatusWidget::~AgentStatusWidget()
{
}

void AgentStatusWidget::setAvailabilityStyle(const QString &object_name)
{
    this->setObjectName(object_name);
    this->style()->unpolish(this);
    this->style()->polish(this);
}

void AgentStatusWidget::setAvailabilityText(const QString &availability_text)
{
    QLabel * agent_availability_label = this->findChild<QLabel *>("agent_status_label");
    agent_availability_label->setText(availability_text);
}

void AgentStatusWidget::setAgentName(const QString &agent_name)
{
    QLabel * agent_name_label = this->findChild<QLabel *>("agent_name_label");
    agent_name_label->setText(agent_name);
}

void AgentStatusWidget::setStatusSince(const QString &status_since)
{
    QLabel * agent_status_since_label = this->findChild<QLabel *>("agent_status_since_label");
    agent_status_since_label->setText(status_since);
}

/* This function is needed in order to apply QSS on QWidget subclass. */
void AgentStatusWidget::paintEvent(QPaintEvent* /*paint_event*/)
{
    QStyleOption opt;
    opt.init(this);
    QStylePainter p(this);
    p.drawPrimitive(QStyle::PE_Widget, opt);
}


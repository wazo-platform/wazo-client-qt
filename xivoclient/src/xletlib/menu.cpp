/* Copyright (C) 2015 Avencall
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

#include "menu.h"

Menu::Menu(QWidget *parent)
    : QMenu(parent)
{
    // These disable the corners around QMenu, so that rounded corners are visible
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    this->setStyleSheet("QMenu {background: white; border: 1px solid grey; border-radius: 10px; padding-top: 7px; padding-bottom: 7px;}"
                        "QMenu::item {padding-top: 5px; padding-left: 13px; padding-right: 23px; padding-bottom: 5px;}"
                        "QMenu::separator {background: #e8e4e2; height: 1px;}"
                        "QMenu::item:selected {background: #e8e4e2; color: black;}");
}

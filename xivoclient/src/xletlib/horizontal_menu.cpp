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

#include <QAction>
#include <QLabel>
#include <QRadioButton>
#include <QPainter>

#include <xletlib/signal_filter.h>

#include "horizontal_menu.h"

HorizontalMenu::HorizontalMenu(QWidget *parent)
    : QWidget(parent),
      m_layout(this)
{
    QString checked_color = "#2c2927";
    QString unchecked_color = "#a09790";
    this->setStyleSheet(QString("QRadioButton::indicator {image: none; height: 0px; width: 0px;}"
                                "QRadioButton {color: %1; font-family: \"Dyno\"; font-size: 16px; spacing: 0px;}"
                                "QRadioButton:checked {color: %2; font-weight: bold; }").arg(unchecked_color).arg(checked_color));
    this->m_layout.setContentsMargins(0, 0, 0, 0);
    this->m_layout.addStretch();
    this->dot = QIcon(":/images/dot.svg").pixmap(QSize(3, 3));
    QPainter tint_painter(&this->dot);
    tint_painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    tint_painter.fillRect(this->dot.rect(), QColor(unchecked_color));
    tint_painter.end();
}

QAction *HorizontalMenu::addAction(const QString &text)
{
    if (this->count() > 0) {
        this->addSeparator();
    }
    return this->addItem(text);
}

QAction *HorizontalMenu::addItem(const QString &text)
{
    Item item;
    item.button = new QRadioButton(this);
    item.button->setFocusPolicy(Qt::NoFocus);
    item.button->setText(text.toUpper());
    item.action = new QAction(this);
    item.signal_filter = new SignalFilter(this);
    connect(item.button, SIGNAL(toggled(bool)),
            item.signal_filter, SLOT(acceptTrue(bool)));
    connect(item.signal_filter, SIGNAL(accepted()),
            item.action, SIGNAL(triggered()));
    this->m_layout.insertWidget(this->m_layout.count() - 1, item.button);
    this->m_items.append(item);
    return item.action;
}

void HorizontalMenu::addSeparator()
{
    QLabel *separator = new QLabel(this);
    separator->setStyleSheet("margin-left: 12px; margin-right: 12px;");
    separator->setPixmap(this->dot);
    this->m_separators.append(separator);
    this->m_layout.insertWidget(this->m_layout.count() - 1, separator);
}

int HorizontalMenu::count()
{
    return this->m_layout.count() / 2;  // 2 * count = (spacer + first_item) + (separator + next_item) * (count - 1)
}

void HorizontalMenu::setSelectedIndex(int index)
{
    if (index < 0 || index >= this->m_items.size()) {
        return;
    }
    this->m_items[index].button->setChecked(true);
}

void HorizontalMenu::setTextIndex(int index, const QString & text)
{
    if (index < 0 || index >= this->m_items.size()) {
        return;
    }
    this->m_items[index].button->setText(text);
}

void HorizontalMenu::hideIndex(int index)
{
    if (index < 1 || index >= this->m_items.size()) {
        return;
    }
    this->m_items[index].button->hide();
    if (index > 0) {
        this->m_separators[index-1]->hide();
    }
}

void HorizontalMenu::showIndex(int index)
{
    if (index < 0 && index >= this->m_items.size()) {
        return;
    }
    this->m_items[index].button->show();
    if (index > 0) {
        this->m_separators[index-1]->show();
    }
}

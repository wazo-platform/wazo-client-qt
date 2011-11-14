/* XiVO Client
 * Copyright (C) 2011, Avencall
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

#include <QDebug>
#include <QKeyEvent>
#include <typeinfo>

#include "completionedit.h"

FilteredCompleter::FilteredCompleter(QObject * parent)
  : QCompleter(parent), m_model()
{
    setModel(&m_model); // Default is QStringListModel
}

void FilteredCompleter::update(const QString & query)
{
    QStringList filtered = m_list.filter(query, caseSensitivity());
    m_model.setStringList(filtered);
    m_word = query;
    complete(); // Start auto completion
}

void FilteredCompleter::insertItem(const QString & item)
{
    if (m_list.contains(item, Qt::CaseInsensitive)) return;
    m_list << item;
}

const QString FilteredCompleter::word() const
{
    return m_word;
}

FilteredLineEdit::FilteredLineEdit(QWidget *parent)
    : QLineEdit(parent), c(0)
{
}

void FilteredLineEdit::setCompleter(FilteredCompleter * completer)
{
    // qDebug() << Q_FUNC_INFO;

    if (c)
        QObject::disconnect(c, 0, this, 0);

    c = completer;
    c->setCompletionMode(QCompleter::PopupCompletion);

    if (!c)
        return;

    c->setWidget(this);
    connect(completer, SIGNAL(activated(const QString &)),
            this, SLOT(insertCompletion(const QString &)));
}

const FilteredCompleter * FilteredLineEdit::completer() const
{
    // qDebug() << Q_FUNC_INFO;

    return c;
}

void FilteredLineEdit::insertCompletion(const QString & completion)
{
    // qDebug() << Q_FUNC_INFO;

    setText(completion);
    selectAll();
}

void FilteredLineEdit::keyPressEvent(QKeyEvent * e)
{
    // qDebug() << Q_FUNC_INFO;

    if (c && c->popup() && c->popup()->isVisible())
    {
        // The following keys are forwarded by the completer to the widget
        switch (e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // Let the completer do default behavior
        }
    }

    bool isShortcut = (e->modifiers() & Qt::ControlModifier)
            && e->key() == Qt::Key_E;
    // Don't send the shortcut (CTRL-E) to the text edit
    if (!isShortcut)
        QLineEdit::keyPressEvent(e);

    if (!c)
        return;

    bool ctrlOrShift = e->modifiers() &
            (Qt::ControlModifier | Qt::ShiftModifier);
    if (!isShortcut && !ctrlOrShift && e->modifiers() != Qt::NoModifier)
    {
        c->popup()->hide();
        return;
    }

    c->update(text());

    if (c && c->popup()) {
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
}


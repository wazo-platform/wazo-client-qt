/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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

/* $Revision$
 * $Date$
 */

#include <QDebug>
#include <QFrame>
#include <QLabel>
#include <QGridLayout>
#include "detailedexternalphonepeerwidget.h"
#include "externalphonedialog.h"

/*! \brief constructor
 */
DetailedExternalPhonePeerWidget::DetailedExternalPhonePeerWidget(const QString & label, const QString & number)
    : BasePeerWidget(0)
{
    m_number = number;
    m_editable = true;
    int spacing = 2;
    QGridLayout * layout = new QGridLayout(this);
    layout->setSpacing(spacing);
    layout->setMargin(spacing);
    QFrame * qhline1 = new QFrame(this);
    QFrame * qhline2 = new QFrame(this);
    QFrame * qvline1 = new QFrame(this);
    QFrame * qvline2 = new QFrame(this);
    qhline1->hide();
    qhline2->hide();
    qvline2->hide();
    qhline1->setFrameShape(QFrame::HLine);
    qhline1->setLineWidth(0);
    qhline2->setFrameShape(QFrame::HLine);
    qhline2->setLineWidth(0);
    qvline1->setFrameShape(QFrame::VLine);
    qvline1->setLineWidth(2);
    qvline2->setFrameShape(QFrame::VLine);
    qvline2->setLineWidth(0);

    m_textlbl = new QLabel(label, this);
    // set TextInteraction Flags so the mouse clicks are not catched by the QLabel widget
    m_textlbl->setTextInteractionFlags( Qt::NoTextInteraction );
    m_lblphone = new QLabel();
    m_lblphone->setPixmap(QPixmap(":/images/phone-grey.png"));
    m_lblphone->setToolTip( m_number );
    // the widget does not feature a context menu, context menu handling is deferred to the widget's parent.
    m_lblphone->setContextMenuPolicy( Qt::NoContextMenu );
    layout->addWidget( qhline1, 0, 0, 1, 10);
    layout->addWidget( qvline1,   1, 0, 2, 1 );
    layout->addWidget( m_textlbl, 1, 2, 1, 6, Qt::AlignLeft );
    layout->addWidget( qvline2,   1, 9, 2, 1 );
    layout->addWidget( m_lblphone, 2, 2, Qt::AlignCenter);
    m_lblphonenum = new QLabel( m_number );
    layout->addWidget( m_lblphonenum, 2, 3, Qt::AlignLeft );
    layout->setColumnStretch(20, 1);
    layout->addWidget( qhline2, 3, 0, 1, 10);
}

/*! \brief set text
 */
void DetailedExternalPhonePeerWidget::setText(const QString & text)
{
    // qDebug() << Q_FUNC_INFO << text;
    m_textlbl->setText( text );
}

/*! \brief edit the number and text with a dialog
 *
 *
 */
void DetailedExternalPhonePeerWidget::edit()
{
    ExternalPhoneDialog dialog(this, m_number, m_textlbl->text());
    if(dialog.exec()){
        if(!dialog.number().isEmpty()){
            m_number = dialog.number();
            m_lblphone->setToolTip(m_number);
            m_lblphonenum->setText(m_number);
        }
        if(!dialog.label().isEmpty())
            setText(dialog.label());
    }
}


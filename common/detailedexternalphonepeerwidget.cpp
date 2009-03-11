/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
 */

/* $Revision$
 * $Date$
 */

#include <QDebug>
#include <QFrame>
#include <QLabel>
#include <QGridLayout>
#include "extendedlabel.h"
#include "detailedexternalphonepeerwidget.h"
#include "externalphonedialog.h"

DetailedExternalPhonePeerWidget::DetailedExternalPhonePeerWidget(BaseEngine * engine, const QString & label, const QString & number)
    : BasePeerWidget(engine, 0)
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
    m_lblphone = new ExtendedLabel();
    m_lblphone->setPixmap(QPixmap(":/images/phone-grey.png"));
    m_lblphone->setToolTip( m_number );
    // the widget does not feature a context menu, context menu handling is deferred to the widget's parent.
    m_lblphone->setContextMenuPolicy( Qt::NoContextMenu );
    layout->addWidget( qhline1, 0, 0, 1, 10);
    layout->addWidget( qvline1,   1, 0, 2, 1 );
    layout->addWidget( m_textlbl, 1, 2, 1, 6, Qt::AlignLeft );
    layout->addWidget( qvline2,   1, 9, 2, 1 );
    layout->addWidget( m_lblphone, 2, 2, Qt::AlignCenter);
    layout->setColumnStretch(20, 1);
    layout->addWidget( qhline2, 3, 0, 1, 10);
}

void DetailedExternalPhonePeerWidget::setText(const QString & text)
{
    qDebug() << "DetailedExternalPhonePeerWidget::setText()" << text;
    m_textlbl->setText( text );
}

/*! \brief 
 *
 *
 */
void DetailedExternalPhonePeerWidget::edit()
{
    ExternalPhoneDialog dialog;
    dialog.setNumber( m_number );
    dialog.setLabel( m_textlbl->text() );
    if(dialog.exec())
    {
        if(!dialog.number().isEmpty())
        {
            m_number = dialog.number();
            m_lblphone->setToolTip( m_number );
        }
        if(!dialog.label().isEmpty())
            setText( dialog.label() );
    }
}


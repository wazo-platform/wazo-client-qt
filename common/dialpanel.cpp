/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
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

#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QRegExp>
#include <QScrollArea>

#include "baseengine.h"
#include "dialpanel.h"
#include "userinfo.h"
#include "xivoconsts.h" // for PEER_MIMETYPE

/*! \brief Constructor
 */
DialPanel::DialPanel(BaseEngine * engine, QWidget * parent)
        : QWidget(parent), m_engine(engine)
{
	QHBoxLayout * vlayout = new QHBoxLayout(this);
	vlayout->setMargin(0);
        m_lbl = new QLabel( tr("Enter &Number :"), this );
	m_input = new QComboBox( this );
	m_lbl->setBuddy(m_input);
	m_input->setToolTip( tr("Input here the phone number to dial") );
	m_input->setEditable( true );
        m_input->setDuplicatesEnabled( false );
        m_input->setInsertPolicy( QComboBox::InsertAlphabetically );
	m_input->setMinimumContentsLength( 15 );
	//m_input->setSizeAdjustPolicy( QComboBox::AdjustToContents );
	m_input->setInsertPolicy( QComboBox::NoInsert );
	connect( m_input->lineEdit(), SIGNAL(returnPressed()),
	         this, SLOT(inputValidated()) );
	connect( m_input, SIGNAL(editTextChanged(const QString &)),
	         this, SIGNAL(textEdited(const QString &)) );
        
        QPixmap pmphone = QPixmap(":/images/sipphone.png");
        QPushButton * dialButton = new QPushButton(this);
        // dialButton->setStyleSheet("QPushButton {border: 0px}");
        dialButton->setIcon(pmphone);
        dialButton->setIconSize(pmphone.size());
	connect( dialButton, SIGNAL(clicked()),
	         this, SLOT(inputValidated()) );
        
        // QPushButton * clearButton = new QPushButton(this);
        // clearButton->setIcon(QIcon(":/images/cancel.png"));
	// connect( clearButton, SIGNAL(clicked()),
        // this, SLOT(clearlist()) );

        setAcceptDrops(true);

        vlayout->addStretch(1);
	// vlayout->addWidget( clearButton, 0, Qt::AlignCenter );
	vlayout->addWidget( m_lbl, 0, Qt::AlignCenter );
	vlayout->addWidget( m_input, 0, Qt::AlignCenter );
	vlayout->addWidget( dialButton, 0, Qt::AlignCenter );
        vlayout->addStretch(1);
}

void DialPanel::setUserInfo(const UserInfo * ui)
{
        m_ui = ui;
}

void DialPanel::setNumberToDial(const QString & text)
{
        // qDebug() << "DialPanel::setNumberToDial()" << text;
        // adds the item to the list
        QString texttmp = text.trimmed();
        texttmp.remove(QRegExp("[. ]"));
        
        if((texttmp.size() > 0) && (! texttmp.contains(QRegExp("\\D")))) {
                if (m_input->findText(texttmp) == -1)
                        m_input->insertItem(0, texttmp);
                // displays it
                m_input->lineEdit()->setText(texttmp);
        }
}

void DialPanel::dragEnterEvent(QDragEnterEvent * event)
{
        // qDebug() << "DialPanel::dragEnterEvent()" << event;
	if(event->mimeData()->hasFormat(PEER_MIMETYPE))
                event->acceptProposedAction();
}

void DialPanel::dropEvent(QDropEvent * event)
{
	QString ext;
	QString originator = QString::fromAscii(event->mimeData()->data("userid"));
	if(m_input->lineEdit()) {
                qDebug() << "DialPanel::dropEvent()" << event << originator << m_input->lineEdit()->text();
                ext = m_input->lineEdit()->text();
                ext.remove(QRegExp("[\\s\\.]")); // remove spaces and full stop characters
                if(ext.length() == 0)	// do nothing if the string is empty
                        return;
                originateCall("user:" + originator,
                              "ext:" + m_input->lineEdit()->text());
                m_input->insertItem(0, ext); // add to history
                // remove the older items related to the same number
                for(int i=1; i<m_input->count(); ) {
                        if(ext == m_input->itemText(i)) {
                                m_input->removeItem(i);
                        }
                        else
                                i++;
                }
                m_input->clearEditText();
        }
}

// void DialPanel::mouseMoveEvent(QMouseEvent *event)
// {
// 	qDebug() << "DialPanel::mouseMoveEvent()" << event;
// }

// void DialPanel::mousePressEvent(QMouseEvent *event)
// {
// 	qDebug() << "DialPanel::mousePressEvent()" << event;
// }

/*! \brief the input was validated
 *
 * check the input and call originateCall() if ok.
 */
void DialPanel::inputValidated()
{
	QString ext;
	if(m_input->lineEdit()) {
		ext = m_input->lineEdit()->text();
                ext.remove(QRegExp("[\\s\\.]")); // remove spaces and full stop characters
                if(ext.length() == 0)	// do nothing if the string is empty
                        return;
                originateCall("user:special:me", "ext:" + ext);
                m_input->insertItem(0, ext); // add to history
                // remove the older items related to the same number
                for(int i=1; i<m_input->count(); ) {
                        if(ext == m_input->itemText(i)) {
                                m_input->removeItem(i);
                        }
                        else
                                i++;
                }
                m_input->clearEditText();
        }
}

void DialPanel::clearlist()
{
        m_input->clear();
}

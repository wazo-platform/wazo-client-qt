/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, Trolltech gives permission to link
 * the code of its release of Qt with the OpenSSL project's "OpenSSL" library
 * (or modified versions of the "OpenSSL" library that use the same license
 * as the original version), and distribute the linked executables.
 *
 * (see <http://doc.trolltech.com/4.4/gpl.html>)
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
DialPanel::DialPanel(BaseEngine * engine,
                     const QVariant & options,
                     QWidget * parent)
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
    
    setGuiOptions(options);
}

DialPanel::~DialPanel()
{
}

void DialPanel::setNumberToDial(const QString & text)
{
    QString oldtext = m_input->lineEdit()->text();
    // qDebug() << "DialPanel::setNumberToDial()" << text;
    // adds the item to the list
    QString texttmp = text.trimmed();
    // remove . and " " because we don't need them
    // remove "+" to avoid matching the "\\D"
    texttmp.remove(QRegExp("[. +]"));
    
    if((! texttmp.isEmpty()) && (! texttmp.contains(QRegExp("\\D")))) {
        // if there was a "+", put it back
        if(text.trimmed()[0] == '+')
            texttmp.insert(0, "+");
        // stop if the selection is the same as the number already displayed
        if(texttmp == oldtext)
            return;
        // put in history if not already there
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
    QString originator = QString::fromAscii(event->mimeData()->data(USERID_MIMETYPE));
    qDebug() << "DialPanel::dropEvent()" << originator << m_input->lineEdit();
    if(m_input->lineEdit()) {
        qDebug() << "DialPanel::dropEvent()" << event << originator << m_input->lineEdit()->text();
        ext = m_input->lineEdit()->text();
        ext.remove(QRegExp("[\\s\\.]")); // remove spaces and full stop characters
        if(ext.length() == 0)        // do nothing if the string is empty
            return;
        actionCall("originate", "user:" + originator, "ext:" + m_input->lineEdit()->text()); // Call
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
//         qDebug() << "DialPanel::mouseMoveEvent()" << event;
// }

// void DialPanel::mousePressEvent(QMouseEvent *event)
// {
//         qDebug() << "DialPanel::mousePressEvent()" << event;
// }

/*! \brief the input was validated
 *
 * check the input and call actionCall() if ok.
 */
void DialPanel::inputValidated()
{
    QString ext;
    if(m_input->lineEdit()) {
        ext = m_input->lineEdit()->text();
        ext.remove(QRegExp("[\\s\\.]")); // remove spaces and full stop characters
        if(ext.length() == 0)        // do nothing if the string is empty
            return;
        actionCall("originate", "user:special:me", "ext:" + ext); // Call
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

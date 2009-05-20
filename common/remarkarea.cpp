/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include "remarkarea.h"

/*! \brief Constructor */
RemarkArea::RemarkArea(QWidget * parent)
    : QWidget(parent), m_currentline(0)
{
    // top level widget : VBoxLayout
    m_layout = new QVBoxLayout( this );
    // on top : a ScrollArea containing the entries
    m_scrollarea = new QScrollArea( this );
    m_layout->addWidget( m_scrollarea );
    m_layout->setStretchFactor( m_scrollarea, 1 );

    QWidget * scrolledwidget = new QWidget( m_scrollarea );
    m_scrollarea->setWidget( scrolledwidget );
    //m_scrollarea->setWidgetResizable( true );
    m_gridlayout = new QGridLayout( scrolledwidget );
    m_gridlayout->setSizeConstraint( QLayout::SetFixedSize );
    m_gridlayout->setVerticalSpacing(0);
    m_gridlayout->setColumnStretch(1, 1);

    // bottom : one HBoxLayout containing a QLineEdit and a QPushButton
    QHBoxLayout * hlayout = new QHBoxLayout();
    m_layout->addLayout( hlayout );
    m_inputline = new QLineEdit( this );
    hlayout->addWidget( m_inputline );
    m_submitbutton = new QPushButton( tr("&Submit"), this );
    hlayout->addWidget( m_submitbutton );
    connect( m_submitbutton, SIGNAL(clicked()),
             this, SLOT(submitClicked()) );
    connect( m_inputline, SIGNAL(returnPressed()),
             this, SLOT(submitClicked()) );
    m_inputline->hide();
    m_submitbutton->hide();

    scrolledwidget->show();
    // automatically scroll to last entry when the size of the scroll area change
    connect( m_scrollarea->verticalScrollBar(), SIGNAL(rangeChanged(int, int)),
             this, SLOT(scrollToLastEntry()) );
}

/*! \brief display input form
 *
 * put the widgets at the right position and give focus to the QLineEdit
 */
void RemarkArea::displayInputForm()
{
    m_inputline->clear();
    //m_layout->addWidget(m_inputline, m_currentline, 0, 1, 2);
    //m_layout->addWidget(m_submitbutton, m_currentline, 2, 1, 1);
    m_inputline->show();
    m_submitbutton->show();
    m_inputline->setFocus();
}

/*! \brief hide input form */
void RemarkArea::hideInputForm()
{
    m_inputline->hide();
    m_submitbutton->hide();
    //m_layout->removeWidget(m_inputline);
    //m_layout->removeWidget(m_submitbutton);
}

/*! \brief add and display a remark
 */
void RemarkArea::addRemark(const QString & header, const QString & text)
{
    QWidget * headerwidget = new QLabel(header, m_scrollarea);
    m_gridlayout->addWidget(headerwidget, m_currentline, 0);
    m_gridlayout->addWidget(new QLabel(text, m_scrollarea), m_currentline, 1);
    m_currentline++;
    //m_scrollarea->ensureWidgetVisible(headerwidget); // doesnt work
    //m_scrollarea->ensureVisible(0, m_currentline*100);
}

void RemarkArea::submitClicked()
{
    QString text = m_inputline->text();
    qDebug() << "RemarkArea::submitClicked()" << text;
    if(!text.isEmpty()) {
        emit textSubmitted( text );
    }
    m_inputline->clear();
}

/*! \brief scroll to the bottom of the scroll area in order to show last entry */
void RemarkArea::scrollToLastEntry()
{
    qDebug() << "RemarkArea::autoScroll()";
    if(m_scrollarea->verticalScrollBar()) {
        //qDebug() << " vScrollBar" << m_scrollarea->verticalScrollBar();
        //qDebug() << m_scrollarea->verticalScrollBar()->value() << m_scrollarea->verticalScrollBar()->maximum();
        m_scrollarea->verticalScrollBar()->setValue( m_scrollarea->verticalScrollBar()->maximum() );
    }
}

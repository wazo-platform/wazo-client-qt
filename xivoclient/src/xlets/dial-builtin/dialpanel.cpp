/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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


#include "dialpanel.h"
#include "phonenumber.h"

/*! \brief Constructor
 */
DialPanel::DialPanel(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Dial"));
    setAccessibleName(tr("Dial Panel"));
    QHBoxLayout * vlayout = new QHBoxLayout(this);
    vlayout->setMargin(0);
    m_lbl = new QLabel(tr("Enter &Number :"), this);
    m_input = new QComboBox(this);
    m_lbl->setBuddy(m_input);
    m_input->setToolTip(tr("Input here the phone number to dial"));
    m_input->setEditable(true);
    m_input->setDuplicatesEnabled(false);
    m_input->setInsertPolicy(QComboBox::InsertAtTop);
    m_input->setMinimumContentsLength(15);
    loadHistory();
    m_input->clearEditText();
    //m_input->setSizeAdjustPolicy( QComboBox::AdjustToContents );
    connect(m_input->lineEdit(), SIGNAL(returnPressed()),
            this, SLOT(inputValidated()));
    connect(m_input, SIGNAL(editTextChanged(const QString &)),
            this, SIGNAL(textEdited(const QString &)));

    QPixmap pmphone = QPixmap(":/images/sipphone.png");
    QPushButton * dialButton = new QPushButton(this);
    // dialButton->setStyleSheet("QPushButton {border: 0px}");
    dialButton->setIcon(pmphone);
    dialButton->setIconSize(pmphone.size());
    
    // That way, we're sure to have the same behavior when enter is pressed and 
    // when the button is pressed
    connect(dialButton, SIGNAL(clicked()),
            m_input->lineEdit(), SIGNAL(returnPressed()));

    // QPushButton * clearButton = new QPushButton(this);
    // clearButton->setIcon(QIcon(":/images/cancel.png"));
    // connect( clearButton, SIGNAL(clicked()),
    // this, SLOT(clearlist()) );

    setAcceptDrops(true);

    vlayout->addStretch(1);
    // vlayout->addWidget( clearButton, 0, Qt::AlignCenter );
    vlayout->addWidget(m_lbl, 0, Qt::AlignCenter);
    vlayout->addWidget(m_input, 0, Qt::AlignCenter);
    vlayout->addWidget(dialButton, 0, Qt::AlignCenter);
    vlayout->addStretch(1);

    // connect signals/slots
    connect(b_engine, SIGNAL(pasteToXlets(const QString &)),
            this, SLOT(setNumberToDial(const QString &)));
    connect(this, SIGNAL(textEdited(const QString &)),
            b_engine, SLOT(textEdited(const QString &)));
}

DialPanel::~DialPanel()
{
    saveHistory();
}

/*! \brief fills the input field
 */
void DialPanel::setNumberToDial(const QString & text)
{
    // qDebug() << Q_FUNC_INFO << text;
    QString oldtext = m_input->currentText();
    QString texttmp = PhoneNumber::extract(text);

    if((! texttmp.isEmpty()) && texttmp != oldtext) {
        addHistory(texttmp);
        // displays it
        m_input->setEditText(texttmp);
    }
}

void DialPanel::dragEnterEvent(QDragEnterEvent * event)
{
    qDebug() << Q_FUNC_INFO << event;
    if(event->mimeData()->hasFormat(XPHONEID_MIMETYPE))
        event->acceptProposedAction();
}

/*! \brief handle drop events
 *
 * This method processes data from the drop event and makes the call
 */
void DialPanel::dropEvent(QDropEvent * event)
{
    QString originator = QString::fromAscii(event->mimeData()->data(XUSERID_MIMETYPE));
    qDebug() << Q_FUNC_INFO << event << originator << m_input->currentText();
    QString ext = PhoneNumber::extract(m_input->currentText());
    if (ext.isEmpty())        // do nothing if the string is empty
        return;
    b_engine->actionCall("originate",
                         "user:" + originator,
                         QString("exten:%1/%2").arg(b_engine->ipbxid()).arg(ext));
    addHistory(ext);
    m_input->clearEditText();
}

/*! \brief the input was validated
 *
 * check the input and call actionCall() if ok.
 */
void DialPanel::inputValidated()
{
    QString ext = PhoneNumber::extract(m_input->currentText());
    if (ext.isEmpty()) // do nothing if the string is empty
        return;
    b_engine->actionDialNumber(ext);
    addHistory(ext);
    m_input->clearEditText();
}

/*! \brief clear the input list
 */
void DialPanel::clearlist()
{
    m_input->clear();
}

/*!
 * \brief Loads the call history from BaseEngine
 */
void DialPanel::loadHistory()
{
    // qDebug() << Q_FUNC_INFO;
    int nb_to_load = b_engine->getConfig("dialpanel.history_length").toInt();
    QStringList history = b_engine->getProfileSetting("dialpanel/history").toStringList();
    m_input->addItems(history.mid(0, nb_to_load));
}

/*!
 * \brief Saves the call history in BaseEngine
 */
void DialPanel::saveHistory()
{
    int nb_to_save = b_engine->getConfig("dialpanel.history_length").toInt();
    if (m_input->count() < nb_to_save) {
        nb_to_save = m_input->count();
    }

    QStringList savedHistory;
    for (int i = 0; i < nb_to_save; i++) {
        savedHistory << m_input->itemText(i);
    }
    b_engine->setProfileSetting("dialpanel/history", savedHistory);
}

/*!
 * \brief inserts the new entry in the combobox, on top and removes duplicates
 */
// It's almost possible to do the same with setInsertPolicy and setDuplicatesEnabled
// but it lacks the "go on top if already existing" feature
void DialPanel::addHistory(const QString &ext)
{
    m_input->insertItem(0, ext); // add to history
    // remove the older items related to the same number
    for(int i=1; i<m_input->count(); ) {
        if(ext == m_input->itemText(i)) {
            m_input->removeItem(i);
        } else {
            i++;
        }
    }
}
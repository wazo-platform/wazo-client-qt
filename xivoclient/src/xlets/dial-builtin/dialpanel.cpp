/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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


#include "dialpanel.h"
#include "phonenumber.h"

DialPanel::DialPanel(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Dial"));
    setAccessibleName(tr("Dial Panel"));

    setAcceptDrops(true);

    m_input = new QComboBox(this);
    m_input->setToolTip(tr("Input here the phone number to dial"));
    m_input->setEditable(true);
    m_input->setDuplicatesEnabled(false);
    m_input->setInsertPolicy(QComboBox::InsertAtTop);
    m_input->setMinimumContentsLength(15);
    m_input->lineEdit()->setMaxLength(30);

    m_lbl = new QLabel(tr("Enter &Number :"), this);
    m_lbl->setBuddy(m_input);

    loadHistory();
    m_input->clearEditText();

    QPixmap pmphone = QPixmap(":/images/sipphone.png");
    QPushButton * dialButton = new QPushButton(this);
    dialButton->setIcon(pmphone);
    dialButton->setIconSize(pmphone.size());

    QHBoxLayout * vlayout = new QHBoxLayout(this);
    vlayout->setMargin(0);
    vlayout->addStretch(1);
    vlayout->addWidget(m_lbl, 0, Qt::AlignCenter);
    vlayout->addWidget(m_input, 0, Qt::AlignCenter);
    vlayout->addWidget(dialButton, 0, Qt::AlignCenter);
    vlayout->addStretch(1);

    connect(dialButton, SIGNAL(clicked()),
            m_input->lineEdit(), SIGNAL(returnPressed()));
    connect(b_engine, SIGNAL(pasteToXlets(const QString &)),
            this, SLOT(setNumberToDial(const QString &)));
    connect(this, SIGNAL(textEdited(const QString &)),
            b_engine, SLOT(textEdited(const QString &)));
    connect(m_input->lineEdit(), SIGNAL(returnPressed()),
            this, SLOT(inputValidated()));
    connect(m_input, SIGNAL(editTextChanged(const QString &)),
            this, SIGNAL(textEdited(const QString &)));
}

DialPanel::~DialPanel()
{
    saveHistory();
}

void DialPanel::setNumberToDial(const QString & text)
{
    QString oldtext = m_input->currentText();

    if((! text.isEmpty()) && text != oldtext) {
        m_input->setEditText(text);
    }
}

void DialPanel::dragEnterEvent(QDragEnterEvent * event)
{
    qDebug() << Q_FUNC_INFO << event;
    if(event->mimeData()->hasFormat(XPHONEID_MIMETYPE))
        event->acceptProposedAction();
}

void DialPanel::dropEvent(QDropEvent * event)
{
    QString originator = QString::fromAscii(event->mimeData()->data(XUSERID_MIMETYPE));
    qDebug() << Q_FUNC_INFO << event << originator << m_input->currentText();
    QString ext = m_input->currentText();
    if (ext.isEmpty())
        return;
    b_engine->actionCall("originate",
                         "user:" + originator,
                         QString("exten:%1/%2").arg(b_engine->ipbxid()).arg(ext));
    addHistory(ext);
    m_input->clearEditText();
}

void DialPanel::inputValidated()
{
    QString ext = m_input->currentText();
    if (ext.isEmpty())
        return;
    b_engine->actionDial(ext);
    addHistory(ext);
    m_input->clearEditText();
}

void DialPanel::clearlist()
{
    m_input->clear();
}

void DialPanel::loadHistory()
{
    int nb_to_load = b_engine->getConfig("dialpanel.history_length").toInt();
    QStringList history = b_engine->getProfileSetting("dialpanel/history").toStringList();
    m_input->addItems(history.mid(0, nb_to_load));
}

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

void DialPanel::addHistory(const QString &ext)
{
    m_input->insertItem(0, ext);
    for(int i=1; i<m_input->count(); ) {
        if(ext == m_input->itemText(i)) {
            m_input->removeItem(i);
        } else {
            i++;
        }
    }
}

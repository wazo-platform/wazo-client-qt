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

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHash>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include <QApplication>
#include <QMenu>
#include <QDebug>

#include "baseengine.h"
#include "peerwidget.h"
#include "xivoconsts.h"
#include "userinfo.h"
#include "phoneinfo.h"

// Initialize static members
QHash<QString, QPixmap> PeerWidget::m_phones = QHash<QString, QPixmap>();
QHash<QString, QPixmap> PeerWidget::m_persons = QHash<QString, QPixmap>();
QHash<QString, QPixmap> PeerWidget::m_agents = QHash<QString, QPixmap>();
QHash<QString, QPixmap> PeerWidget::m_mobiles = QHash<QString, QPixmap>();

/*! \brief Constructor
 */
PeerWidget::PeerWidget(BaseEngine * engine, UserInfo * ui)
    : BasePeerWidget(engine, ui), m_availlbl(NULL), m_agentlbl(NULL)
{
    // fill m_persons, m_phones and m_agents if needed.
    if(m_persons.count() == 0)
    {
        QString color = "grey"; // we use only one color now
        m_persons[color] = QPixmap(QString(":/images/personal-%1.png").arg(color));
        m_phones[color] = QPixmap(QString(":/images/phone-%1.png").arg(color));
        m_agents[color] = QPixmap(QString(":/images/agent-%1.png").arg(color));
        m_mobiles[color] = QPixmap(QString(":/images/mobile-%1.png").arg(color));
    }

    // qDebug() << "PeerWidget::PeerWidget()" << id;
    //        QHBoxLayout * layout = new QHBoxLayout(this);
    
    QFrame * qhline1 = new QFrame(this);
    QFrame * qhline2 = new QFrame(this);
    QFrame * qvline1 = new QFrame(this);
    QFrame * qvline2 = new QFrame(this);
    
    int spacing, fsize, width_left, width_right, width_top, width_bottom;
    bool stretch_last;
    Qt::Alignment alignment;
    
    spacing = 2;
    fsize = 30;
    width_left = 2;
    width_right = 0;
    width_top = 0;
    width_bottom = 0;
    qhline1->hide();
    qhline2->hide();
    qvline2->hide();
    alignment = Qt::AlignLeft;
    stretch_last = true;
    
    // spacing = 0;
    // fsize = 20;
    // width_left = 3;
    // width_right = 1;
    // width_top = 3;
    // width_bottom = 1;
    // alignment = Qt::AlignCenter;
    // stretch_last = false;
    
    qhline1->setFrameShape(QFrame::HLine);
    qhline1->setLineWidth(width_top);
    qhline2->setFrameShape(QFrame::HLine);
    qhline2->setLineWidth(width_bottom);
    qvline1->setFrameShape(QFrame::VLine);
    qvline1->setLineWidth(width_left);
    qvline2->setFrameShape(QFrame::VLine);
    qvline2->setLineWidth(width_right);
    
    QGridLayout * layout = new QGridLayout(this);
    layout->setSpacing(spacing);
    layout->setMargin(spacing);
    
    // QLabels definitions
    if(m_ui->fullname().isEmpty())
        qDebug() << "PeerWidget::PeerWidget()" << "the callerid information m_ui->fullname() is empty for :" << m_ui->userid();
    //m_textlbl = new QLabel(m_ui->fullname().isEmpty() ? tr("(No callerid yet)") : m_ui->fullname(),
    //                       this);
    m_textlbl = new QLabel(this);
    setName(m_ui->fullname());
    
    // set TextInteraction Flags so the mouse clicks are not catched by the QLabel widget
    m_textlbl->setTextInteractionFlags( Qt::NoTextInteraction );
    
    if(! m_ui->ctilogin().isEmpty()) {
        m_availlbl = new QLabel(this);
        m_availlbl->setPixmap(m_persons["grey"]);
        m_availlbl->setAlignment(Qt::AlignCenter);
        m_availlbl->setMinimumSize(fsize, fsize);
        m_availlbl->setObjectName("onlyme");
        m_availlbl->setProperty("kind", "person");
        // setColorAvail("person", "grey", "");
    }
    
    //m_voicelbl = new QLabel();
    //m_fwdlbl   = new QLabel();
    
    foreach (QString phone, ui->phonelist())
    {
        m_lblphones[phone] = new QLabel(this);
        m_lblphones[phone]->setPixmap(m_phones["grey"]);
        m_lblphones[phone]->setAlignment(Qt::AlignCenter);
        m_lblphones[phone]->setObjectName("onlyme");
        m_lblphones[phone]->setMinimumSize(fsize, fsize);
        m_lblphones[phone]->setProperty("kind", "term");
    }
        
    if(! m_ui->mobilenumber().isEmpty()) {
        m_mobilelbl = new QLabel(this);
        m_mobilelbl->setPixmap(m_mobiles["grey"]);
        m_mobilelbl->setAlignment(Qt::AlignCenter);
        m_mobilelbl->setMinimumSize(fsize, fsize);
        m_mobilelbl->setObjectName("onlyme");
        m_mobilelbl->setProperty("kind", "mobile");
        setMobileState("grey");
    }

    if(! ui->agentnumber().isEmpty()) {
        m_agentlbl = new QLabel(this);
        m_agentlbl->setPixmap(m_agents["grey"]);
        m_agentlbl->setAlignment(Qt::AlignCenter);
        m_agentlbl->setMinimumSize(fsize, fsize);
        m_agentlbl->setObjectName("onlyme");
        m_agentlbl->setToolTip(tr("Agent %1").arg(ui->agentnumber()));
        m_agentlbl->setProperty("kind", "agent");
        //setColorAvail("agent", "grey", "");
        setAgentState("grey");
    }
    
    // Put the Labels into layouts
    int linenum = 0;
    layout->addWidget( qhline1, linenum, 0, 1, 10);
    // layout->setColumnStretch( 1, 1 );
    // layout->setColumnStretch( 8, 1 );
    linenum ++;
    layout->addWidget( qvline1,   linenum, 0, 2, 1 );
    layout->addWidget( m_textlbl, linenum, 2, 1, 6, alignment );
    layout->addWidget( qvline2,   linenum, 9, 2, 1 );
    
    linenum ++;
    int colnum = 2;
    foreach (QString phone, ui->phonelist())
    {
        layout->addWidget( m_lblphones[phone],
                           linenum, colnum++,
                           Qt::AlignCenter );
    }
    if(! ui->mobilenumber().isEmpty())
        layout->addWidget( m_mobilelbl, linenum, colnum++, Qt::AlignCenter );
    if(! ui->ctilogin().isEmpty())
        layout->addWidget( m_availlbl, linenum, colnum++, Qt::AlignCenter );
    if(! ui->agentnumber().isEmpty())
        layout->addWidget( m_agentlbl, linenum, colnum++, Qt::AlignCenter );
    if(stretch_last)
        layout->setColumnStretch(20, 1);
    
    linenum ++;
    layout->addWidget( qhline2, linenum, 0, 1, 10);

    reloadSavedName();
}

/*! \brief destructor
 */
PeerWidget::~PeerWidget()
{
    //qDebug() << "PeerWidget::~PeerWidget()";
}

void PeerWidget::setAgentState(const QString & color)
{
    if(m_agentlbl) {
        QString qss =  QString("QLabel#onlyme {border-style: solid; border-bottom-width: 3px; border-color: %1; }").arg(color);
        m_agentlbl->setStyleSheet( qss );
    }
}

void PeerWidget::setMobileState(const QString & color)
{
    if(m_mobilelbl) {
        QString qss = QString("QLabel#onlyme {border-style: solid; border-bottom-width: 3px; border-color: %1; }").arg(color);
        m_mobilelbl->setStyleSheet( qss );
        m_mobilelbl->setToolTip(tr("Mobile number : %1").arg(m_ui->mobilenumber()));
    }
}

void PeerWidget::updatePresence()
{
    if(m_availlbl)
    {
        QString qss = QString("QLabel#onlyme {border-style: solid; border-bottom-width: 3px; border-color: %1; }").arg(m_ui->availstate()["color"]);
        m_availlbl->setStyleSheet( qss );
        m_availlbl->setToolTip( tr("User : %1").arg(m_ui->availstate()["longname"]) );
    }
}

void PeerWidget::updatePhonesStates()
{
    //qDebug() << "PeerWidget::updatePhonesStates()" << m_ui->phonelist();
    foreach(QString phone, m_ui->phonelist())
    {
        const PhoneInfo * pi = m_ui->getPhoneInfo(phone);
        if(pi)
        {
            QString qss = QString("QLabel#onlyme {border-style: solid; border-bottom-width: 3px; border-color: %1; }").arg(pi->hintstatus("color"));
            m_lblphones[phone]->setStyleSheet( qss );
            m_lblphones[phone]->setToolTip( tr("Phone %1 : %2")
                                            .arg(pi->number())
                                            .arg(pi->hintstatus("longname")) );
        }
    }
}

void PeerWidget::setAgentToolTip(const QString & agentnum,
                                 const QStringList & queues)
{
    if(! m_agentlbl)
        return;
    if(agentnum == "")
        m_agentlbl->setToolTip("");
    else {
        m_agentlbl->setToolTip( tr("Agent %1\n%n Queue(s) %2", "",
                                queues.size()).arg(agentnum).arg(queues.join(",")) );
    }
}

/*! \brief change displayed name
 */
void PeerWidget::setName(const QString & name)
{
    Q_UNUSED(name)
    //m_ui->setFullName(name);
    QString text = tr("(No callerid yet)");
    if(!m_ui->fullname().isEmpty() && !m_ui->phonenumber().isEmpty())
        text = tr("%1 <%2>").arg(m_ui->fullname()).arg(m_ui->phonenumber());
    else if(!m_ui->fullname().isEmpty())
        text = m_ui->fullname();
    else if(!m_ui->phonenumber().isEmpty())
        text = m_ui->phonenumber();
    m_textlbl->setText(text);
}


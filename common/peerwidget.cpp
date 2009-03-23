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
#include "extendedlabel.h"
#include "peerwidget.h"
#include "xivoconsts.h"
#include "userinfo.h"
#include "phoneinfo.h"

// Initialize static members
QHash<QString, QPixmap> PeerWidget::m_phones = QHash<QString, QPixmap>();
QHash<QString, QPixmap> PeerWidget::m_persons = QHash<QString, QPixmap>();
QHash<QString, QPixmap> PeerWidget::m_agents = QHash<QString, QPixmap>();

/*! \brief Constructor
 */
PeerWidget::PeerWidget(BaseEngine * engine, UserInfo * ui)
    : BasePeerWidget(engine, ui), m_availlbl(NULL), m_agentlbl(NULL)
{
    // fill m_persons, m_phones and m_agents if needed.
    if(m_persons.count() == 0)
    {
        QStringList colors = (QStringList() << "red" << "blue" << "green" << "grey" << "orange" << "yellow");
        foreach(QString color, colors) {
            m_persons[color] = QPixmap(":/images/personal-" + color + ".png");
            m_phones[color] = QPixmap(":/images/phone-" + color + ".png");
            m_agents[color] = QPixmap(":/images/agent-" + color + ".png");
        }
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
    m_textlbl = new QLabel(m_ui->fullname().isEmpty() ? tr("(No callerid yet)") : m_ui->fullname(),
                           this);
    // set TextInteraction Flags so the mouse clicks are not catched by the QLabel widget
    m_textlbl->setTextInteractionFlags( Qt::NoTextInteraction );
    
    if(! m_ui->ctilogin().isEmpty()) {
        m_availlbl = new ExtendedLabel();
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
        m_lblphones[phone] = new ExtendedLabel();
        m_lblphones[phone]->setPixmap(m_phones["grey"]);
        m_lblphones[phone]->setAlignment(Qt::AlignCenter);
        m_lblphones[phone]->setObjectName("onlyme");
        m_lblphones[phone]->setMinimumSize(fsize, fsize);
        m_lblphones[phone]->setProperty("kind", "term");
    }
        
    if(! ui->agentid().isEmpty()) {
        m_agentlbl = new ExtendedLabel();
        m_agentlbl->setPixmap(m_agents["grey"]);
        m_agentlbl->setAlignment(Qt::AlignCenter);
        m_agentlbl->setMinimumSize(fsize, fsize);
        m_agentlbl->setObjectName("onlyme");
        m_agentlbl->setToolTip(tr("Agent ") + ui->agentnumber());
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
    if(! ui->ctilogin().isEmpty())
        layout->addWidget( m_availlbl, linenum, colnum++, Qt::AlignCenter );
    if(! ui->agentnumber().isEmpty())
        layout->addWidget( m_agentlbl, linenum, colnum++, Qt::AlignCenter );
    if(stretch_last)
        layout->setColumnStretch(20, 1);
    
    linenum ++;
    layout->addWidget( qhline2, linenum, 0, 1, 10);
}

/*! \brief destructor
 */
PeerWidget::~PeerWidget()
{
    //qDebug() << "PeerWidget::~PeerWidget()";
}

void PeerWidget::setAgentState(const QString & color)
{
    QString qss =  "QLabel#onlyme {border-style: solid; border-bottom-width: 3px; border-color: " + color + "; }";
    if(m_agentlbl)
        m_agentlbl->setStyleSheet( qss );
}

void PeerWidget::updatePresence()
{
    if(m_availlbl)
    {
        QString qss = "QLabel#onlyme {border-style: solid; border-bottom-width: 3px; border-color: " + m_ui->availstate()["color"] + "; }";
        m_availlbl->setStyleSheet( qss );
        m_availlbl->setToolTip( tr("User : ") + m_ui->availstate()["longname"] );
    }
}

void PeerWidget::updatePhonesStates()
{
    //qDebug() << "PeerWidget::updatePhonesStates()";
    foreach(QString phone, m_ui->phonelist())
    {
        const PhoneInfo * pi = m_ui->getPhoneInfo(phone);
        if(pi)
        {
            QString qss = "QLabel#onlyme {border-style: solid; border-bottom-width: 3px; border-color: " + pi->hintstatus("color") + "; }";
            m_lblphones[phone]->setStyleSheet( qss );
            m_lblphones[phone]->setToolTip( tr("Phone ") + pi->number() + " : " +  pi->hintstatus("longname") );
        }
    }
}

void PeerWidget::setAgentToolTip(const QString & agentnum, const QStringList & queues)
{
    if(! m_agentlbl)
        return;
    if(agentnum == "")
        m_agentlbl->setToolTip("");
    else {
#if 0
        if(queues.size() == 0)
            m_agentlbl->setToolTip(tr("Agent ") + agentnum + "\n0 Queue");
        else if (queues.size() == 1)
            m_agentlbl->setToolTip(tr("Agent ") + agentnum + "\n1 Queue " + queues[0]);
        else
            m_agentlbl->setToolTip(tr("Agent ") + agentnum + "\n" + QString::number(queues.size()) + " Queues " + queues.join(","));
#endif
        m_agentlbl->setToolTip( tr("Agent %1\n%n Queue(s) %2", "", queues.size()).arg(agentnum).arg(queues.join(",")) );
    }
}

/*! \brief change displayed name
 */
void PeerWidget::setName(const QString & name)
{
    m_ui->setFullName(name);
    m_textlbl->setText(m_ui->fullname());
}


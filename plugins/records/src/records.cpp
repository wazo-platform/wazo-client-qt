/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

#include "records.h"
#include "etvng.h"

Q_EXPORT_PLUGIN2(xletrecordsplugin, XLetRecordsPlugin);

XLet* XLetRecordsPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/records_%1");
    return new XletRecords(parent);
}

QHash<QString, QVariantMap> g_results;

XletRecords::XletRecords(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Records Management"));

    m_xletLayout = new QVBoxLayout(this);
    m_xletLayout->setSpacing(0);

    m_titlewidget = new QLabel(this);
    m_xletLayout->addWidget(m_titlewidget);
    m_titlewidget->setText("title zone");
    m_titlewidget->setAlignment(Qt::AlignHCenter);

    m_qhline1 = new QFrame(this);
    m_qhline1->setFrameShape(QFrame::HLine);
    m_xletLayout->addWidget(m_qhline1);

    m_searchwidget = new SearchWidget(this);
    m_xletLayout->addWidget(m_searchwidget);

    m_qhline2 = new QFrame(this);
    m_qhline2->setFrameShape(QFrame::HLine);
    m_xletLayout->addWidget(m_qhline2);

    m_resultswidget = new ResultsWidget(this);
    m_xletLayout->addWidget(m_resultswidget);
    connect(this, SIGNAL(update()),
            m_resultswidget, SLOT(update()));

    ETVListProperties * elp = new ETVListProperties();
    elp->addProperty(tr("ID"), "id", QVariant::Int, "id");
    elp->addProperty(tr("Start Date"), "callstart", QVariant::DateTime, "id");
    elp->addProperty(tr("Duration"), "callduration", QVariant::Int, "id");
    elp->addProperty(tr("CallerIdNum"), "calleridnum", QVariant::String, "id");
    elp->addProperty(tr("Queues"), "queuenames", QVariant::String, "id");
    elp->addProperty(tr("Agents"), "agentnames", QVariant::String,   "id");
    elp->addProperty(tr("Records"), "recordstatus", QVariant::String, "id");
    elp->addProperty(tr("Actions"), "in", QVariant::String, "id");
    ETVListWidget * el = new ETVListWidget(elp, this);
    m_xletLayout->addWidget(el);

    // ( "channel" ,  QVariant(QString, "SIP/hpueygdepcqbew-0000009e") )
    // ) ( "uniqueid" ,  QVariant(QString, "1291592887.368") ) )  

    // m_xletLayout->insertStretch(-1, 1);

    b_engine->registerClassEvent("records-campaign",
                                 XletRecords::recordResults_t, this);
}

XletRecords::~XletRecords()
{
    qDebug() << Q_FUNC_INFO;
    g_results.clear();
}

void XletRecords::clientrequest()
{
    QVariantMap command;
    command["class"] = "records-campaign";
    command["function"] = "search";
    b_engine->sendJsonCommand(command);
}

void XletRecords::recordResults(const QVariantMap & p)
{
    // qDebug() << Q_FUNC_INFO << p["function"];
    QVariantList qvl = p.value("payload").toList();
    foreach (QVariant r, qvl) {
        QString id = r.toMap().value("id").toString();
        g_results[id] = r.toMap();
        // qDebug() << g_results[id];
        b_engine->tree()->populate(QString("records/%1").arg(id), r.toMap());
    }
    emit update();
}

SearchWidget::SearchWidget(QWidget * parent)
    : QWidget(parent)
{
    QHBoxLayout * searchlayout = new QHBoxLayout(this);
    searchlayout->setSpacing(0);

    // fetch last

    QLabel * ll = new QLabel(this);
    QPushButton * add = new QPushButton(this);
    m_researchkind = new QComboBox(this);
    m_searchwidget = new QLineEdit(this);
    QPushButton * remove = new QPushButton(this);
    QPushButton * requestwidget = new QPushButton(this);
    // "clear" button

    searchlayout->addWidget(ll);
    searchlayout->addWidget(add);
    searchlayout->addWidget(m_researchkind);
    searchlayout->addWidget(m_searchwidget);
    searchlayout->addWidget(remove);
    searchlayout->addWidget(requestwidget);

    ll->setText("filter");
    m_researchkind->addItem(tr("Agent"));
    m_researchkind->addItem(tr("Queue"));
    m_researchkind->addItem(tr("Skill"));
    m_researchkind->addItem(tr("Direction"));
    m_searchwidget->setText("search zone");
    remove->setText("-");
    add->setText("+");
    requestwidget->setText("lookup");

    connect(requestwidget, SIGNAL(clicked()),
            this, SLOT(lookup()));
}

SearchWidget::~SearchWidget()
{
    qDebug() << Q_FUNC_INFO;
}

void SearchWidget::lookup()
{
    qDebug() << Q_FUNC_INFO << m_searchwidget->text() << m_researchkind->currentIndex();
    QVariantMap command;
    command["class"] = "records-campaign";
    command["function"] = "search";
    b_engine->sendJsonCommand(command);
}


ResultsWidget::ResultsWidget(QWidget * parent)
    : QWidget(parent)
{
    QVBoxLayout * resultslayout = new QVBoxLayout(this);
    resultslayout->setSpacing(0);

    QHBoxLayout * summarylayout = new QHBoxLayout(this);
    summarylayout->setSpacing(0);

    m_summary = new QLabel(this);
    summarylayout->addWidget(m_summary);
    summarylayout->addStretch(true);
    QLabel * resperpage = new QLabel(this);
    summarylayout->addWidget(resperpage);
    resperpage->setText("N results / page");
    QPushButton * prev = new QPushButton(this);
    summarylayout->addWidget(prev);
    prev->setText("<");
    QPushButton * next = new QPushButton(this);
    summarylayout->addWidget(next);
    next->setText(">");

    resultslayout->addLayout(summarylayout);

    update();
}

ResultsWidget::~ResultsWidget()
{
    qDebug() << Q_FUNC_INFO;
}

void ResultsWidget::update()
{
    int nresults = g_results.count();
    m_summary->setText(QString("%1 results").arg(nresults));
}


// QString id = this->sender()->property("id").toString();
// QString action = this->sender()->property("action").toString();

// QVariantMap command;
// command["class"] = "records-campaign";
// command["function"] = action;
// command["id"] = id;
// b_engine->sendJsonCommand(command);

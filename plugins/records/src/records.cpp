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
#include "commontable.h"

Q_EXPORT_PLUGIN2(xletrecordsplugin, XLetRecordsPlugin);

XLet* XLetRecordsPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/records_%1");
    return new XletRecords(parent);
}

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

    CommonTableProperties * ctp = new CommonTableProperties("records");
    // last item : should define : editable or not, in tooltip or not, hidden or not ...
    ctp->addColumn(tr("ID"), "id", QVariant::Int, "id");
    ctp->addColumn(tr("Start Date"), "callstart", QVariant::DateTime, "id");
    ctp->addColumn(tr("Filename"), "filename", QVariant::String, "id");
    ctp->addColumn(tr("Stop Date"), "callstop", QVariant::DateTime, "id");
    ctp->addColumn(tr("Duration"), "callduration", QVariant::Int, "id");
    ctp->addColumn(tr("Direction"), "direction", QVariant::String, "id");
    ctp->addColumn(tr("CallerIdNum"), "calleridnum", QVariant::String, "id");
    ctp->addColumn(tr("Queues"), "queuenames", QVariant::String, "id");
    ctp->addColumn(tr("Agents"), "agentnames", QVariant::String, "id");
    ctp->addColumn(tr("RecStatus"), "recordstatus", QVariant::String, "id");
    ctp->addColumn(tr("Tag"), "callrecordtag", QVariant::String, "id");
    ctp->addColumn(tr("Comment"), "callrecordcomment", QVariant::String, "edit");
    ctp->addColumn(tr("SVI e"), "svientries", QVariant::String, "id");
    ctp->addColumn(tr("SVI v"), "svivariables", QVariant::String, "id");
    ctp->addColumn(tr("SVI c"), "svichoices", QVariant::String, "id");
    CommonTableWidget * el = new CommonTableWidget(ctp, this);
    m_xletLayout->addWidget(el);

    // m_xletLayout->insertStretch(-1, 1);

    b_engine->registerClassEvent("records-campaign",
                                 XletRecords::recordResults_t, this);
}

XletRecords::~XletRecords()
{
    qDebug() << Q_FUNC_INFO;
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
        b_engine->tree()->populate(QString("records/%1").arg(id), r.toMap());
    }
    emit update();
}

void XletRecords::mousePressEvent(QMouseEvent * event)
{
    m_lastPressed = event->button();
}

void XletRecords::onViewClick(const QModelIndex & modelindex)
{
    int row = modelindex.row();
    int column = modelindex.column();

    QString id = modelindex.sibling(row, 0).data().toString();
    QString startdate = modelindex.sibling(row, 1).data().toString();
    QString xx = modelindex.sibling(row, 5).data().toString();

    // if (m_lastPressed & Qt::LeftButton)
    if (m_lastPressed & Qt::RightButton) {
        if (column != 5) {
            QMenu * menu = new QMenu(this);
            QAction * action1 = new QAction(tr("Remove call %1 (%2)")
                                            .arg(xx).arg(startdate), menu);
            QAction * action2 = new QAction(tr("Change comment"), menu);
            action1->setProperty("id", id);
            action2->setProperty("id", id);
            //     connect(action, SIGNAL(triggered(bool)),
            //             parentWidget(), SLOT(openConfRoom()));
            //     connect(action, SIGNAL(triggered(bool)),
            //             parentWidget(), SLOT(phoneConfRoom()));
            menu->addAction(action1);
            menu->addAction(action2);
            menu->exec(QCursor::pos());
            delete action1;
            delete action2;
            delete menu;
        }
    }
}

void XletRecords::onViewDoubleClick(const QModelIndex & modelindex)
{
    int row = modelindex.row();
    int column = modelindex.column();
    QString id = modelindex.sibling(row, 0).data().toString();
    QString startdate = modelindex.sibling(row, 1).data().toString();
    QString xx = modelindex.sibling(row, 5).data().toString();

    if (m_lastPressed & Qt::LeftButton) {
        QMenu * menu = new QMenu(this);
        QAction * action1 = new QAction(tr("Tzzzz call %1 (%2)")
                                        .arg(xx).arg(startdate), menu);
        QAction * action2 = new QAction(tr("Change comment"), menu);
        action1->setProperty("id", id);
        action2->setProperty("id", id);
        //     connect(action, SIGNAL(triggered(bool)),
        //             parentWidget(), SLOT(openConfRoom()));
        //     connect(action, SIGNAL(triggered(bool)),
        //             parentWidget(), SLOT(phoneConfRoom()));
        menu->addAction(action1);
        menu->addAction(action2);
        menu->exec(QCursor::pos());
        delete action1;
        delete action2;
        delete menu;
    }
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
    m_searchwidget->setText("");
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

    QVariantList searchitems;
    QVariantMap searchitem;
    searchitem["searchkind"] = m_researchkind->currentIndex();
    searchitem["searchfield"] = m_searchwidget->text();
    searchitems << searchitem;
    command["searchitems"] = searchitems;
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
    m_summary->setText(QString("%1 results").arg(666));
}


// QString id = this->sender()->property("id").toString();
// QString action = this->sender()->property("action").toString();

// QVariantMap command;
// command["class"] = "records-campaign";
// command["function"] = action;
// command["id"] = id;
// b_engine->sendJsonCommand(command);

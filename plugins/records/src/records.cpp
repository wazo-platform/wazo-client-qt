/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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
    // m_xletLayout->setSpacing(0);

    m_searchwidget = new SearchWidget(this);
    m_xletLayout->addWidget(m_searchwidget);

    m_qhline = new QFrame(this);
    m_qhline->setFrameShape(QFrame::HLine);
    m_xletLayout->addWidget(m_qhline);

    m_resultswidget = new ResultsWidget(this);
    m_xletLayout->addWidget(m_resultswidget);
    connect(this, SIGNAL(update(int)),
            m_resultswidget, SLOT(update(int)));

    m_ctp = new CommonTableProperties("records");
    // last item : should define : editable or not, in tooltip or not, hidden or not ...
    m_ctp->addColumn(tr("ID"), "id", QVariant::String, "id");
    m_ctp->addColumn(tr("Start Date"), "callstart", QVariant::DateTime, "id");
    m_ctp->addColumn(tr("Stop Date"), "callstop", QVariant::DateTime, "id");
    m_ctp->addColumn(tr("Filename"), "filename", QVariant::String, "id");
    m_ctp->addColumn(tr("Duration"), "callduration", QVariant::Int, "id");
    m_ctp->addColumn(tr("Direction"), "direction", QVariant::String, "id");
    m_ctp->addColumn(tr("CallerIdNum"), "calleridnum", QVariant::String, "id");
    m_ctp->addColumn(tr("Queues"), "queuenames", QVariant::String, "id");
    m_ctp->addColumn(tr("Agents"), "agentnames", QVariant::String, "id");
    // m_ctp->addColumn(tr("RecStatus"), "recordstatus", QVariant::String, "id");
    m_ctp->addColumn(tr("Tag"), "callrecordtag", QVariant::String, "id");
    m_ctp->addColumn(tr("Comment"), "callrecordcomment", QVariant::String, "edit");
    m_ctp->addColumn(tr("SVI e"), "svientries", QVariant::String, "id");
    m_ctp->addColumn(tr("SVI v"), "svivariables", QVariant::String, "id");
    m_ctp->addColumn(tr("SVI c"), "svichoices", QVariant::String, "id");
    m_ctp->setTooltipCallBack(XletRecords::tooltip_t, this);
    CommonTableWidget * el = new CommonTableWidget(m_ctp, this);
    m_xletLayout->addWidget(el);

    // m_xletLayout->insertStretch(-1, 1);
    b_engine->registerClassEvent("records-campaign",
                                 XletRecords::recordResults_t, this);
}

XletRecords::~XletRecords()
{
    // qDebug() << Q_FUNC_INFO;
}

QString XletRecords::tooltip(const QModelIndex & modelindex)
{
    int row = modelindex.row();
    int column = modelindex.column();
    QString ttip = "";

    if (column < 7)
        ttip = QString("%1\n"
                       "svi entries : %2")
            .arg(modelindex.sibling(row, 2).data().toString())
            .arg(modelindex.sibling(row, 12).data().toString());
    return ttip;
}

void XletRecords::recordResults(const QVariantMap & p)
{
    QString function = p.value("function").toString();
    if (function == "search") {
        QVariantList qvl = p.value("payload").toList();
        b_engine->tree()->rmPath("records");
        QVariantMap qvm;
        foreach (QVariant r, qvl) {
            QString id = r.toMap().value("id").toString();
            qvm[id] = r.toMap();
        }
        b_engine->tree()->populate("records", qvm);
        emit update(qvl.size());
    } else if (function == "getprops") {
        m_tags = p.value("tags").toMap();
        m_tags.remove("notag");
    } else if (function == "tag") {
        QString id = p.value("id").toString();
        QString returncode = p.value("returncode").toString();
        if (returncode == "ok")
            m_searchwidget->Lookup();
    } else if (function == "comment") {
        QString id = p.value("id").toString();
        QString returncode = p.value("returncode").toString();
        if (returncode == "ok")
            m_searchwidget->Lookup();
    } else {
        qDebug() << function << p;
    }
}

void XletRecords::setDataEdit(const QModelIndex & modelindex,
                              const QVariant & value)
{
    int row = modelindex.row();
    // int column = modelindex.column(); // the column should match with an editable item anyway
    int idcolumn = 0;
    QString truerow = modelindex.sibling(row, idcolumn).data().toString();

    QVariantMap command;
    command["class"] = "records-campaign";
    command["function"] = "comment";
    command["id"] = truerow;
    command["comment"] = value.toString();
    b_engine->sendJsonCommand(command);
}

void XletRecords::mousePressEvent(QMouseEvent * event)
{
    m_lastPressed = event->button();
}

void XletRecords::onViewClick(const QModelIndex & modelindex)
{
    int row = modelindex.row();
    int column = modelindex.column();
    int idcolumn = 0;
    QString id = modelindex.sibling(row, idcolumn).data().toString();
    QString c_eventfield = m_ctp->eventfield(column);

//     for(int y = 2 ; y < 10 ; y++)
//         qDebug() << y << modelindex.sibling(row, y).data() << m_ctp->eventfield(y) << m_ctp->title(y);

    QString callstart = modelindex.sibling(row, 1).data().toString();
//     QString filename = modelindex.sibling(row, 2).data().toString();
//     QString callstop = modelindex.sibling(row, 3).data().toString();
//     QString callduration = modelindex.sibling(row, 4).data().toString();
    QString direction = modelindex.sibling(row, 5).data().toString();
//     QString calleridnum = modelindex.sibling(row, 6).data().toString();
//     QString queuenames = modelindex.sibling(row, 7).data().toString();
//     QString agentnames = modelindex.sibling(row, 8).data().toString();
//     QString recordstatus = modelindex.sibling(row, 9).data().toString();
    QString callrecordtag = modelindex.sibling(row, 10).data().toString();

    // if (m_lastPressed & Qt::LeftButton)
    if (m_lastPressed & Qt::RightButton) {
        if (c_eventfield == "callrecordtag") {
            QMenu * menu = new QMenu(this);
            QAction * actionm = new QAction(tr("Change tag to"), menu);
            menu->addAction(actionm);
            QVariantMap validtags = m_tags;
            validtags.remove(callrecordtag);
            foreach (QString ti, validtags.keys()) {
                QString itemname = validtags[ti].toMap().value("label").toString();
                QAction * action = new QAction(tr("%1").arg(itemname), menu);
                action->setProperty("id", id);
                action->setProperty("tag", ti);
                menu->addAction(action);
                connect(action, SIGNAL(triggered()),
                        this, SLOT(changeTag()) );
            }
            menu->exec(QCursor::pos());
            // delete action;
            delete menu;

        } else if (c_eventfield != "queuenames") {
            QMenu * menu = new QMenu(this);
            QAction * action1 = new QAction(tr("Remove call %1 (%2)")
                                            .arg(direction).arg(callstart), menu);
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
    QString c_eventfield = m_ctp->eventfield(column);

    QString startdate = modelindex.sibling(row, 1).data().toString();
    QString xx = modelindex.sibling(row, 5).data().toString();

    if (m_lastPressed & Qt::LeftButton) {
        if (c_eventfield != "callrecordcomment") {
            QMenu * menu = new QMenu(this);
            QAction * action1 = new QAction(tr("Tzzzz call %1 (%2)")
                                            .arg(xx).arg(startdate), menu);
            QAction * action2 = new QAction(tr("Change comment"), menu);
            QAction * action3 = new QAction(tr("Change tag"), menu);
            action1->setProperty("id", id);
            action2->setProperty("id", id);
            action2->setProperty("action", "changecomment");
            action3->setProperty("id", id);
            action3->setProperty("action", "changetag");
            //     connect(action, SIGNAL(triggered(bool)),
            //             parentWidget(), SLOT(openConfRoom()));
            //     connect(action, SIGNAL(triggered(bool)),
            //             parentWidget(), SLOT(phoneConfRoom()));
            menu->addAction(action1);
            menu->addAction(action2);
            menu->addAction(action3);
            menu->exec(QCursor::pos());
            delete action1;
            delete action2;
            delete action3;
            delete menu;
        }
    }
}

void XletRecords::changeTag()
{
    QVariantMap command;
    command["class"] = "records-campaign";
    command["function"] = "tag";
    command["id"] = sender()->property("id").toString();
    command["tag"] = sender()->property("tag").toString();
    b_engine->sendJsonCommand(command);
}

SearchWidget::SearchWidget(QWidget * parent)
    : QWidget(parent)
{
    m_searchlayout = new QGridLayout(this);
    // m_searchlayout->setSpacing(0);
    // m_searchlayout->setMargin(0);

    // fetch last

    QLabel * filterstitle = new QLabel(this);
    QPushButton * addbutton = new QPushButton(this);
    QPushButton * requestwidget = new QPushButton(this);
    // "clear" button

    m_nfilterlines = 0;
    AddSearchField();

    m_searchlayout->addWidget(filterstitle, 0, 0);
    m_searchlayout->addWidget(addbutton, 0, 1);
    m_searchlayout->addWidget(requestwidget, 0, 5);

    filterstitle->setText(tr("Search Filter(s)"));
    addbutton->setIcon(QIcon(":/images/add.png"));
    requestwidget->setText(tr("Lookup"));

    connect(requestwidget, SIGNAL(clicked()),
            this, SLOT(Lookup()));
    connect(addbutton, SIGNAL(clicked()),
            this, SLOT(AddSearchField()));
}





SearchWidget::~SearchWidget()
{
    qDebug() << Q_FUNC_INFO;
}

void SearchWidget::DrawSearchFields()
{
    for(int z = 0 ; z < m_nfilterlines ; z++) {
        m_searchlayout->addWidget(m_researchkind[z], z, 2);
        m_searchlayout->addWidget(m_searchwidget[z], z, 3);
        m_searchlayout->addWidget(m_removebutton[z], z, 4);
        m_removebutton[z]->setProperty("linenumber", z);
    }
}

void SearchWidget::AddSearchField()
{
    if (m_nfilterlines < 10) {
        int linenumber = m_nfilterlines;
        QComboBox * qc = new QComboBox(this);
        QLineEdit * ql = new QLineEdit(this);
        QPushButton * qp = new QPushButton(this);
        m_researchkind << qc;
        m_searchwidget << ql;
        m_removebutton << qp;

        m_researchkind[linenumber]->addItem(tr("Agent"));
        m_researchkind[linenumber]->addItem(tr("Queue"));
        m_researchkind[linenumber]->addItem(tr("Skill"));
        m_researchkind[linenumber]->addItem(tr("Direction"));
        m_searchwidget[linenumber]->setText("");
        m_removebutton[linenumber]->setText("-");

        connect(m_removebutton[linenumber], SIGNAL(clicked()),
                this, SLOT(RemoveSearchField()));
        connect(m_searchwidget[linenumber], SIGNAL(returnPressed()),
                this, SLOT(Lookup()));

        m_nfilterlines ++;
        DrawSearchFields();
    }
}

void SearchWidget::RemoveSearchField()
{
    if (m_nfilterlines > 1) {
        int linenumber = sender()->property("linenumber").toInt();
        disconnect(m_removebutton[linenumber], SIGNAL(clicked()),
                   this, SLOT(RemoveSearchField()));
        disconnect(m_searchwidget[linenumber], SIGNAL(returnPressed()),
                   this, SLOT(Lookup()));
        delete m_researchkind[linenumber];
        delete m_searchwidget[linenumber];
        delete m_removebutton[linenumber];
        m_researchkind.removeAt(linenumber);
        m_searchwidget.removeAt(linenumber);
        m_removebutton.removeAt(linenumber);
        m_nfilterlines --;
        DrawSearchFields();
    }
}

void SearchWidget::Lookup()
{
    // qDebug() << Q_FUNC_INFO << sender();

    QVariantMap command;
    command["class"] = "records-campaign";
    command["function"] = "search";

    QVariantList searchitems;
    for(int z = 0 ; z < m_nfilterlines ; z ++) {
        QVariantMap searchitem;
        searchitem["searchkind"] = m_researchkind[z]->currentIndex();
        searchitem["searchfield"] = m_searchwidget[z]->text();
        searchitems << searchitem;
    }
    command["searchitems"] = searchitems;
    b_engine->sendJsonCommand(command);

    command.clear();
    command["class"] = "records-campaign";
    command["function"] = "getprops";
    b_engine->sendJsonCommand(command);
}


ResultsWidget::ResultsWidget(QWidget * parent)
    : QWidget(parent)
{
    QVBoxLayout * resultslayout = new QVBoxLayout(this);
    // resultslayout->setSpacing(0);

    QHBoxLayout * summarylayout = new QHBoxLayout(this);
    // summarylayout->setSpacing(0);

    m_summary = new QLabel(this);
    summarylayout->addWidget(m_summary);
    summarylayout->addStretch(true);

    resultslayout->addLayout(summarylayout);

    update(0);
}

ResultsWidget::~ResultsWidget()
{
    // qDebug() << Q_FUNC_INFO;
}

void ResultsWidget::update(int nresults)
{
    // qDebug() << Q_FUNC_INFO;
    m_summary->setText(tr("Results : %1 found").arg(nresults));
}

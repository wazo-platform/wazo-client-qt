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
    m_ctwidget = new CommonTableWidget(m_ctp, this);
    m_xletLayout->addWidget(m_ctwidget);

    // m_xletLayout->insertStretch(-1, 1);
    registerListener("records_campaign");
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

    QString fullfilename = modelindex.sibling(row, m_ctp->revindex("filename")).data().toString();
    QFileInfo pathInfo(fullfilename);
    QString pathname = pathInfo.path();
    QString filename = pathInfo.fileName();

    ttip = tr("call start : %1 (duration %2 s, direction %3)\n"
              "path : %4\n"
              "filename : %5\n"
              "current tag : %6\n"
              "svi entries : %7\n"
              "svi variables : %8\n"
              "(this column : %9)")
        .arg(modelindex.sibling(row, m_ctp->revindex("callstart")).data().toString())
        .arg(modelindex.sibling(row, m_ctp->revindex("callduration")).data().toString())
        .arg(modelindex.sibling(row, m_ctp->revindex("direction")).data().toString())
        .arg(pathname)
        .arg(filename)
        .arg(modelindex.sibling(row, m_ctp->revindex("callrecordtag")).data().toString())
        .arg(modelindex.sibling(row, m_ctp->revindex("svientries")).data().toString())
        .arg(modelindex.sibling(row, m_ctp->revindex("svivariables")).data().toString())
        .arg(m_ctp->eventfield(column));
    return ttip;
}

void XletRecords::parseCommand(const QVariantMap &map) {
    recordResults(map);
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
        foreach (QString ti, m_tags.keys()) {
            QString itemname = m_tags[ti].toMap().value("label").toString();
            m_ctp->setMatches("callrecordtag", ti, itemname);
        }
    } else if (function == "tag") {
        QString id = p.value("id").toString();
        QString returncode = p.value("returncode").toString();
        if (returncode == "ok")
            m_searchwidget->Lookup();
        else
            QMessageBox::warning(NULL, tr("Tag"),
                                 tr("You attempted to change tag for id %1.\n"
                                    "The return code is %2.").arg(id).arg(returncode));
    } else if (function == "comment") {
        QString id = p.value("id").toString();
        QString returncode = p.value("returncode").toString();
        if (returncode == "ok")
            m_searchwidget->Lookup();
        else
            QMessageBox::warning(NULL, tr("Comment"),
                                 tr("You attempted to change tag for id %1.\n"
                                    "The return code is %2.").arg(id).arg(returncode));
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
    command["class"] = "records_campaign";
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
    int idcolumn = 0;
    if (m_ctwidget->currentSelection().count() > 1) {
        // multiline selection
        foreach (QModelIndex qmi, m_ctwidget->currentSelection()) {
            QString id = qmi.sibling(qmi.row(), idcolumn).data().toString();
        }
        return;
    }

    QString c_eventfield = m_ctp->eventfield(modelindex.column());

    if (m_lastPressed & Qt::RightButton) {
        if (c_eventfield == "callrecordtag") {
            QMenu * menu = new QMenu(this);
            menu->addAction(tr("Change tag to"));
            menu->addSeparator();
            showAvailableTags(modelindex, menu);
            menu->exec(QCursor::pos());
            delete menu;
        } else {
            commonMenuDisplay(modelindex);
        }
    }
}

void XletRecords::showAvailableTags(const QModelIndex & modelindex, QMenu * menu)
{
    int row = modelindex.row();
    int idcolumn = 0;
    QString id = modelindex.sibling(row, idcolumn).data().toString();

    QString callrecordtag = modelindex.sibling(row, m_ctp->revindex("callrecordtag")).data().toString();
    foreach (QString ti, m_tags.keys()) {
        QString taglabel = m_tags[ti].toMap().value("label").toString();
        if(callrecordtag != taglabel) {
            QAction * action = new QAction(tr("%1").arg(taglabel), menu);
            action->setProperty("id", id);
            action->setProperty("tag", ti);
            menu->addAction(action);
            connect(action, SIGNAL(triggered()),
                    this, SLOT(changeTag()) );
        }
    }
    // XXX delete + disconnect action;
}

void XletRecords::commonMenuDisplay(const QModelIndex & modelindex)
{
    int row = modelindex.row();
    int idcolumn = 0;
    QString id = modelindex.sibling(row, idcolumn).data().toString();

    QString callstart = modelindex.sibling(row, m_ctp->revindex("callstart")).data().toString();
    QString fullfilename = modelindex.sibling(row, m_ctp->revindex("filename")).data().toString();
    QString idv = modelindex.sibling(row, m_ctp->revindex("id")).data().toString();

    QFileInfo pathInfo(fullfilename);
    QString filename = pathInfo.fileName();

    QMenu * menu = new QMenu(this);
    // QAction * action2 = new QAction(tr("Change comment"), menu);
    QMenu * submenu_tag = new QMenu(tr("Change tag"), menu);
    submenu_tag->addAction(tr("Change tag to"));
    submenu_tag->addSeparator();
    showAvailableTags(modelindex, submenu_tag);

    // action2->setProperty("id", id);
    // action2->setProperty("action", "changecomment");

    menu->addAction(tr("Call starting on %1 (id %2)").arg(callstart).arg(idv));
    menu->addAction(tr("(%1)").arg(filename));
    menu->addSeparator();
    // menu->addAction(action2);
    menu->addMenu(submenu_tag);

    menu->exec(QCursor::pos());

    delete submenu_tag;
    // delete action2;
    delete menu;
}

void XletRecords::onViewDoubleClick(const QModelIndex & modelindex)
{
    QString c_eventfield = m_ctp->eventfield(modelindex.column());

    if (m_lastPressed & Qt::LeftButton) {
        if (c_eventfield != "callrecordcomment")
            commonMenuDisplay(modelindex);
    }
}

void XletRecords::changeTag()
{
    QVariantMap command;
    command["class"] = "records_campaign";
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

    m_filterstitle = new QLabel(this);
    m_addbutton = new QPushButton(this);
    m_andorbutton = new QPushButton(this);
    m_requestbutton = new QPushButton(this);
    // "clear" button

    m_nfilterlines = 0;
    AddSearchField();

    m_searchlayout->addWidget(m_filterstitle, 0, 0);
    m_searchlayout->addWidget(m_addbutton, 0, 1);
    m_searchlayout->addWidget(m_andorbutton, 1, 1);
    m_searchlayout->addWidget(m_requestbutton, 0, 5);

    m_filterstitle->setText(tr("Search Filter(s)"));
    m_addbutton->setIcon(QIcon(":/images/add.png"));
    m_andorbutton->setFont(QFont("", 12));
    m_andorbutton->setText("|");
    m_andorbutton->setProperty("operator", "or");
    m_requestbutton->setText(tr("Lookup"));

    connect(m_requestbutton, SIGNAL(clicked()),
            this, SLOT(Lookup()));
    connect(m_addbutton, SIGNAL(clicked()),
            this, SLOT(AddSearchField()));
    connect(m_andorbutton, SIGNAL(clicked()),
            this, SLOT(SwitchAndOrMode()));
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

void SearchWidget::SwitchAndOrMode()
{
    if(m_andorbutton->text() == "|") {
        m_andorbutton->setText("&&");
        m_andorbutton->setProperty("operator", "and");
    } else {
        m_andorbutton->setText("|");
        m_andorbutton->setProperty("operator", "or");
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
        m_removebutton[linenumber]->setIcon(QIcon(":/images/cancel.png"));

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
    command["class"] = "records_campaign";
    command["function"] = "search";

    QVariantList searchitems;
    for(int z = 0 ; z < m_nfilterlines ; z ++) {
        QVariantMap searchitem;
        searchitem["searchkind"] = m_researchkind[z]->currentIndex();
        searchitem["searchfield"] = m_searchwidget[z]->text();
        searchitems << searchitem;
    }
    command["searchitems"] = searchitems;
    command["searchoperator"] = m_andorbutton->property("operator").toString();
    b_engine->sendJsonCommand(command);

    command.clear();
    command["class"] = "records_campaign";
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

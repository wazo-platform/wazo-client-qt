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

#include "commontable.h"

//
// CommonTableProperties class
//

CommonTableProperties::CommonTableProperties(const QString & treebase)
{
    m_properties.clear();
    m_properties["display_qss"] = "border: none; color:black;";
    m_properties["display_grid"] = 1;
    m_properties["treebase"] = treebase;
    m_properties["columns"] = "";
}

// to define, per column : "id" for "eV", type (boolean, date/time, phone)
void CommonTableProperties::addColumn(const QString & title,
                                      const QString & eventfield,
                                      const QVariant::Type & qttype,
                                      const QString & xivotype)
{
    QVariantList columns = m_properties.value("columns").toList();
    QVariantMap u;
    u["title"] = title;
    u["eventfield"] = eventfield;
    u["qttype"] = int(qttype);
    u["xivotype"] = xivotype;
    columns << u;
    m_properties["columns"] = columns;
}

int CommonTableProperties::displayOptionShowGrid() const
{
    return m_properties.value("display_grid").toInt();
}

QString CommonTableProperties::displayOptionStyleSheet() const
{
    return m_properties.value("display_qss").toString();
}

int CommonTableProperties::ncolumns() const
{
    return m_properties.value("columns").toList().size();
}

QString CommonTableProperties::treebase() const
{
    return m_properties.value("treebase").toString();
}

QString CommonTableProperties::title(int index) const
{
    return m_properties.value("columns").toList()[index].toMap().value("title").toString();
}

QString CommonTableProperties::eventfield(int index) const
{
    return m_properties.value("columns").toList()[index].toMap().value("eventfield").toString();
}

QVariant::Type CommonTableProperties::qttype(int index) const
{
    return QVariant::Type(m_properties.value("columns").toList()[index].toMap().value("qttype").toInt());
}

QString CommonTableProperties::xivotype(int index) const
{
    return m_properties.value("columns").toList()[index].toMap().value("xivotype").toString();
}

//
// CommonTableModel class
//

CommonTableModel::CommonTableModel(const CommonTableProperties * const qv)
    : QAbstractTableModel()
{
    m_fieldoptions = qv;
    b_engine->tree()->onChange(m_fieldoptions->treebase(),
                               this,
                               SLOT(mylistChange(const QString &,
                                                 DStoreEvent)));
    // startTimer(1000);
}

int CommonTableModel::displayOptionShowGrid() const
{
    return m_fieldoptions->displayOptionShowGrid();
}

QString CommonTableModel::displayOptionStyleSheet() const
{
    return m_fieldoptions->displayOptionStyleSheet();
}

void CommonTableModel::timerEvent(QTimerEvent *)
{
    reset();
}

void CommonTableModel::mylistChange(const QString &, DStoreEvent)
{
    m_myList = b_engine->eVM(m_fieldoptions->treebase());
    int row = 0;
    if (m_myList.size() != m_row2id.size()) {
        foreach(QString myId, m_myList.keys()) {
            m_row2id.insert(row++, myId);
        }
    }
    reset();
}

Qt::ItemFlags CommonTableModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags itemflags;
    // return Qt::NoItemFlags;
    itemflags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if(m_fieldoptions->xivotype(index.column()) == "edit")
        itemflags |= Qt::ItemIsEditable;
    return itemflags;
}

int CommonTableModel::rowCount(const QModelIndex&) const
{
    return m_myList.size();
}

int CommonTableModel::columnCount(const QModelIndex &) const
{
    return m_fieldoptions->ncolumns();
}

bool CommonTableModel::setData(const QModelIndex & modelindex, const QVariant & value, int)
{
    int row = modelindex.row();
    int column = modelindex.column();
    if (m_row2id.contains(row))
        row = m_row2id[row].toInt();
    QString eventfield = m_fieldoptions->eventfield(column);
    QString request = QString("%1/%2/%3").arg(m_fieldoptions->treebase()).arg(row).arg(eventfield);
    b_engine->tree()->populate(request, value);

    QVariantMap command;
    command["class"] = "records-campaign";
    command["function"] = "comment";
    command["id"] = row;
    command["comment"] = value.toString();
    b_engine->sendJsonCommand(command);

    return false;
}

QVariant CommonTableModel::data(const QModelIndex & modelindex, int role) const
{
    QVariant ret = QVariant();
    int row = modelindex.row();
    int column = modelindex.column();
    switch(role) {
    case Qt::TextAlignmentRole:
        ret = Qt::AlignCenter;
        break;
    case Qt::ToolTipRole:
        ret = QString("%1\n"
                      "%2 : %3")
            .arg(modelindex.sibling(row, 2).data().toString())
            .arg("svi entries")
            .arg(modelindex.sibling(row, 12).data().toString());
        break;
    case Qt::EditRole:
        break;
    case Qt::DisplayRole:
    case Qt::UserRole:
        if (m_row2id.contains(row))
            row = m_row2id[row].toInt();

        QString eventfield = m_fieldoptions->eventfield(column);
        QVariant::Type qttype = m_fieldoptions->qttype(column);
        QString request = QString("%1/%2/%3").arg(m_fieldoptions->treebase()).arg(row).arg(eventfield);

        if ((qttype == QVariant::String) || (qttype == QVariant::Int))
            ret = b_engine->eV(request);
        else if (qttype == QVariant::DateTime) {
            uint ii = int(b_engine->eV(request).toDouble());
            QDateTime qdt = QDateTime::fromTime_t(ii);
            if (role == Qt::UserRole)
                ret = ii;
            else
                ret = qdt.toString();
        }
        break;
    }

    return ret;
}

QVariant CommonTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant ret = QVariant();

    switch(role) {
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal)
            ret = m_fieldoptions->title(section);
    }

    return ret;
}

void CommonTableModel::sort(int column, Qt::SortOrder order)
{
    struct {
        static bool ascending(const QPair<int, QString> &a,
                              const QPair<int, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ? true : false;
        }
        static bool descending(const QPair<int, QString> &a,
                               const QPair<int, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ? false : true;
        }
    } sFun;

    QList<QPair<int, QString> > toSort;

    int i, e;
    for (i = 0, e = rowCount(QModelIndex()); i < e; i++) {
        toSort.append(QPair<int, QString>(index(i, 0).data(Qt::UserRole).toInt(),
                                          index(i, column).data(Qt::UserRole).toString()));
    }

    qSort(toSort.begin(), toSort.end(), (order == Qt::AscendingOrder) ?
                                         sFun.ascending :
                                         sFun.descending);

    for (i = 0; i < e; i++) {
        m_row2id.insert(i, QString::number(toSort[i].first));
    }
    reset();
}

//
// CommonTableView class
//

CommonTableView::CommonTableView(QWidget * parent,
                                 XLet * parentxlet,
                                 CommonTableModel * model)
    : QTableView(parent)
{
    setSortingEnabled(true);
    setModel(model);
    setShowGrid(model->displayOptionShowGrid());
    verticalHeader()->hide();
    horizontalHeader()->setResizeMode(QHeaderView::Interactive); // ::Stretch, ::ResizeToContents
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setMovable(true);
    setStyleSheet("CommonTableView {" + model->displayOptionStyleSheet() + "}");
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    // hideColumn(0);

    connect(this, SIGNAL(signalMousePressEvent(QMouseEvent *)),
            parentxlet, SLOT(mousePressEvent(QMouseEvent *)));
    connect(this, SIGNAL(clicked(const QModelIndex &)),
            parentxlet, SLOT(onViewClick(const QModelIndex &)));
    connect(this, SIGNAL(doubleClicked(const QModelIndex &)),
            parentxlet, SLOT(onViewDoubleClick(const QModelIndex &)));
}

void CommonTableView::selectionChanged(const QItemSelection & selected,
                                       const QItemSelection & deselected)
{
    QList<QModelIndex> mil;
    mil.clear();
    int prevrow = -1;
    foreach (QModelIndex qmi, QAbstractItemView::selectedIndexes()) {
        if (qmi.column() == 0) {
            mil.append(qmi);
            prevrow = qmi.row();
        }
    }
    qDebug() << Q_FUNC_INFO << "selected" << mil << mil.count();
    QAbstractItemView::selectionChanged(selected, deselected);
}

void CommonTableView::mousePressEvent(QMouseEvent *event)
{
    emit signalMousePressEvent(event);
    QTableView::mousePressEvent(event);
}

//
// CommonTableWidget class
//

CommonTableWidget::CommonTableWidget(const CommonTableProperties * const qv,
                                     XLet * parentxlet)
    : QWidget()
{
    QVBoxLayout  * vBox = new QVBoxLayout(this);
    QHBoxLayout  * hBox = new QHBoxLayout();
    CommonTableModel * model = new CommonTableModel(qv);
    CommonTableView * view = new CommonTableView(this, parentxlet, model);

    view->setStyleSheet("CommonTableView {" + model->displayOptionStyleSheet() + "}");
    view->verticalHeader()->hide();
    // hBox->addStretch(1);
    hBox->addWidget(view, Qt::AlignJustify);
    // hBox->addStretch(1);

    vBox->addLayout(hBox);
    setLayout(vBox);
}

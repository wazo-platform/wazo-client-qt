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
    m_properties["revindex"] = "";
    m_properties["matches"] = "";
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

    QVariantMap revindex = m_properties.value("revindex").toMap();
    revindex[eventfield] = columns.count() - 1;
    m_properties["revindex"] = revindex;
}

void CommonTableProperties::setMatches(const QString & eventfield,
                                       const QString & key,
                                       const QString & value)
{
    QVariantMap u = m_properties.value("matches").toMap();
    QVariantMap k = u.value(eventfield).toMap();
    k[key] = value;
    u[eventfield] = k;
    m_properties["matches"] = u;
}

bool CommonTableProperties::hasMatchFor(const QString & eventfield) const
{
    return m_properties.value("matches").toMap().contains(eventfield);
}

QString CommonTableProperties::match(const QString & eventfield, const QString & key) const
{
    return m_properties.value("matches").toMap().value(eventfield).toMap().value(key).toString();
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

int CommonTableProperties::revindex(const QString & eventfield) const
{
    return m_properties.value("revindex").toMap().value(eventfield).toInt();
}

QVariant::Type CommonTableProperties::qttype(int index) const
{
    return QVariant::Type(m_properties.value("columns").toList()[index].toMap().value("qttype").toInt());
}

QString CommonTableProperties::xivotype(int index) const
{
    return m_properties.value("columns").toList()[index].toMap().value("xivotype").toString();
}

void CommonTableProperties::setTooltipCallBack(QString (* tooltip_cb)(const QModelIndex &, void *),
                                               void * tooltip_xlet)
{
    m_tooltip_cb = tooltip_cb;
    m_tooltip_xlet = tooltip_xlet;
}

QString CommonTableProperties::tooltip(const QModelIndex & modelindex) const
{
    return m_tooltip_cb(modelindex, m_tooltip_xlet);
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
    m_myList = b_engine->eVM(m_fieldoptions->treebase()).keys();
    m_row2id.clear();
    if (m_myList.size() != m_row2id.size()) {
        int row = 0;
        foreach(QString myId, m_myList) {
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

int CommonTableModel::rowCount(const QModelIndex &) const
{
    return m_myList.size();
}

int CommonTableModel::columnCount(const QModelIndex &) const
{
    return m_fieldoptions->ncolumns();
}

bool CommonTableModel::setData(const QModelIndex & modelindex, const QVariant & value, int /*role*/)
{
    int row = modelindex.row();
    int column = modelindex.column();
    int idcolumn = 0;
    QString truerow = modelindex.sibling(row, idcolumn).data().toString();

    // it is important to make modelindex used before proceeding, since the order will change
    // afterwards
    emit setDataEdit(modelindex, value);

    // these lines are not compulsory for the value to be taken into account,
    // but they make the user interaction a little bit friendlier
    QString eventfield = m_fieldoptions->eventfield(column);
    QString request = QString("%1/%2/%3").arg(m_fieldoptions->treebase()).arg(truerow).arg(eventfield);
    b_engine->tree()->populate(request, value);

    return false;
}

QString CommonTableModel::tooltip(const QModelIndex & modelindex) const
{
    return m_fieldoptions->tooltip(modelindex);
}

QVariant CommonTableModel::data(const QModelIndex & modelindex, int role) const
{
    QVariant ret = QVariant();
    int row = modelindex.row();
    int column = modelindex.column();
    QString truerow = "";

    switch(role) {
    case Qt::TextAlignmentRole:
        ret = Qt::AlignCenter;
        break;
    case Qt::ToolTipRole:
        ret = tooltip(modelindex);
        break;
    case Qt::EditRole:
    case Qt::DisplayRole:
    case Qt::UserRole:
        if (m_row2id.contains(row))
            truerow = m_row2id[row];

        QString eventfield = m_fieldoptions->eventfield(column);
        QVariant::Type qttype = m_fieldoptions->qttype(column);
        QString request = QString("%1/%2/%3").arg(m_fieldoptions->treebase()).arg(truerow).arg(eventfield);

        if ((qttype == QVariant::String) || (qttype == QVariant::Int)) {
            QString dataval = b_engine->eV(request).toString();
            if (m_fieldoptions->hasMatchFor(eventfield))
                ret = m_fieldoptions->match(eventfield, dataval);
            else
                ret = dataval;
        } else if (qttype == QVariant::DateTime) {
            uint idate = int(b_engine->eV(request).toDouble());
            QDateTime qdt = QDateTime::fromTime_t(idate);
            if (role == Qt::UserRole) // when sorting
                ret = idate;
            else {
                if (idate > 0)
                    ret = qdt.toString();
                else
                    ret = "-";
            }
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
    QVariant::Type qttype = m_fieldoptions->qttype(column);
    int nrows = rowCount(QModelIndex());
    int idcolumn = 0;

    struct {
        static bool s_ascending(const QPair<QString, QString> &a,
                                const QPair<QString, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ? true : false;
        }
        static bool s_descending(const QPair<QString, QString> &a,
                                 const QPair<QString, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ? false : true;
        }
        static bool i_ascending(const QPair<QString, int> & a,
                                const QPair<QString, int> & b) {
            return (a.second < b.second);
        }
        static bool i_descending(const QPair<QString, int> & a,
                                 const QPair<QString, int> & b) {
            return (a.second > b.second);
        }
    } sFun;

    if (qttype == QVariant::String) {
        QList<QPair<QString, QString> > toSort;
        for (int i = 0; i < nrows; i++) {
            toSort.append(QPair<QString, QString>(index(i, idcolumn).data(Qt::UserRole).toString(),
                                                  index(i, column).data(Qt::UserRole).toString()));
        }
        qSort(toSort.begin(), toSort.end(),
              (order == Qt::AscendingOrder) ?
              sFun.s_ascending :
              sFun.s_descending);
        for (int i = 0; i < nrows; i++) {
            m_row2id.insert(i, QString(toSort[i].first));
        }

    } else if ((qttype == QVariant::DateTime) ||
               (qttype == QVariant::Int)) {
        QList<QPair<QString, int> > toSort;
        for (int i = 0; i < nrows; i++) {
            toSort.append(QPair<QString, int>(index(i, idcolumn).data(Qt::UserRole).toString(),
                                              index(i, column).data(Qt::UserRole).toInt()));
        }
        qSort(toSort.begin(), toSort.end(),
              (order == Qt::AscendingOrder) ?
              sFun.i_ascending :
              sFun.i_descending);
        for (int i = 0; i < nrows; i++) {
            m_row2id.insert(i, QString(toSort[i].first));
        }
    }

    reset();
}

QString CommonTableModel::row2id(int row)
{
    if (!m_row2id.contains(row))
        return NULL;

    return m_row2id[row];
}

//
// CommonTableView class
//

CommonTableView::CommonTableView(QWidget * parent,
                                 XLet * parentxlet,
                                 CommonTableModel * model)
    : QTableView(parent)
{
    int idcolumn = 0;
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
    hideColumn(idcolumn);
    // sortByColumn(0);

    connect(this, SIGNAL(signalMousePressEvent(QMouseEvent *)),
            parentxlet, SLOT(mousePressEvent(QMouseEvent *)));
    connect(this, SIGNAL(clicked(const QModelIndex &)),
            parentxlet, SLOT(onViewClick(const QModelIndex &)));
    connect(this, SIGNAL(doubleClicked(const QModelIndex &)),
            parentxlet, SLOT(onViewDoubleClick(const QModelIndex &)));
    connect(model, SIGNAL(setDataEdit(const QModelIndex &, const QVariant &)),
            parentxlet, SLOT(setDataEdit(const QModelIndex &, const QVariant &)));
}

void CommonTableView::selectionChanged(const QItemSelection & selected,
                                       const QItemSelection & deselected)
{
    m_modelindexlist.clear();
    int columntomatch = 2; // XXX first unhidden column actually
    foreach (QModelIndex qmi, QAbstractItemView::selectedIndexes()) {
        if (qmi.column() == columntomatch)
            m_modelindexlist.append(qmi);
    }
    QAbstractItemView::selectionChanged(selected, deselected);
}

QList<QModelIndex> CommonTableView::currentSelection() const
{
    return m_modelindexlist;
}

void CommonTableView::mousePressEvent(QMouseEvent * event)
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
    m_view = new CommonTableView(this, parentxlet, model);

    m_view->setStyleSheet("CommonTableView {" + model->displayOptionStyleSheet() + "}");
    m_view->verticalHeader()->hide();
    // hBox->addStretch(1);
    hBox->addWidget(m_view, Qt::AlignJustify);
    // hBox->addStretch(1);

    vBox->addLayout(hBox);
    setLayout(vBox);
}

QList<QModelIndex> CommonTableWidget::currentSelection() const
{
    return m_view->currentSelection();
}

CommonTableView* CommonTableWidget::view()
{
    return m_view;
}


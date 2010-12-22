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

#include "etvng.h"

//
// ETVListProperties class
//

ETVListProperties::ETVListProperties()
{
    m_properties.clear();
    m_properties["display_qss"] = "border: none; color:black;";
    m_properties["display_grid"] = 1;
    m_properties["treebase"] = "records";
    m_properties["columns"] = "";
}

// to define, per column : "id" for "eV", type (boolean, date/time, phone)
void ETVListProperties::addProperty(const QString & title,
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

int ETVListProperties::displayOptionShowGrid() const
{
    return m_properties.value("display_grid").toInt();
}

QString ETVListProperties::displayOptionStyleSheet() const
{
    return m_properties.value("display_qss").toString();
}

int ETVListProperties::ncolumns() const
{
    return m_properties.value("columns").toList().size();
}

QString ETVListProperties::treebase() const
{
    return m_properties.value("treebase").toString();
}

QString ETVListProperties::title(int index) const
{
    return m_properties.value("columns").toList()[index].toMap().value("title").toString();
}

QString ETVListProperties::eventfield(int index) const
{
    return m_properties.value("columns").toList()[index].toMap().value("eventfield").toString();
}

QVariant::Type ETVListProperties::qttype(int index) const
{
    return QVariant::Type(m_properties.value("columns").toList()[index].toMap().value("qttype").toInt());
}

//
// ETVListModel class
//

ETVListModel::ETVListModel(const ETVListProperties * const qv)
    : QAbstractTableModel()
{
    m_fieldoptions = qv;
    b_engine->tree()->onChange(m_fieldoptions->treebase(),
                               this,
                               SLOT(mylistChange(const QString &,
                                                 DStoreEvent)));
    // startTimer(1000);
}

int ETVListModel::displayOptionShowGrid() const
{
    return m_fieldoptions->displayOptionShowGrid();
}

QString ETVListModel::displayOptionStyleSheet() const
{
    return m_fieldoptions->displayOptionStyleSheet();
}

void ETVListModel::timerEvent(QTimerEvent *)
{
    reset();
}

void ETVListModel::mylistChange(const QString &, DStoreEvent)
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

Qt::ItemFlags ETVListModel::flags(const QModelIndex &) const
{
    // return Qt::NoItemFlags;
    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
}

int ETVListModel::rowCount(const QModelIndex&) const
{
    return m_myList.size();
}

int ETVListModel::columnCount(const QModelIndex &) const
{
    return m_fieldoptions->ncolumns();
}

QVariant ETVListModel::data(const QModelIndex & index, int role) const
{
    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
        return QVariant();
    }

    int row = index.row();
    int col = index.column();

    if (m_row2id.contains(row))
        row = m_row2id[row].toInt();

    QString eventfield = m_fieldoptions->eventfield(col);
    QVariant::Type qttype = m_fieldoptions->qttype(col);
    QString request = QString("%1/%2/%3").arg(m_fieldoptions->treebase()).arg(row).arg(eventfield);

    if (qttype == QVariant::String)
        return b_engine->eV(request).toString();
    else if (qttype == QVariant::Int)
        return b_engine->eV(request).toInt();
    else if (qttype == QVariant::DateTime) {
        uint ii = int(b_engine->eV(request).toDouble());
        QDateTime qdt = QDateTime::fromTime_t(ii);
        return qdt.toString();
    } else
        return "zzz";

    // return b_engine->eV(request).toString().isEmpty() ? tr("No") : tr("Yes");
    // return b_engine->eV(request).toInt() ? tr("Yes") : tr("No");
    // return b_engine->eVM(request).size();

//             QVariantMap UserIn = b_engine->eVM(request);
//             double time = 0;
//             QString displayed = QString::fromUtf8("Ø");
//             foreach (QString uid, UserIn.keys()) {
//                 double utime = UserIn.value(uid).toMap().value("time-start").toDouble();
//                 if ((time == 0) || (time > utime)) {
//                     time = utime;
//                 }
//             }
//             if (time != 0) {
//                 displayed =
//                     QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() -
//                                           b_engine->timeDeltaServerClient() -
//                                           time)
//                                          .toUTC().toString("hh:mm:ss");
//             }

//             return displayed;
//         }
//         default:
//             break;
//     }
//     return QVariant();
}

QVariant ETVListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        return m_fieldoptions->title(section);
    }

    return QVariant();
}

void ETVListModel::sort(int column, Qt::SortOrder order)
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
        toSort.append(QPair<int, QString>(index(i, 0).data().toInt(),
                                          index(i, column).data().toString()));
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
// ETVListView class
//

ETVListView::ETVListView(QWidget * parent,
                         XLet * parentxlet,
                         ETVListModel * model)
    : QTableView(parent)
{
    setSortingEnabled(true);
    setModel(model);
    setShowGrid(model->displayOptionShowGrid());
    verticalHeader()->hide();
    horizontalHeader()->setResizeMode(QHeaderView::Interactive); // ::Stretch, ::ResizeToContents
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setMovable(true);
    setStyleSheet("ETVListView {" + model->displayOptionStyleSheet() + "}");
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    // hideColumn(0);

    connect(this, SIGNAL(pevent(QMouseEvent *)),
            parentxlet, SLOT(pevent(QMouseEvent *)));
    connect(this, SIGNAL(clicked(const QModelIndex &)),
            parentxlet, SLOT(onViewClick(const QModelIndex &)));
    connect(this, SIGNAL(doubleClicked(const QModelIndex &)),
            parentxlet, SLOT(onViewDoubleClick(const QModelIndex &)));
}

void ETVListView::selectionChanged(const QItemSelection & selected,
                                   const QItemSelection & deselected)
{
    QAbstractItemView::selectionChanged(selected, deselected);
    // foreach (QModelIndex qmi, deselected.indexes());
    // qDebug() << Q_FUNC_INFO;
    // foreach (QModelIndex qmi, selected.indexes())
    // qDebug() << Q_FUNC_INFO << "selected" << qmi.row() << qmi.column();
}

void ETVListView::mousePressEvent(QMouseEvent *event)
{
    emit pevent(event);
    QTableView::mousePressEvent(event);
}

//
// ETVListWidget class
//

ETVListWidget::ETVListWidget(const ETVListProperties * const qv,
                             XLet * parentxlet)
    : QWidget()
{
    QVBoxLayout  * vBox = new QVBoxLayout(this);
    QHBoxLayout  * hBox = new QHBoxLayout();
    ETVListModel * model = new ETVListModel(qv);
    ETVListView  * view = new ETVListView(this, parentxlet, model);

    view->setStyleSheet("ETVListView {" + model->displayOptionStyleSheet() + "}");
    view->verticalHeader()->hide();

    // hBox->addStretch(1);
    hBox->addWidget(view, Qt::AlignJustify);
    // hBox->addStretch(1);

    vBox->addLayout(hBox);
    setLayout(vBox);
}

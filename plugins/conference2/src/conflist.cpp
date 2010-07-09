#include "conflist.h"

ConfList::ConfList(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(parent);
    QTableView *view = new QTableView();
    QHBoxLayout *hBox = new QHBoxLayout();
    view->setStyleSheet("QTableView { border: none; background:transparent; color:black; }");
    view->verticalHeader()->hide();
    view->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    hBox->addStretch(1);
    hBox->addWidget(view, 4);
    hBox->addStretch(1);

    view->setModel(new ConfListModel());
    layout->addLayout(hBox);
    setLayout(layout);
}

ConfListModel::ConfListModel()
{
    startTimer(1000);
}

void ConfListModel::timerEvent(QTimerEvent *)
{
    reset();
}

Qt::ItemFlags ConfListModel::flags(const QModelIndex &) const
{
    return Qt::NoItemFlags;
}

int ConfListModel::rowCount(const QModelIndex&) const
{
    return b_engine->tree()->extractVMap("confrooms/*").size();
}

int ConfListModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant ConfListModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
        return QVariant();
    }

    int row = index.row(), col = index.column();
    qDebug() << row;

    switch (col) {
        case NUMBER:
            return b_engine->tree()->extractVariant(QString("confrooms/%0/number").arg(row));
        case NAME:
            return b_engine->tree()->extractVariant(QString("confrooms/%0/name").arg(row));
        default:
            break;
    }
    return QVariant();
}

QVariant ConfListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
     if (role != Qt::DisplayRole)
        return QVariant();
    
    if (orientation == Qt::Horizontal) {
        if (section == NUMBER) {
            return QVariant(tr("Number"));
        } else if (section == NAME) {
            return QVariant(tr("Name"));
        } else if (section == MEMBER_COUNT) {
            return QVariant(tr("Member count"));
        } else if (section == ADMIN_COUNT) {
            return QVariant(tr("Admin count"));
        } else if (section == STARTED_SINCE) {
            return QVariant(tr("Started since"));
        }
    }

    return QVariant();

}

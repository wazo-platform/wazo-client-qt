#include "conflist.h"

ConfList::ConfList(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(parent);
    QHBoxLayout *hBox = new QHBoxLayout();
    ConfListView *view = new ConfListView(this, new ConfListModel());
    view->setStyleSheet("QTableView { border: none; background:transparent; color:black; }");
    view->verticalHeader()->hide();

    hBox->addStretch(1);
    hBox->addWidget(view, 4);
    hBox->addStretch(1);

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

    switch (col) {
        case ID:
            return b_engine->tree()->extractVariant(QString("confrooms/%0/id").arg(row));
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
        if (section == ID) {
            return QVariant(tr("Room UID"));
        } else if (section == NUMBER) {
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


ConfListView::ConfListView(QWidget *parent, ConfListModel *model)
    : QTableView(parent)
{
    setSortingEnabled(true);
    setModel(model);
    verticalHeader()->hide();
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    setStyleSheet("QTableView { border: none; background:transparent; color:black; }");
    hideColumn(0);

    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(onViewClick(const QModelIndex &)));
}


void ConfListView::onViewClick(const QModelIndex &model)
{
    QString roomId = model.sibling(model.row(), ConfListModel::ID).data().toString();
    QString roomName = model.sibling(model.row(), ConfListModel::NAME).data().toString();
    QString roomNumber = model.sibling(model.row(), ConfListModel::NUMBER).data().toString();

    if (roomId != "") {
        if (lastPressed&Qt::LeftButton) {
            b_engine->pasteToDial(roomId);
        } else {
            QMenu *menu = new QMenu(this);

            QAction *action = new QAction(tr("Go in room %1 (%2)")
                                             .arg(roomName).arg(roomNumber), menu);
            action->setProperty("id", roomId);
            connect(action, SIGNAL(triggered(bool)),
                    this, SIGNAL(openConfRoom()));

            menu->addAction(action);
            menu->exec(QCursor::pos());
        }
    }
}

void ConfListView::mousePressEvent(QMouseEvent *event)
{
    lastPressed = event->button();
    QTableView::mousePressEvent(event);
}

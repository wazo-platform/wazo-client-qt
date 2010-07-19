#include "conflist.h"

ConfList::ConfList(XletConference *parent)
    : QWidget(), manager(parent)
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

void ConfList::openConfRoom()
{
    manager->openConfRoom(sender()->property("id").toString());
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

    if (m_row2id.contains(row))
        row = m_row2id[row].toInt();

    switch (col) {
        case ID:
            return b_engine->tree()->extractVariant(QString("confrooms/%0/id").arg(row));
        case NUMBER:
            return b_engine->tree()->extractVariant(QString("confrooms/%0/number").arg(row));
        case NAME:
            return b_engine->tree()->extractVariant(QString("confrooms/%0/name").arg(row));
        case PIN_REQUIRED:
            return b_engine->tree()->extractVariant(
                        QString("confrooms/%0/pin").arg(row));
        case MODERATED:
            return b_engine->tree()->extractVariant(
                        QString("confrooms/%0/moderated").arg(row))
                        .toBool() ? tr("Yes") : tr("No") ;
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
        } else if (section == PIN_REQUIRED) {
            return QVariant(tr("pin code"));
        } else if (section == MEMBER_COUNT) {
            return QVariant(tr("Member count"));
        } else if (section == MODERATED) {
            return QVariant(tr("Moderated"));
        } else if (section == STARTED_SINCE) {
            return QVariant(tr("Started since"));
        }
    }

    return QVariant();
}


void ConfListModel::sort(int column, Qt::SortOrder order)
{
    QList<QPair<int, QString> > toSort;

    struct {
        static bool ascending(const QPair<int, QString> &a,
                              const QPair<int, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ?
                                               true :
                                               false;
        }
        static bool descending(const QPair<int, QString> &a,
                                 const QPair<int, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ?
                                               false :
                                               true;
        }
    } sorting;

    int i, e;
    for (i=0,e=rowCount(QModelIndex());i<e;i++) {
        toSort.append(QPair<int, QString>(index(i, ID).data().toInt(),
                                          index(i, column).data().toString()));
    }

    qSort(toSort.begin(), toSort.end(), (order == Qt::AscendingOrder) ? 
                                         sorting.ascending :
                                         sorting.descending);

    for (i=0;i<e;i++) {
        m_row2id.insert(i, QString::number(toSort[i].first));
    }
    reset();
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
            b_engine->pasteToDial(roomNumber);
        } else {
            QMenu *menu = new QMenu(this);

            QAction *action = new QAction(tr("Get in room %1 (%2)")
                                             .arg(roomName).arg(roomNumber), menu);

            action->setProperty("id", roomId);
            connect(action, SIGNAL(triggered(bool)),
                    parentWidget(), SLOT(openConfRoom()));

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

#include "conflist.h"

ConfListModel::ConfListModel()
    : QAbstractTableModel()
{
    b_engine->tree()->onChange("confrooms", this,
        SLOT(confRoomsChange(const QString &, DStoreEvent)));

    startTimer(1000);
}

void ConfListModel::timerEvent(QTimerEvent *)
{
    reset();
}

void ConfListModel::confRoomsChange(const QString &, DStoreEvent)
{
    QVariantMap roomList = b_engine->eVM("confrooms");

    int row = 0;
    if (roomList.size() != m_row2id.size()) {
        foreach(QString roomId, roomList.keys()) {
            m_row2id.insert(row++, roomId);
        }
    }
    reset();
}

Qt::ItemFlags ConfListModel::flags(const QModelIndex &) const
{
    return Qt::NoItemFlags;
}

int ConfListModel::rowCount(const QModelIndex&) const
{
    return b_engine->eVM("confrooms").size();
}

int ConfListModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant
ConfListModel::data(const QModelIndex &index,
                    int role) const
{
    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
        return QVariant();
    }

    int row = index.row(), col = index.column();

    if (m_row2id.contains(row))
        row = m_row2id[row].toInt();

    QString room = QString("confrooms/%0/").arg(row);
    switch (col) {
        case ID:
            return b_engine->eV(room + "id");
        case NUMBER:
            return b_engine->eV(room + "number");
        case NAME:
            return b_engine->eV(room + "name");
        case PIN_REQUIRED:
            return b_engine->eV(room + "pin");
        case MODERATED:
            return b_engine->eV(room + "admin-id").toInt() ?
                       tr("Yes") : tr("No");
        case MEMBER_COUNT:
            return b_engine->eVM(room + "in").size();
        case STARTED_SINCE:
        {
            QVariantMap UserIn = b_engine->eVM(room + "in");
            double time = 0;
            QString displayed = QString::fromUtf8("Ø");
            foreach (QString uid, UserIn.keys()) {
                double utime = UserIn[uid].toMap()["time-start"].toDouble();
                if ((time == 0) || (time > utime)) {
                    time = utime;
                }
            }
            if (time != 0) {
                displayed = 
                    QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() - time)
                                         .toUTC().toString("hh:mm:ss");
            }

            return displayed;
        }
        default:
            break;
    }
    return QVariant();
}

QVariant
ConfListModel::headerData(int section,
                          Qt::Orientation orientation,
                          int role) const
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
    struct {
        static bool ascending(const QPair<int, QString> &a,
                              const QPair<int, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ?
                                               true : false;
        }
        static bool descending(const QPair<int, QString> &a,
                               const QPair<int, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ?
                                               false : true;
        }
    } sFun;

    QList<QPair<int, QString> > toSort;

    int i, e;
    for (i=0,e=rowCount(QModelIndex());i<e;i++) {
        toSort.append(QPair<int, QString>(index(i, ID).data().toInt(),
                                          index(i, column).data().toString()));
    }

    qSort(toSort.begin(), toSort.end(), (order == Qt::AscendingOrder) ? 
                                         sFun.ascending :
                                         sFun.descending);

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
    setShowGrid(0);
    verticalHeader()->hide();
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setMovable(true);
    setStyleSheet("ConfListView { border: none; background:transparent; color:black; }");
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
            QTimer *timer = new QTimer(this);
            timer->setSingleShot(true);
            timer->setProperty("id", roomId);
            connect(timer, SIGNAL(timeout()), parentWidget(), SLOT(openConfRoom()));
            timer->start(10);
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


ConfList::ConfList(XletConference *parent)
    : QWidget(), manager(parent)
{
    QVBoxLayout *vBox = new QVBoxLayout(this);
    QHBoxLayout *hBox = new QHBoxLayout();
    ConfListView *view = new ConfListView(this, new ConfListModel());

    view->setStyleSheet("ConfListView { border: none; background:transparent; color:black; }");
    view->verticalHeader()->hide();

    hBox->addStretch(1);
    hBox->addWidget(view, 4);
    hBox->addStretch(1);

    vBox->addLayout(hBox);
    setLayout(vBox);
}

void ConfList::openConfRoom()
{
    manager->openConfRoom(sender()->property("id").toString());
}

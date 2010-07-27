#include "confchamber.h"

ConfChamberModel::ConfChamberModel(const QString &id)
    : QAbstractTableModel(), m_admin(0), m_id(id), m_view(NULL)
{
    b_engine->tree()->onChange(QString("confrooms/%0").arg(id), this,
        SLOT(confRoomChange(const QString &, DStoreEvent)));
    extractRow2IdMap();
    startTimer(1000);
    timerEvent(NULL);
}

void ConfChamberModel::timerEvent(QTimerEvent *)
{
    QVariantMap  adminList = 
        b_engine->eVM(QString("confrooms/%0/in[admin=@1]").arg(m_id));

    foreach(QString id, adminList.keys()) {
        if (adminList[id].toMap()["user-id"].toString() ==  b_engine->xivoUserId()) {
            m_admin = 1;
        }
    }

    reset();
}

void ConfChamberModel::setView(ConfChamberView *v)
{
    m_view = v;

    if (!m_admin) {
        if (m_view) {
            m_view->hideColumn(ACTION_KICK);
            m_view->hideColumn(ACTION_ALLOW_IN);
            m_view->hideColumn(ACTION_TALK_TO);
        }

    }
}

void ConfChamberModel::confRoomChange(const QString &path, DStoreEvent event)
{
    extractRow2IdMap();
}

void ConfChamberModel::extractRow2IdMap()
{
    QVariantMap roomInList = b_engine->eVM(QString("confrooms/%0/in").arg(m_id));

    int row = 0;
    if (roomInList.size() != m_row2id.size()) {
        foreach(QString roomId, roomInList.keys()) {
            m_row2id.insert(row++, roomId);
        }
    }
    reset();
}

void ConfChamberModel::sort(int column, Qt::SortOrder order)
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


int ConfChamberModel::rowCount(const QModelIndex&) const
{
    return b_engine->eVM(QString("confrooms/%0/in").arg(m_id)).size();
}

int ConfChamberModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant
ConfChamberModel::data(const QModelIndex &index,
                       int role) const
{
    int row = index.row(), col = index.column();
    QString rowId;

    rowId = m_row2id[row];

    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
        } else if (role == Qt::DecorationRole) {
            if (col == ACTION_KICK) {
                return QPixmap(":images/cancel.png").scaledToHeight(16,
                               Qt::SmoothTransformation);
            } else if (col == ACTION_ALLOW_IN) {
                return QPixmap(":images/add.png").scaledToHeight(16,
                               Qt::SmoothTransformation);
            } else if (col == ACTION_TALK_TO) {
                return QPixmap(":in/speak.png").scaledToHeight(16,
                               Qt::SmoothTransformation);
            } else if (col == ACTION_MUTE) {
                return QPixmap(":in/mute.png").scaledToHeight(16,
                               Qt::SmoothTransformation);
            }
        } else if (role == Qt::ToolTipRole) {
            if (col == ACTION_KICK) {
                return tr("Kick");
            } else if (col == ACTION_ALLOW_IN) {
                return tr("Allow in");
            } else if (col == ACTION_TALK_TO) {
                return tr("Talk to");
            } else if (col == ACTION_MUTE) {
                return tr("Mute");
            }
        }
        return QVariant();
    }

    QString in = QString("confrooms/%0/in/%1/").arg(m_id).arg(rowId);
    switch (col) {
        case ID:
            return b_engine->eV(in + "id");
        case NUMBER:
            return b_engine->eV(in + "phonenum");
        case ADMIN:
            return (b_engine->eV(in + "admin") != QVariant()) ? tr("yes") : tr("no");
        case NAME:
        {
            QString name = b_engine->eV(QString("users/*[id=%0user-id]").arg(in))
                                       .toMap()["fullname"].toString();
            if (name.isEmpty()) {
                return tr("nobody");
            }
            return name;
        }
        case SINCE:
            return QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() -
                                         b_engine->eV(in + "time-start").toDouble()).toUTC()
                                         .toString("hh:mm:ss");
        default:
            break;
    }
    return QVariant();
}

QVariant
ConfChamberModel::headerData(int section,
                             Qt::Orientation orientation,
                             int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    
    if (orientation == Qt::Horizontal) {
        if (section == ID) {
            return QVariant(tr("ID"));
        } else if (section == NUMBER) {
            return QVariant(tr("Number"));
        } else if (section == NAME) {
            return QVariant(tr("Name"));
        } else if (section == SINCE) {
            return QVariant(tr("Since"));
        } else if (section == ADMIN) {
            return QVariant(tr("Admin"));
        } else if (section == ACTION_KICK) {
            return "K";
        } else if (section == ACTION_ALLOW_IN) {
            return "A";
        } else if (section == ACTION_TALK_TO) {
            return "T";
        } else if (section == ACTION_MUTE) {
            return "M";
        }
    }

    return QVariant();
}

Qt::ItemFlags ConfChamberModel::flags(const QModelIndex &index) const
{
    int row = index.row(), col = index.column();

    if (m_admin) {
        if ((col == ACTION_MUTE) || (col == ACTION_KICK) ||
            (col == ACTION_ALLOW_IN) || (col == ACTION_TALK_TO)) {
            return Qt::ItemIsEnabled;
        }
    }

    QString rowId;
    rowId = m_row2id[row];
    QString in = QString("confrooms/%0/in/%1/").arg(m_id).arg(rowId);

    if (b_engine->eV(in + "user-id").toString() == b_engine->xivoUserId()) {
        if (col == ACTION_MUTE) {
            return Qt::ItemIsEnabled;
        } 
    }

    return Qt::NoItemFlags;
}

QString ConfChamberModel::row2participantId(int row) const
{
    return m_row2id[row];
}

QString ConfChamberModel::id() const
{
    return m_id;
}


ConfChamberView::ConfChamberView(QWidget *parent, ConfChamberModel *model)
    : QTableView(parent)
{
    setSortingEnabled(true);
    setModel(model);
    setShowGrid(0);
    verticalHeader()->hide();
    horizontalHeader()->setMovable(true);
    horizontalHeader()->setStretchLastSection(true);


    int ActionCol[] = { ConfChamberModel::ACTION_MUTE,
                        ConfChamberModel::ACTION_TALK_TO,
                        ConfChamberModel::ACTION_ALLOW_IN,
                        ConfChamberModel::ACTION_KICK };
    int i;
    for(i=0;i<(int)(sizeof(ActionCol)/sizeof(ActionCol[0]));i++) {
        setColumnWidth(ActionCol[i], 18);
        horizontalHeader()->setResizeMode(ActionCol[i], QHeaderView::Fixed);
    }

    setColumnWidth(ConfChamberModel::ADMIN, 60);
    horizontalHeader()->setResizeMode(ConfChamberModel::ADMIN, QHeaderView::Fixed);
    setStyleSheet("ConfListView { border: none; background:transparent; color:black; }");
    hideColumn(0);

    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(onViewClick(const QModelIndex &)));
}

void ConfChamberView::onViewClick(const QModelIndex &index)
{
    int row = index.row(), col = index.column();

    QString roomId = static_cast<ConfChamberModel*>(model())->id();
    QString castId = model()->index(row, ConfChamberModel::ID).data().toString();

    switch (col) {
        case ConfChamberModel::ACTION_MUTE:
        case ConfChamberModel::ACTION_KICK:
            b_engine->meetmeAction("MeetmeKick", castId + " " + roomId);
            break;
        case ConfChamberModel::ACTION_TALK_TO:
        case ConfChamberModel::ACTION_ALLOW_IN:
        default:
            break;
    }
}

void ConfChamberView::mousePressEvent(QMouseEvent *event)
{
    lastPressed = event->button();
    QTableView::mousePressEvent(event);
}


ConfChamber::ConfChamber(const QString &id)
    : QWidget()
{
    QVBoxLayout *vBox = new QVBoxLayout(this);
    QHBoxLayout *hBox = new QHBoxLayout();
    ConfChamberModel *model = new ConfChamberModel(id);
    ConfChamberView *view = new ConfChamberView(this, model);
    model->setView(view);

    view->setStyleSheet("ConfChamberView { border: none; background:transparent; color:black; }");
    view->verticalHeader()->hide();

    hBox->addStretch(1);
    hBox->addWidget(view, 8);
    hBox->addStretch(1);

    vBox->addLayout(hBox);
    setLayout(vBox);
}

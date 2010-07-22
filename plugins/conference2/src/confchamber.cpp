#include "confchamber.h"

ConfChamberModel::ConfChamberModel(const QString &id)
    : QAbstractTableModel(), m_id(id)
{
    b_engine->tree()->onChange(QString("confrooms/%0").arg(id), this,
        SLOT(confRoomChange(const QString &, DStoreEvent)));
    extractRow2IdMap();
    startTimer(1000);
}

void ConfChamberModel::timerEvent(QTimerEvent *)
{
    reset();
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

void ConfChamberModel::sort(int, Qt::SortOrder)
{
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
    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
        return QVariant();
    }

    int row = index.row(), col = index.column();
    QString rowId;

    rowId = m_row2id[row];

    QString in = QString("confrooms/%0/in/%1/").arg(m_id).arg(rowId);
    switch (col) {
        case ID:
            return b_engine->eV(in + "id");
        case NUMBER:
            return b_engine->eV(in + "phonenum");
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
            return QVariant(tr("Channel ID"));
        } else if (section == NUMBER) {
            return QVariant(tr("Number"));
        } else if (section == NAME) {
            return QVariant(tr("Name"));
        } else if (section == SINCE) {
            return QVariant(tr("Since"));
        } else if (section == ADMIN) {
            return QVariant(tr("Admin"));
        } else if (section == ACTION_KICK) {
            return QVariant(tr("Kick"));
        } else if (section == ACTION_MUTE) {
            return QVariant(tr("Mute"));
        }
    }

    return QVariant();
}

Qt::ItemFlags ConfChamberModel::flags(const QModelIndex &) const
{
    return Qt::NoItemFlags;
}


ConfChamberView::ConfChamberView(QWidget *parent, ConfChamberModel *model)
    : QTableView(parent)
{
    setSortingEnabled(true);
    setModel(model);
    verticalHeader()->hide();
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    setStyleSheet("ConfListView { border: none; background:transparent; color:black; }");
    hideColumn(0);

    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(onViewClick(const QModelIndex &)));
}

void ConfChamberView::onViewClick(const QModelIndex &)
{
}

void ConfChamberView::mousePressEvent(QMouseEvent *event)
{
}


ConfChamber::ConfChamber(const QString &id)
    : QWidget()
{
    QVBoxLayout *vBox = new QVBoxLayout(this);
    QHBoxLayout *hBox = new QHBoxLayout();
    ConfChamberView *view = new ConfChamberView(this, new ConfChamberModel(id));

    view->setStyleSheet("ConfChamberView { border: none; background:transparent; color:black; }");
    view->verticalHeader()->hide();

    hBox->addStretch(1);
    hBox->addWidget(view, 4);
    hBox->addStretch(1);

    vBox->addLayout(hBox);
    setLayout(vBox);
}

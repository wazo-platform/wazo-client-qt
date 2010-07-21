#include "confchamber.h"

ConfChamberModel::ConfChamberModel()
    : QAbstractTableModel()
{
}

void ConfChamberModel::sort(int, Qt::SortOrder)
{
}

void ConfChamberModel::timerEvent(QTimerEvent *)
{
    reset();
}

int ConfChamberModel::rowCount(const QModelIndex&) const
{
    return 1;
}

int ConfChamberModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant
ConfChamberModel::data(const QModelIndex &index,
                       int role) const
{
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
    ConfChamberView *view = new ConfChamberView(this, new ConfChamberModel());

    view->setStyleSheet("ConfChamberView { border: none; background:transparent; color:black; }");
    view->verticalHeader()->hide();

    hBox->addStretch(1);
    hBox->addWidget(view, 4);
    hBox->addStretch(1);

    vBox->addLayout(hBox);
    setLayout(vBox);
}

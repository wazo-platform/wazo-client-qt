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

#include "confroom.h"

enum ColOrder {
    ID, ACTION_MUTE, ACTION_KICK, ACTION_TALK_TO,
    ACTION_ALLOW_IN, ACTION_RECORD, ADMIN,
    NUMBER, SINCE, NAME, NB_COL
};

static QVariant COL_TITLE[NB_COL];

ConfRoomModel::ConfRoomModel(ConfTab *tab, QWidget *parent, const QString &id)
    : QAbstractTableModel(parent), m_tab(tab), m_parent(parent), m_admin(0),
      m_authed(0), m_id(id), m_view(NULL)
{
    // b_engine->tree()->onChange(QString("confrooms/%0").arg(id), this,
    //                            SLOT(confRoomChange(const QString &, DStoreEvent)));
    connect(b_engine, SIGNAL(updateMeetmesConfig(const QString &)),
            this, SLOT(updateMeetmesConfig(const QString &)));
    connect(b_engine, SIGNAL(updateMeetmesStatus(const QString &)),
            this, SLOT(updateMeetmesStatus(const QString &)));
    extractRow2IdMap();
    startTimer(1000);
    timerEvent(NULL);

    COL_TITLE[ID] = tr("ID");
    COL_TITLE[NUMBER] = tr("Number");
    COL_TITLE[NAME] = tr("Name");
    COL_TITLE[SINCE] = tr("Since");
    COL_TITLE[ADMIN] = tr("Admin");
    COL_TITLE[ACTION_KICK] = tr("K");
    COL_TITLE[ACTION_RECORD] = tr("R");
    COL_TITLE[ACTION_ALLOW_IN] = tr("A");
    COL_TITLE[ACTION_TALK_TO] = tr("T");
    COL_TITLE[ACTION_MUTE] = tr("M");
}

ConfRoomModel::~ConfRoomModel()
{
    // b_engine->tree()->unregisterAllCb(this);
}

void ConfRoomModel::timerEvent(QTimerEvent *)
{
    // QString req = QString("confrooms/%0/in").arg(m_id);
    const MeetmeInfo * m = b_engine->meetme(m_id);
    if (m) {
        foreach (QString key, m->channels().keys()) {
            const UserInfo * u = b_engine->getUserForXChannelId(key);
            if (u && u->xid() == b_engine->getFullId()) {
                QVariantMap self = m->channels().value(key).toMap();
                m_admin = self.value("isadmin").toBool();
                m_authed = self.value("authed").toBool();
                break;
            }
        }
        updateView();
        reset();
    }
    // QVariantMap users = b_engine->eVM(req);
    // foreach (const QString key, users.keys()) {
    //     if (users[key].toMap().value("user-id") == b_engine->getFullId()) {
    //         QVariantMap self = users[key].toMap();
    //         m_admin = self.value("admin").toBool();
    //         m_authed = self.value("authed").toBool();
    //         break;
    //     }
    // }
}

void ConfRoomModel::setView(ConfRoomView *v)
{
    m_view = v;
    updateView();
}

void ConfRoomModel::updateView()
{
    static int actions[] = { ACTION_RECORD,
                             ACTION_KICK,
                             ACTION_ALLOW_IN,
                             ACTION_TALK_TO };
    if (m_view) {
        if (m_admin) {
            //for (int i = nelem(actions);i--;) {
            for (int i = 0; i < nelem(actions); ++i) {
                m_view->showColumn(actions[i]);
            }
        } else {
            //for(i=nelem(actions);i--;) {
            for (int i = 0; i < nelem(actions); ++i) {
                m_view->hideColumn(actions[i]);
            }
        }
    }
}

void ConfRoomModel::updateMeetmesConfig(const QString & meetme_id)
{
    // qDebug() << Q_FUNC_INFO << meetme_id;
}

void ConfRoomModel::updateMeetmesStatus(const QString & meetme_id)
{
    // Check if our channel left the meetme and close the tab
    const MeetmeInfo * m = b_engine->meetme(m_id);
    if (m) {
        const QVariantMap & channels = m->channels();
        const UserInfo * self = b_engine->user(b_engine->getFullId());
        foreach (const QString & xcid, channels.keys()) {
            if (b_engine->getUserForXChannelId(xcid) == self) {
                // No need to close the tab if we are still in the room
                return;
            }
        }
    }
    m_tab->closeTab(m_parent);
    QTimer::singleShot(0, this, SLOT(extractRow2IdMap()));
}

void ConfRoomModel::extractRow2IdMap()
{
    // m_pplInRoom = b_engine->eVM(QString("confrooms/%0/in").arg(m_id));
    const MeetmeInfo * m = b_engine->meetme(m_id);
    if (m) {
        m_row2id.clear();
        if (m->channels().size() != m_row2id.size()) {
            int row = 0;
            foreach (QString xcid, m->channels().keys()) {
                m_row2id.insert(row++, xcid);
            }
        }
        reset();
    }

    // m_row2id.clear();

    // int row = 0;
    // if (m_pplInRoom.size() != m_row2id.size()) {
    //     foreach(QString roomId, m_pplInRoom.keys()) {
    //         m_row2id.insert(row++, roomId);
    //     }
    // }
    // reset();
}

void ConfRoomModel::sort(int column, Qt::SortOrder order)
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


int ConfRoomModel::rowCount(const QModelIndex&) const
{
    const MeetmeInfo * m = b_engine->meetme(m_id);
    if (m) {
        return m->channels().size();
    }
    // TODO: Check authed and moderation mode...
    // QString room = QString("confrooms/%0/").arg(m_id);
    // if ((b_engine->eV(room + "moderated").toInt()) && (!m_authed))
    //     return 0;

    // return m_pplInRoom.size();
}

int ConfRoomModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant ConfRoomModel::data(const QModelIndex & index, int role) const
{
    int row = index.row(), col = index.column();
    QString chanid = m_row2id[row];

    // QString in = QString("confrooms/%0/in/%1/").arg(m_id).arg(rowId);
    const MeetmeInfo * m = b_engine->meetme(m_id);
    if (! m || (m && ! m->channels().contains(chanid))) {
        qDebug() << Q_FUNC_INFO << m_id << chanid
                 << "No such channel in this meetme";
        return QVariant();
    }

    const QVariantMap & user_chan = m->channels().value(chanid).toMap();
    const ChannelInfo * c = b_engine->channel(chanid);
    const UserInfo * u = b_engine->getUserForXChannelId(chanid);
    bool my_channel = (u && u == b_engine->user(b_engine->getFullId()));

    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
        } else if (role == Qt::DecorationRole) {
            if (col == ACTION_KICK) {
                return QPixmap(":images/cancel.png").scaledToHeight(16,
                               Qt::SmoothTransformation);
            } else if (col == ACTION_ALLOW_IN) {
                //if (!b_engine->eV(in + "authed").toBool()) {
                if (user_chan.value("is_authed").toBool()) {
                    return QPixmap(":images/add.png").scaledToHeight(16,
                                   Qt::SmoothTransformation);
                } else {
                    return QVariant();
                }
            } else if (col == ACTION_TALK_TO) {
                return QPixmap(":in_conference/speak.png").scaledToHeight(16,
                               Qt::SmoothTransformation);
            } else if (col == ACTION_MUTE) {
                if (m_admin || my_channel) {
                    return QPixmap(":in_conference/mute.png").scaledToHeight(16, Qt::SmoothTransformation);
                } else {
                    return QVariant();
                }
            }
        } else if (role == Qt::ToolTipRole) {
            if (col == ACTION_KICK) {
                return tr("Kick");
            } else if (col == ACTION_ALLOW_IN) {
                // if (b_engine->eV(in + "authed").toBool()) {
                if (user_chan.value("is_authed").toBool()) {
                    return tr("User already authed");
                }
                return tr("Allow in");
            } else if (col == ACTION_TALK_TO) {
                // if (b_engine->eV(in + "authed").toBool()) {
                if (user_chan.value("is_authed").toBool()) {
                    return tr("User already authed");
                }
                return tr("Talk to");
            } else if (col == ACTION_RECORD) {
                // if (b_engine->eV(in + "recorded").toBool()) {
                // TODO: Make sure ismonitored means that this channel is being recorded
                if (c && c->ismonitored()) {
                    return tr("User already recorded");
                }
                return tr("Record conference until this user leaves");
            } else if (col == ACTION_MUTE) {
                if (m_admin || my_channel) {
                    // (b_engine->eV(in + "user-id").toString() == b_engine->getFullId())) {
                    //if (b_engine->eV(in + "muted").toBool()) {
                    if (user_chan.value("is_muted").toBool()) {
                        return tr("Unmute");
                    }
                    return tr("Mute");
                }
            }
        }
        return QVariant();
    }

    switch (col) {
    case ID:
        return user_chan.value("id").toString();
        // return b_engine->eV(in + "id");
    case NUMBER:
        // return b_engine->eV(in + "phonenum");
        return (u ? u->findNumberForXChannel(chanid) : tr("unknown"));
    case ACTION_RECORD:
        return (c && c->ismonitored()) ? tr("Yes") : tr("No");
    case ADMIN:
        return (user_chan.value("is_admin").toBool()) ? tr("Yes") : tr("No");
        // return (b_engine->eV(in + "admin").toBool()) ? tr("Yes") : tr("No");
    case NAME:
        return (u ? u->fullname() : tr("unknown"));
        // return b_engine->eV(in + "displayname").toString();
    case ACTION_ALLOW_IN:
        // return (b_engine->eV(in + "authed").toBool()) ? tr("Yes") : tr("No");
        return (user_chan.value("is_authed").toBool() ? tr("Yes") : tr("No"));
    case SINCE:
        // return QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() -
        //                              b_engine->eV(in + "time-start").toDouble() -
        //                              b_engine->timeDeltaServerClient()).toUTC()
        //     .toString("hh:mm:ss");
        return "Fix me";
    default:
        break;
    }
    return QVariant();
}

QVariant ConfRoomModel::headerData(int section,
                                   Qt::Orientation orientation,
                                   int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        return COL_TITLE[section];
    }

    return QVariant();
}

Qt::ItemFlags ConfRoomModel::flags(const QModelIndex &index) const
{
    int row = index.row(), col = index.column();

    QString chanid = m_row2id[row];
    // QString in = QString("confrooms/%0/in/%1/").arg(m_id).arg(rowId);
    const MeetmeInfo * m = b_engine->meetme(m_id);
    if (! m) return Qt::NoItemFlags;
    const QVariantMap & user_chan = m->channels().value(chanid).toMap();
    const UserInfo * u = b_engine->getUserForXChannelId(chanid);
    bool my_channel =  (u && u == b_engine->user(b_engine->getFullId()));
    

    if (m_admin) {
        if (col == ACTION_KICK) {
            return Qt::ItemIsEnabled;
        }
        if (((col == ACTION_ALLOW_IN) || (col == ACTION_TALK_TO))
            // && (!b_engine->eV(in + "authed").toBool())) {
            && (user_chan.value("is_authed").toBool())) {
            return Qt::ItemIsEnabled;
        }
        if ((col == ACTION_MUTE) && (user_chan.value("is_muted").toBool())) {
            return Qt::ItemIsEnabled;
        }
    } else if (my_channel && col == ACTION_MUTE
               && user_chan.value("is_muted").toBool()) {
        return Qt::ItemIsEnabled;
    }
    return Qt::NoItemFlags;
}

QString ConfRoomModel::row2participantId(int row) const
{
    return m_row2id[row];
}

QString ConfRoomModel::id() const
{
    return m_id;
}


ConfRoomView::ConfRoomView(QWidget *parent, ConfRoomModel *model)
    : QTableView(parent)
{
    setSortingEnabled(true);
    setModel(model);
    setShowGrid(0);
    verticalHeader()->hide();
    horizontalHeader()->setMovable(true);
    horizontalHeader()->setStretchLastSection(true);


    int ActionCol[] = { ACTION_MUTE,
                        ACTION_TALK_TO,
                        ACTION_RECORD,
                        ACTION_ALLOW_IN,
                        ACTION_KICK };
    int i;
    for(i=0;i<nelem(ActionCol);i++) {
        setColumnWidth(ActionCol[i], 24);
        horizontalHeader()->setResizeMode(ActionCol[i], QHeaderView::Fixed);
    }

    setColumnWidth(ADMIN, 60);
    horizontalHeader()->setResizeMode(ADMIN, QHeaderView::Fixed);
    setStyleSheet("ConfListView {"
                      "border: none;"
                      "background:transparent;"
                      "color:black;"
                  "}");
    hideColumn(0);

    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(onViewClick(const QModelIndex &)));

    QHeaderView *h = horizontalHeader();
    connect(h, SIGNAL(sectionClicked(int)),
            this, SLOT(sectionHeaderClicked(int)));
}

void ConfRoomView::sectionHeaderClicked(int index)
{
    int nonSortable[] = { ACTION_MUTE,
                          ACTION_TALK_TO,
                          ACTION_RECORD,
                          ACTION_ALLOW_IN,
                          ACTION_KICK };
    int i;
    for(i=0;i<nelem(nonSortable);i++) {
        if (nonSortable[i] == index) {
            setSortingEnabled(false);
            return ;
        }
    }

    setSortingEnabled(true);
}

void ConfRoomView::onViewClick(const QModelIndex &index)
{
    int row = index.row(), col = index.column();

    QString roomId = static_cast<ConfRoomModel*>(model())->id();
    QString castId = model()->index(row, ID).data().toString();
    
    QString in = QString("confrooms/%0/in").arg(roomId);
    QVariantMap users = b_engine->eVM(in);
    QString current_user_key;
    foreach (const QString & k, users.keys()) {
        if (users.value(k).toMap().value("user-id") == b_engine->getFullId()) {
            current_user_key = k;
            break;
        }
    }
    QVariantMap current_user = users.value(current_user_key).toMap();

    if (!(static_cast<ConfRoomModel*>(model())->isAdmin() ||
          current_user.value("id") == castId)) {
        return;
    }

    switch (col) {
        case ACTION_MUTE:
            if (b_engine->eV(in + "muted").toBool()) {
                b_engine->meetmeAction("MeetmeUnmute", castId + " " + roomId);
            } else {
                b_engine->meetmeAction("MeetmeMute", castId + " " + roomId);
            }
            break;
        case ACTION_KICK:
            if (!b_engine->eV(in + "authed").toBool()) {
                b_engine->meetmeAction("MeetmeKick", castId + " " + roomId);
            } else {
                b_engine->meetmeAction("kick", castId + " " + roomId);
            }
            break;
        case ACTION_TALK_TO:
            b_engine->meetmeAction("MeetmeTalk", castId + " " + roomId);
            break;
        case ACTION_RECORD:
            {
            int status = !b_engine->eV(in + "recorded").toBool();
            b_engine->tree()->populate(in + "recorded", status);

            b_engine->meetmeAction("record", castId + " " +
                                             roomId + " " +
                                             ( status ? "stop" : "start"));
            }
            break;
        case ACTION_ALLOW_IN:
            b_engine->meetmeAction("MeetmeAccept", castId + " " + roomId);
            break;
        default:
            break;
    }
}

void ConfRoomView::mousePressEvent(QMouseEvent *event)
{
    lastPressed = event->button();
    QTableView::mousePressEvent(event);
}


ConfRoom::ConfRoom(QWidget *parent, ConfTab *tab, const QString &id)
    : QWidget(parent), m_id(id)
{
    qDebug() << Q_FUNC_INFO << id;
    // QString room = QString("confrooms/%0/").arg(m_id);
    const MeetmeInfo * m = b_engine->meetme(m_id);
    // int moderated = b_engine->eV(room + "moderated").toInt();
    bool moderated = (m ? m->admin_moderationmode() != "0" : false);

    QVBoxLayout *vBox = new QVBoxLayout(this);
    setLayout(vBox);
    QHBoxLayout *hBox = new QHBoxLayout();
    m_model = new ConfRoomModel(tab, this, id);
    QPushButton *roomPause = new QPushButton(tr("&Pause conference"), this);
    QLabel *redondant = new QLabel(tr(" Conference room %1 (%2) ")
                                   .arg(m ? m->name() : tr("unknown"))
                                   .arg(m ? m->number() : tr("unknown")));
    setProperty("id", id);

    roomPause->setProperty("state", true);
    hBox->addStretch(1);
    hBox->addWidget(redondant, 6);
    hBox->addWidget(roomPause, 2);
    hBox->addStretch(1);
    if (! m_model->isAdmin() || (!moderated)) {
        roomPause->hide();
        hBox->setStretch(1, 8);
    }
    vBox->addLayout(hBox);

    hBox = new QHBoxLayout();
    connect(roomPause, SIGNAL(clicked()), this, SLOT(pauseConf()));

    ConfRoomView *view = new ConfRoomView(this, m_model);
    m_model->setView(view);

    view->setStyleSheet("ConfRoomView {"
                            "border: none;"
                            "background:transparent;"
                            "color:black;"
                        "}");

    view->verticalHeader()->hide();

    hBox->addStretch(1);
    hBox->addWidget(view, 8);
    hBox->addStretch(1);

    vBox->addLayout(hBox);

    if (m && m->admin_moderationmode() != "0" && (! m_model->isAuthed())) {
        QTimer *timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(timer, SIGNAL(timeout()), this, SLOT(allowedIn()));

        timer->start(100);
        m_moderatedRoom = new QLabel(tr("This room is moderated. You can't"
                                        " see any participant, until an admin allow you in."), this);
        hBox = new QHBoxLayout();
        hBox->addStretch(1);
        hBox->addWidget(m_moderatedRoom, 8);
        hBox->addStretch(1);

        vBox->addLayout(hBox);
    }
}

void ConfRoom::allowedIn()
{
    if (m_model->isAuthed()) {
        m_moderatedRoom->hide();
        static_cast<QTimer*>(sender())->stop();
    }

}

void ConfRoom::pauseConf()
{
    QPushButton *button = static_cast<QPushButton*>(sender());
    bool confPaused = button->property("state").toBool();

    if (confPaused) {
        button->setText(tr("&Restart the conference"));
    } else {
        button->setText(tr("&Pause the conference"));
    }
    button->setProperty("state", !confPaused);
    b_engine->meetmeAction("MeetmePause", m_id + " " + (confPaused? "on" : "off"));
}

/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

void ConfRoomModel::setView(ConfRoomView *v)
{
    m_view = v;
    updateView();
}

QString ConfRoomModel::id() const
{
    return m_id;
}

QString ConfRoomModel::row2participantId(int row) const
{
    return m_row2id[row];
}

void ConfRoomModel::timerEvent(QTimerEvent *)
{
    const MeetmeInfo * m = b_engine->meetme(m_id);
    if (m) {
        foreach (QString key, m->channels().keys()) {
            const UserInfo * u = b_engine->getUserForXChannelId(key);
            if (u && u->xid() == b_engine->getFullId()) {
                QVariantMap self = m->channels().value(key).toMap();
                m_admin = self.value("isadmin").toBool();
                m_authed = self.value("isauthed").toBool();
                break;
            }
        }
        updateView();
        reset();
    }
}

void ConfRoomModel::updateView()
{
    static int actions[] = { ACTION_RECORD,
                             ACTION_KICK,
                             ACTION_ALLOW_IN,
                             ACTION_TALK_TO };
    if (m_view) {
        if (m_admin) {
            for (int i = 0; i < nelem(actions); ++i) {
                m_view->showColumn(actions[i]);
            }
        } else {
            for (int i = 0; i < nelem(actions); ++i) {
                m_view->hideColumn(actions[i]);
            }
        }
    }
}

void ConfRoomModel::updateMeetmesStatus(const QString &)
{
    // Check if our channel left the meetme and close the tab
    extractRow2IdMap();
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

/*! \brief Refresh the map between row and meetme channel ids */
void ConfRoomModel::extractRow2IdMap()
{
    m_row2id.clear();
    const MeetmeInfo * m = b_engine->meetme(m_id);
    if (m) {
        if (m->channels().size() != m_row2id.size()) {
            int row = 0;
            foreach (QString xcid, m->channels().keys()) {
                m_row2id.insert(row++, xcid);
            }
        }
    }
    reset();
}

void ConfRoomModel::sort(int column, Qt::SortOrder order)
{
    struct {
        static bool ascending(const QPair<QString, QString> &a,
                              const QPair<QString, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ?
                                               true : false;
        }
        static bool descending(const QPair<QString, QString> &a,
                               const QPair<QString, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ?
                                               false : true;
        }
    } sFun;

    QList<QPair<QString, QString> > toSort;

    int count = rowCount(QModelIndex());
    for (int i = 0; i < count; i++) {
        toSort.append(QPair<QString, QString>(index(i, ID).data().toString(),
                                              index(i, column).data().toString()));
    }

    qSort(toSort.begin(), toSort.end(), (order == Qt::AscendingOrder) ?
                                         sFun.ascending :
                                         sFun.descending);

    for (int i = 0; i < count; i++) {
        m_row2id.insert(i, QString(toSort[i].first));
    }
    reset();
}


/*! \brief Returns the number of rows in the table
 * This is not the actual size of the table but the size of the data we have to
 * fit in it.
 * \return the number of users in this meetme or 0 if not authorized
 */
int ConfRoomModel::rowCount(const QModelIndex &) const
{
    const MeetmeInfo * m = b_engine->meetme(m_id);
    if (m) {
        bool moderated = m->admin_moderationmode() != "0";
        if ((! moderated) || m_authed) {
            return m->channels().size();
        }
    }
    return 0;
}

int ConfRoomModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant ConfRoomModel::data(const QModelIndex & index, int role) const
{
    int row = index.row(), col = index.column();
    QString chanid = m_row2id[row];
    const MeetmeInfo * m = b_engine->meetme(m_id);
    if (! m || (m && ! m->channels().contains(chanid))) {
        qDebug() << Q_FUNC_INFO << m_id << "chanid(" <<  chanid
                 << ") No such channel in this meetme" << m->channels() << m_row2id;
        return QVariant();
    }
    const QVariantMap & user_chan = m->channels().value(chanid).toMap();
    QString chanxid = QString("%0/%1").arg(m->ipbxid()).arg(m_row2id[row]);
    const ChannelInfo * c = b_engine->channel(chanxid);
    const UserInfo * u = b_engine->getUserForXChannelId(chanxid);
    bool my_channel = (u && u == b_engine->user(b_engine->getFullId()));

    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
        } else if (role == Qt::DecorationRole) {
            if (col == ACTION_KICK) {
                return QPixmap(":images/cancel.png").scaledToHeight(16,
                               Qt::SmoothTransformation);
            } else if (col == ACTION_ALLOW_IN) {
                if (! user_chan.value("isauthed").toBool()) {
                    return QPixmap(":images/add.png").scaledToHeight(16,
                                   Qt::SmoothTransformation);
                } else {
                    return QVariant();
                }
            } else if (col == ACTION_TALK_TO) {
                return QPixmap(":images/conference/speak.png").scaledToHeight(16,
                               Qt::SmoothTransformation);
            } else if (col == ACTION_MUTE) {
                if (m_admin || my_channel) {
                    return QPixmap(":images/conference/mute.png").scaledToHeight(16, Qt::SmoothTransformation);
                } else {
                    return QVariant();
                }
            }
        } else if (role == Qt::ToolTipRole) {
            if (col == ACTION_KICK) {
                return tr("Kick");
            } else if (col == ACTION_ALLOW_IN) {
                if (user_chan.value("isauthed").toBool()) {
                    return tr("User already authed");
                }
                return tr("Allow in");
            } else if (col == ACTION_TALK_TO) {
                if (user_chan.value("isauthed").toBool()) {
                    return tr("User already authed");
                }
                return tr("Talk to");
            } else if (col == ACTION_RECORD) {
                if (c && c->ismonitored()) {
                    return tr("User already recorded");
                }
                return tr("Record conference until this user leaves");
            } else if (col == ACTION_MUTE) {
                if (m_admin || my_channel) {
                    if (user_chan.value("ismuted").toBool()) {
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
        return chanid;
    case NUMBER:
        return (u ? u->findNumberForXChannel(chanxid) : tr("unknown"));
    case ACTION_RECORD:
        return (c && c->ismonitored()) ? tr("Yes") : tr("No");
    case ADMIN:
        return (user_chan.value("isadmin").toBool()) ? tr("Yes") : tr("No");
    case NAME:
        return (u ? u->fullname() : tr("unknown"));
    case ACTION_ALLOW_IN:
        return (user_chan.value("isauthed").toBool() ? tr("Yes") : tr("No"));
    case SINCE:
        return QDateTime::fromTime_t(
            QDateTime::currentDateTime().toTime_t()
            - (c ? c->timestamp() : 0)
            - b_engine->timeDeltaServerClient()).toUTC().toString("hh:mm:ss");
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
    int col = index.column();
    QString chanid = m_row2id[index.row()];
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
            && (! user_chan.value("isauthed").toBool())) {
            return Qt::ItemIsEnabled;
        }
        if ((col == ACTION_MUTE) && (user_chan.value("ismuted").toBool())) {
            return Qt::ItemIsEnabled;
        }
    } else if (my_channel && col == ACTION_MUTE
               && user_chan.value("ismuted").toBool()) {
        return Qt::ItemIsEnabled;
    }
    return Qt::NoItemFlags;
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

    for(int i = 0; i < nelem(ActionCol); i++) {
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

    for(int i = 0; i < nelem(nonSortable); i++) {
        if (nonSortable[i] == index) {
            setSortingEnabled(false);
            return ;
        }
    }
    setSortingEnabled(true);
}

void ConfRoomView::onViewClick(const QModelIndex &index)
{
    QString channel_id = model()->index(index.row(), ID).data().toString();
    
    const ChannelInfo * c = b_engine->channel(channel_id);
    const UserInfo * u = b_engine->getUserForXChannelId(channel_id);
    const UserInfo * my_userinfo = b_engine->user(b_engine->getFullId());
    
    bool is_admin = static_cast<ConfRoomModel *>(model())->isAdmin();
    bool is_my_channel = (u && my_userinfo && u == my_userinfo);

    if ((! is_admin) && (! is_my_channel)) return;

    QString meetme_id = static_cast<ConfRoomModel *>(model())->id();
    const MeetmeInfo * m = b_engine->meetme(meetme_id);
    const QVariantMap & channels = (m ? m->channels() : QVariantMap());
    const QVariantMap & user_chan = channels.value(channel_id).toMap();

    const QString & usernum = user_chan.value("usernum").toString();
    QString adminnum;

    foreach (const QString & key, channels.keys()) {
        if (my_userinfo && channels.value(key).toMap().value("isadmin").toBool()
            && my_userinfo == b_engine->getUserForXChannelId(key)) {
            adminnum = channels.value(key).toMap().value("usernum").toString();
        }
    }

    switch (index.column()) {
        case ACTION_MUTE:
        {
            qDebug() << Q_FUNC_INFO << "Mute/unmute";
            bool is_muted = user_chan.value("ismuted").toBool();
            if (is_muted) {
                b_engine->meetmeAction("MeetmeUnmute", meetme_id + " "
                                       + usernum);
            } else {
                b_engine->meetmeAction("MeetmeMute", meetme_id + " "
                                       + usernum);
            }
            break;
        }
        case ACTION_KICK:
            qDebug() << Q_FUNC_INFO << "Kick/MeetmeKick";
            if (! adminnum.isEmpty()) {
                // bool is_authed = user_chan.value("isauthed").toBool();
                b_engine->meetmeAction("MeetmeKick", meetme_id + " " +
                                       usernum + " " + adminnum);
                // if (! is_authed) {
                //     b_engine->meetmeAction("MeetmeKick", meetme_id + " " + 
                //                            usernum + " " + adminnum);
                // } else {
                //     b_engine->meetmeAction("kick", meetme_id + " " +
                //                            usernum + " " + adminnum);
                // }
            }
            break;
        case ACTION_TALK_TO:
            qDebug() << Q_FUNC_INFO << "Talk";
            if (! adminnum.isEmpty()) {
                b_engine->meetmeAction("MeetmeTalk", meetme_id + " " + usernum
                                       + " " + adminnum);
            }
            break;
        case ACTION_RECORD:
        {
            qDebug() << Q_FUNC_INFO << "Record";
            bool is_recorded = (c && c->ismonitored());
            b_engine->meetmeAction("record", meetme_id + " " +
                                             usernum + " " +
                                             ( is_recorded ? "stop" : "start"));
            break;
        }
        case ACTION_ALLOW_IN:
            qDebug() << Q_FUNC_INFO << "Accept";
            if (! adminnum.isEmpty()) {
                b_engine->meetmeAction("MeetmeAccept", meetme_id + " " + usernum
                                       + " " + adminnum);
            }
            break;
        default:
            qDebug() << Q_FUNC_INFO << "No Action";
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
    const MeetmeInfo * m = b_engine->meetme(m_id);
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

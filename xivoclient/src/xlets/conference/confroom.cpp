/* XiVO Client
 * Copyright (C) 2007-2012, Avencall
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

ConfRoomModel::ConfRoomModel(ConfTab *tab, QWidget *parent, const QString &number, const QVariantMap &members)
    : QAbstractTableModel(parent), m_tab(tab), m_parent(parent), m_admin(0),
      m_authed(0), m_number(number), m_view(NULL), m_members(members)
{
    connect(b_engine, SIGNAL(meetmeUpdate(const QVariantMap &)),
            this, SLOT(updateMeetmeConfig(const QVariantMap &)));
    connect(b_engine, SIGNAL(meetmeMembershipUpdated()),
            this, SLOT(updateMembership()));

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

void ConfRoomModel::timerEvent(QTimerEvent *)
{
    reset();
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

void ConfRoomModel::updateMeetmeConfig(const QVariantMap &config)
{
    m_members = config[m_number].toMap()["members"].toMap();
    qDebug() << Q_FUNC_INFO << "number" << m_number << "members" << m_members;
    extractRow2IdMap();
    reset();
}

/*! \brief Refresh the map between row and meetme channel ids */
void ConfRoomModel::extractRow2IdMap()
{
    m_row2number = m_members.keys();
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
        m_row2number.insert(i, QString(toSort[i].first));
    }
    reset();
}

int ConfRoomModel::rowCount(const QModelIndex &) const
{
    return m_members.size();
}

int ConfRoomModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant ConfRoomModel::data(const QModelIndex & index, int role) const
{
    int row = index.row();
    int col = index.column();
    const QString &number = m_row2number[row];
    const QVariantMap &member = m_members[number].toMap();
    int join_sequence = member["join_order"].toInt();
    bool isMe = b_engine->isMeetmeMember(m_number, join_sequence);

    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
        } else if (role == Qt::DecorationRole) {
            if (col == ACTION_KICK) {
                return QVariant();
            } else if (col == ACTION_ALLOW_IN) {
                return QVariant();
            } else if (col == ACTION_TALK_TO) {
                return QPixmap(":images/conference/speak.png").scaledToHeight(16, Qt::SmoothTransformation);
            } else if (col == ACTION_MUTE && isMe) {
                return QPixmap(":images/conference/mute.png").scaledToHeight(16, Qt::SmoothTransformation);
            }
        } else if (role == Qt::ToolTipRole) {
            if (col == ACTION_KICK) {
                return tr("Kick");
            } else if (col == ACTION_ALLOW_IN) {
                return tr("Allow in");
            } else if (col == ACTION_TALK_TO) {
                return tr("Talk to");
            } else if (col == ACTION_RECORD) {
                return tr("Record conference until this user leaves");
            } else if (col == ACTION_MUTE) {
                return tr("Mute/UnMute");
            }
        }
        return QVariant();
    }

    switch (col) {
    case ID:
        return member["join_order"].toInt();
    case NUMBER:
        return member["number"].toString();
    case ACTION_RECORD:
        return tr("No");
    case ADMIN:
        return tr("No");
    case NAME:
        return member["name"].toString();
    case ACTION_ALLOW_IN:
        return tr("Yes");
    case SINCE:
        return QDateTime::fromTime_t(
            QDateTime::currentDateTime().toTime_t()
            - member["join_time"].toInt()
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
    if (col != ACTION_MUTE) return Qt::NoItemFlags;

    int row = index.row();
    const QString &number = m_row2number[row];
    const QVariantMap &member = m_members[number].toMap();
    bool isMuted = member["muted"] == "Yes";
    bool isMe = b_engine->isMeetmeMember(m_number, number.toInt());

    if (isMe && col == ACTION_MUTE && isMuted) {
        qDebug() << "Enabling mute button";
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
        setColumnWidth(ActionCol[i], 32);
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
    return;
    // const MeetmeInfo * m = b_engine->meetme(meetme_id);
    // const QVariantMap & channels = (m ? m->channels() : QVariantMap());
    // const QVariantMap & user_chan = channels.value(channel_id).toMap();

    // const QString & usernum = user_chan.value("usernum").toString();
    // QString adminnum;

    // foreach (const QString & key, channels.keys()) {
    //     if (my_userinfo && channels.value(key).toMap().value("isadmin").toBool()
    //         && my_userinfo == b_engine->getUserForXChannelId(key)) {
    //         adminnum = channels.value(key).toMap().value("usernum").toString();
    //     }
    // }

    // switch (index.column()) {
    //     case ACTION_MUTE:
    //     {
    //         qDebug() << Q_FUNC_INFO << "Mute/unmute";
    //         bool is_muted = user_chan.value("ismuted").toBool();
    //         if (is_muted) {
    //             b_engine->meetmeAction("MeetmeUnmute", meetme_id + " "
    //                                    + usernum);
    //         } else {
    //             b_engine->meetmeAction("MeetmeMute", meetme_id + " "
    //                                    + usernum);
    //         }
    //         break;
    //     }
    //     case ACTION_KICK:
    //         qDebug() << Q_FUNC_INFO << "Kick/MeetmeKick";
    //         if (! adminnum.isEmpty()) {
    //             // bool is_authed = user_chan.value("isauthed").toBool();
    //             b_engine->meetmeAction("MeetmeKick", meetme_id + " " +
    //                                    usernum + " " + adminnum);
    //             // if (! is_authed) {
    //             //     b_engine->meetmeAction("MeetmeKick", meetme_id + " " + 
    //             //                            usernum + " " + adminnum);
    //             // } else {
    //             //     b_engine->meetmeAction("kick", meetme_id + " " +
    //             //                            usernum + " " + adminnum);
    //             // }
    //         }
    //         break;
    //     case ACTION_TALK_TO:
    //         qDebug() << Q_FUNC_INFO << "Talk";
    //         if (! adminnum.isEmpty()) {
    //             b_engine->meetmeAction("MeetmeTalk", meetme_id + " " + usernum
    //                                    + " " + adminnum);
    //         }
    //         break;
    //     case ACTION_RECORD:
    //     {
    //         qDebug() << Q_FUNC_INFO << "Record";
    //         bool is_recorded = (c && c->ismonitored());
    //         b_engine->meetmeAction("record", meetme_id + " " +
    //                                          usernum + " " +
    //                                          ( is_recorded ? "stop" : "start"));
    //         break;
    //     }
    //     case ACTION_ALLOW_IN:
    //         qDebug() << Q_FUNC_INFO << "Accept";
    //         if (! adminnum.isEmpty()) {
    //             b_engine->meetmeAction("MeetmeAccept", meetme_id + " " + usernum
    //                                    + " " + adminnum);
    //         }
    //         break;
    //     default:
    //         qDebug() << Q_FUNC_INFO << "No Action";
    //         break;
    // }
}

void ConfRoomView::mousePressEvent(QMouseEvent *event)
{
    lastPressed = event->button();
    QTableView::mousePressEvent(event);
}


ConfRoom::ConfRoom(QWidget *parent, ConfTab *tab, const QString &number, const QVariantMap &members)
    : QWidget(parent), m_number(number)
{
    QVBoxLayout *vBox = new QVBoxLayout(this);
    setLayout(vBox);
    QHBoxLayout *hBox = new QHBoxLayout();
    m_model = new ConfRoomModel(tab, this, number, members);
    QLabel *redondant = new QLabel(tr(" Conference room %1").arg(number));
    setProperty("id", number);

    hBox->addStretch(1);
    hBox->addWidget(redondant, 6);
    hBox->addStretch(1);
    vBox->addLayout(hBox);

    hBox = new QHBoxLayout();

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
}

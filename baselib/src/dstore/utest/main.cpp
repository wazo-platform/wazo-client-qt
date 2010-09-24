/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

#include <QApplication>

#include "dstore.h"
#include "main.h"

void WatchChange::allTreeChange(const QString &path, DStoreEvent event)
{
    qDebug() << "this " <<  path << " node changed" << event;
}

void WatchChange::specificTreeChange(const QString &path, DStoreEvent event)
{
    qDebug() << "this " <<  path << " node changed" << event;
}

//   /phone/[0-9]*/(number|context|agent-id|user-id)
//   /group/[0-9]*/(group-name|user-list)
//   /state/[0-9]*/(name|color|possible-state-list)
//   /user/[0-9]*/(name|group|state-id)
//   /agent/[0-9]*/(name|number|user-id)
//   /conference/[0-9]*/(name|paused|canal-list)
//   /canal/[0-9]*/(source-id|destination-id|start)

QStringList stateNameList;

void addUser(DStore *tree, int id)
{
    QVariantMap user;
    user.insert("id", id);
    user.insert("name", QString("someUserName%0").arg(id));
    user.insert("state-id", qrand()%3);
    tree->populate(QString("user/%1").arg(id), user);
}

void addPhone(DStore *tree, int pid, int uid)
{
    QVariantMap phone;
    phone.insert("id", pid);
    phone.insert("connected", ((qrand()&2)?1:0));
    phone.insert("user-id", uid);

    tree->populate(QString("phone/%1").arg(pid), phone);
}

void addState(DStore *tree, int id)
{
    QString name = QString("ee");
    stateNameList << name;
    
    QVariantMap state;
    state.insert("id", id);
    state.insert("name", name);
    QVariantMap canChangeTo;
    int i;
    for(i=0;i<3;i++) {
        canChangeTo.insert(QString("%1").arg(i).toUtf8().constData(), QVariant());
    }
    state.insert("changable-to", canChangeTo);
    tree->populate(QString("state/%1").arg(id), state);
}


int main(int argc, char *argv[])
{
    DStore *tree = new DStore();
    WatchChange *dog = new WatchChange;
    
    int i, i2;

    tree->mkPath("phone");
    tree->onChange("phone", dog, SLOT(specificTreeChange(const QString &, DStoreEvent)));
    
    for(i=0;i<3;i++) {
        addState(tree, i);
    }
    for(i=0;i<10;i++) {
        addUser(tree, i);
    }
    for(i=0,i2=0;i<5;i++,i2++) {
        addPhone(tree,i2,i);
    }
    for(i=0;i<5;i++,i2++) {
        addPhone(tree,i2,i);
    }
    for(i=4;i<9;i++,i2++) {
        addPhone(tree,i2,i);
    }

    DStore *resulta = tree->extractb(argv[1]);

    qDebug() << DStoreNode::pp(*resulta->root(), 2, 0);
    qDebug() << tree->extractVariant(argv[1]);

    delete resulta;
    delete dog;
    delete tree;
    return 0;
}

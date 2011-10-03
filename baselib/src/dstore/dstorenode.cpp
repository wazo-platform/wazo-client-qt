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

#include "dstore.h"

DStoreNode::DStoreNode(const QString &name,
                       NodeType type,
                       VMapNode *parent)
    : m_name(name), m_parent(parent), m_type(type)
{
    //qDebug() << "build" << name << this;
}

void DStoreNode::construct(DStore *tree)
{
    int getNewUid = true;

    if (parent()) {
        if (parent()->nodeExist(name())) {
            DStoreNode *existingNode = parent()->node(name());
            m_uid = existingNode->uid();
            if (type() == INNER) {
                if (existingNode->type() == INNER) {
                    static_cast<VMapNode*>(this)
                        ->takeChild(static_cast<VMapNode*>(existingNode));

                    existingNode->destroy(tree, 0);
                    getNewUid = 0;
                } else {
                    existingNode->destroy(tree, 0);
                }
            } else {
                if (existingNode->type() != INNER) {
                    m_uid = existingNode->uid();
                    getNewUid = 0;
                }
                existingNode->destroy(tree, 1);
            }
            parent()->remove(name());
        }
    }

    if (getNewUid) {
        m_uid = tree->getUid();
    }

    tree->registerNode(this);

    if (parent()) {
        parent()->addNode(name(), this);
    }
}

VMapNode* DStoreNode::muteToVMapNode(DStore *tree)
{
    if (m_type == INNER) {
        return static_cast<VMapNode*>(this);
    } else {
        QString _name = name();
        VMapNode *_parent = parent();
        delete this;
        return new VMapNode(_name, _parent, tree);
    }
}

static QString _pp(const VMapNode *map, int lvl, int tc, int showUid)
{
    int nspace = lvl * tc;

    QString ret = "(\n";
    foreach (QString nodeName, map->nodeNames()) {
        DStoreNode *node = map->node(nodeName);
        QString uid(QString::number(node->uid()));
        if (!showUid) {
            uid = "";
        }


        if (node->type() == LEAF) {
            QVariant var(static_cast<VNode*>(node)->variant());
            if (var.type() == QVariant::String) {
                ret.append(QString().fill(' ', nspace) + uid +
                           ":" + nodeName + " \"" + var.toString() + "\"\n");
            } else if (var.type() == QVariant::Int) {
                ret.append(QString().fill(' ', nspace) + uid +
                           ":" + nodeName + " " + var.toString() + "\n");
            } else {
                ret.append(QString().fill(' ', nspace) + uid +
                           ":" + nodeName + " ^" + var.toString() + "$\n");
            }
        } else {
            VMapNode *var(static_cast<VMapNode*>(node));
            ret.append(QString().fill(' ', nspace) + uid +
                       ":" + nodeName + " " + _pp(var, lvl + 1, tc, showUid) + "\n");
        }
    }
    ret.append(QString().fill(' ', ((lvl>1)?lvl-1:0) * tc) + ")");

    return ret;
}

QString DStoreNode::pp(const DStoreNode &node, int tc, int showUid)
{
    if (node.type() == INNER) {
        return _pp(static_cast<const VMapNode*>(&node), 0, tc, showUid);
    } else {
        QVariant leaf =
            static_cast<VNode>(*static_cast<const VNode*>(&node)).variant();

        if (leaf.type() == QVariant::String) {
            return QString("\"") + leaf.toString() + "\"";
        } else if (leaf.type() == QVariant::Int) {
            return QString("\"") + leaf.toInt() + "\"";
        } else {
            return QString("|") + leaf.toString() + "|";
        }
    }
}

void DStoreNode::destroy(DStore *tree, int)
{
    //qDebug() << "DStoreNode destroy " << uid() << ":" << name();
    tree->unregisterNode(this);
    delete this;
}

QString DStoreNode::path() const
{
    //qDebug() << "path" << name() << parent() ;
    if (parent()) {
        return DStore::sanitize(parent()->path() + "/" + name());
    } else {
        return name();
    }
}

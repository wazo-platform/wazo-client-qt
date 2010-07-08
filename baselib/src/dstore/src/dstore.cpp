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

#include "dstore.h"

static QRegExp finalSlash, leadingSlash;

DStore::DStore()
    : m_4genUid(0),
      m_path2uid(QHash<QString, qlonglong>()),
      m_uid2node(QHash<qlonglong, DStoreNode*>()),
      m_blockSignal(0),
      m_root(new VMapNode("", NULL, this))
{ 
    leadingSlash = QRegExp("^/*");
    finalSlash = QRegExp("/*$");
}

int DStore::getUid()
{
    while (m_uid2node.contains(++m_4genUid));
    return m_4genUid;
}

QString DStore::sanitize(const QString &path)
{
    return QString(path).remove(leadingSlash)
                        .remove(finalSlash);
}

void DStore::populate(const QString &path, const QVariant &value)
{
    QStringList traverseList = sanitize(path).split("/");
    QString baseName = traverseList.takeLast();
    QString dirName = traverseList.join("/");
    VMapNode *dadNode = mkPath(dirName);
  
    if (value.type() == QVariant::Map) {
        VMapNode *node = new VMapNode(baseName, dadNode, this);
        node->populate(value.toMap(), this);
    } else {
        new VNode(baseName, dadNode, this, value);
    }

    if (!m_blockSignal) {
        dynamicInvocation(path, NODE_POPULATED);
    }
}

VMapNode* DStore::mkPath(const QString &path, VMapNode *from)
{
    QStringList traverseList = sanitize(path).split("/");
    VMapNode *traverse = from?from:m_root;

    int i, e;
    for (i=0, e=traverseList.count();i<e;i++) {
        VMapNode *map = VMapNode::getNode(traverseList[i], traverse, this);
        traverse = map;
    }

    if (!m_blockSignal) {
        dynamicInvocation(traverse->path(), NODE_POPULATED);
    }

    return traverse;
}

DStoreNode* DStore::getNode(qlonglong uid)
{
    if (m_uid2node.contains(uid)) {
        return m_uid2node.value(uid);
    } else {
        return NULL;
    }
}

DStoreNode* DStore::getNode(const QString &path)
{
    if (m_path2uid.contains(path)) {
        return getNode(m_path2uid.value(path));
    } else {
        return NULL;
    }
}

void DStore::registerNode(DStoreNode *node)
{
    m_path2uid.insert(node->path(), node->uid());
    m_uid2node.insert(node->uid(), node);
}

void DStore::unregisterNode(DStoreNode *node)
{
    m_path2uid.remove(node->path());
    m_uid2node.remove(node->uid());
}

DStore::~DStore()
{
    root()->destroy(this, 1);
    QHashIterator<QString, DStoreCallback*> i(m_callbackList);

    while (i.hasNext()) {
        i.next();
        delete i.value();
    }

    m_callbackList.clear();
}

void DStore::rmPath(const QString &path)
{
    DStoreNode *node = getNode(path);
    QStringList traverseList = sanitize(path).split("/");
    QString baseName = traverseList.takeLast();

    if (node) {
        if (node->parent()) {
            node->parent()->remove(baseName);
        }
        if (node->type() == INNER) {
            static_cast<VMapNode*>(node)->destroy(this, 1);
        } else {
            static_cast<VNode*>(node)->destroy(this, 1);
        }
    }

    if (!m_blockSignal) {
        dynamicInvocation(path, NODE_REMOVED);
    }
}

void DStore::filter(Op0p op, const QString &filter, const QVariant &with)
{
    VMapNode *r = root();
    foreach(QString nodeName, r->nodeNames()) {
        DStoreNode *dnode = r->node(nodeName);
        if (dnode->type() == INNER) {
            QVariant value = static_cast<VMapNode *>(dnode)->variant(filter);
            if ((value == with) ^ (op == IS_EQUAL)) {
                rmPath(nodeName);
            }
        }
    }
}

// path      = nodepath, { "/*[", filter, "]"}? ;
// filter    = nodepath, test, ( nodepath | value ) ;
// test      = ("=" | "~") ;
// value     = "@", [^]]*;
// quote     = "'" ;
// nodepath  = nodename, { "/", nodename } ;
// nodename  = [^/*=~]+ ;



QString readNodePath(const QString &s)
{
    QString nodePath = s;
    int i, e;
    for (i=0,e=s.size();i<e;i++) {
        if ((s[i] == QChar('*')) ||
            (s[i] == QChar('=')) ||
            (s[i] == QChar('~')))
            break;
    }
    if (i<e) {
        if ((i)&&(s[i-1]==QChar('/'))) {
            nodePath = nodePath.left(i-1);
        } else {
            nodePath = nodePath.left(i);
        }
    }
    return nodePath;
}

DStore* DStore::extract(const QString &path)
{
    QString cleanPath = sanitize(path);
    QString baseNodePath = readNodePath(cleanPath);
    DStore *tree = new DStore();

    DStoreNode *node = getNode(baseNodePath);

    if (node != NULL) {
        node->clone(tree, tree->root(), 1);
    }

    if (cleanPath.indexOf("[") != -1) {
        QString filter = path.right(cleanPath.size() -
                                    baseNodePath.size() - 3);
        QString path = readNodePath(filter);
        QVariant with;

        if (filter[path.size()+1] == QChar('@')) {
            with = QVariant(filter.mid(path.size()+2, filter.size()-path.size()-3));
        } else { 
            with = root()->variant(filter.mid(path.size()+1, filter.size()-path.size()-2));
        }
        tree->filter(((filter[path.size()] == QChar('='))?
                       IS_EQUAL:IS_DIFFERENT), path, with);
    }
    return tree;
}

QVariant DStore::extractVariant(const QString &path)
{
    DStore *tree = extract(path);
    QVariant ret = tree->root()->variantMap();
    delete tree;
    return ret;
}

void DStore::onChange(const QString &path, QObject *target, const char *slot)
{
    m_callbackList.insert(sanitize(path), new DStoreCallback(target, slot));
}

void DStore::dynamicInvocation(const QString &path, DStoreEvent event)
{
    QString triggerPath = sanitize(path);
    QStringList traverseList = triggerPath.split("/");

    do {
        if (m_callbackList.contains(triggerPath)) {
            QList<DStoreCallback*> cbList = m_callbackList.values(triggerPath);
            int i, e;
            for (i=0, e=cbList.size();i<e;++i) {
                cbList.at(i)->call(path, event);
            }
        }

        traverseList.removeLast();
        triggerPath = traverseList.join("/");

    } while (traverseList.count());
}

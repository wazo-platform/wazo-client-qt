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

DStore* DStore::filter(Op0p op, const QString &filter, const QVariant &with)
{
    /*
    VMapNode *r = root();

    foreach(QString nodeName, r->nodeNames()) {
        DStoreNode *dnode = r->node(nodeName);
        if (dnode->type()==INNER) {
            VNode *node = static_cast<VMapNode *>(dnode)->getVNode(filter);
            if ((node)&&(node->variant() == with)) {
            }
        }
    }
    */

    return NULL;
}

DStore* DStore::extract(const QString &path)
{
    QString baseNodePath = sanitize(path);
    DStore *tree = new DStore();
    int index;
    
    if ((index = baseNodePath.indexOf("*")) != -1) {
      baseNodePath = baseNodePath.left(index-1);
    }
    
    DStoreNode *node = getNode(baseNodePath);
    
    if (node != NULL) {
        node->clone(tree, tree->root(), 1);
    }
    
    return tree;
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

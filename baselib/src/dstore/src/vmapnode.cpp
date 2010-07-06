#include "dstore.h"

VMapNode::VMapNode(const QString &name,
                   VMapNode *parent,
                   DStore *tree,
                   const QVariantMap &map)
    : DStoreNode(name, INNER, parent),
      m_nodeMap(new QMap<QString, DStoreNode**>())
{
    construct(tree);
    populate(map, tree);
}

void VMapNode::populate(const QVariantMap &map, DStore *tree)
{
    foreach(QString nodeName, map.keys()) {
        QVariant toInsert = map.value(nodeName);
        if (toInsert.type() == QVariant::Map) {
            new VMapNode(nodeName, this, tree, toInsert.toMap());
        } else {
            new VNode(nodeName, this, tree, toInsert);
        }
    }
}

VMapNode* VMapNode::getNode(const QString &name, VMapNode *parent, DStore *tree)
{
    if (parent->nodeExist(name)) {
        return parent->node(name)->muteToVMapNode(tree);
    }
    return new VMapNode(name, parent, tree);
}

QStringList VMapNode::nodeNames() const
{
    return m_nodeMap->keys();
}

int VMapNode::count()
{
    return m_nodeMap->count();
}

int VMapNode::nodeExist(const QString &name)
{
    return m_nodeMap->contains(name);
}

DStoreNode* VMapNode::node(const QString &name) const
{
    return *m_nodeMap->value(name);
}

int VMapNode::remove(const QString &nodeName)
{
    if (nodeExist(nodeName)) {
        int found = 0;
        DStoreNode **node = m_nodeMap->value(nodeName);
        found = m_nodeMap->remove(nodeName);
        delete node;
        return found;
    } else {
      return 0;
    }
}

void VMapNode::destroyNodeList()
{

  foreach (QString nodeName, m_nodeMap->keys()) {
      delete m_nodeMap->value(nodeName);
  }

  delete m_nodeMap;
}

void VMapNode::takeChild(VMapNode *node)
{
    DStoreNode *childNode;

    foreach (QString nodeName, node->nodeNames()) {
        childNode = node->node(nodeName);
        childNode->setParent(this);
        addNode(nodeName, childNode);
    }
}

void VMapNode::destroy(DStore *tree, int destroyNode)
{
    //qDebug() << "VMapNode destroy " << uid() << ":" << name();
    if (destroyNode) {
        foreach (QString nodeName, nodeNames()) {
            node(nodeName)->destroy(tree, destroyNode);
        }
    }
    destroyNodeList();

    DStoreNode::destroy(tree, destroyNode);
}

void VMapNode::addNode(const QString &name, DStoreNode *node)
{
    m_nodeMap->insert(name, new DStoreNode*(node));
}

DStoreNode* VMapNode::clone(DStore* tree, VMapNode *parent, int ripDad)
{
    VMapNode *cnode;

    //qDebug() << "clone" << name() << this;

    if (ripDad) {
        cnode = parent;
    } else {
        cnode = new VMapNode(name(), parent, tree);
    }

    foreach (QString nodeName, nodeNames()) {
        node(nodeName)->clone(tree, cnode, 0);
    }

    return cnode;
}

VNode* VMapNode::getVNode(const QString &path)
{
    QStringList traverseList = DStore::sanitize(path).split("/");
    QString baseName = traverseList.takeLast();
    QString dirName = traverseList.join("/");

    VMapNode *traverse = this;
    int i, e;
    for (i=0, e=traverseList.count();i<e;i++) {
        if (traverse->nodeExist(traverseList[i])) {
            DStoreNode *cnode = traverse->node(traverseList[i]);
            if (i==e-1) {
                if (cnode->type() == LEAF) {
                    return static_cast<VNode*>(cnode);
                } else {
                    return NULL;
                }
            } else {
                if (cnode->type() == INNER) {
                  traverse = static_cast<VMapNode*>(cnode);
                } else {
                  return NULL;
                }
            }
        }
    }

    return NULL;
}

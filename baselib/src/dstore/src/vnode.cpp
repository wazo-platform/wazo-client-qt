#include "dstore.h"

VNode::VNode(const QString &name,
             VMapNode *parent,
             DStore *tree,
             const QVariant &node)
    : DStoreNode(name, LEAF, parent), m_variant(node)
{
    construct(tree);
}

void VNode::destroy(DStore *tree, int recurse)
{
    m_variant.clear();
    DStoreNode::destroy(tree, recurse);
}

DStoreNode* VNode::clone(DStore* tree, VMapNode *parent, int)
{
    //qDebug() << "new VNode";
    return new VNode(name(), parent, tree, this->variant());
}

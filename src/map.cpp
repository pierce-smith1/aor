#include "map.h"

MapNode::MapNode(MapNode *parent)
    : parent(parent)
{
    if (parent != nullptr) {
        parent->children.push_back(this);
        index_in_parent = parent->children.size() - 1;
    }
}

MapNode::~MapNode() {
    for (MapNode *child : children) {
        delete child;
    }
}

WorldMap::WorldMap() {
    m_root = new MapNode(nullptr);

    m_root->children.push_back(new MapNode(m_root));
    m_root->children.push_back(new MapNode(m_root));

    m_root->children[0]->children.push_back(new MapNode(m_root->children[0]));
}

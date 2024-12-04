#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsPathItem>
#include "Node.h"

class Edge : public QGraphicsPathItem {
public:
    Edge(Node *start, Node *end, QGraphicsItem *parent = nullptr);


private:
    Node *startNode;
    Node *endNode;
};

#endif // EDGE_H

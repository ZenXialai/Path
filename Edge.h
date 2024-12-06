#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsPathItem>
#include "Node.h"

class Edge : public QGraphicsPathItem {
public:
    Edge(Node *start, Node *end, double length,QGraphicsItem *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    Node *startNode;
    Node *endNode;
    double length;
};

#endif // EDGE_H

#include "Edge.h"
#include <QPainterPath>
#include <QPen> // 确保包含 QPen 的头文件

Edge::Edge(Node *start, Node *end, QGraphicsItem *parent)
    : QGraphicsPathItem(parent), startNode(start), endNode(end) {
    setPen(QPen(Qt::black, 2)); // 设置路径为黑色，宽度为 2

}


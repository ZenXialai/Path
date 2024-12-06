#include "Edge.h"
#include <QPainterPath>
#include <QPen> // 确保包含 QPen 的头文件
#include <QPainter>
Edge::Edge(Node *start, Node *end, double length, QGraphicsItem *parent)
    : QGraphicsPathItem(parent), startNode(start), endNode(end), length(length) {
    setPen(QPen(Qt::black, 2)); // 设置路径为黑色，宽度为 2
}


void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // 调用基类的绘制函数，绘制路径
    QGraphicsPathItem::paint(painter, option, widget);

    // 设置文本颜色和字体大小
    painter->setPen(Qt::black);
    QFont font = painter->font();
    font.setPointSize(8);  // 设置文本大小为 8（可以根据需要调整）
    painter->setFont(font);

    // 计算路径中点的位置
    QPointF startPos = startNode->getPosition();
    QPointF endPos = endNode->getPosition();
    QPointF midPoint = (startPos + endPos) / 2;

    // 绘制路径长度，四舍五入到小数点后一位
    painter->drawText(midPoint, QString::number(length, 'f', 1));
}

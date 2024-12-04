#include "Node.h"
#include <QBrush>

Node::Node(const QString &name, const QPointF &position, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent), name(name), position(position) {
    setRect(position.x() - 5, position.y() - 5, 10, 10);
    setBrush(Qt::blue);
}

QString Node::getName() const {
    return name;
}

QPointF Node::getPosition() const {
    return position;
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(this); // 触发点击信号
    }
    QGraphicsEllipseItem::mousePressEvent(event);
}

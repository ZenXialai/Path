#include "Node.h"
#include <QBrush>

// Node::Node(const QString &name, const QPointF &position, QGraphicsItem *parent)
//     : QGraphicsEllipseItem(parent), name(name), position(position) {
//     setRect(position.x() - 5, position.y() - 5, 15, 15);
//     setBrush(Qt::blue);
//     setFlag(QGraphicsItem::ItemIsSelectable);  // 设置节点可以被选中
// }

// QString Node::getName() const {
//     return name;
// }

// QPointF Node::getPosition() const {
//     return position;
// }

// void Node::mousePressEvent(QGraphicsSceneMouseEvent *event) {
//     if (event->button() == Qt::LeftButton) {
//         emit clicked(this); // 触发点击信号
//     }
//     QGraphicsEllipseItem::mousePressEvent(event);
// }



Node::Node(const QString &name, const QPointF &position, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent), name(name), position(position) {
    // 设置节点的外观，例如设置节点的圆形大小
    setRect(-10, -10, 20, 20);  // 定义节点为直径20的圆形
    setPos(position);           // 设置节点的位置
    setBrush(Qt::blue);         // 设置节点的默认颜色
    setFlag(QGraphicsItem::ItemIsSelectable);  // 设置节点可以被选中
    // setFlag(QGraphicsItem::ItemIsMovable);     // 让节点可以被移动（可选）
    setAcceptedMouseButtons(Qt::LeftButton);  // 接受鼠标左键点击
}

QString Node::getName() const {
    return name;
}

QPointF Node::getPosition() const {
    return pos();  // 返回节点的位置
}

// 实现鼠标点击事件
void Node::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsEllipseItem::mousePressEvent(event);  // 调用基类的鼠标点击事件处理，确保正常响应
    emit clicked(this);  // 发出节点点击信号
}

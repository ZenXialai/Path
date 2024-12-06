#include "Node.h"
#include <QBrush>
#include <QPainter>
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

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // 调用基类的绘制函数，绘制节点圆形
    QGraphicsEllipseItem::paint(painter, option, widget);

    // 设置文本颜色
    painter->setPen(Qt::blue);
    QFont font = painter->font();
    font.setPointSize(8);  // 设置文本大小为 8（可以根据需要调整）
    painter->setFont(font);
    // 绘制节点名称，名称位置可以调整（这里绘制在节点的正下方，偏移一点）
    painter->drawText(boundingRect().bottomLeft() + QPointF(-5, 15), name);
}

void Node::setBlinking(bool blink) {
    setVisible(blink);  // 通过设置可见性来实现闪烁
}

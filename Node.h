#ifndef NODE_H
#define NODE_H

#include <QGraphicsEllipseItem>
#include <QObject>
#include <QGraphicsSceneMouseEvent>

class Node : public QObject, public QGraphicsEllipseItem {
    Q_OBJECT

public:
    explicit Node(const QString &name, const QPointF &position, QGraphicsItem *parent = nullptr);

    QString getName() const;         // 获取节点名称
    QPointF getPosition() const;     // 获取节点坐标

signals:
    void clicked(Node *node);        // 点击信号

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString name;                    // 节点名称
    QPointF position;                // 节点坐标
};

#endif // NODE_H

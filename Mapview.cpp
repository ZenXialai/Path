#include "MapView.h"


MapView::MapView(QWidget *parent)
    : QGraphicsView(parent) {
    // 启用拖拽模式
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void MapView::wheelEvent(QWheelEvent *event) {
    // 放大/缩小因子
    const double scaleFactor = 1.2;

    if (event->angleDelta().y() > 0) {
        // 滚轮向上放大
        scale(scaleFactor, scaleFactor);
    } else {
        // 滚轮向下缩小
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void MapView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        emit nodeAdded(mapToScene(event->pos())); // 右键点击添加节点
    } else if (isDrawingPath && event->button() == Qt::LeftButton) {
        QPointF scenePos = mapToScene(event->pos());
        currentPathPoints.append(scenePos);
        scene()->addEllipse(scenePos.x() - 2, scenePos.y() - 2, 4, 4,
                            QPen(Qt::red), QBrush(Qt::red)); // 显示路径点
    }
    QGraphicsView::mousePressEvent(event);
}

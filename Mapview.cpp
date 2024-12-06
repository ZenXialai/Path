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
    }
    QGraphicsView::mousePressEvent(event);
}

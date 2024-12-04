#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>
class MapView : public QGraphicsView {
    Q_OBJECT

public:
    explicit MapView(QWidget *parent = nullptr);

signals:
    void nodeAdded(const QPointF &position);              // 右键添加节点信号
    void pathCompleted(const QList<QPointF> &points);     // 路径绘制完成信号

protected:
    void mousePressEvent(QMouseEvent *event) override;


    void wheelEvent(QWheelEvent *event) override; // 支持鼠标滚轮缩放
private:
    QList<QPointF> currentPathPoints; // 当前路径点
    bool isDrawingPath;               // 是否正在绘制路径
};

#endif // MAPVIEW_H

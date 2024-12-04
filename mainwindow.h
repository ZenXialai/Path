#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "MapView.h"
#include "Node.h"
#include "ui_mainwindow.h"  // 包含生成的 UI 头文件

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadMap(const QString &mapPath);  // 加载地图背景
    void loadData(const QString &filePath); // 加载节点和路径数据
    void saveData(const QString &filePath); // 保存节点和路径数据


private slots:
    void addNode(const QPointF &position); // 添加节点
    void startPathDrawing(Node *node);     // 开始路径绘制
    void finishPathDrawing(Node *node);    // 完成路径绘制
    void onFindPathButtonClicked(); // 处理“寻路”按钮点击的槽函数
    void handleNodeClicked(Node* node); // 处理节点点击事件
    void resetPathHighlights(); // 重置所有路径和节点的高亮状态
private:
    bool isFindingPath;           // 标识当前是否处于寻路模式
    Node *StartNode;       // 当前选中的路径起点
    Node *EndNode;         // 当前选中的路径终点
    Ui::MainWindow *ui; // 指向 UI 界面的指针
    QGraphicsScene *scene; // 场景用于显示地图内容


    struct NodeData {
        QString name;
        QPointF position;
    };

    struct EdgeData {
        QString startNode;
        QString endNode;
        QList<QPointF> points;
        double length;
        QGraphicsPathItem *pathItem; // 添加一个指针来存储路径对象
    };


    QList<NodeData> nodes;   // 节点数据列表
    QList<EdgeData> edges;   // 路径数据列表
    int nodeCount;           // 节点计数器

    Node *currentStartNode;  // 路径绘制起点
    QList<QPointF> tempPoints; // 临时路径点
};

#endif // MAINWINDOW_H

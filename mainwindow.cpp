#include "mainwindow.h"
#include "ui_mainwindow.h"  // 包含生成的 UI 头文件
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <cmath>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), scene(new QGraphicsScene(this)), nodeCount(0), currentStartNode(nullptr), StartNode(nullptr), EndNode(nullptr), isFindingPath(false) {
    // 加载 UI
    ui->setupUi(this);

    // 将 scene 绑定到 ui 中的 graphicsView（即 MapView）
    ui->graphicsView->setScene(scene);

    // 加载地图背景
    loadMap("C:/Users/刘夏莱/Desktop/640.png");

    // 连接 MapView 的信号槽
    connect(ui->graphicsView, &MapView::nodeAdded, this, &MainWindow::addNode);

    // 连接寻路按钮的信号槽
    connect(ui->findPathButton, &QPushButton::clicked, this, &MainWindow::onFindPathButtonClicked);

    // 加载节点和路径数据
    loadData("data.txt");
}

MainWindow::~MainWindow() {
    // 保存节点和路径数据到文件
    saveData("data.txt");
    delete ui;  // 清理 UI 对象
}

void MainWindow::loadMap(const QString &mapPath) {
    QPixmap map(mapPath);
    if (!map.isNull()) {
        scene->addPixmap(map); // 添加地图背景到场景
    } else {
        qDebug() << "Failed to load map from:" << mapPath;
    }
}

void MainWindow::addNode(const QPointF &position) {
    // 弹出对话框，让用户输入节点名称
    bool ok;
    QString nodeName = QInputDialog::getText(this, "添加节点", "输入节点名称:",
                                             QLineEdit::Normal, QString("节点%1").arg(++nodeCount), &ok);

    if (ok && !nodeName.isEmpty()) {
        // 创建并显示节点
        Node *node = new Node(nodeName, position);
        nodes.append({nodeName, position});
        scene->addItem(node);

        // 连接节点点击信号到新的槽函数 handleNodeClicked()
        connect(node, &Node::clicked, this, &MainWindow::handleNodeClicked);
    } else {
        // 如果用户取消输入，还原计数
        --nodeCount;
    }
}

void MainWindow::startPathDrawing(Node *node) {
    if (!currentStartNode) {
        currentStartNode = node; // 设置路径起点
        tempPoints.clear();
        tempPoints.append(node->getPosition());
        return;
    }
}
void MainWindow::finishPathDrawing(Node *node) {
    if (currentStartNode && node != currentStartNode) {
        // 添加路径终点
        tempPoints.append(node->getPosition());
        QPainterPath path(tempPoints.first());

        // 计算路径总长度
        double totalLength = 0.0;
        for (int i = 1; i < tempPoints.size(); ++i) {
            path.lineTo(tempPoints[i]);
            totalLength += std::sqrt(std::pow(tempPoints[i].x() - tempPoints[i - 1].x(), 2) +
                                     std::pow(tempPoints[i].y() - tempPoints[i - 1].y(), 2));
        }

        // 绘制路径
        QGraphicsPathItem *pathItem = new QGraphicsPathItem(path);
        pathItem->setPen(QPen(Qt::black, 2));
        scene->addItem(pathItem);

        // 保存路径信息
        edges.append({currentStartNode->getName(), node->getName(), tempPoints, totalLength, pathItem});
        currentStartNode = nullptr; // 重置路径起点
    }
}


void MainWindow::loadData(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for loading:" << filePath;
        return;
    }

    QTextStream in(&file);
    QString line;
    bool readingNodes = false, readingEdges = false;

    while (!in.atEnd()) {
        line = in.readLine().trimmed();

        if (line == "[Nodes]") {
            readingNodes = true;
            readingEdges = false;
            continue;
        } else if (line == "[Edges]") {
            readingNodes = false;
            readingEdges = true;
            continue;
        }

        if (readingNodes) {
            auto parts = line.split(",");
            if (parts.size() == 3) {
                QString name = parts[0];
                QPointF position(parts[1].toDouble(), parts[2].toDouble());

                // 创建并显示节点
                Node *node = new Node(name, position);
                scene->addItem(node);
                nodes.append({name, position});

                // 连接节点点击信号
                connect(node, &Node::clicked, this, &MainWindow::handleNodeClicked);
            }
        } else if (readingEdges) {
            auto parts = line.split(",");
            if (parts.size() >= 4) {
                QString startNode = parts[0];
                QString endNode = parts[1];
                double length = parts[2].toDouble();

                QList<QPointF> points;
                for (int i = 3; i < parts.size(); ++i) {
                    auto coords = parts[i].split(" ");
                    if (coords.size() == 2) {
                        points.append(QPointF(coords[0].toDouble(), coords[1].toDouble()));
                    }
                }

                // 在场景中绘制路径
                QPainterPath path(points.first());
                for (int i = 1; i < points.size(); ++i) {
                    path.lineTo(points[i]);
                }

                QGraphicsPathItem *pathItem = new QGraphicsPathItem(path);
                pathItem->setPen(QPen(Qt::black, 2));
                scene->addItem(pathItem);

                // 保存路径信息到 edges 列表中，包括绘制的图形对象指针
                edges.append({startNode, endNode, points, length, pathItem});
            }
        }
    }

    file.close();
}

void MainWindow::saveData(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for saving:" << filePath;
        return;
    }

    QTextStream out(&file);

    // 保存节点数据
    out << "[Nodes]\n";
    for (const auto &node : nodes) {
        out << node.name << "," << node.position.x() << "," << node.position.y() << "\n";
    }

    // 保存路径数据
    out << "[Edges]\n";
    for (const auto &edge : edges) {
        out << edge.startNode << "," << edge.endNode << "," << edge.length;
        for (const auto &point : edge.points) {
            out << "," << point.x() << " " << point.y();
        }
        out << "\n";
    }

    file.close();
}

void MainWindow::onFindPathButtonClicked() {
    isFindingPath = !isFindingPath; // 切换寻路状态
    if (isFindingPath) {
        ui->findPathButton->setText("寻路中...");  // 按钮文本变为“寻路中...”
        ui->statusLabel->setText("请选择起点");   // 提示用户选择起点
    } else {
        ui->findPathButton->setText("寻路");       // 恢复按钮文本
        resetPathHighlights();                    // 重置所有路径和节点的颜色
    }
}

void MainWindow::handleNodeClicked(Node *node) {
    if (isFindingPath) {
        if (!StartNode) {
            // 设置起点
            StartNode = node;
            node->setBrush(Qt::green);  // 起点节点变为绿色
            ui->statusLabel->setText("请选择终点");
        } else if (StartNode && node != StartNode) {
            // 设置终点
            EndNode = node;
            node->setBrush(Qt::red);  // 终点节点变为红色
            ui->statusLabel->setText("最短路径查找中...");
            // 在此处可以调用算法接口进行最短路径查找
            // findShortestPath(StartNode, EndNode);
        }
    }
}

void MainWindow::resetPathHighlights() {
    if (StartNode) {
        StartNode->setBrush(Qt::blue);  // 恢复起点节点的颜色
        StartNode = nullptr;            // 重置起点节点
    }
    if (EndNode) {
        EndNode->setBrush(Qt::blue);    // 恢复终点节点的颜色
        EndNode = nullptr;              // 重置终点节点
    }

    // 恢复所有路径的默认颜色
    for (auto &edge : edges) {
        if (edge.pathItem) {
            edge.pathItem->setPen(QPen(Qt::black, 2));  // 恢复路径颜色
        }
    }

    ui->statusLabel->clear();  // 清除状态栏提示信息
}

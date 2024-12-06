#include "mainwindow.h"
#include "ui_mainwindow.h"  // 包含生成的 UI 头文件
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <cmath>
#include <QPixmap>
#include<queue>
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
    for (auto &nodeData : nodes) {
        connect(nodeData.nodePtr, &Node::clicked, this, &MainWindow::handlePathDrawing);
    }
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
    bool ok;
    QString nodeName = QInputDialog::getText(this, "添加节点", "输入节点名称:",
                                             QLineEdit::Normal, QString("节点%1").arg(++nodeCount), &ok);


    if (ok && !nodeName.isEmpty()) {
        // 创建并显示节点
        Node *newNode = new Node(nodeName, position);
        scene->addItem(newNode);

        // 存储新节点的信息
        nodes.append({nodeName, position, newNode});

        // 将新节点的点击信号连接到 handlePathDrawing()
        connect(newNode, &Node::clicked, this, &MainWindow::handlePathDrawing);

        // 重新连接已有节点的信号槽，使其都能参与路径绘制
        for (auto &nodeData : nodes) {
            if (nodeData.nodePtr != newNode) {
                // 断开旧的信号连接，防止重复连接
                disconnect(nodeData.nodePtr, &Node::clicked, this, &MainWindow::handlePathDrawing);
                connect(nodeData.nodePtr, &Node::clicked, this, &MainWindow::handlePathDrawing);
            }
        }

        qDebug() << "Node added and signal-slot connected for node:" << nodeName;
    } else {
        --nodeCount;
    }
}

void MainWindow::handlePathDrawing(Node *node) {
    if (!node) {
        qDebug() << "Node is nullptr!";
        return;
    }
    if(isFindingPath)
    {
        return;
    }
    qDebug() << "Clicked node:" << node->getName();  // 输出被点击节点的名称

    if (!currentStartNode) {
        startPathDrawing(node);
    } else if (currentStartNode && currentStartNode != node) {
        finishPathDrawing(node);
    }
}

void MainWindow::startPathDrawing(Node *node) {
    if (!node) {
        qDebug() << "Start node is nullptr!";
        return;
    }

    currentStartNode = node;  // 设置路径起点
    tempPoints.clear();
    tempPoints.append(node->getPosition());

    qDebug() << "Start drawing from node:" << currentStartNode->getName();
}

void MainWindow::finishPathDrawing(Node *node) {
    if (!currentStartNode || !node) {
        qDebug() << "Finish node is nullptr!";
        return;
    }

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
        scene->addItem(pathItem);  // 确保将路径添加到场景中

        // 保存路径信息
        edges.append({currentStartNode->getName(), node->getName(), tempPoints, totalLength, pathItem});

        // 重置起点以便进行下次路径绘制
        currentStartNode = nullptr;

        qDebug() << "Finished drawing to node:" << node->getName();
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
                 nodes.append({name, position, node});

                // 连接节点点击信号
                connect(node, &Node::clicked, this, &MainWindow::handleNodeClicked);
                connect(node, &Node::clicked, this, &MainWindow::handlePathDrawing);
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
                adjacencyList[startNode].append(qMakePair(endNode, length));
                adjacencyList[endNode].append(qMakePair(startNode, length));
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
    isFindingPath = !isFindingPath;  // 切换寻路状态
    if (isFindingPath) {
        ui->findPathButton->setText("寻路中...");
        ui->statusLabel->setText("   请选择起点……");
        // 重新连接节点点击信号，用于寻路模式
        for (auto &node : nodes) {
            connect(node.nodePtr, &Node::clicked, this, &MainWindow::handleNodeClicked);
        }
    } else {
        ui->findPathButton->setText("寻路");
        resetPathHighlights();  // 重置所有路径和节点的颜色
        // 重新连接节点点击信号，用于默认模式
        for (auto &node : nodes) {
            connect(node.nodePtr, &Node::clicked, this, &MainWindow::handlePathDrawing);
        }
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

            // 第一步：调用Dijkstra算法计算最短路径
            QList<Node*> shortestPath = findShortestPathDijkstra(StartNode, EndNode);

            // 第二步：高亮显示路径
            highlightShortestPath(shortestPath);
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
void MainWindow::highlightPathForTest(Node* start, Node* end) {
    for (auto &edge : edges) {
        if ((edge.startNode == start->getName() && edge.endNode == end->getName()) ||
            (edge.startNode == end->getName() && edge.endNode == start->getName())) {
            // 找到边，设置高亮颜色
            if (edge.pathItem) {
                edge.pathItem->setPen(QPen(Qt::red, 2)); // 高亮为红色
                edge.isHighlighted = true;
            }
            return; // 找到后就可以退出
        }
    }
    // 如果没有找到路径，可以在 statusLabel 上显示提示信息
    ui->statusLabel->setText("没有找到直接连接的路径。");
}

QList<Node*> MainWindow::findShortestPathDijkstra(Node* start, Node* end) {
    QMap<QString, double> distances;
    QMap<QString, QString> previous;
    QSet<QString> visited;

    for (const auto& node : nodes) {
        distances[node.name] = std::numeric_limits<double>::infinity();
    }
    distances[start->getName()] = 0.0;

    auto compare = [&](const QString &left, const QString &right) {
        return distances[left] > distances[right];
    };
    std::priority_queue<QString, QList<QString>, decltype(compare)> queue(compare);
    queue.push(start->getName());

    while (!queue.empty()) {
        QString current = queue.top();
        queue.pop();

        if (current == end->getName()) {
            break; // 找到最短路径
        }

        if (visited.contains(current)) {
            continue;
        }
        visited.insert(current);

        for (const auto &neighbor : adjacencyList[current]) {
            QString neighborNode = neighbor.first;
            double edgeWeight = neighbor.second;

            if (visited.contains(neighborNode)) {
                continue;
            }

            double newDist = distances[current] + edgeWeight;
            if (newDist < distances[neighborNode]) {
                distances[neighborNode] = newDist;
                previous[neighborNode] = current;
                queue.push(neighborNode);
            }
        }
    }

    // 生成路径
    QList<Node*> path;
    QString current = end->getName();
    while (previous.contains(current)) {
        for (auto &node : nodes) {
            if (node.name == current) {
                path.prepend(new Node(node.name, node.position));
                break;
            }
        }
        current = previous[current];
    }

    for (auto &node : nodes) {
        if (node.name == start->getName()) {
            path.prepend(start);
            break;
        }
    }

    return path;
}
void MainWindow::highlightShortestPath(const QList<Node*>& path) {
    for (int i = 0; i < path.size() - 1; ++i) {
        QString startNodeName = path[i]->getName();
        QString endNodeName = path[i + 1]->getName();

        // 遍历所有的 edges，找到连接这些节点的边
        for (auto &edge : edges) {
            if ((edge.startNode == startNodeName && edge.endNode == endNodeName) ||
                (edge.startNode == endNodeName && edge.endNode == startNodeName)) {
                if (edge.pathItem) {
                    edge.pathItem->setPen(QPen(Qt::red, 2)); // 高亮路径为红色
                    edge.isHighlighted = true;
                }
            }
        }
    }
}

void MainWindow::handleNodeClickForPathDrawing(Node *node) {
    if (!node) {
        qDebug() << "handleNodeClickForPathDrawing: Node is nullptr!";
        return;
    }

    // 如果 currentStartNode 为空，则将当前节点设为起点
    if (!currentStartNode) {
        currentStartNode = node;
        qDebug() << "Path drawing started from node:" << currentStartNode->getName();
    }
    // 如果 currentStartNode 已经存在，且点击的节点不是起点，则设为终点并绘制路径
    else if (currentStartNode && currentStartNode != node) {
        finishPathDrawing(node);
        qDebug() << "Path drawing finished to node:" << node->getName();
    }
}



# Path · 中国石油大学（华东）校园导航系统

一个用 **Qt 6 + C++17** 写的校园地图寻路演示程序：把校园里的地点抽象成图，在图上点两个地点，自动用 Dijkstra 算出最短路线并高亮出来。

没有数据库、没有网络请求、没有第三方依赖 —— 整个程序就是 5 个源文件 + 一份纯文本地图数据。

- 语言 / 框架：C++17、Qt 6.5.3（Qt Widgets + QGraphicsView 场景图）
- 构建系统：qmake（`Path.pro`）
- 规模：约 1100 行代码，15 个文件
- 数据：`data.txt` 中 **73 个节点、105 条边**，覆盖体育馆、校医院、图书馆、工科楼 A–E 座、1–17 号学生公寓、各餐厅、各校门等

---

## 目录

- [功能](#功能)
- [界面与操作](#界面与操作)
- [快速开始](#快速开始)
- [地图背景图（必读）](#地图背景图必读)
- [数据文件格式](#数据文件格式)
- [代码结构](#代码结构)
- [寻路算法](#寻路算法)
- [已知限制](#已知限制)
- [后续可做](#后续可做)
- [提交历史](#提交历史)
- [许可证](#许可证)

---

## 功能

| # | 功能 | 说明 |
|---|------|------|
| 1 | 地图浏览 | 滚轮缩放（每格 1.2 倍）、左键拖拽平移 |
| 2 | 最短路径查询 | 点「寻路」进入寻路模式，点起点（变绿）再点终点（变红），Dijkstra 求解后把经过的边加粗染红 |
| 3 | 手动连边 | 非寻路状态下依次点击两个节点，画一条直线边并记录进 `edges` |
| 4 | 添加节点 | 在地图空白处**右键**，输入名称后新增节点 |
| 5 | 地点搜索 | 输入完整节点名（回车或点「搜索」），该节点黄蓝闪烁 3 秒；找不到则弹窗提示 |
| 6 | 长度标注 | 每条边的中点显示长度，数值为原始权值 × 46 |
| 7 | 数据持久化 | 启动时读 `data.txt`，退出时把当前节点与边**覆写**回 `data.txt` |

## 界面与操作

窗口标题「中国石油大学（华东）校园导航系统」，中间是占据绝大部分面积的 `MapView` 画布，另有「寻路」按钮、状态提示标签、搜索输入框和「搜索」按钮。

| 操作 | 效果 |
|------|------|
| 滚轮上 / 下 | 放大 / 缩小视图 |
| 左键按住拖动 | 平移地图（`ScrollHandDrag`） |
| 地图空白处右键 | 添加节点（弹出输入框，默认名「节点N」） |
| 左键点节点（普通模式） | 第一次 = 连边起点，第二次 = 终点，画出一条直线边 |
| 点「寻路」按钮 | 进入 / 退出寻路模式（按钮文字变为「寻路中...」） |
| 寻路模式点节点 | 第一次 = 起点（绿），第二次 = 终点（红）并立即求解 |
| 搜索框回车 / 点「搜索」 | 定位并闪烁该节点 |

## 快速开始

### 环境

- Qt **6.5.3**（MinGW 64-bit 套件即可，其它 6.x 版本通常也能编译）
- 支持 C++17 的编译器（MinGW g++ / MSVC / clang 均可）

### 用 Qt Creator

1. `文件 → 打开文件或项目`，选择 `Path.pro`
2. 配置套件（如 `Desktop Qt 6.5.3 MinGW 64-bit`）
3. 点绿色三角运行

### 命令行（MinGW）

```bat
set PATH=C:\Qt\6.5.3\mingw_64\bin;C:\Qt\Tools\mingw1120_64\bin;%PATH%
qmake Path.pro
mingw32-make -j
release\Path.exe
```

### 运行时的两个注意点

1. **`data.txt` 必须位于进程的当前工作目录**。`loadData("data.txt")` / `saveData("data.txt")` 用的都是相对路径（`mainwindow.cpp:37`、`mainwindow.cpp:42`）。用 Qt Creator 的 shadow build 时，把 `data.txt` 复制到构建输出目录，或直接在源码目录运行。
2. **首次运行会没有底图**，原因见下一节。

## 地图背景图（必读）

底图路径是**硬编码的绝对路径**，且这张图片没有随仓库提交：

```cpp
// mainwindow.cpp:26
loadMap("C:/Users/刘夏莱/Desktop/640.png");
```

也就是说，换一台机器直接运行只会得到一张空白画布（节点仍按 `data.txt` 里的像素坐标摆好，只是没有底图）。想恢复底图：

1. 把校园平面图放进项目，例如 `assets/campus.png`；
2. 把上面那一行改成相对路径 `loadMap("assets/campus.png");`（若想彻底摆脱路径问题，可以把图片加进 Qt 资源文件 `qrc`）。

## 数据文件格式

`data.txt` 是纯文本，分两段，段名必须**单独成行且完全一致**：

```
[Nodes]
名称,x,y
名称,x,y
...
[Edges]
起点名称,终点名称,长度,x1 y1,x2 y2,...
起点名称,终点名称,长度,x1 y1,x2 y2,...
...
```

| 字段 | 含义 |
|------|------|
| 节点 `名称,x,y` | `x y` 为场景像素坐标（原点在左上角），名称就是节点身份，**必须唯一** |
| 边 `起点,终点,长度` | 前三个字段之后是折线的各个点，点与点用逗号分隔，`x` 与 `y` 之间用空格 |
| `长度` | 边的权值（原始单位）。界面显示时乘以 `scaleFactor = 46`（`mainwindow.cpp:66`），所以想标「米」就按米填 |
| 方向 | 无向图：加载时正反两个方向都会写入邻接表 |

真实例子（节选）：

```
[Nodes]
体育馆,308,276
校医院,192,340
玉兰苑餐厅,369,342

[Edges]
体育馆,校医院,3.22727,308 276,192 340
体育馆,玉兰苑餐厅,2.18927,308 276,369 342
```

**写入注意**：程序退出时会用 `saveData()`（`mainwindow.cpp:241`）把内存里的节点和边**整体覆写**回 `data.txt`，格式会被重新序列化（浮点数按默认 6 位有效数字）。手动编辑 `data.txt` 前建议先备份一份。

## 代码结构

| 文件 | 职责 |
|------|------|
| `main.cpp` | 入口，创建 `QApplication` 与主窗口 |
| `mainwindow.h` / `mainwindow.cpp` | 主窗口：加载地图与数据、维护节点/边/邻接表、寻路、连边、搜索、持久化 |
| `Mapview.h` / `Mapview.cpp` | `QGraphicsView` 子类：滚轮缩放、右键发 `nodeAdded`、左键拖拽平移 |
| `Node.h` / `Node.cpp` | `QGraphicsEllipseItem` 子类：直径 20px 的圆形节点 + 下方 8pt 名称标签，点击时发 `clicked(Node*)` |
| `Edge.h` / `Edge.cpp` | `QGraphicsPathItem` 子类：带首尾节点指针、在中点画长度文字。**当前未被使用**（见「已知限制」） |
| `Path.pro` | qmake 工程文件 |
| `data.txt` | 地图数据（节点 + 边） |
| `Path.pro.user` | Qt Creator 的本机配置，含当前机器绝对路径，一般不建议提交 |
| `.gitignore` | 忽略 `build/`、`*.o`、`*.exe`、`*.dll`、`.qtc_clangd/` 等构建产物 |

核心数据结构（`mainwindow.h:42-61`）：

```cpp
struct NodeData { QString name; QPointF position; Node* nodePtr; };
struct EdgeData { QString startNode, endNode; QList<QPointF> points;
                  double length; QGraphicsPathItem* pathItem; bool isHighlighted; };

QList<NodeData> nodes;                                      // 全部节点
QList<EdgeData> edges;                                      // 全部边（含图形对象指针）
QMap<QString, QList<QPair<QString, double>>> adjacencyList; // 邻接表，键为节点名
```

图形与数据是**双份**的：场景里的 `Node`/`QGraphicsPathItem` 负责显示，`nodes`/`edges` 列表负责业务逻辑，两者通过指针关联。

## 寻路算法

`MainWindow::findShortestPathDijkstra(start, end)`（`mainwindow.cpp:373`）：

```cpp
QMap<QString, double> distances;   // 距离表
QMap<QString, QString> previous;   // 前驱表，用于回溯路径
QSet<QString> visited;             // 已确定最短距离的节点

std::priority_queue<QString, QList<QString>, decltype(compare)> queue(compare);
// compare: distances[left] > distances[right]  → 距离小的优先
```

- 标准 Dijkstra：小顶堆 + 惰性删除（出堆时若已 `visited` 就跳过）
- 距离初值为 `std::numeric_limits<double>::infinity()`，起点为 0
- 出堆节点等于终点即提前结束
- 时间复杂度 **O(E log V)**，本图规模（73 节点 / 105 边）下可忽略不计
- 得到路径后由 `highlightShortestPath()`（`mainwindow.cpp:441`）把沿途的边 `setPen(QPen(Qt::red, 4))` 加粗标红

## 已知限制

写在这里是为了让后来者（和未来的自己）不用重新踩一遍：

1. **底图路径硬编码**到原作者机器的 `C:/Users/刘夏莱/Desktop/640.png`，仓库内没有这张图，开箱运行没有背景地图（见上节）。
2. **搜索是全等匹配**：`node->getName() == searchText`（`mainwindow.cpp:488`），输入「体育馆-东」能搜到，「体育」搜不到。
3. **手动画的边当次运行不参与寻路**：`finishPathDrawing()` 只往 `edges` 里追加，没有同步更新 `adjacencyList`，只有重启（重新 `loadData`）后才进图。
4. **不能删除 / 修改节点和边**，没有撤销。想改只能退出后手改 `data.txt` 再启动。
5. **只能画直线边**：交互式连边只在两点间画一条直线；`data.txt` 支持折线，但界面上没有折线编辑。
6. **退出即覆写 `data.txt`**（`~MainWindow()` → `saveData`），没有备份也没有「是否保存」的询问。
7. **未被使用的历史代码**：`Edge` 类、`MapView::pathCompleted` 信号、`handleNodeClickForPathDrawing()`、`highlightPathForTest()` 都没有调用点；`loadData()` 里的边实际用裸 `QGraphicsPathItem` + `QGraphicsTextItem` 绘制。
8. **文件名大小写**：`Mapview.cpp` 里写的是 `#include "MapView.h"`，Windows 大小写不敏感所以能编译，Linux/macOS 上需要统一成 `Mapview.h`。
9. **闪烁状态是静态的**：`startNodeBlink()` 里用 `static bool toggleColor`，多个节点同时闪烁会互相干扰。
10. **`statusLabel` 不会自动清空**：寻路完成后提示一直留着，要点一次「寻路」退出模式才清空。

## 后续可做

- [ ] 底图与数据改用相对路径 / `qrc` 资源，开箱可跑
- [ ] 支持删除、编辑节点与边，加撤销
- [ ] 模糊搜索 + 候选下拉
- [ ] 用折线的实际累计长度作为边权（现在用的是 `data.txt` 里手填的数值）
- [ ] 交互式绘制折线边，并同步更新邻接表
- [ ] 清理未使用的 `Edge` 类与死函数，或把它们真正接进流程
- [ ] 点击空白处的节点命中判定（现在 20px 圆点，缩得很小时不好点）
- [ ] 加一张运行截图和 `LICENSE`

## 提交历史

| 提交 | 日期 | 说明 |
|------|------|------|
| `fd68262` | 2024-12-04 | Initial commit |
| `f7dba88` | 2024-12-06 | test |
| `2bc9c94` | 2024-12-06 | 实现了路径和节点的显示 |
| `e8f5e73` | 2024-12-06 | 实现了搜索功能 |
| `0a2811f` | 2026-11-03 | 补充 `data.txt` 地图数据，添加 `.gitignore` 忽略构建产物 |

> 仓库曾误提交过整个 Qt 构建目录（1114 个文件、约 61 MB 的 `.exe` / `.dll` / `.o` / clangd 索引缓存）。这些文件已从全部历史中清除，仓库体积从 62 MB 降到约 112 KB。

## 许可证

本仓库**未声明许可证**。如需被他人复用，请补一份 `LICENSE`（MIT 或 Apache-2.0 都是常见选择）。

---

作者：[@ZenXialai](https://github.com/ZenXialai) · 中国石油大学（华东）
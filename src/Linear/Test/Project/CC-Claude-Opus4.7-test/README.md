# 祖玛 · 栈、队列与链表的综合应用

本项目用 **C++（后端）** 与 **HTML/CSS/JS（前端）** 实现了简化版祖玛消除游戏。
后端内置一个最小 HTTP 服务器并暴露 REST 接口，前端通过 `fetch` 调用接口完成全部交互。

> 由 Claude (Opus 4.7) 在 Claude Code 中按要求生成；目录命名 `CC-Claude-Opus4.7-test`。

## 1. 目录结构

```
CC-Claude-Opus4.7-test/
├── Makefile
├── README.md            ← 本实验报告
├── backend/             ← C++ 后端
│   ├── LinkedList.hpp     双向链表（彩球轨道）
│   ├── Queue.hpp          队列（待发射彩球）
│   ├── Stack.hpp          栈（撤销历史）
│   ├── Game.hpp           游戏逻辑：插入 / 消除 / 连锁 / 道具 / 胜负判定
│   ├── HttpServer.hpp     极简 HTTP/1.1 服务器（POSIX socket）
│   └── main.cpp           路由装配与入口
└── frontend/            ← 浏览器前端
    ├── index.html
    ├── style.css
    └── script.js
```

## 2. 编译与运行

依赖：`clang++` 或 `g++`，C++17。仅依赖系统 POSIX 套接字 API，无第三方库。

```bash
cd CC-Claude-Opus4.7-test
make            # 产出可执行文件 zuma_server
make run        # 监听 127.0.0.1:8080
# 或自定义端口： ./zuma_server --port 9000 --static ./frontend
```

启动后，在浏览器中访问 [http://127.0.0.1:8080/](http://127.0.0.1:8080/) 即可游玩。

## 3. 设计思路（即所用 prompt 的核心）

提示要求：
- 仅参考 `Project/README.md`；不读取任何其他源文件。
- 后端 C++、前端 HTML，并在 `CC-Claude-Opus4.7-test/` 子目录中工作。

实现思路：

1. **三种线性结构各司其职**
   - **轨道** 是中间可任意插入 / 区段删除的序列 → **双向链表**。
   - **待发射彩球** 是先进先出的发射序列 → **队列**。
   - **撤销历史** 是后进先出的状态序列 → **栈**。
2. **撤销的本质是状态快照**。每次"可撤销操作"前，把轨道、队列、道具次数等
   一起拷贝压栈；撤销时弹栈赋回。链表 / 队列均实现深拷贝构造与拷贝赋值，
   保证快照彼此隔离。
3. **后端只暴露状态机 + 操作语义**，对协议保持简单：
   单线程阻塞 HTTP/1.1 服务，按 `method+path` 路由，JSON 直接由字符串拼接生成。
   前端只负责视图与发起 `fetch`。
4. **道具作为数据结构的应用展示**：炸弹 = 链表区间删除；变色 = 节点字段
   修改 + 重新触发连锁判定；跳过 = 队列的 `front → back` 重排（队首移到队尾）。

## 4. 核心代码原理

下文中 `[文件:行]` 链接到具体位置以便对照。

### 4.1 双向链表（彩球轨道）—— [LinkedList.hpp](backend/LinkedList.hpp)

- 使用 **头尾哨兵节点**，使在头部 / 尾部插入与中间插入的代码完全一致，
  避免空表的边界判断。[LinkedList.hpp:21-26](backend/LinkedList.hpp#L21-L26)
- `insertAt(idx, v)` 复用 `insertBefore(pos, v)`：先按下标定位到节点，
  再调用 O(1) 的"在指定节点前插入"。[LinkedList.hpp:62-79](backend/LinkedList.hpp#L62-L79)
- `remove(pos)` 在 O(1) 内拆链。游戏中消除一段连续同色球时，
  我们持有起点节点直接顺链删除，整体复杂度 O(段长)。

### 4.2 队列（待发射彩球）—— [Queue.hpp](backend/Queue.hpp)

- 单链表 + head / tail 双指针，`enqueue` 与 `dequeue` 都是 O(1)。
- 额外提供 `sendToBack()`：把队首节点摘下挂到队尾，实现"跳过当前球"道具。
  这是利用链表节点级别操作的小技巧，不需要 O(n) 重建。
  [Queue.hpp:62-72](backend/Queue.hpp#L62-L72)

### 4.3 栈（撤销历史）—— [Stack.hpp](backend/Stack.hpp)

- 单链表实现的最朴素栈，`push` / `pop` 均 O(1)。
- 显式 `= delete` 拷贝构造 / 拷贝赋值：栈本身不应被复制；
  栈内存放的元素（`Game::Snapshot`）由 push 时按值拷贝即可。

### 4.4 游戏逻辑 —— [Game.hpp](backend/Game.hpp)

#### 初始化
`reset(seed)` 用 `mt19937` 生成可重现随机数。生成初始轨道时，
通过 `wouldCreateTriple` **避免一开局就出现三连色**，保证起手是干净的局面。
[Game.hpp:46-66](backend/Game.hpp#L46-L66)

#### 发射 / 插入
`shoot(idx)`：
1. 校验状态与队列非空；
2. **先做一次快照压栈**，保证此次操作可撤销；
3. 从队列 `dequeue` 取出当前球，插入到链表 `idx` 位置；
4. 调用 `eliminateAround(idx, ...)` 触发消除与连锁；
5. 更新得分与游戏状态。
[Game.hpp:69-99](backend/Game.hpp#L69-L99)

#### 自动消除与连锁
`eliminateAround(idx)` 是核心算法。以"刚发生变化的位置 idx"为枢轴：

```text
loop:
    center = node@pivot
    L = center; 向左扩展，只要 L.prev 颜色相同
    R = center; 向右扩展，只要 R.next 颜色相同
    若 [L,R] 长度 < 3 → break
    记录 L 左邻居的下标 leftIdx，作为下一轮的检查点
    删除区间 [L,R]，统计 chainCount++
    pivot = leftIdx；若链表空则终止
```

要点：

- **以"被删段的左邻居"为下一轮枢轴**，使得当左右两段在删除后相邻并形成
  新的三连色时，能被立刻发现 —— 这就是连锁消除的实现。
- 删除区间前 **先保存 `R->next`**（命名为 `stop`），避免在 `R` 被释放后
  再读取 `R->next` 造成悬空指针。这是实现过程中实际遇到并修复的一个
  use-after-free 段错误。[Game.hpp:225-233](backend/Game.hpp#L225-L233)

#### 撤销
`snapshot()` 把当前游戏的"可观察状态"打包进 `Snapshot`（含 `track`、`pending`、
`score`、各道具次数）压入 `history` 栈；`undo()` 弹栈并 `applySnapshot` 整体替换。
[Game.hpp:248-275](backend/Game.hpp#L248-L275)

注意：链表与队列的拷贝是深拷贝（自定义拷贝构造 / 拷贝赋值），
保证撤销点之间互不影响。

#### 三种道具

- **炸弹** `useBomb(idx)`：以 idx 为中心，对链表做 **区间删除**（最多 3 个节点），
  随后调用 `eliminateAround` 处理可能的连锁。展示链表区间删除。
- **变色** `useRecolor(idx, color)`：把指定节点的颜色字段改掉，
  然后调用 `eliminateAround`。展示"在保留节点拓扑的情况下修改数据 + 重判定"。
- **跳过** `useSkip()`：调用队列的 `sendToBack`，把当前球放到队尾，
  展示队列的链节级别重排。

每个道具在改动状态前同样会先 `snapshot()` 入栈，保证可被撤销。

#### 胜负判断
`updateStatus()`：
- 轨道清空 ⇒ **胜利**；
- 已没有待发射的球但轨道非空 ⇒ **失败**；
- 轨道长度超过 `maxTrackLength` ⇒ **失败**；
- 否则继续游戏。

### 4.5 HTTP 服务器 —— [HttpServer.hpp](backend/HttpServer.hpp)

仅使用 `<sys/socket.h>`、`<arpa/inet.h>` 的 BSD 套接字 API：

- `start(port)` 绑定到 `127.0.0.1:port` 并开始监听。
- `run()` 单线程 `accept` → `recv` 直到读到 `\r\n\r\n` 及 `Content-Length` 个字节
  → 调用 `parseRequest` → 按路由表 `handlers["METHOD PATH"]` 分发 → 发送响应。
- 若路径不匹配 API 路由且方法为 GET，则尝试从 `staticDir` 提供静态文件，
  这样后端可以一并托管前端。
- 路由用 `std::function<HttpResponse(const HttpRequest&)>` 注入，
  `main.cpp` 中通过 lambda 把 HTTP 调用桥接到 `Game` 方法。

### 4.6 REST API

| 路径 | 方法 | 请求体 | 含义 |
|---|---|---|---|
| `/api/state` | GET | – | 取当前完整状态 |
| `/api/init` | POST | `{seed,numColors,initialTrackLength,queueLength}` | 用新参数开局 |
| `/api/shoot` | POST | `{position}` | 发射当前球到指定位置 |
| `/api/undo` | POST | – | 撤销上一步可逆操作 |
| `/api/bomb` | POST | `{position}` | 使用炸弹道具 |
| `/api/recolor` | POST | `{position,color}` | 使用变色道具 |
| `/api/skip` | POST | – | 跳过当前球（送回队尾） |

所有"操作类"接口返回：
```json
{
  "success": true,
  "message": "消除 3 球",
  "eliminated": 3,
  "chain": 1,
  "state": { /* 完整 state */ }
}
```

### 4.7 前端 —— [frontend/script.js](frontend/script.js)

页面有三种交互模式：

- `shoot`：在每个球之间渲染一个"插入槽"，点击调用 `/api/shoot`。
- `bomb`：点击炸弹道具按钮进入，下一次点轨道上的球将其作为爆破中心。
- `recolor`：点击变色道具按钮进入，下一次点击会弹出颜色选择器。

每个 API 响应都会把后端返回的 `state` 整体重新渲染到 DOM，
避免前后端不一致。撤销 / 道具次数 / 得分 / 状态徽章随之刷新。

## 5. 运行说明

启动服务后：

1. 顶部状态徽章显示当前局面（`游戏中` / `胜利` / `失败`）和得分。
2. **轨道区**横向显示链表中的彩球。两球之间会出现一个细长的"插入槽"，
   鼠标悬停时会高亮，点击即把当前球插入该位置。
3. **当前球**是即将发射的球；**待发射队列**自左向右依次是接下来要发射的球。
4. **道具栏**：
   - "撤销"：弹出栈顶快照恢复局面。
   - "炸弹 (N)"：进入炸弹模式，再点轨道任一球，引爆其周围至多 3 球。
   - "变色 (N)"：进入变色模式，再点目标球，在弹窗中选目标颜色。
   - "跳过当前 (N)"：把队首球放到队尾，方便延后处理"难发射的颜色"。
5. **配置栏**：可调整颜色数、初始球数、队列长度、随机种子，点"开始新游戏"重开。

## 6. 自评：评分点对应

| 评分项 | 体现 |
|---|---|
| 数据结构设计（30%） | 双向链表 / 队列 / 栈 各自承担轨道、待发射、撤销三个职责；并基于这些结构实现了三种道具 |
| 功能实现（30%） | 轨道初始化、发射、指定位置插入、自动 + 连锁消除、撤销、胜负判断 全部具备；额外提供炸弹 / 变色 / 跳过 三个小道具 |
| 用户交互（20%） | 浏览器图形界面，颜色球可视化、模式切换、操作日志、可调参数 |
| 代码质量（20%） | 模块化分文件、数据结构有拷贝语义、HTTP 与游戏逻辑解耦、关键算法处添加 *为什么* 注释 |

## 7. 已知限制

- HTTP 服务器单线程串行处理，仅适用于本地单人游玩。
- 撤销采用完整快照而非反操作日志：每次撤销点 O(N) 内存；游戏规模有限时实际可忽略。
- JSON 序列化是简易字符串拼接，仅覆盖本项目所需的有限字段，并非通用实现。

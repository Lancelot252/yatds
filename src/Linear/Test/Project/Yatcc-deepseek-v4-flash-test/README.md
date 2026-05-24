# 祖玛游戏 - Zuma Game

## 项目简介

基于栈（撤销功能）、队列（待发射彩球队列）和链表（轨道彩球序列）数据结构实现的祖玛游戏。

玩家发射彩球到轨道上，当连续三个或以上相同颜色的彩球出现时，它们会被自动消除并触发连锁消除。

## 技术栈

- **后端**: C++ (cpp-httplib 轻量级 HTTP 服务器)
- **前端**: HTML/CSS/JavaScript (原生，无框架)
- **数据结构**: `std::list` (轨道双向链表), `std::deque` (待发射队列), `std::stack` (撤销栈)

## 项目结构

```
Yatcc-deepseek-v4-flash-test/
├── ZumaGame.hpp      # 游戏核心逻辑
├── server.cpp         # HTTP 服务器
├── httplib.h          # HTTP 库 (header-only)
├── run.sh             # 构建和运行脚本
└── public/
    └── index.html     # 前端界面
```

## 游戏功能

### 核心功能
- **轨道初始化**: 生成带有随机彩球的轨道，保证初始无三连
- **彩球发射**: 从待发射队列取出彩球，插入到轨道的指定位置
- **自动消除**: 插入后自动检测并消除连续3个及以上相同颜色的彩球
- **连锁消除**: 消除后彩球重新连接，继续检测并消除
- **胜负判断**: 清空轨道为胜利，发射次数用尽为失败

### 道具功能
- **💣 炸弹**: 消除轨道上所有指定颜色的彩球
- **🔀 反转**: 反转轨道上彩球的排列顺序
- **♻️ 洗牌**: 重新排列待发射队列中的彩球

### 其他功能
- **↩ 撤销**: 回退到上一步操作（使用栈实现）
- **🔄 新游戏**: 重新开始游戏
- **键盘快捷键**: `Enter` 发射，`Ctrl+Z` / `Cmd+Z` 撤销

## 数据结构设计

| 数据结构 | 用途 | 选择原因 |
|---------|------|---------|
| `std::list<char>` | 轨道彩球序列 | 支持在任意位置高效插入和删除，适合频繁的中间插入操作 |
| `std::deque<char>` | 待发射队列 | 双端队列，前端取球、后端补充，操作高效 |
| `std::stack<Snapshot>` | 撤销功能 | 后进先出的特性完美匹配撤销操作的场景 |

## 运行说明

### 方式一：一键运行

```bash
cd Yatcc-deepseek-v4-flash-test
chmod +x run.sh
./run.sh
```

### 方式二：手动构建和运行

```bash
cd Yatcc-deepseek-v4-flash-test

# 下载 httplib.h（如已存在可跳过）
curl -sL https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h -o httplib.h

# 编译
clang++ -std=c++11 -O2 -Wall -pthread server.cpp -o zuma_server

# 运行
./zuma_server
```

### 访问游戏

打开浏览器访问: http://localhost:8080

## API 接口

| 接口 | 说明 |
|------|------|
| `GET /api/state` | 获取游戏状态 |
| `GET /api/fire?pos=N` | 发射球到位置 N |
| `GET /api/powerup/bomb?color=R` | 使用炸弹消除指定颜色 |
| `GET /api/powerup/reverse` | 反转轨道 |
| `GET /api/powerup/shuffle` | 洗牌待发射队列 |
| `GET /api/undo` | 撤销上一步 |
| `GET /api/newgame` | 开始新游戏 |
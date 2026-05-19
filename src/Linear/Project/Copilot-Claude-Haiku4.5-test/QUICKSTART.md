# 快速启动指南

## 项目已成功创建！

所有文件已生成在: `/Users/a252/Documents/code/yatds/src/Linear/Project/Copilot-Claude-Haiku4.5-test`

## 快速开始

### 步骤 1: 编译项目

```bash
cd /Users/a252/Documents/code/yatds/src/Linear/Project/Copilot-Claude-Haiku4.5-test
bash build.sh
```

✓ 编译成功！生成的可执行文件在 `build/zuma_server`

### 步骤 2: 启动游戏

#### 方式 A: 直接打开 HTML 文件

在浏览器中打开：
```
file:///Users/a252/Documents/code/yatds/src/Linear/Project/Copilot-Claude-Haiku4.5-test/public/index.html
```

这样可以直接使用游戏，所有逻辑都在浏览器中运行（不需要后端服务器）。

#### 方式 B: 运行 C++ 后端（可选）

```bash
cd /Users/a252/Documents/code/yatds/src/Linear/Project/Copilot-Claude-Haiku4.5-test
bash run.sh
```

这会启动 C++ 编写的游戏服务器，进行测试和演示。

## 游戏界面说明

### 初始化游戏
1. 在"轨道初始状态"输入框中输入球的颜色（用空格分隔）
   - 例如: `R B B B G` （红蓝蓝蓝绿）
2. 在"待发射队列"输入框中输入队列中的球
   - 例如: `R G B`
3. 点击"初始化游戏"按钮

### 玩游戏
1. 选择要插入的位置（0 到轨道长度）
2. 从下拉菜单选择颜色（R/B/G/Y/P）
3. 点击"发射球"按钮插入
4. 如果形成连续 3 个或以上相同颜色的球，会自动消除
5. 消除后两侧的球重新连接，可能产生连锁消除

### 其他操作
- **撤销**: 回到上一步操作
- **重置**: 清空游戏，重新初始化
- **刷新状态**: 刷新当前显示

## 游戏胜负条件

### 胜利 ✓
轨道和队列中的所有球都被消除

### 失败 ✗
轨道上的球数超过 50 个

## 示例

### 例 1：简单消除
```
初始轨道: R B B B G
在位置 2 插入 B
结果: B B B 被消除，轨道变为 R G
```

### 例 2：连锁消除
```
初始轨道: R R B B B R R
在位置 3 插入 B
步骤:
  1. 轨道变为 R R B B B B R R
  2. B B B B 被消除，轨道变为 R R R R
  3. R R R R 被消除，轨道变为空
```

## 项目文件说明

| 文件 | 说明 |
|------|------|
| `src/zuma_game.hpp` | 游戏核心类定义 |
| `src/zuma_game.cpp` | 游戏核心逻辑实现 |
| `src/main.cpp` | C++ 主程序（用于测试） |
| `public/index.html` | 游戏前端界面 |
| `public/style.css` | 样式表（美化界面） |
| `public/app.js` | JavaScript 游戏逻辑 |
| `build.sh` | 编译脚本 |
| `run.sh` | 运行脚本 |
| `README.md` | 详细项目说明 |

## 技术栈

- **后端**: C++11（std::vector, std::queue 等 STL）
- **前端**: HTML5, CSS3, Vanilla JavaScript
- **编译**: g++ (macOS)

## 核心数据结构

1. **std::vector<Ball>** - 轨道存储
   - 支持快速随机访问
   - 支持插入和删除

2. **std::queue<Ball>** - 待发射队列
   - FIFO 特性符合游戏需求

3. **std::vector<GameState>** - 历史状态
   - 支持撤销功能

## 算法复杂度

| 操作 | 时间复杂度 | 说明 |
|------|----------|------|
| 插入球 | O(n) | vector 的插入操作 |
| 消除 | O(n) | 单次扫描轨道 |
| 连锁消除 | O(n²) | 最坏情况：多次扫描 |
| 撤销 | O(1) | 直接状态切换 |

## 扩展功能建议

- [ ] 加入分数系统
- [ ] 添加难度等级
- [ ] 实现特殊道具（炸弹、彩虹球等）
- [ ] 保存游戏进度
- [ ] 排行榜功能
- [ ] 音效和背景音乐
- [ ] 动画效果

## 常见问题

### Q: 游戏无法启动？
A: 确保已编译项目（运行 `bash build.sh`），然后在浏览器打开 `public/index.html`

### Q: 球没有被消除？
A: 检查是否真的形成了连续 3 个或以上相同颜色的球

### Q: 无法撤销？
A: 无法撤销初始化操作，只能撤销插入后的操作

### Q: 编译出错？
A: 确保系统安装了 g++，运行 `g++ --version` 检查

## 总结

本项目成功实现了一个完整的祖玛游戏，包括：
- ✓ 完整的数据结构设计
- ✓ 正确的游戏逻辑实现
- ✓ 美观的用户界面
- ✓ 撤销和其他辅助功能
- ✓ 清晰的代码注释和文档

可以直接打开 HTML 文件即刻游玩！

# YatDS · 数据结构教程与实验

面向数据结构课程学习与实验的资料仓库，涵盖线性结构、树、图、查找和排序五个专题，提供中文教程、基础实验（Baseline）、综合项目（Project）、算法接口与部分公开测试样例，以及 LaTeX 实验报告模板。

仓库中的代码包含供学习者补全的接口和实验骨架。各模块的实现要求、测试方式和评分标准以对应目录下的 README 为准。

## 内容导航

| 专题 | 基础内容 | 综合项目 |
| --- | --- | --- |
| [线性结构](src/Linear/) | [链表](src/Linear/Baseline/List/README.md)、[栈与队列](src/Linear/Baseline/StackAndQueue/README.md)、[字符串与模式匹配](src/Linear/Baseline/String/README.md) | [祖玛游戏](src/Linear/Project/README.md)、[双链 DNA 目标蛋白编码片段检索](src/Linear/Project_string/README.md) |
| [树结构](src/Tree/) | [二叉树与森林](src/Tree/Baseline/forest/README.md)、[哈夫曼树](src/Tree/Baseline/huffman/README.md) | [基于哈夫曼树的简化 Deflate 编码与解码](src/Tree/Project/huffman/README.md) |
| [图结构](src/Graph/) | [图的表示、遍历、最短路径与最小生成树](src/Graph/Baseline/src/README.md) | [终端迷宫寻路](src/Graph/Project/README.md) |
| [查找算法](src/Search/) | [静态查找表、BST、AVL、B-树、B+ 树与 Trie](src/Search/Baseline/README.md) | [校园资料检索与查找结构性能对比](src/Search/Project/README.md) |
| [排序算法](src/Sort/README.md) | 插入排序、希尔排序、归并排序、基数排序、选择排序、堆排序、冒泡排序和快速排序 | [卡牌对战游戏](src/Sort/project/README.md) |

## 目录结构

```text
yatds/
├── README.md
├── src/
│   ├── Linear/          # 线性结构与字符串实验
│   ├── Tree/            # 树、森林与哈夫曼编码实验
│   ├── Graph/           # 图算法与迷宫寻路项目
│   ├── Search/          # 查找算法与综合项目要求
│   ├── Sort/            # 排序算法与综合项目要求
│   └── Pages/           # 静态 HTML 课程页面及配套资源
└── lab模板/             # LaTeX 实验报告模板
```

`Baseline` 目录用于基础算法练习，`Project` 目录用于综合应用；排序专题的综合项目目录名为小写 `project`。具体文件组织随模块而异。

## 开始学习

### 获取仓库

```bash
git clone https://github.com/Lancelot252/yatds.git
cd yatds
```

### 浏览课程页面

直接用浏览器打开 [src/Pages/index.html](src/Pages/index.html)，即可进入课程导航。也可以在仓库根目录使用 Python 3 启动本地静态服务器：

```bash
python3 -m http.server 8000 --bind 127.0.0.1 --directory src/Pages
```

然后访问 <http://127.0.0.1:8000/>。停止服务时按 `Ctrl+C`。

### 完成实验

1. 阅读对应专题的教程和 Baseline README，明确接口、任务和输入输出约定。
2. 补全所需算法实现，按模块说明编译并验证公开样例。
3. 在综合项目中复用基础接口，说明数据结构选择和算法适用条件。
4. 整理运行结果、复杂度分析与实验报告，按模块要求准备提交材料。

各实验按模块独立编译。C++ 实验需要编译器，具体命令见各模块说明；图综合项目提供了使用 C++17 的 Makefile：

```bash
cd src/Graph/Project
make
./graph_project
```

运行前请按 [图综合项目说明](src/Graph/Project/README.md) 完成所需接口与功能函数。

## 实验报告模板

[lab模板/README.md](lab模板/README.md) 提供模板的文件组织、封面信息配置和编译说明。主文档为 `lab模板/main.tex`，可先参考目录中的 `main.pdf`。

使用前准备支持 XeLaTeX 的 TeX 发行版，并根据模板说明配置 Biber 和代码排版依赖。

## 参与修改

欢迎通过 Issue 反馈问题，或通过 Fork 和 Pull Request 提交文档修正、样例改进与其他贡献。提交时请说明修改目的和验证方式。

`main` 分支已启用保护规则，仅仓库所有者 `Lancelot252` 可以更新或合并修改。

# 多项式

本组任务是完成 C++ 版多项式。多项式类和示例入口都写在 `poly.cpp` 中，文件内应标示主要实现部分。

## 需要完成的内容

- 在 `dsaac::Polynomial<Coeff>` 中维护指数到系数的映射，可以使用 `std::map<int, Coeff, std::greater<int>>` 让高次项排在前面。
- 实现 `set(int exponent, Coeff coefficient)`：设置某一项系数；系数为零时删除该项。
- 实现 `coefficient(int exponent) const`：查询指定指数的系数，不存在时返回零值。
- 实现 `operator+`：返回两个多项式的和。
- 实现 `operator*`：返回两个多项式的积。
- 实现 `terms() const`：返回内部项集合的只读引用，便于测试或输出。
- 在 `poly.cpp` 中用 `/* START: polynomial implementation */` 和 `/* END */` 标出主要实现部分。

## 行为要求

- 不应保存系数为零的项。
- 加法中同指数项需要合并。
- 乘法中每一对项相乘后按指数相加合并。
- 原多项式对象不应在 `operator+` 或 `operator*` 中被修改。

## 验证方式

```sh
c++ -std=c++17 poly.cpp -o /tmp/poly
/tmp/poly
```

# 多项式

本组任务是完成基于数组的多项式表示和基本运算。主要代码位于 `poly.c`，该文件自带一个简单的 `main` 示例。

## 需要完成的内容

- 定义 `Polynomial` 结构，使用 `CoeffArray[MaxDegree + 1]` 保存各指数项系数，使用 `HighPower` 保存最高非零指数。
- 实现 `ZeroPolynomial`：把所有系数清零，并把最高次数置为 `0`。
- 实现 `AddPolynomial`：把两个多项式相加，结果写入 `PolySum`。
- 实现 `MultPolynomial`：把两个多项式相乘，结果写入 `PolyProd`。
- 实现或保留 `PrintPoly`：按从高次到低次的顺序输出多项式系数。
- 使用 `main` 或自定义测试构造多项式，验证加法和乘法结果。

## 行为要求

- 多项式最高次数不能超过 `MaxDegree`。
- 乘法结果的最高次数为两个输入最高次数之和，超过 `MaxDegree` 时应调用 `Error`。
- 结果多项式在写入前应先清零，避免旧数据影响计算。
- 加法和乘法都应把结果写入调用方提供的结果对象。

## 验证方式

```sh
cc -std=gnu89 poly.c -o /tmp/poly
/tmp/poly
```

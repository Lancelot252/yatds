#ifndef MERGESORT_H
#define MERGESORT_H

#include <vector>
using namespace std;

// 归并函数
void Merge(vector<int> &Array, int front, int mid, int end);

// 归并排序递归实现
void MergeSort(vector<int> &Array, int front, int end);

#endif
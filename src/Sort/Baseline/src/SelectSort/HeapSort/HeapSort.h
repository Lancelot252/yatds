#ifndef HEAPSORT_H
#define HEAPSORT_H

// 向下调整（构建堆核心）
void DownAdjust(int a[], int parent, int n);

// 堆排序
void HeapSort(int a[], int n);

#endif
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

int parent(int);
int left(int);
int right(int);
void Max_Heapify(int [], int, int);
void Build_Max_Heap(int [], int);
void print(int [], int);
void HeapSort(int [], int);

void print(int a[], int heap_size, string desp) {
  cout << desp << "\t";
  for(int i = 0; i < heap_size; i++)
  {
    cout << a[i] << ' ';
  }
  cout << endl;
}
// node i
// parent floor((i - 1) / 2)
void Build_Max_Heap(int A[], int heap_size) {
  // init
  for (int i = (heap_size - 2)/2; i >= 0; --i) {
    Max_Heapify(A, i, heap_size);
  }
  print(A, heap_size, "First:");
}

void HeapSort(int A[], int heap_size) {
  print(A, heap_size, "Raw:");
  Build_Max_Heap(A, heap_size);
  for (int i = heap_size - 1; i > 0; --i) {
    swap(A[0], A[i]);
    Max_Heapify(A, 0, i);
  }
  print(A, heap_size, "Final:");
}


void Max_Heapify(int A[], int i, int heap_size) {
  int largest = i;
  int left = 2 * i + 1;
  int right = 2 * i + 2;

  if (left < heap_size && A[left] > A[largest]) largest = left;
  if (right < heap_size && A[right] > A[largest]) largest = right;

  if (i != largest) {
    swap(A[largest], A[i]);
    Max_Heapify(A, largest, heap_size);
  }
}

int main(int argc, char* argv[])
{
    const int heap_size = 13;
    int A[] = {6, 1, 10, 14, 16, 4, 7, 9, 3, 2, 8, 5, 11};
    HeapSort(A, heap_size);
    return 0;
}

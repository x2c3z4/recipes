#include <assert.h>
#include <stdio.h>

#include "thirdparty/gtest/gtest.h"


using namespace std;
void print_array(int A[], int n) {
  for(int i = 0; i < n; i++) {
    printf("%d ", A[i]);
  }
  printf("\n");
}

class Solution {
public:
    void merge(int A[], int m, int B[], int n) {
      int i = 0;
      int j = 0;
      int size = m;
      while(j < n) {
        if(i >= size || A[i] > B[j]) { //
          insert_array(A, m+n, i, B[j]);
          ++j;
          ++i;
          ++size;
        } else {
          ++i;
        }
      }
    }
  void insert_array(int A[], int len, int index, int v) {
    assert(index < len);
    for(int i = len - 1; i > index ; --i) {
      A[i] = A[i-1];
    }
    A[index] = v;
    // printf("move\n");
    // print_array(A, len);
  }
};



TEST(MergeSortedArray, merge) {
  int A[6] = {1, 3, 5, 0, 0, 0};
  int B[3] = {2, 4, 6};

  Solution s;
  s.merge(A, 3, B, 3);
  print_array(A, 6);
}

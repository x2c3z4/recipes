#include <assert.h>
#include <stdio.h>

#include "thirdparty/gtest/gtest.h"


using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

void print_list(ListNode *head) {
  while(head) {
    printf("%d ", head->val);
    head = head->next;
  }
  printf("\n");
}

class Solution {
public:
    void insert_node(ListNode *pprev1, ListNode *pprev2, ListNode *node) {
      pprev2->next = node->next;
      node->next = pprev1->next;
      pprev1->next = node;
    }
    ListNode *mergeTwoLists(ListNode *l1, ListNode *l2) {
      if(l1 == NULL) return l2;
      if(l2 == NULL) return l1;

      ListNode head1(0), head2(0);
      head1.next = l1;
      head2.next = l2;
      ListNode *pprev1, *pprev2;
      pprev1 = &head1;
      pprev2 = &head2;

      ListNode *pnext;
      while(head2.next) {
        if(l1 == NULL || l1->val > l2->val) {
          pnext = l2->next;
          insert_node(pprev1, pprev2, l2);
          pprev1 = l2;
          l2 = pnext;
        } else if(l1) {
          pprev1 = l1;
          l1 = l1->next;
        }
      }
      return head1.next;
    }
};


TEST(MergeSortedArray, merge) {
  ListNode a1(2), a2(4), a3(6);
  ListNode b1(1), b2(3), b3(5);
  a1.next = &a2;
  a2.next = &a3;

  b1.next = &b2;
  b2.next = &b3;

  Solution s;
  ListNode *merged = NULL;
  merged = s.mergeTwoLists(&a1, &b1);
  print_list(merged);
}

TEST(MergeSortedArray, merge2) {
  ListNode a1(5);
  ListNode b1(1), b2(2), b3(4);

  b1.next = &b2;
  b2.next = &b3;

  Solution s;
  ListNode *merged = NULL;
  merged = s.mergeTwoLists(&a1, &b1);
  print_list(merged);
}

TEST(MergeSortedArray, mergeNone) {
  ListNode b1(1), b2(3), b3(5);

  b1.next = &b2;
  b2.next = &b3;

  Solution s;
  ListNode *merged = NULL;
  merged = s.mergeTwoLists(NULL, &b1);
  print_list(merged);
}

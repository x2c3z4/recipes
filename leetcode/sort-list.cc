#include <iostream>
#include <assert.h>

#include "thirdparty/gtest/gtest.h"
#include "base/scoped_ptr.h"

using namespace std;

struct ListNode {
  int val;
  ListNode *next;
  ListNode(int x) : val(x), next(NULL) {}
};


class Solution {
  public:
    ListNode *sortList(ListNode *head) {
      ListNode ances(0);
      ances.next = head;
      ListNode *cur = head;

      int len = 0;
      while (cur) ++len,cur = cur->next;

      int width = 1;
      ListNode *start1, *end1, *start2, *end2, *tail, *last;
      for (; width < len; width += width) {
        last = &ances;
        while(last->next) {
          start1  = last->next;
          end1 = listMove(start1, width);
          if (!end1)
            break;
          start2 = end1;
          end2 = listMove(start2, width);

          last->next = bottomUpMerge(start1, end1, start2, end2, &tail);
          last = tail;
        }
      }
      return ances.next;
    }

    ListNode *listMove(ListNode *cur, int k) {
      for(int i = 0; i < k && cur; ++i) cur = cur->next;
      return cur;
    }
    ListNode *bottomUpMerge(ListNode *start1, ListNode *end1, ListNode *start2,
        ListNode *end2, ListNode **tail) {
      ListNode *merge, *head, *next_for_last = end2;
      if (start1->val < start2->val) {
        merge = start1;
        start1 = start1->next;
      } else {
        merge = start2;
        start2 = start2->next;
      }

      head = merge;
      while (start1 != end1 && start2 != end2) {
        if (start1->val < start2->val) {
          merge->next = start1;
          start1 = start1->next;
        } else {
          merge->next = start2;
          start2 = start2->next;
        }
        merge = merge->next;
      }
      while (start1 != end1) {
          merge->next = start1;
          merge = merge->next;
          start1 = start1->next;
      }

      while (start2 != end2) {
          merge->next = start2;
          merge = merge->next;
          start2 = start2->next;
      }
      merge->next = next_for_last;
      *tail = merge;
      return head;
    }
};

void dumpList(ListNode *head, const std::string &desc) {
  printf("%s:\n\t",desc.c_str());
  while(head != NULL) {
    printf("%d,", head->val);
    head = head->next;
  }
  printf("\n");
}

TEST(SortList, sortList) {
  scoped_ptr<Solution> s(new Solution());
  ListNode head(1);
  ListNode next1(8), next2(4);

  head.next = &next1;
  next1.next = &next2;

  dumpList(&head, "before");
  dumpList(s->sortList(&head), "after");
}

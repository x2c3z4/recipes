#include <stack>
#include "thirdparty/gtest/gtest.h"

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

using namespace std;

class Solution {
public:
    int GetThreeNodes(TreeNode *root) {
      int num = 0;
      stack<TreeNode*> st;
      st.push(root);
      while (!st.empty()) {
        TreeNode *p = st.top();
        st.pop();
        if (p->right)
          st.push(p->right);
        if (p->left)
          st.push(p->left);
        num += GetOne(p, 3);
      }
      // dfs(root, 1, num);
      return num;
    }

    int GetOne(TreeNode* node, int len) { // 获取包含node的长度len的数目。
      int total = 0;
      // cout << node->val << ": " << endl;
      for (int i = 0; i <= len; ++i) {
        int count1 = 0, count2 = 0;
        dfs(node->left, i - 1, count1);
        dfs(node->right, len - i - 1, count2);
        total += count1 * count2;
        // cout << "\t" << count1 << " | " << count2 << endl;
      }

      return total;
    }
    void dfs(TreeNode* node, int step, int& count) {// 任意节点，子树长度为step的数量。
      if (!node) {return;}
      if (step <= 0) {
        count++;
        return;
      }
      dfs(node->left, step - 1, count);
      dfs(node->right, step - 1, count);
    }
  //   bool dfs(TreeNode* node, int step, int& count) {
  //     if (node == nullptr) {
  //       return false;
  //     }
  //     if (step <= 0) {
  //       return true;
  //     }
  //     bool found = false;
  //     for (int i = 0; i <= step; ++i) {
  //       bool left = dfs(node->left, i - 1, count);
  //       bool right = dfs(node->right, step - i - 1, count);
  //       cout << i << " " << (step - i) << " | " << node->val << " | " << left << right << endl;
  //       if (left && right) {
  //         found = true;
  //         count++;
  //       }
  //     }
  //     return found;
  // }
};

TEST(ThreeNode, count) {
  Solution s;
  TreeNode node1(1), node2(2), node3(3), node4(4), node5(5);
  node1.left = &node2;
  node2.left = &node3;
  node2.right = &node4;
  node1.right = &node5;

  TreeNode *root = &node1;
  cout << s.GetThreeNodes(root) << endl;
}

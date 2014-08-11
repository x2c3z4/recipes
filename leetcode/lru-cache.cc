#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdint>

#include "thirdparty/gtest/gtest.h"

using namespace std;

class LRUCache {
public:
  LRUCache(int capacity) {
    capacity_ = capacity;
  }

  int get(int key) {
    auto index = cache_.find(key);
    if (index == cache_.end()) {
      return -1;
    } else {
      ++cache_[key].second;
      return cache_[key].first;
    }

    return 0;
  }

  void set(int key, int value) {
    auto index = cache_.find(key);
    if (index == cache_.end()) {
      if (cache_.size() >= capacity_) {
        lruRemove();
      }
      cache_[key] = make_pair(value, 1);
    } else {
      cache_[key].first = value;
      ++cache_[key].second;
    }
  }
  void dump() {
    printf("%10s %10s %10s\n", "key", "value", "lru");
    for (auto it = cache_.begin(); it != cache_.end(); ++it) {
      printf("%10d %10d %10d\n", it->first, (it->second).first, (it->second).second);
    }
    printf("\n\n");
  }

  void dumpElse() {
    printf("sorted:");
    printf("%10s %10s %10s\n", "key", "value", "lru");
#if 0
    for (auto it = pairs.begin(); it != pairs.end(); ++it) {
      printf("%10d %10d %10d\n", it->first, (it->second).first, (it->second).second);
    }
#endif
    printf("\n\n");
  }

private:
  void lruRemove() {
    // dump();
    vector<pair<int, pair<int, int>>> pairs(cache_.begin(), cache_.end());
    sort(pairs.begin(), pairs.end(), [](const pair<int, pair<int, int>>& a,  const pair<int, pair<int, int>>& b) {
      return a.second.second < b.second.second;
    });
    cache_.erase(cache_.find(pairs[0].first));
    // dumpElse();
  }
  map<int, pair<int, int>> cache_; // first value, second is count
  unsigned int capacity_;
};

TEST(LRUCache, getAndSet) {
  LRUCache s(7);

  s.set('n', 100);

  s.set('a', 1);
  ASSERT_EQ(s.get('a'), 1);
  s.set('a', 5);
  ASSERT_EQ(s.get('a'), 5);
  s.set('z', 9);
  s.get('z');
  s.get('z');
  s.get('z');
  s.set('z', 2);

  s.set('b', 2);
  s.get('z');
  s.get('z');
  s.set('b', 2);
  s.get('b');

  s.set('k', 2);
  s.get('k');

  s.set('d', 2);
  s.get('d');

  s.set('e', 2);
  s.get('e');

  s.set('s', 2);
  s.get('s');
  
  s.set('f', 2);
  s.get('f');

  s.set('m', 2);
  s.get('m');

  s.dump();
}

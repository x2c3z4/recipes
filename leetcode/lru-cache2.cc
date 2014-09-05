#include <unordered_map>
#include <list>

#include "thirdparty/gtest/gtest.h"

using namespace std;

class LRUCache {
public:
  typedef typename std::pair<int, int> key_value_pair_t;
  typedef typename std::list<key_value_pair_t>::iterator list_iterator_t;
  LRUCache(int capacity) {
    capacity_ = capacity;
  }

  int get(int key) {
    auto it = cache_items_map_.find(key); // O(1)
    if (it == cache_items_map_.end()) {
      return -1;
    } else {
      // move to begin
      cache_items_.splice(cache_items_.begin(), cache_items_, it->second);
      return it->second->second;
    }
    return 0;
  }

  void set(int key, int value) {
    auto it = cache_items_map_.find(key);
    if (it != cache_items_map_.end()){
      // find it!  must delete, don't move, becasue user can update value
      cache_items_.erase(it->second);
      cache_items_map_.erase(it);
    }

    cache_items_.push_front(key_value_pair_t(key, value));
    cache_items_map_[key] = cache_items_.begin(); // dont't use front(), it's value

    if (cache_items_map_.size() > capacity_) {
      // remove front
      auto tmpIt = cache_items_.end();
      --tmpIt;
      cache_items_.erase(tmpIt);
      cache_items_map_.erase(tmpIt->first);
    }
  }
  void dump() {
    printf("%10s %10s %10s\n", "key", "value", "lru");
    for (auto it = cache_items_.begin(); it != cache_items_.end(); ++it) {
      printf("(%d, %d) ", it->first, it->second);
    }
    printf("\n");
  }

private:
  list<key_value_pair_t> cache_items_;
  unordered_map<int, list_iterator_t> cache_items_map_; // for find, O(1) hash map
  unsigned int capacity_;
};

TEST(LRUCache, firstTest) {
  LRUCache s(1);
  s.set(2,1);
  ASSERT_EQ(s.get(2), 1);
}
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

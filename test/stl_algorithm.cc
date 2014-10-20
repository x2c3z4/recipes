// lower_bound/upper_bound example
#include <iostream>     // std::cout
#include <algorithm>    // std::lower_bound, std::upper_bound, std::sort
#include <vector>       // std::vector
#include <iterator>

template <class ForwardIterator, class T>
ForwardIterator lower_bound2(ForwardIterator first, ForwardIterator last, const T& val)
{
  ForwardIterator it;
  typename std::iterator_traits<ForwardIterator>::difference_type count, step;
  count = std::distance(first,last);
  while (count>0)
  {
    it = first;
    step=count/2;
    std::advance(it,step);
    if (*it<val) {                 // or: if (comp(*it,val)), for version (2)
      std::cout << "it:" << *it << " count: " << count << std::endl;
      first=++it;
      count-=step+1;
    }
    else count=step;
  }
  return first;
}

int main () {
  int myints[] = {10,20,30,30,20,10,10,20};
  std::vector<int> v(myints,myints+8);           // 10 20 30 30 20 10 10 20

  std::sort (v.begin(), v.end());                // 10 10 10 20 20 20 30 30

  std::vector<int>::iterator low, low2, up;
  low=std::lower_bound (v.begin(), v.end(), 20);
  low2=lower_bound2(v.begin(), v.end(), 20);
  up= std::upper_bound (v.begin(), v.end(), 20);

  std::cout << "lower_bound at position " << (low- v.begin()) << '\n';
  std::cout << "lower_bound2 at position " << (low2- v.begin()) << '\n';
  std::cout << "upper_bound at position " << (up - v.begin()) << '\n';
  std::vector<int> a = {1, 2, 3, 4, 5, 6, 7};
  auto len = std::distance(a.begin(), a.end());
  std::cout << len << std::endl;
  std::cout << *next(a.begin(), len / 2) << std::endl;
  return 0;
}

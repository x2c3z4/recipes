#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <deque>

#include <iostream>
#include "thirdparty/gtest/gtest.h"
using namespace std;

class Solution {
public:
    bool isNeighbor (const string& start, const string& str) {
      int count = 0;
      for (unsigned int i = 0; i< start.size(); ++i) {
        if(start[i] != str[i])
          count++;
      }
      if(count == 1) {
        return true;
      }
      return false;
    };

    void gen_path(unordered_map<string, vector<string> > &father,
      vector<string> &path, const string& word, const string& start, vector<vector<string>> &rst) {
      path.push_back(word);
      if (word == start) {
        // reverse(path.begin(), path.end());
        rst.push_back(path);
        reverse(rst.back().begin(), rst.back().end());
        // path.resize(0);
      } else {
        auto next = father[word];
        for (auto &it : next) {
          gen_path(father, path, it, start, rst);
        }
      }
      path.pop_back();
    };


    vector<vector<string>> findLadders(string start, string end, unordered_set<string> &dict) {
      vector<string> curr, next; // vector or unordered_set
      unordered_set<string> visited;
      unordered_map<string, vector<string> > father;
      bool found = false;

      auto is_target = [&] (const string& item) {
        return isNeighbor(item, end);
      };

      auto state_extend = [&] (const string& item) {
        deque<string> match;
        for (auto &it : dict) {
          if (visited.count(it) == 0 && isNeighbor(it, item)) {
            match.push_back(it);
          }
        }
        return match;
      };

      curr.push_back(start);
      while (!curr.empty()) {
        for (auto &it : curr) {
          visited.insert(it);
        }
        while (!curr.empty()) {
          const string item = curr.back();
          curr.pop_back();
          // cout << "item: " << item << endl;
          // curr has arrived one path, then the next should not go there,
          // because then the path is more length than the first
          const auto &new_states = state_extend(item);

          for (const auto &it : new_states) {
            // cout << it << " ";
            if (is_target(it)) {
              found = true;
              father[it].push_back(item);
              father[end].push_back(it);
            } else {
              next.push_back(it);
              father[it].push_back(item);
            }
          } // for
          // cout << endl;
        } // end while
        swap(curr, next);
      } // end outer while

      // gen path, dfs
      vector<vector<string>> rst;
      vector<string> path;
      if (found) {
        gen_path(father, path, end, start, rst);
      }
      return rst;
    }
};

void print_result(const vector<vector<string>>& vecs) {
  for(auto vec: vecs) {
    for(auto v: vec) {
      cout << v << " ";
    }
    cout << endl;
  }
}

TEST(WordLadder, isNeighbor) {
  Solution s;
  EXPECT_TRUE(s.isNeighbor("hit", "hot"));
}

TEST(WordLadder, findLadders) {
  string start("hit");
  string end("cog");
  unordered_set<string> dict =   {"hot","dot","dog","lot","log"};
  vector<vector<string>> ret;
  Solution s;
  ret = s.findLadders(start, end, dict);
  print_result(ret);
}

TEST(WordLadder, bigTest) {
  string start("qa");
  string end("sq");
  unordered_set<string> dict =
  {"si","go","se","cm","so","ph","mt","db","mb","sb","kr","ln","tm","le","av","sm","ar","ci","ca","br","ti","ba","to","ra","fa","yo","ow","sn","ya","cr","po","fe","ho","ma","re","or","rn","au","ur","rh","sr","tc","lt","lo","as","fr","nb","yb","if","pb","ge","th","pm","rb","sh","co","ga","li","ha","hz","no","bi","di","hi","qa","pi","os","uh","wm","an","me","mo","na","la","st","er","sc","ne","mn","mi","am","ex","pt","io","be","fm","ta","tb","ni","mr","pa","he","lr","sq","ye"};

  vector<vector<string>> ret;
  Solution s;
  ret = s.findLadders(start, end, dict);
  print_result(ret);
}

TEST(WordLadder, bigbigTest) {
  string start("cet");
  string end("ism");
  unordered_set<string> dict =
  {"kid","tag","pup","ail","tun","woo","erg","luz","brr","gay","sip","kay","per","val","mes","ohs","now","boa","cet","pal","bar","die","war","hay","eco","pub","lob","rue","fry","lit","rex","jan","cot","bid","ali","pay","col","gum","ger","row","won","dan","rum","fad","tut","sag","yip","sui","ark","has","zip","fez","own","ump","dis","ads","max","jaw","out","btu","ana","gap","cry","led","abe","box","ore","pig","fie","toy","fat","cal","lie","noh","sew","ono","tam","flu","mgm","ply","awe","pry","tit","tie","yet","too","tax","jim","san","pan","map","ski","ova","wed","non","wac","nut","why","bye","lye","oct","old","fin","feb","chi","sap","owl","log","tod","dot","bow","fob","for","joe","ivy","fan","age","fax","hip","jib","mel","hus","sob","ifs","tab","ara","dab","jag","jar","arm","lot","tom","sax","tex","yum","pei","wen","wry","ire","irk","far","mew","wit","doe","gas","rte","ian","pot","ask","wag","hag","amy","nag","ron","soy","gin","don","tug","fay","vic","boo","nam","ave","buy","sop","but","orb","fen","paw","his","sub","bob","yea","oft","inn","rod","yam","pew","web","hod","hun","gyp","wei","wis","rob","gad","pie","mon","dog","bib","rub","ere","dig","era","cat","fox","bee","mod","day","apr","vie","nev","jam","pam","new","aye","ani","and","ibm","yap","can","pyx","tar","kin","fog","hum","pip","cup","dye","lyx","jog","nun","par","wan","fey","bus","oak","bad","ats","set","qom","vat","eat","pus","rev","axe","ion","six","ila","lao","mom","mas","pro","few","opt","poe","art","ash","oar","cap","lop","may","shy","rid","bat","sum","rim","fee","bmw","sky","maj","hue","thy","ava","rap","den","fla","auk","cox","ibo","hey","saw","vim","sec","ltd","you","its","tat","dew","eva","tog","ram","let","see","zit","maw","nix","ate","gig","rep","owe","ind","hog","eve","sam","zoo","any","dow","cod","bed","vet","ham","sis","hex","via","fir","nod","mao","aug","mum","hoe","bah","hal","keg","hew","zed","tow","gog","ass","dem","who","bet","gos","son","ear","spy","kit","boy","due","sen","oaf","mix","hep","fur","ada","bin","nil","mia","ewe","hit","fix","sad","rib","eye","hop","haw","wax","mid","tad","ken","wad","rye","pap","bog","gut","ito","woe","our","ado","sin","mad","ray","hon","roy","dip","hen","iva","lug","asp","hui","yak","bay","poi","yep","bun","try","lad","elm","nat","wyo","gym","dug","toe","dee","wig","sly","rip","geo","cog","pas","zen","odd","nan","lay","pod","fit","hem","joy","bum","rio","yon","dec","leg","put","sue","dim","pet","yaw","nub","bit","bur","sid","sun","oil","red","doc","moe","caw","eel","dix","cub","end","gem","off","yew","hug","pop","tub","sgt","lid","pun","ton","sol","din","yup","jab","pea","bug","gag","mil","jig","hub","low","did","tin","get","gte","sox","lei","mig","fig","lon","use","ban","flo","nov","jut","bag","mir","sty","lap","two","ins","con","ant","net","tux","ode","stu","mug","cad","nap","gun","fop","tot","sow","sal","sic","ted","wot","del","imp","cob","way","ann","tan","mci","job","wet","ism","err","him","all","pad","hah","hie","aim","ike","jed","ego","mac","baa","min","com","ill","was","cab","ago","ina","big","ilk","gal","tap","duh","ola","ran","lab","top","gob","hot","ora","tia","kip","han","met","hut","she","sac","fed","goo","tee","ell","not","act","gil","rut","ala","ape","rig","cid","god","duo","lin","aid","gel","awl","lag","elf","liz","ref","aha","fib","oho","tho","her","nor","ace","adz","fun","ned","coo","win","tao","coy","van","man","pit","guy","foe","hid","mai","sup","jay","hob","mow","jot","are","pol","arc","lax","aft","alb","len","air","pug","pox","vow","got","meg","zoe","amp","ale","bud","gee","pin","dun","pat","ten","mob"};
  cout << "data size: " << dict.size() << endl;
  vector<vector<string>> ret;
  Solution s;
  ret = s.findLadders(start, end, dict);
  print_result(ret);
}



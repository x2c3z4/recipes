#include <vector>
#include <string>
#include <unordered_set>
#include <map>
#include "thirdparty/gtest/gtest.h"
using namespace std;

class Solution {
public:
  #define INT_MAX  2147483647
  bool isNeighbor(const string& start, const string& str) {
    int count = 0;
    for(unsigned int i = 0; i< start.size(); ++i) {
      if(start[i] != str[i])
        count++;
    }
    if(count == 1) {
      return true;
    }
    return false;
  }

  vector<string> getNeighbor(const string& start) {
    vector<string> ret;
    for(auto word: *dict_) {
      if(!used_[word] && isNeighbor(start, word)) {
        ret.push_back(word);
      }
    }
    // cout << "[" << start << "] neighbor:" << endl;
    // for(auto w: ret) {
    //   cout << w << " ";
    // }
    // cout << endl;
    return ret;
  }

    vector<vector<string>> findLadders(string start, string end, unordered_set<string> &dict) {
      vector<vector<string>> ret, ret2;
      dict_ = &dict;
      end_ = end;
      path_depth_ = INT_MAX;
      // used_ = new bool*[dict.size()];
      for(auto word: dict) {
        used_[word] = false;
      }
      // for(int i = 0; i < dict.size(); i++) {
      //   used_[] = false;
      // }
      vector<string> neis = getNeighbor(start);
      if(neis.empty()) {
        // bad path
        return ret;
      }
      vector<string> item;
      item.push_back(start);

      for(auto nei: neis) {
        __findLadders(nei, ret, item);
      }

      for(auto it = ret.begin(); it != ret.end(); ++it) {
        if(it->size() == path_depth_) {
          ret2.push_back(*it);
        }
      }
      return ret2;
    }

    void __findLadders(const string& start, vector<vector<string>>& ret, vector<string> item) {
      if(item.size() > path_depth_ - 2) {
        return;
      }
      if(isNeighbor(start, end_)) {
        // TODO(fengli): add one item
        // vector<string> item;
        // for(auto it: used_) {
        //   if(it.second)
        //     item.push_back(it.first);
        // }
        // cout << "[neighbor] " << start << " " << end_ << endl;
        item.push_back(start);
        item.push_back(end_);
        ret.push_back(item);

        if(item.size() < path_depth_)
          path_depth_ = item.size();

        // cout << "[ item ]" << endl;
        // for(auto w: item) {
        //   cout << w << " ";
        // }
        // cout << endl;
        return;
      }

      vector<string> neis = getNeighbor(start);
      if(neis.empty()) {
        // bad path
        return;
      }
      used_[start] = true;
      item.push_back(start);
      for(string nei: neis) {
        __findLadders(nei, ret, item);
      }
      used_[start] = false;
      // item.pop_back();
    }
  private:
    unordered_set<string> *dict_;
    string end_;
    // bool *used_;
    map<string, bool> used_;
    unsigned int path_depth_;
};

void print_result(const vector<vector<string>>& vecs) {
  for(auto vec: vecs) {
    for(auto v: vec) {
      cout << v << " ";
    }
    cout << endl;
  }
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

TEST(WordLadder, isNeighbor) {
  Solution s;
  EXPECT_TRUE(s.isNeighbor("hit", "hot"));
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



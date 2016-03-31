#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

class Solution {
public:
    int minCut(string s) {
        // Start typing your C/C++ solution below
        // DO NOT write int main() function
        int result1 = onesideCut(s);
        reverse (s.begin(), s.end());
        int result2 = onesideCut(s);
        return (result1>result2 ? result2:result1);
    }
    
    int partion(string s){
        int result = 0;
        string tmp ,rest = s;
        
        while(rest.length()>0){
            tmp = rest;
             while(!isPalindrome(tmp)){
                tmp = tmp.substr(0,tmp.length()-1);
            }
            if(tmp.length()<rest.length())
                result += 1;
            rest = rest.substr(tmp.length(),rest.length()-tmp.length());
        }
        
        return result;
    }
    bool isPalindrome(string s) {
        int len = s.length();
        int i=0;
        while(i<len/2){
            if(s[i] != s[len-i-1])
                return false;
            i++;
        }
        
        return true;
    }
};

int main()
{
  Solution sol;
  cout<<sol.minCut("ababbbabbaba")<<endl;
}

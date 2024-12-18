#ifndef __CHARACTER_CLASS_H_
#define __CHARACTER_CLASS_H_

#include "redBlackTree.hpp"

#include <string>
#include <vector>

class CharacterClassDecider {
  
private:
  RedBlackTree<char> rbt;
  bool inverted;
  void expandRanges(std::string s, std::vector<char> &expanded);
  void splitOutRanges(std::string s, std::vector<std::string> &ranges);
  void commonConstructorCode(std::string s, bool inverted);

public:
  CharacterClassDecider(std::string s, bool inverted);
  CharacterClassDecider(std::vector<char> cv, bool inverted);
  bool characterClassContains(char c);
  
};


#endif

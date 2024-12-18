#include "characterClass.hpp"
#include "redBlackTree.hpp"

#include <stdexcept>
#include <string>


CharacterClassDecider::CharacterClassDecider(std::string s, bool inverted) {
  commonConstructorCode(s, inverted);
}

CharacterClassDecider::CharacterClassDecider(std::vector<char> cv, bool inverted) {
  std::string s(cv.begin(), cv.end());
  commonConstructorCode(s, inverted);
}

void CharacterClassDecider::commonConstructorCode(std::string s, bool inverted) {
  std::vector<char> expanded;
  expandRanges(s, expanded);
  for(std::vector<char>::iterator it = expanded.begin();
      it != expanded.end();
      ++it) {
    rbt.redBlackUniqueInsert(*it);
  }
  
  this->inverted = inverted;
}

// Does not support - as a range boundary because I cannot be bothered
void CharacterClassDecider::splitOutRanges(std::string s, std::vector<std::string> &ranges) {
  if(s.find("--") != std::string::npos) {
    throw std::invalid_argument("I cannot be bothered to have - as a range boundary");
  }
  if(s[0] == '-') {
    s = s.substr(1);
    ranges.push_back("-");
  }
  size_t nextIdx;
  while(std::string::npos != (nextIdx = s.find('-'))) {
    if(nextIdx > 1) {
      ranges.push_back(s.substr(0, nextIdx-1));
      s = s.substr(nextIdx-1);
    }
    else if(nextIdx == 1 && s.length() > 2) {
      ranges.push_back(s.substr(0, 3));
      s = s.substr(3);
    } else {
      break;
    }
  }
  ranges.push_back(s);
}

// Does not support - as a range boundary because I cannot be bothered
void CharacterClassDecider::expandRanges(std::string s, std::vector<char> &expanded) {
  std::vector<std::string> rangeSubstrs;
  splitOutRanges(s, rangeSubstrs);
  for(std::vector<std::string>::iterator it = rangeSubstrs.begin();
      it != rangeSubstrs.end();
      ++it) {
    if(it->length() == 3 && (*it)[1] == '-') {
      char firstChar = (*it)[0];
      char lastChar = (*it)[2];
      if(firstChar < lastChar) {
	for(char c = firstChar; c <= lastChar; c++) {
	  expanded.push_back(c);
	}
      }
    } else {
      for(std::string::iterator jt = it->begin();
	  jt != it->end();
	  ++jt) {
	expanded.push_back(*jt);
      }
    }
  }
  for(std::vector<char>::iterator kt = expanded.begin();
      kt != expanded.end();
      ++kt) {
  }
}

bool CharacterClassDecider::characterClassContains(char c) {
  return inverted ^ rbt.find(c);
}

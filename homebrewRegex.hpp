#ifndef __HOMEBREW_REGEX_H_
#define __HOMEBREW_REGEX_H_

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include "nfa.hpp"
#include "nfaFactory.hpp"
#include "regexParser.hpp"
#include "regexTokenizer.hpp"
#include "tokenizerParserTypes.hpp"


class RegexMatch {
private:
  std::string inputString;
  std::vector<std::vector<size_t>> matchLocations;

public:
  RegexMatch() {}
  RegexMatch(std::string inputString,
	     std::vector<std::vector<size_t>> matchLocations) : inputString(inputString),
								matchLocations(matchLocations) {}
  size_t getNumGroups() const;
  std::string getGroup(size_t idx);
  
};


class RegexEngine {
private:
  NFA* nfa;
  void setEngine(NFA* nfa);
public:
  ~RegexEngine();
  void match(RegexMatch &matchObj, std::string inputString);
  static void compile(RegexEngine &engine, std::string regex);
};


#endif

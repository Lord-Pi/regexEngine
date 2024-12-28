#ifndef __HOMEBREW_REGEX_H_
#define __HOMEBREW_REGEX_H_

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include "nfa.hpp"
#include "nfaFactory.hpp"
#include "regexParser.hpp"
#include "regexTokenizer.hpp"
#include "tokenizerParserTypes.hpp"


class RegexMatch {
private:
  std::string inputString;
  //std::vector<std::vector<size_t>> matchLocations;
  std::vector<std::pair<size_t, size_t>> matchLocations;

public:
  RegexMatch() {}
  // RegexMatch(std::string inputString,
  // 	     std::vector<std::vector<size_t>> matchLocations) : inputString(inputString),
  // 								matchLocations(matchLocations) {}

  RegexMatch(std::string inputString,
	     std::vector<std::pair<size_t, size_t>> matchLocations) : inputString(inputString),
								      matchLocations(matchLocations) {}

  size_t getNumGroups() const;
  std::string getGroup(size_t idx);
  std::pair<size_t, size_t> getGroupBounds(size_t idx);
};


class RegexEngine {
private:
  NFA* nfa;
  void setEngine(NFA* nfa);
public:
  ~RegexEngine();
  void match(RegexMatch &matchObj, std::string inputString);
  void findNextMatchFrom(RegexMatch &matchObj, std::string inputString, size_t fromIdx);
  void search(RegexMatch &matchObj, std::string inputString);
  void findAll(std::vector<RegexMatch> &matches, std::string inputString);
  void findAllFrom(std::vector<RegexMatch> &matches, std::string inputString, size_t fromIdx);
  static void compile(RegexEngine &engine, std::string regex);
};


#endif

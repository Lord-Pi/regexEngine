#include "homebrewRegex.hpp"

#include <cstddef>
#include <string>
#include <vector>
#include "tokenizerParserTypes.hpp"
#include "regexParser.hpp"
#include "regexTokenizer.hpp"
#include "nfa.hpp"
#include "nfaFactory.hpp"


size_t RegexMatch::getNumGroups() const {
  return matchLocations.size();
}
std::string RegexMatch::getGroup(size_t idx) {

  // std::vector<char> cv;
  // for(std::vector<size_t>::iterator it = matchLocations[idx].begin();
  //     it != matchLocations[idx].end();
  //     ++it) {
  //   cv.push_back(inputString[*it]);
  // }
  // std::string group(cv.begin(), cv.end());
  // return group;

  std::pair<size_t, size_t> groupBounds = getGroupBounds(idx);
  std::string group(inputString.begin()+groupBounds.first,
		    inputString.begin()+groupBounds.second);
  return group;
		    
}
std::pair<size_t, size_t> RegexMatch::getGroupBounds(size_t idx) {
  return matchLocations[idx];
}



RegexEngine::~RegexEngine() {
  delete nfa;
}
void RegexEngine::setEngine(NFA* nfa) {
  this->nfa = nfa;
}
void RegexEngine::match(RegexMatch &matchObj, std::string inputString) {
  std::vector<std::pair<size_t, size_t>> matchLocations = nfa->engineMatch(inputString, 0);
  matchObj = RegexMatch(inputString, matchLocations);
}

void RegexEngine::findNextMatchFrom(RegexMatch &matchObj, std::string inputString, size_t fromIdx) {
  std::vector<std::pair<size_t, size_t>> matchLocations;
  for(size_t i = fromIdx; i < inputString.length(); i++) {
    matchLocations = nfa->engineMatch(inputString, i);
    if(matchLocations.size() > 0) {
      matchObj = RegexMatch(inputString, matchLocations);
    }
  }
}

void RegexEngine::search(RegexMatch &matchObj, std::string inputString) {
  findNextMatchFrom(matchObj, inputString, 0);
}

void RegexEngine::findAllFrom(std::vector<RegexMatch> &matches,
			      std::string inputString,
			      size_t fromIdx) {
  matches.clear();
  size_t roverIdx = fromIdx;
  do {
    matches.push_back(RegexMatch());
    findNextMatchFrom(matches.back(), inputString, roverIdx);
    if(matches.back().getNumGroups() > 0) {
      roverIdx = matches.back().getGroupBounds(0).second;
    }
  } while(matches.back().getNumGroups() > 0);
}

void RegexEngine::findAll(std::vector<RegexMatch> &matches,
	     std::string inputString) {
  return findAllFrom(matches, inputString, 0);
}

void RegexEngine::compile(RegexEngine &engine, std::string regex) {
  
  std::vector<RegexToken*> v = RegexTokenizer::stringToTokensPass(regex);
  std::vector<RegexToken*> v2 = RegexTokenizer::insertConcatPass(v);
  AstNode* treeRoot = RegexParser::shuntingYard(v2);
  NFAFactory factory;
  NFA* engineNfa = factory.createNFA(treeRoot);
  engine.setEngine(engineNfa);
  
}

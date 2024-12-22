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

  std::vector<char> cv;
  for(std::vector<size_t>::iterator it = matchLocations[idx].begin();
      it != matchLocations[idx].end();
      ++it) {
    cv.push_back(inputString[*it]);
  }
  std::string group(cv.begin(), cv.end());
  return group;
}



RegexEngine::~RegexEngine() {
  delete nfa;
}
void RegexEngine::setEngine(NFA* nfa) {
  this->nfa = nfa;
}
void RegexEngine::match(RegexMatch &matchObj, std::string inputString) {
  std::vector<std::vector<size_t>> matchLocations = nfa->engineMatch(inputString, 0);
  matchObj = RegexMatch(inputString, matchLocations);
}
void RegexEngine::compile(RegexEngine &engine, std::string regex) {
  
  std::vector<RegexToken*> v = RegexTokenizer::stringToTokensPass(regex);
  std::vector<RegexToken*> v2 = RegexTokenizer::insertConcatPass(v);
  AstNode* treeRoot = RegexParser::shuntingYard(v2);
  NFAFactory factory;
  NFA* engineNfa = factory.createNFA(treeRoot);
  engine.setEngine(engineNfa);
  
}

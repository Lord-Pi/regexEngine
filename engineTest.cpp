#include <iostream>
#include <string>
#include <vector>
#include "homebrewRegex.hpp"
#include "tokenizerParserTypes.hpp"
#include "regexParser.hpp"
#include "regexTokenizer.hpp"
#include "nfa.hpp"
#include "nfaFactory.hpp"




int main(int argc, char *argv[]) {
  std::string testRegex = "[abc].[^def][]][^]].(a|b|.)+a?|c";
  testRegex = "(0+|1+|23+)";
  testRegex = "([abc])([abc])([abc])";
  //testRegex = "([abc])";
  //testRegex = "a";
  testRegex = "mul[(]([0-9]+),([0-9]+)[)]";

  std::string testInput = "xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))";

  

  RegexEngine re;
  RegexEngine::compile(re, testRegex);
  RegexMatch m;
  std::vector<RegexMatch> ms;
  re.findAllFrom(ms, testInput, 1);
  for(std::vector<RegexMatch>::iterator it = ms.begin();
      it != ms.end();
      ++it) {
    std::cout << (*it).getNumGroups() << std::endl;
    for(size_t i = 0; i < (*it).getNumGroups(); i++) {
      std::cout << (*it).getGroup(i) << std::endl;
    }
    std::cout << std::endl;
  }
  // std::cout << m.getNumGroups() << std::endl;
  // for(size_t i = 0; i < m.getNumGroups(); i++) {
  //   std::cout << m.getGroup(i) << std::endl;
  // }
  
  
  /*
  
  std::vector<RegexToken*> v = RegexTokenizer::stringToTokensPass(testRegex);

  for(std::vector<RegexToken*>::iterator it = v.begin();
      it != v.end();
      ++it) {
    std::cout << (*it) -> get_str_rep();
    std::cout << " ";
  }
  std::cout << std::endl;

  std::vector<RegexToken*> v2 = RegexTokenizer::insertConcatPass(v);
  
  for(std::vector<RegexToken*>::iterator it = v2.begin();
      it != v2.end();
      ++it) {
    std::cout << (*it) -> get_str_rep();
    std::cout << " ";
  }
  std::cout << std::endl;

  AstNode* treeRoot = RegexParser::shuntingYard(v2);
  std::cout << treeRoot->toString() << std::endl;

  NFAFactory factory;
  NFA* nfa = factory.createNFA(treeRoot);

  std::cout << nfa->printableForm() << std::endl;

  std::vector<std::pair<size_t, size_t>> matchOutput = nfa->engineMatch(testInput, 0);
  std::cout << matchOutput.size() << std::endl;
  for(std::vector<std::pair<size_t, size_t>>::iterator it = matchOutput.begin();
      it != matchOutput.end();
      ++it) {
    std::cout << (*it).first << " " << (*it).second << std::endl;
    std::cout << std::endl;
  }
  

  delete nfa;

  */

}

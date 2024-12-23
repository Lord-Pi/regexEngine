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
  testRegex = "([0-9])";


  RegexEngine re;
  RegexEngine::compile(re, testRegex);
  RegexMatch m;
  re.match(m, "a");
  std::cout << m.getNumGroups() << std::endl;
  for(size_t i = 0; i < m.getNumGroups(); i++) {
    std::cout << m.getGroup(i) << std::endl;
  }

  
  /*
  
  std::vector<RegexToken*> v = RegexTokenizer::stringToTokensPass(testRegex);

  // for(std::vector<RegexToken*>::iterator it = v.begin();
  //     it != v.end();
  //     ++it) {
  //   std::cout << (*it) -> get_str_rep();
  //   std::cout << " ";
  // }
  // std::cout << std::endl;

  std::vector<RegexToken*> v2 = RegexTokenizer::insertConcatPass(v);
  
  // for(std::vector<RegexToken*>::iterator it = v2.begin();
  //     it != v2.end();
  //     ++it) {
  //   std::cout << (*it) -> get_str_rep();
  //   std::cout << " ";
  // }
  // std::cout << std::endl;

  AstNode* treeRoot = RegexParser::shuntingYard(v2);
  // std::cout << treeRoot->toString() << std::endl;

  NFAFactory factory;
  NFA* nfa = factory.createNFA(treeRoot);

  //  std::cout << nfa->printableForm() << std::endl;

  std::vector<std::vector<size_t>> matchOutput = nfa->engineMatch("abc", 0);
  std::cout << matchOutput.size() << std::endl;
  for(std::vector<std::vector<size_t>>::iterator it = matchOutput.begin();
      it != matchOutput.end();
      ++it) {
    std::vector<size_t> inner = *it;
    for(std::vector<size_t>::iterator jt = inner.begin();
	jt != inner.end();
	++jt) {
      std::cout << *jt << " ";
    }
    std::cout << std::endl;
  }
  

  delete nfa;

  */
}

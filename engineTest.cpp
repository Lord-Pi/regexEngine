#include <iostream>
#include <string>
#include <vector>
#include "tokenizerParserTypes.hpp"
#include "regexParser.hpp"
#include "regexTokenizer.hpp"
#include "nfa.hpp"
#include "nfaFactory.hpp"




int main(int argc, char *argv[]) {
  std::string testRegex = "[abc].[^def][]][^]].(a|b|.)+a?|c";
  testRegex = "(0+|1+|23+)";

  
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
  // std::cout << treeRoot->toString() << std::endl;

  NFAFactory factory;
  NFA* nfa = factory.recursiveCreateNFA(treeRoot);

  std::cout << nfa->printableForm() << std::endl;
}

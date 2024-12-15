#include <iostream>
#include <string>
#include <vector>
#include "regexParser.hpp"
#include "regexTokenizer.hpp"


int main(int argc, char *argv[]) {
  std::string testRegex = "[abc].[^def][]][^]].(a|b|.)+a?|c";
  
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
}

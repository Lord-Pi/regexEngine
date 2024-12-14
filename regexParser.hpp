#ifndef __REGEX_PARSER_H_
#define __REGEX_PARSER_H

#include "tokenizerParserTypes.hpp"

#include <stack>
#include <vector>



class RegexParser {
public:
  static std::stack<RegexToken*> shuntingYard(std::vector<RegexToken*> infix);
}


#endif

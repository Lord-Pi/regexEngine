#ifndef __TOKENIZER_PARSER_TYPES_H_
#define __TOKENIZER_PARSER_TYPES_H_

#include <string>


enum TokenType {
  OPENPAREN,
  NONCAPOPENPAREN,
  CLOSEPAREN,
  CONCAT,
  ALTERNATOR,
  KLEENESTAR,
  KLEENEPLUS,
  KLEENEQUESTION,
  CHARCLASS,
  WILDCHAR
};

enum AstNodeType {
  CHARACTER,
  UNARY,
  BINARY,
  GROUP
};


class TokenTypeMethods {
public:
  static std::string tokenTypeToShortString(TokenType tt);
  static std::string tokenTypeToLongString(TokenType tt);
};



#endif

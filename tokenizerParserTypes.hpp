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
  LAZYSTAR,
  LAZYPLUS,
  LAZYQUESTION,
  CHARCLASS,
  WILDCHAR,
  ESCAPE
};

enum AstNodeType {
  TEXT,
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

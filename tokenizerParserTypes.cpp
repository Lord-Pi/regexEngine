#include "tokenizerParserTypes.hpp"

#include <string>



std::string TokenTypeMethods::tokenTypeToShortString(TokenType tt) {
  switch(tt) {
  case OPENPAREN:
    return "(";
  case NONCAPOPENPAREN:
    return "(?:";
  case CLOSEPAREN:
    return ")";
  case CONCAT:
    return "::";
  case ALTERNATOR:
    return "|";
  case KLEENESTAR:
    return "*";
  case KLEENEPLUS:
    return "+";
  case KLEENEQUESTION:
    return "?";
  case LAZYSTAR:
    return "*?";
  case LAZYPLUS:
    return "+?";
  case LAZYQUESTION:
    return "??";
  case WILDCHAR:
    return ".";
  default:
    return "[]";
  }
}

std::string TokenTypeMethods::tokenTypeToLongString(TokenType tt) {
  switch(tt) {
  case OPENPAREN:
    return "OPENPAREN";
  case NONCAPOPENPAREN:
    return "NONCAPOPENPAREN";
  case CLOSEPAREN:
    return "CLOSEPAREN";
  case CONCAT:
    return "CONCAT";
  case ALTERNATOR:
    return "ALTERNATOR";
  case KLEENESTAR:
    return "KLEENESTAR";
  case KLEENEPLUS:
    return "KLEENEPLUS";
  case KLEENEQUESTION:
    return "KLEENEQUESTION";
  case LAZYSTAR:
    return "LAZYSTAR";
  case LAZYPLUS:
    return "LAZYPLUS";
  case LAZYQUESTION:
    return "LAZYQUESTION";
  case WILDCHAR:
    return "WILDCHAR";
  case CHARCLASS:
    return "CHARCLASS";
  default:
    return "UNKNOWN";
  }
}



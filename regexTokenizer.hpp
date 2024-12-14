#ifndef __REGEX_TOKENIZER_H_
#define __REGEX_TOKENIZER_H_

#include <stack>
#include <string>
#include <vector>


class RegexToken {
public:
  virtual TokenType get_token_type() const = 0;
  virtual std::string get_str_rep() const = 0;
};

class OpenParenToken : public RegexToken {
public:
  TokenType get_token_type() const;
  std::string get_str_rep() const;
};

class NonCaptOpenParenToken : public RegexToken {
public:
  TokenType get_token_type() const;
  std::string get_str_rep() const;
};

class CloseParenToken : public RegexToken {
public:
  TokenType get_token_type() const;
  std::string get_str_rep() const;
};

class ConcatToken : public RegexToken {
public:
  TokenType get_token_type() const;
  std::string get_str_rep() const;
};

class AlternatorToken : public RegexToken {
public:
  TokenType get_token_type() const;
  std::string get_str_rep() const;
};

class KleeneStarToken : public RegexToken {
public:
  TokenType get_token_type() const;
  std::string get_str_rep() const;
};

class KleenePlusToken : public RegexToken {
public:
  TokenType get_token_type() const;
  std::string get_str_rep() const;
};

class KleeneQuestionToken : public RegexToken {
public:
  TokenType get_token_type() const;
  std::string get_str_rep() const;
};

class CharacterClassToken : public RegexToken {
private:
  std::string str_rep;
  std::vector<char> acceptedCharacters;
  bool invert_selection;
public:
  CharacterClassToken(std::string s, bool invert);
  TokenType get_token_type() const;
  std::string get_str_rep() const;
};

class WildCharToken : public RegexToken {
public:
  TokenType get_token_type() const;
  std::string get_str_rep() const;
};


class RegexTokenizer {
public:
  static std::vector<RegexToken*> stringToTokensPass(std::string s);
  static std::vector<RegexToken*> insertConcatPass(std::vector<RegexToken*> tokensWithoutConcats);
};

#endif
  

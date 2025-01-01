

#include "regexTokenizer.hpp"
#include "tokenizerParserTypes.hpp"

#include <stack>
#include <string>
#include <vector>


OpenParenToken::OpenParenToken(int groupCount) : groupCount(groupCount) {}
TokenType OpenParenToken::get_token_type() const {
  return OPENPAREN;
}
std::string OpenParenToken::get_str_rep() const {
  return "<G" + std::to_string(groupCount) + ">(";
}
int OpenParenToken::getGroupCount() const {
  return groupCount;
}

TokenType NonCaptOpenParenToken::get_token_type() const {
  return NONCAPOPENPAREN;
}
std::string NonCaptOpenParenToken::get_str_rep() const {
  return "(?:";
}

TokenType CloseParenToken::get_token_type() const {
  return CLOSEPAREN;
}
std::string CloseParenToken::get_str_rep() const {
  return ")";
}

TokenType ConcatToken::get_token_type() const {
  return CONCAT;
}
std::string ConcatToken::get_str_rep() const {
  return "::";
}

TokenType AlternatorToken::get_token_type() const {
  return ALTERNATOR;
}
std::string AlternatorToken::get_str_rep() const {
  return "|";
}

TokenType KleeneStarToken::get_token_type() const {
  return KLEENESTAR;
}
std::string KleeneStarToken::get_str_rep() const {
  return "*";
}

TokenType KleenePlusToken::get_token_type() const {
  return KLEENEPLUS;
}
std::string KleenePlusToken::get_str_rep() const {
  return "+";
}

TokenType KleeneQuestionToken::get_token_type() const {
  return KLEENEQUESTION;
}
std::string KleeneQuestionToken::get_str_rep() const {
  return "?";
}

TokenType LazyStarToken::get_token_type() const {
  return LAZYSTAR;
}
std::string LazyStarToken::get_str_rep() const {
  return "*?";
}

TokenType LazyPlusToken::get_token_type() const {
  return LAZYPLUS;
}
std::string LazyPlusToken::get_str_rep() const {
  return "+?";
}

TokenType LazyQuestionToken::get_token_type() const {
  return LAZYQUESTION;
}
std::string LazyQuestionToken::get_str_rep() const {
  return "??";
}

TokenType CaretAnchorToken::get_token_type() const {
  return CARETANCHOR;
}
std::string CaretAnchorToken::get_str_rep() const {
  return "^";
}

TokenType DollarAnchorToken::get_token_type() const {
  return DOLLARANCHOR;
}
std::string DollarAnchorToken::get_str_rep() const {
  return "$";
}

CharacterClassToken::CharacterClassToken(std::string s, bool invert) {
  invert_selection = invert;
  str_rep = std::string(s.begin(), s.end());
  for(std::string::iterator it = s.begin(); it != s.end(); ++it) {
    acceptedCharacters.push_back(*it);
  }
}
TokenType CharacterClassToken::get_token_type() const {
  return CHARCLASS;
}
bool CharacterClassToken::get_is_inverted() const {
  return invert_selection;
}
std::string CharacterClassToken::get_raw_str_rep() const {
  return str_rep;
}
std::string CharacterClassToken::get_str_rep() const {
  std::string s;
  if(invert_selection) s = "^";
  return "[" + s + str_rep + "]";
}

TokenType WildCharToken::get_token_type() const {
  return WILDCHAR;
}
std::string WildCharToken::get_str_rep() const {
  return ".";
}

EscapeToken::EscapeToken(char c) {
  escapedChar = c;
}
TokenType EscapeToken::get_token_type() const {
  return ESCAPE;
}
std::string EscapeToken::get_str_rep() const {
  std::string retVal = "\\";
  retVal.push_back(escapedChar);
  return retVal;
}
char EscapeToken::getEscapedChar() const {
  return escapedChar;
}

std::vector<RegexToken*> RegexTokenizer::stringToTokensPass(std::string s) {
  int groupCount = 1;
  std::vector<RegexToken*> v;
  for(std::string::iterator it = s.begin(); it != s.end(); ++it) {
    switch(*it) {
    case '(':
      {
	if(it+1 != s.end() && *(it+1)=='?' && it+2 != s.end() && *(it+2)==':') {
	  v.push_back(new NonCaptOpenParenToken());
	  it += 2;
	} else {
	  v.push_back(new OpenParenToken(groupCount++));
	}
	break;
      }
    case ')':
      {
	v.push_back(new CloseParenToken());
	break;
      }
    case '|':
      {
	v.push_back(new AlternatorToken());
	break;
      }
    case '*':
      {
	if(it+1 != s.end() && *(it+1)=='?') {
	  v.push_back(new LazyStarToken());
	  ++it;
	} else {
	  v.push_back(new KleeneStarToken());
	}
	break;
      }
    case '+':
      {
	if(it+1 != s.end() && *(it+1)=='?') {
	  v.push_back(new LazyPlusToken());
	  ++it;
	} else {
	  v.push_back(new KleenePlusToken());
	}
	break;
      }
    case '?':
      {
	if(it+1 != s.end() && *(it+1)=='?') {
	  v.push_back(new LazyQuestionToken());
	  ++it;
	} else {
	  v.push_back(new KleeneQuestionToken());
	}
	break;
      }
    case '^':
      {
	v.push_back(new CaretAnchorToken());
	break;
      }
    case '$':
      {
	v.push_back(new DollarAnchorToken());
	break;
      }
    case '.':
      {
	v.push_back(new WildCharToken());
	break;
      }
    case '\\':
      {
	v.push_back(new EscapeToken(*(++it)));
	break;
      }
    default:
      {
	std::vector<char> cv;
	bool invert = false;
	if(*it == '[') {
	  ++it;
	  if(*it == '^') {
	    invert = true;
	    ++it;
	  }
	  if(*it == ']') {
	    // Special case for including the closing square bracket,
	    // which can only be the first character in the character class
	    cv.push_back(*it);
	    ++it;
	  }
	  std::string remainingString(it, s.end());
	  size_t closeLocation = remainingString.find(']');
	  for(std::string::iterator jt=remainingString.begin();
	      jt != remainingString.begin()+closeLocation;
	      ++jt) {
	    cv.push_back(*jt);
	    ++it;
	  }
	  v.push_back(new CharacterClassToken(std::string(cv.begin(), cv.end()), invert));
	} else {
	  cv.push_back(*it);
	  v.push_back(new CharacterClassToken(std::string(cv.begin(), cv.end()), invert));
	}
	break;
      }
    }
  }
  return v;
}


std::vector<RegexToken*> RegexTokenizer::insertConcatPass(std::vector<RegexToken*> tokensWithoutConcats) {
  std::vector<RegexToken*> v2;
  if(tokensWithoutConcats.size() < 2) {
    v2 = tokensWithoutConcats;
    return v2;
  }
  v2.push_back(tokensWithoutConcats[0]);
  for(std::vector<RegexToken*>::iterator it=tokensWithoutConcats.begin()+1;
      it != tokensWithoutConcats.end();
      ++it) {
    RegexToken* prevToken = *(it-1);
    bool okPrevToken;
    switch(prevToken->get_token_type()) {
    case OPENPAREN:
    case NONCAPOPENPAREN:
    case CONCAT:
    case ALTERNATOR:
      okPrevToken = false;
      break;
    case CLOSEPAREN:
    case KLEENESTAR:
    case KLEENEPLUS:
    case KLEENEQUESTION:
    case LAZYSTAR:
    case LAZYPLUS:
    case LAZYQUESTION:
    case CARETANCHOR:
    case DOLLARANCHOR:
    case CHARCLASS:
    case WILDCHAR:
    default:
      okPrevToken = true;
      break;
    }
    RegexToken* currToken = *it;
    bool okCurrToken;
    switch(currToken->get_token_type()) {
    case OPENPAREN:
    case NONCAPOPENPAREN:
    case CARETANCHOR:
    case DOLLARANCHOR:
    case CHARCLASS:
    case WILDCHAR:
      okCurrToken = true;
      break;
    case CLOSEPAREN:
    case CONCAT:
    case ALTERNATOR:
    case KLEENESTAR:
    case KLEENEPLUS:
    case KLEENEQUESTION:
    case LAZYSTAR:
    case LAZYPLUS:
    case LAZYQUESTION:
    default:
      okCurrToken = false;
      break;
    }

    if(okPrevToken && okCurrToken) {
      v2.push_back(new ConcatToken());
    }
    v2.push_back(currToken);
    
  }

  return v2;
}

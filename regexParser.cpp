#include "regexParser.hpp"
#include "tokenizerParserTypes.hpp"
#include "regexTokenizer.hpp"

#include <cassert>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <vector>


std::string AstCharacterNode::toString() const {
  return "CHAR(" + tkn->get_str_rep() + ")";
}

std::string AstUnaryOpNode::toString() const {
  return TokenTypeMethods::tokenTypeToLongString(op) + "(" +
    child->toString() + ")";
}

std::string AstBinaryOpNode::toString() const {
  return TokenTypeMethods::tokenTypeToLongString(op) + "(" +
    leftChild->toString() + ", " +
    rightChild->toString() + ")";  
}

std::string AstGroupNode::toString() const {
  if(capturing) {
    return "CAPTUREGROUP(" + child->toString() + ")";
  } else {
    return "NOCAPGROUP(" + child->toString() + ")";
  }
}


int RegexParser::getOperatorPrecedence(TokenType op) {
  switch(op) {
  case ALTERNATOR:
    return 1;
  case CONCAT:
    return 3;
  case KLEENESTAR:
  case KLEENEPLUS:
  case KLEENEQUESTION:
    return 4;
  case OPENPAREN:
  case NONCAPOPENPAREN:
  case CLOSEPAREN:
    return 5;
  default:
    throw std::invalid_argument("Received non-operator token in operator precedence conversion");
  }
}

void RegexParser::applyOperatorToStack(std::stack<AstNode*> &s, TokenType op) {
  switch(op) {
  case OPENPAREN:
  case NONCAPOPENPAREN:
    {
      AstNode* inner = s.top();
      s.pop();
      AstGroupNode* newTop = new AstGroupNode(inner, op==OPENPAREN);
      s.push(newTop);
      break;
    }
  case CONCAT:
  case ALTERNATOR:
    {
      AstNode* right = s.top();
      s.pop();
      AstNode* left = s.top();
      s.pop();
      AstBinaryOpNode* newTop = new AstBinaryOpNode(left, right, op);
      s.push(newTop);
      break;
    }
  case KLEENESTAR:
  case KLEENEPLUS:
  case KLEENEQUESTION:
    {
      AstNode* inner = s.top();
      s.pop();
      AstUnaryOpNode* newTop = new AstUnaryOpNode(inner, op);
      s.push(newTop);
      break;
    }
  default:
    throw std::invalid_argument("Received non-operator token in stack application");
  }
}

void RegexParser::shuntingYardInternal(std::stack<AstNode*> &outputLine,
				       std::stack<RegexToken*> &operatorStandby,
				       RegexToken* &tok) {
  switch(tok->get_token_type()) {
  case CHARCLASS:
  case WILDCHAR:
    {
      outputLine.push(new AstCharacterNode(tok));
      break;
    }
  case OPENPAREN:
  case NONCAPOPENPAREN:
    {
      operatorStandby.push(tok);
      break;
    }
  case CLOSEPAREN:
    {
      do {
	applyOperatorToStack(outputLine, (operatorStandby.top())->get_token_type());
	RegexToken* used = operatorStandby.top();
	operatorStandby.pop();
	free(used);
      } while((!operatorStandby.empty()) &&
	      (operatorStandby.top())->get_token_type() != OPENPAREN &&
	      (operatorStandby.top())->get_token_type() != NONCAPOPENPAREN);
      break;
    }
  default:
    {
      while(!operatorStandby.empty() &&
	    (operatorStandby.top())->get_token_type() != OPENPAREN &&
	    (operatorStandby.top())->get_token_type() != NONCAPOPENPAREN &&
	    getOperatorPrecedence((operatorStandby.top())->get_token_type()) >= getOperatorPrecedence(tok->get_token_type())) {
	applyOperatorToStack(outputLine, (operatorStandby.top())->get_token_type());
	RegexToken* used = operatorStandby.top();
	operatorStandby.pop();
	free(used);
      }
      operatorStandby.push(tok);
    }
  }
  
}


AstNode* RegexParser::shuntingYard(std::vector<RegexToken*> infix) {

  std::stack<AstNode*> outputLine;
  std::stack<RegexToken*> operatorStandby;

  for(std::vector<RegexToken*>::iterator it = infix.begin();
      it != infix.end();
      ++it) {
    shuntingYardInternal(outputLine, operatorStandby, *it);
  }
  while(!operatorStandby.empty()) {
    RegexToken* nextTok = operatorStandby.top();
    operatorStandby.pop();
    applyOperatorToStack(outputLine, nextTok->get_token_type());
  }

  assert(outputLine.size() == 1);

  return outputLine.top();
  
}

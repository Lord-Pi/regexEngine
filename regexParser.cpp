#include "regexParser.hpp"
#include "tokenizerParserTypes.hpp"
#include "regexTokenizer.hpp"

#include <cassert>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <vector>


AstCharacterNode::~AstCharacterNode() {
  delete tkn;
}
std::string AstCharacterNode::toString() const {
  return "CHAR(" + tkn->get_str_rep() + ")";
}
AstNodeType AstCharacterNode::getAstNodeType() const {
  return CHARACTER;
}
RegexToken* AstCharacterNode::getToken() {
  return tkn;
}

AstUnaryOpNode::~AstUnaryOpNode() {
  delete child;
}
std::string AstUnaryOpNode::toString() const {
  return TokenTypeMethods::tokenTypeToLongString(op) + "(" +
    child->toString() + ")";
}
AstNodeType AstUnaryOpNode::getAstNodeType() const {
  return UNARY;
}
TokenType AstUnaryOpNode::getTokenType() const {
  return op;
}
AstNode* AstUnaryOpNode::getChild() {
  return child;
}

AstBinaryOpNode::~AstBinaryOpNode() {
  delete leftChild;
  delete rightChild;
}
std::string AstBinaryOpNode::toString() const {
  return TokenTypeMethods::tokenTypeToLongString(op) + "(" +
    leftChild->toString() + ", " +
    rightChild->toString() + ")";  
}
AstNodeType AstBinaryOpNode::getAstNodeType() const {
  return BINARY;
}
TokenType AstBinaryOpNode::getTokenType() const {
  return op;
}
AstNode* AstBinaryOpNode::getLeftChild() {
  return leftChild;
}
AstNode* AstBinaryOpNode::getRightChild() {
  return rightChild;
}

AstGroupNode::~AstGroupNode() {
  delete child;
}
std::string AstGroupNode::toString() const {
  if(capturing) {
    return "CAPTUREGROUP(" + child->toString() + ")";
  } else {
    return "NOCAPGROUP(" + child->toString() + ")";
  }
}
AstNodeType AstGroupNode::getAstNodeType() const {
  return GROUP;
}
AstNode* AstGroupNode::getChild() {
  return child;
}
int AstGroupNode::getGroupNum() const {
  return groupNum;
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
  case LAZYSTAR:
  case LAZYPLUS:
  case LAZYQUESTION:
    return 4;
  case OPENPAREN:
  case NONCAPOPENPAREN:
  case CLOSEPAREN:
    return 5;
  default:
    throw std::invalid_argument("Received non-operator token in operator precedence conversion");
  }
}

void RegexParser::applyOperatorToStack(std::stack<AstNode*> &s, TokenType op, int groupNum) {
  switch(op) {
  case OPENPAREN:
  case NONCAPOPENPAREN:
    {
      AstNode* inner = s.top();
      s.pop();
      AstGroupNode* newTop = new AstGroupNode(inner, op==OPENPAREN, groupNum);
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
  case LAZYSTAR:
  case LAZYPLUS:
  case LAZYQUESTION:
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
      while((!operatorStandby.empty()) &&
	    (operatorStandby.top())->get_token_type() != OPENPAREN &&
	    (operatorStandby.top())->get_token_type() != NONCAPOPENPAREN) {
	applyOperatorToStack(outputLine, (operatorStandby.top())->get_token_type(), -1);
	RegexToken* used = operatorStandby.top();
	operatorStandby.pop();
	delete used;
      } 
      
      int groupNum = -1;
      if((operatorStandby.top())->get_token_type() == OPENPAREN) {
	groupNum = (static_cast<OpenParenToken*>(operatorStandby.top()))->getGroupCount();
      }
      applyOperatorToStack(outputLine, (operatorStandby.top())->get_token_type(), groupNum);
      RegexToken* used = operatorStandby.top();
      operatorStandby.pop();
      delete used;
      delete tok;
      break;
    }
  default:
    {
      while(!operatorStandby.empty() &&
	    (operatorStandby.top())->get_token_type() != OPENPAREN &&
	    (operatorStandby.top())->get_token_type() != NONCAPOPENPAREN &&
	    getOperatorPrecedence((operatorStandby.top())->get_token_type()) >= getOperatorPrecedence(tok->get_token_type())) {
	applyOperatorToStack(outputLine, (operatorStandby.top())->get_token_type(), -1);
	RegexToken* used = operatorStandby.top();
	operatorStandby.pop();
	delete used;
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
    // Since we should have already seen all the open parens,
    // I think I can just ignore group numbers here?
    applyOperatorToStack(outputLine, nextTok->get_token_type(), -1);

    // Delete used operators here otherwise they are lost forever
    delete nextTok;
  }

  assert(outputLine.size() == 1);

  return outputLine.top();
  
}




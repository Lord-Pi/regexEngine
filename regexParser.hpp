#ifndef __REGEX_PARSER_H_
#define __REGEX_PARSER_H

#include "tokenizerParserTypes.hpp"
#include "regexTokenizer.hpp"

#include <stack>
#include <vector>


class AstNode {

};

class AstCharacterNode : public AstNode {
private:
  RegexToken* tkn;
public:
  AstCharacterNode(RegexToken* tkn) : tkn(tkn) {}
};

class AstUnaryOpNode : public AstNode {
private:
  AstNode* child;
  TokenType op;
public:
  AstUnaryOpNode(AstNode* child, TokenType op) : child(child),
						 op(op) {}
};

class AstBinaryOpNode : public AstNode {
private:
  AstNode* leftChild;
  AstNode* rightChild;
  TokenType op;
public:
  AstBinaryOpNode(AstNode* leftChild, AstNode* rightChild, TokenType op) : leftChild(leftChild),
									   rightChild(rightChild),
									   op(op) {}
};

class AstGroupNode : public AstNode {
private:
  AstNode* child;
  bool capturing;
public:
  AstGroupNode(AstNode* child, bool capturing) : child(child),
						 capturing(capturing) {}
};


class RegexParser {
private:
  static int getOperatorPrecedence(TokenType op);
  static void applyOperatorToStack(std::stack<AstNode*> &s, TokenType op);
  static void shuntingYardInternal(std::stack<AstNode*> &outputLine,
				   std::stack<RegexToken*> &operatorStandby,
				   RegexToken* &tok);
public:
  static AstNode* shuntingYard(std::vector<RegexToken*> infix);
};


#endif

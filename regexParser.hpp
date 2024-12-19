#ifndef __REGEX_PARSER_H_
#define __REGEX_PARSER_H

#include "tokenizerParserTypes.hpp"
#include "regexTokenizer.hpp"

#include <stack>
#include <string>
#include <vector>


class AstNode {
public:
  virtual std::string toString() const = 0;
};

class AstCharacterNode : public AstNode {
private:
  RegexToken* tkn;
public:
  AstCharacterNode(RegexToken* tkn) : tkn(tkn) {}
  std::string toString() const;
  AstNodeType getAstNodeType() const;
};

class AstUnaryOpNode : public AstNode {
private:
  AstNode* child;
  TokenType op;
public:
  AstUnaryOpNode(AstNode* child, TokenType op) : child(child),
						 op(op) {}
  std::string toString() const;
  AstNodeType getAstNodeType() const;
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
  std::string toString() const;
  AstNodeType getAstNodeType() const;
};

class AstGroupNode : public AstNode {
private:
  AstNode* child;
  bool capturing;
public:
  AstGroupNode(AstNode* child, bool capturing) : child(child),
						 capturing(capturing) {}
  std::string toString() const;
  AstNodeType getAstNodeType() const;
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

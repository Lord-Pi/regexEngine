
#include "nfa.hpp"
#include "nfaFactory.hpp"
#include "regexParser.hpp"
#include "regexTokenizer.hpp"
#include "tokenizerParserTypes.hpp"


#include <iostream>
#include <stdexcept>
#include <string>


NFAFactory::NFAFactory() {
  stateNameTracker = 0;
}


NFA* NFAFactory::recursiveCreateNFA(AstNode* ast) {
  std::string startStateLabel = "S"+std::to_string(stateNameTracker++);
  std::string endStateLabel = "S"+std::to_string(stateNameTracker++);
  switch(ast->getAstNodeType()) {
  case TEXT:
    {
      NFA* newNFA = new NFA(startStateLabel, endStateLabel);
      AstTextNode* charAst = dynamic_cast<AstTextNode*>(ast);
      if(charAst->getToken()->get_token_type() == WILDCHAR) {
	Transition* wildTransition = new CharacterTransition("\n",
							     true,
							     newNFA->getStates()[newNFA->getEndStateIdx()]);
	newNFA->getStates()[newNFA->getStartStateIdx()]->addTransition(wildTransition);
      } else if(charAst->getToken()->get_token_type() == ESCAPE) {
	EscapeToken* escapeToken = dynamic_cast<EscapeToken*>(charAst->getToken());
	char escapedChar = escapeToken->getEscapedChar();
	switch(escapedChar) {
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	  {
	    Transition* tn = new BackreferenceTransition(escapedChar-'0',
							 newNFA->getStates()[newNFA->getEndStateIdx()]);
	    newNFA->getStates()[newNFA->getStartStateIdx()]->addTransition(tn);
	    break;
	  }
	case 'd':
	  {
	    Transition* tn = new CharacterTransition("0123456789",
						     false,
						     newNFA->getStates()[newNFA->getEndStateIdx()]);
	    newNFA->getStates()[newNFA->getStartStateIdx()]->addTransition(tn);
	    break;
	  }
	case 'D':
	  {
	    Transition* tn = new CharacterTransition("0123456789",
						     true,
						     newNFA->getStates()[newNFA->getEndStateIdx()]);
	    newNFA->getStates()[newNFA->getStartStateIdx()]->addTransition(tn);
	    break;
	  }
	case '.':
	case '^':
	case '$':
	case '*':
	case '+':
	case '?':
	case '{':
	case '}':
	case '[':
	case ']':
	case '(':
	case ')':
	case '|':
	case '\\':
	  {
	    Transition* tn = new CharacterTransition(std::to_string(escapedChar),
						     false,
						     newNFA->getStates()[newNFA->getEndStateIdx()]);
	    newNFA->getStates()[newNFA->getStartStateIdx()]->addTransition(tn);
	    break;
	  }					
	default:
	  {
	    throw std::invalid_argument("Currently unsupported escape sequence: " +
					std::to_string(escapedChar));
	  }
	}
      } else {
	// This should probably hopefully be a CharacterClassToken
	CharacterClassToken* classToken = dynamic_cast<CharacterClassToken*>(charAst->getToken());
	Transition* classTransition = new CharacterTransition(classToken->get_raw_str_rep(),
							      classToken->get_is_inverted(),
							      newNFA->getStates()[newNFA->getEndStateIdx()]);
	newNFA->getStates()[newNFA->getStartStateIdx()]->addTransition(classTransition);
      }
      delete charAst;
      return newNFA;
    }
  case UNARY:
    {
      AstUnaryOpNode* unaryAst = dynamic_cast<AstUnaryOpNode*>(ast);
      NFA* inner = recursiveCreateNFA(unaryAst->getChild());
      NFA* outer = new NFA(startStateLabel, endStateLabel);

      Transition* epsilon1 = new EpsilonTransition(inner->getStates()[inner->getStartStateIdx()]);
      Transition* epsilon2 = new EpsilonTransition(outer->getStates()[outer->getEndStateIdx()]);
      outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon1);
      inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon2);

      // The transitions go onto a stack during execution and are therefore
      // processed in reverse of the order in which they're added
      switch(unaryAst->getTokenType()) {
      case KLEENESTAR:
	{
	  Transition* epsilon3 = new EpsilonTransition(outer->getStates()[outer->getEndStateIdx()]);
	  Transition* epsilon4 = new EpsilonTransition(inner->getStates()[inner->getStartStateIdx()]);
	  
	  inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon2);
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon3);
	  
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon1);
	  inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon4);
	  break;
	}
      case KLEENEPLUS:
	{
	  Transition* epsilon4 = new EpsilonTransition(inner->getStates()[inner->getStartStateIdx()]);
	  
	  inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon2);
	  
	  inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon4);
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon1);
	  break;
	}
      case KLEENEQUESTION:
	{
	  Transition* epsilon3 = new EpsilonTransition(outer->getStates()[outer->getEndStateIdx()]);
	  
	  inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon2);
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon3);
	  
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon1);
	  break;
	}
      case LAZYSTAR:
	{
	  Transition* epsilon3 = new EpsilonTransition(outer->getStates()[outer->getEndStateIdx()]);
	  Transition* epsilon4 = new EpsilonTransition(inner->getStates()[inner->getStartStateIdx()]);
	  
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon1);
	  inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon4);
	  
	  inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon2);
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon3);
	  break;
	}
      case LAZYPLUS:
	{
	  Transition* epsilon4 = new EpsilonTransition(inner->getStates()[inner->getStartStateIdx()]);
	  
	  inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon4);
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon1);
	  
	  inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon2);
	  break;
	}
      case LAZYQUESTION:
	{
	  Transition* epsilon3 = new EpsilonTransition(outer->getStates()[outer->getEndStateIdx()]);
	  
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon1);
	  
	  inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon2);
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon3);

	  break;
	}
      default:
	throw std::invalid_argument("Got a unary operation of an invalid type");
      }
      std::vector<State*> innerStates = inner->getStates();
      for(std::vector<State*>::iterator it = innerStates.begin();
	  it != innerStates.end();
	  ++it) {
	outer->addState(*it);
      }
      inner->clearStates();
      delete inner;
      return outer;
    }
  case BINARY:
    {
      AstBinaryOpNode* binaryAst = dynamic_cast<AstBinaryOpNode*>(ast);
      NFA* outer = new NFA(startStateLabel, endStateLabel);
      NFA* innerLeft = recursiveCreateNFA(binaryAst->getLeftChild());
      NFA* innerRight = recursiveCreateNFA(binaryAst->getRightChild());
      
      Transition* epsilon1 = new EpsilonTransition(innerLeft->getStates()[innerLeft->getStartStateIdx()]);
      Transition* epsilon2 = new EpsilonTransition(outer->getStates()[outer->getEndStateIdx()]);
      outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon1);
      innerRight->getStates()[innerRight->getEndStateIdx()]->addTransition(epsilon2);
      
      Transition* epsilon3 = new EpsilonTransition(innerRight->getStates()[innerRight->getStartStateIdx()]);

      switch(binaryAst->getTokenType()) {
      case ALTERNATOR:
	{
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon3);
	  Transition* epsilon4 = new EpsilonTransition(outer->getStates()[outer->getEndStateIdx()]);
	  innerLeft->getStates()[innerLeft->getEndStateIdx()]->addTransition(epsilon4);
	  break;
	}
      case CONCAT:
	{
	  innerLeft->getStates()[innerLeft->getEndStateIdx()]->addTransition(epsilon3);
	  break;
	}
      default:
	throw std::invalid_argument("Got a binary operation of an invalid type");
      }

      std::vector<State*> leftStates = innerLeft->getStates();
      for(std::vector<State*>::iterator it = leftStates.begin();
	  it != leftStates.end();
	  ++it) {
	outer->addState(*it);
      }

      std::vector<State*> rightStates = innerRight->getStates();
      for(std::vector<State*>::iterator jt = rightStates.begin();
	  jt != rightStates.end();
	  ++jt) {
	outer->addState(*jt);
      }

      innerLeft->clearStates();
      innerRight->clearStates();
      delete innerLeft;
      delete innerRight;

      return outer;
    }
  case GROUP:
    {
      AstGroupNode* groupAst = dynamic_cast<AstGroupNode*>(ast);
      NFA* inner = recursiveCreateNFA(groupAst->getChild());
      NFA* outer = new NFA(startStateLabel, endStateLabel);
      int groupNum = groupAst->getGroupNum();
      Transition* epsilon1 = new EpsilonTransition(inner->getStates()[inner->getStartStateIdx()]);
      if(groupNum >= 0)
	epsilon1->addGroupStart(groupNum);
      Transition* epsilon2 = new EpsilonTransition(outer->getStates()[outer->getEndStateIdx()]);
      if(groupNum >= 0)
	epsilon2->addGroupEnd(groupNum);
      outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon1);
      inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon2);
      std::vector<State*> innerStates = inner->getStates();
      for(std::vector<State*>::iterator it = innerStates.begin();
	  it != innerStates.end();
	  ++it) {
	outer->addState(*it);
      }
      inner->clearStates();
      delete inner;
      return outer;
    }
  default:
    throw std::invalid_argument("Somehow got an AST node type that does not exist");
  }
}

NFA* NFAFactory::createNFA(AstNode* ast) {
  NFA* inner = recursiveCreateNFA(ast);
  NFA* outer = new NFA("S_BEGIN", "S_END");
  Transition* epsilon1 = new EpsilonTransition(inner->getStates()[inner->getStartStateIdx()]);
  epsilon1->addGroupStart(0);
  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon1);
  Transition* epsilon2 = new EpsilonTransition(outer->getStates()[outer->getEndStateIdx()]);
  epsilon2->addGroupEnd(0);
  inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon2);
  
  std::vector<State*> innerStates = inner->getStates();
  for(std::vector<State*>::iterator it = innerStates.begin();
      it != innerStates.end();
      ++it) {
    outer->addState(*it);
  }
  inner->clearStates();
  delete inner;

  std::cout << outer->printableForm() << std::endl;
  
  return outer;
}

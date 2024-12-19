#include "nfa.hpp"
#include "nfaFactory.hpp"
#include "regexParser.hpp"
#include "regexTokenizer.hpp"
#include "tokenizerParserTypes.hpp"

#include <stdexcept>
#include <string>


NFAFactory::NFAFactory() {
  groupTracker = 0;
  stateNameTracker = 0;
}


NFA* NFAFactory::recursiveCreateNFA(AstNode* ast) {
  std::string startStateLabel = "S"+std::to_string(stateNameTracker++);
  std::string endStateLabel = "S"+std::to_string(stateNameTracker++);
  NFA* newNFA = new NFA(startStateLabel, endStateLabel);
  switch(ast->getAstNodeType()) {
  case CHARACTER:
    {
      AstCharacterNode* charAst = dynamic_cast<AstCharacterNode*>(ast);
      if(charAst->getToken()->get_token_type() == WILDCHAR) {
	Transition* wildTransition = new Transition("\n",
						    true,
						    newNFA->getStates()[newNFA->getEndStateIdx()]);
	newNFA->getStates()[newNFA->getStartStateIdx()]->addTransition(wildTransition);
      } else {
	// This should probably hopefully be a CharacterClassToken
	CharacterClassToken* classToken = dynamic_cast<CharacterClassToken*>(charAst->getToken());
	Transition* classTransition = new Transition(classToken->get_raw_str_rep(),
						     classToken->get_is_inverted(),
						     newNFA->getStates()[newNFA->getEndStateIdx()]);
	newNFA->getStates()[newNFA->getStartStateIdx()]->addTransition(classTransition);
      }
      return newNFA;
    }
  case UNARY:
    {
      AstUnaryOpNode* unaryAst = dynamic_cast<AstUnaryOpNode*>(ast);
      NFA* inner = recursiveCreateNFA(unaryAst->getChild());
      NFA* outer = new NFA(startStateLabel, endStateLabel);

      Transition* epsilon1 = new Transition("",
					    false,
					    inner->getStates()[inner->getStartStateIdx()]);
      Transition* epsilon2 = new Transition("",
					    false,
					    outer->getStates()[outer->getEndStateIdx()]);
      outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon1);
      inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon2);

      switch(unaryAst->getTokenType()) {
      case KLEENESTAR:
	{
	  Transition* epsilon3 = new Transition("",
						false,
						outer->getStates()[outer->getEndStateIdx()]);
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon3);
	  Transition* epsilon4 = new Transition("",
						false,
						inner->getStates()[inner->getStartStateIdx()]);
	  inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon4);
	  break;
	}
      case KLEENEPLUS:
	{
	  Transition* epsilon3 = new Transition("",
						false,
						inner->getStates()[inner->getStartStateIdx()]);
	  inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon3);
	  break;
	}
      case KLEENEQUESTION:
	{
	  Transition* epsilon3 = new Transition("",
						false,
						outer->getStates()[outer->getEndStateIdx()]);
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon3);
	  break;
	}
      default:
	throw std::invalid_argument("Got a unary operation of an invalid type");
      }
      for(std::vector<State*>::iterator it = inner->getStates().begin();
	  it != inner->getStates().end();
	  ++it) {
	outer->addState(*it);
      }
      delete inner;
      return outer;
    }
  case BINARY:
    {
      AstBinaryOpNode* binaryAst = dynamic_cast<AstBinaryOpNode*>(ast);
      NFA* outer = new NFA(startStateLabel, endStateLabel);
      NFA* innerLeft = recursiveCreateNFA(binaryAst->getLeftChild());
      NFA* innerRight = recursiveCreateNFA(binaryAst->getRightChild());
      
      Transition* epsilon1 = new Transition("",
					    false,
					    innerLeft->getStates()[innerLeft->getStartStateIdx()]);
      Transition* epsilon2 = new Transition("",
					    false,
					    outer->getStates()[outer->getEndStateIdx()]);
      outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon1);
      innerRight->getStates()[innerRight->getEndStateIdx()]->addTransition(epsilon2);

      Transition* epsilon3 = new Transition("",
					    false,
					    innerRight->getStates()[innerRight->getStartStateIdx()]);
      
      switch(binaryAst->getTokenType()) {
      case ALTERNATOR:
	{
	  outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon3);
	  Transition* epsilon4 = new Transition("",
						false,
						outer->getStates()[outer->getEndStateIdx()]);
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

      for(std::vector<State*>::iterator it = innerLeft->getStates().begin();
	  it != innerLeft->getStates().end();
	  ++it) {
	outer->addState(*it);
      }
      for(std::vector<State*>::iterator jt = innerRight->getStates().begin();
	  jt != innerRight->getStates().end();
	  ++jt) {
	outer->addState(*jt);
      }

      delete innerLeft;
      delete innerRight;
      return outer;
    }
  case GROUP:
    {
      AstGroupNode* groupAst = dynamic_cast<AstGroupNode*>(ast);
      NFA* inner = recursiveCreateNFA(groupAst->getChild());
      NFA* outer = new NFA(startStateLabel, endStateLabel);
      Transition* epsilon1 = new Transition("",
					    false,
					    inner->getStates()[inner->getStartStateIdx()]);
      Transition* epsilon2 = new Transition("",
					    false,
					    outer->getStates()[outer->getEndStateIdx()]);
      outer->getStates()[outer->getStartStateIdx()]->addTransition(epsilon1);
      inner->getStates()[inner->getEndStateIdx()]->addTransition(epsilon2);
      for(std::vector<State*>::iterator it = inner->getStates().begin();
	  it != inner->getStates().end();
	  ++it) {
	outer->addState(*it);
      }
      delete inner;
      return outer;
    }
  default:
    throw std::invalid_argument("Somehow got an AST node type that does not exist");
}
}

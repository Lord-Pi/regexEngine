#ifndef __NFA_FACTORY_H_
#define __NFA_FACTORY_H_


#include "nfa.hpp"
#include "regexParser.hpp"


class NFAFactory {
private:
  int stateNameTracker;
  NFA* recursiveCreateNFA(AstNode* ast);
  
public:
  NFAFactory();
  NFA* createNFA(AstNode* ast);

};


#endif

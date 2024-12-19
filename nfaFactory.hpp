#ifndef __NFA_FACTORY_H_
#define __NFA_FACTORY_H_


#include "nfa.hpp"
#include "regexParser.hpp"


class NFAFactory {
private:
  int groupTracker;
  int stateNameTracker;

public:
  NFAFactory();
  NFA* recursiveCreateNFA(AstNode* ast);
};


#endif

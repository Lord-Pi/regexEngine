#ifndef __NFA_H_
#define __NFA_H_

#include "characterClass.hpp"

#include <optional>
#include <string>
#include <vector>


class Transition;
class State;


class Transition {
private:
  // Set to None for an epsilon transition
  std::string label;
  std::optional<CharacterClassDecider> ccd_o;
  State* destination;
  std::vector<int> groupStarts;
  std::vector<int> groupEnds;

public:
  Transition(std::string label, bool inverted, State* destination);
  bool isEpsilon() const;
  State* getDestination() const;
  bool characterMatches(std::string s, int idx);
  void addGroupStart(int i);
  void addGroupEnd(int i);
  std::vector<int> getGroupStarts();
  std::vector<int> getGroupEnds();
};


class State {
private:
  bool isAccepting;
  std::string label;
  std::vector<Transition*> transitions;

public:
  State(std::string label, bool isAccepting);
  
  bool getIsAccepting() const;
  void setIsAccepting(bool b);

  std::string getLabel() const;
  void setLabel(std::string s);

  std::vector<Transition*> getTransitions();
  void addTransition(Transition* t);
  
};


class NFA {
private:
  std::vector<State*> states;
  int startStateIdx;
  int endStateIdx;
  
public:
  NFA(std::string l1, std::string l2);
  State* getStartState();
  State* getEndState();
  void addState(State* s);
};



#endif

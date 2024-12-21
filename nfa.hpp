#ifndef __NFA_H_
#define __NFA_H_

#include "characterClass.hpp"

#include <cstddef>
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
  std::vector<size_t> groupStarts;
  std::vector<size_t> groupEnds;

public:
  Transition(std::string label, bool inverted, State* destination);
  bool isEpsilon() const;
  State* getDestination() const;
  bool characterMatches(std::string s, size_t idx);
  void addGroupStart(size_t i);
  void addGroupEnd(size_t i);
  std::vector<size_t> getGroupStarts();
  std::vector<size_t> getGroupEnds();
  std::string printableForm();
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

  std::string printableForm();  
};


class ExecutionMemoryObject {
private:
  State* currentState;
  size_t stringIdx;
  std::vector<bool> groupRecording;
  std::vector<std::string> groupContents;
  std::vector<State*> epsilonLoopTracker;
  
public:
  ExecutionMemoryObject(State* startState, size_t stringStartIdx, size_t groupCount);
  ExecutionMemoryObject(const ExecutionMemoryObject &emo);

  void applyTransition(std::string str, Transition* t);
  bool isStateInEpsilonLoop(State* s);
  State* getCurrentState() const;
  size_t getStringIdx() const;
  std::vector<std::string> getGroupContents() const;
};


class NFA {
private:
  std::vector<State*> states;
  size_t startStateIdx;
  size_t endStateIdx;

  size_t getGroupCount();
  
public:
  NFA(std::string l1, std::string l2);
  std::vector<State*> getStates();
  size_t getStartStateIdx() const;
  size_t getEndStateIdx() const;
  void addState(State* s);
  std::string printableForm();
  std::vector<std::string> engineMatch(std::string input, size_t stringStartIdx);
  
};



#endif

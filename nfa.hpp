#ifndef __NFA_H_
#define __NFA_H_

#include "characterClass.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <utility>
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
  ~State();
  
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
  std::vector<std::vector<size_t>> groupContents;
  std::vector<size_t> groupStarts;
  std::vector<size_t> groupEnds;
  std::vector<State*> epsilonLoopTracker;
  
public:
  ExecutionMemoryObject(State* startState, size_t stringStartIdx, size_t groupCount);
  ExecutionMemoryObject(const ExecutionMemoryObject &emo);

  void applyTransition(Transition* t);
  bool isStateInEpsilonLoop(State* s);
  State* getCurrentState() const;
  size_t getStringIdx() const;
  std::vector<std::vector<size_t>> getGroupContents() const;
  std::vector<size_t> getGroupStarts() const;
  std::vector<size_t> getGroupEnds() const;
  std::string getPrintableForm();
};


class NFA {
private:
  std::vector<State*> states;
  size_t startStateIdx;
  size_t endStateIdx;

  size_t getGroupCount();

  static std::vector<std::pair<size_t, size_t>> zipGroupBounds(std::pair<
							       std::vector<size_t>,
							       std::vector<size_t>> groupBounds);
  
public:
  NFA(std::string l1, std::string l2);
  ~NFA();
  std::vector<State*> getStates();
  size_t getStartStateIdx() const;
  size_t getEndStateIdx() const;
  void addState(State* s);
  void clearStates();
  std::string printableForm();
  std::vector<std::pair<size_t, size_t>> engineMatch(std::string input,
						     size_t stringStartIdx);
  
};



#endif

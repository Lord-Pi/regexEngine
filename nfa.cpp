#include "nfa.hpp"


#include <optional>
#include <string>
#include <vector>


Transition::Transition(std::string label, bool inverted, State* destination) {
  self->label = label;
  if(label.length() == 0) {
    ccd_o.reset();
  } else {
    ccd_o.emplace(label, inverted);
  }
  self->destination = destination;
}
bool Transition::isEpsilon() const {
  return !ccd_o.has_value();
}
State* Transition::getDestination() const {
  return destination;
}
bool Transition::characterMatches(std::string s, int idx) {
  if(isEpsilon()) {
    return true;
  }
  return ccd_o->characterClassContains(s[idx]);
}

void Transition::addGroupStart(int i) {
  groupStarts.push_back(i);
}
void Transition::addGroupEnd(int i) {
  groupEnds.push_back(i);
}
std::vector<int> Transition::getGroupStarts() {
  return groupStarts;
}
std::vector<int> Transition::getGroupEnds() {
  return groupEnds;
}




State::State(std::string label, bool isAccepting) {
  this->label = label;
  this->isAccepting = isAccepting;
}


bool State::getIsAccepting() const {
  return isAccpeting;
}
void State::setIsAccepting(bool b) {
  isAccepting = b;
}

std::string State::getLabel() const {
  return label;
}
void State::setLabel(std::string s) {
  label = s;
}

std::vector<Transition*> State::getTransitions() {
  return transitions;
}
void State::addTransitions(Transition* t) {
  transitions.push_back(t);
}



NFA::NFA(std::string l1, std::string l2) {
  states.clear();
  states.push_back(new State(l1, false));
  startStateIdx = 0;
  states.push_back(new State(l2, true));
  endStateIdx = 1;
}

State* NFA::getStartState() {
  return states[startStateIdx];
}
State* NFA::getEndState() {
  return states[endStateIdx];
}
void NFA::addState(State* s) {
  states.push_back(s);
}

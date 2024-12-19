#include "nfa.hpp"


#include <optional>
#include <string>
#include <vector>


class Transition;
class State;

Transition::Transition(std::string label, bool inverted, State* destination) {
  this->label = label;
  if(label.length() == 0) {
    ccd_o.reset();
  } else {
    ccd_o.emplace(label, inverted);
  }
  this->destination = destination;
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
std::string Transition::printableForm() {
  std::string printable;
  if(label.length() == 0) {
    printable.append("\u03B5");
  } else {
    printable.append(label);
  }
  printable.append(" -> ");
  printable.append(destination->getLabel());
  return printable;
}




State::State(std::string label, bool isAccepting) {
  this->label = label;
  this->isAccepting = isAccepting;
}


bool State::getIsAccepting() const {
  return isAccepting;
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
void State::addTransition(Transition* t) {
  transitions.push_back(t);
}

std::string State::printableForm() {
  std::string printable;
  printable.append(label);
  printable.append(" {\n");
  for(std::vector<Transition*>::iterator it = transitions.begin();
      it != transitions.end();
      ++it) {
    printable.append((*it)->printableForm());
    printable.append("\n");
  }
  printable.append("}");
  return printable;
}



NFA::NFA(std::string l1, std::string l2) {
  states.clear();
  states.push_back(new State(l1, false));
  startStateIdx = 0;
  states.push_back(new State(l2, true));
  endStateIdx = 1;
}

std::vector<State*> NFA::getStates() {
  return states;
}
int NFA::getStartStateIdx() const {
  return startStateIdx;
}
int NFA::getEndStateIdx() const {
  return endStateIdx;
}
void NFA::addState(State* s) {
  states.push_back(s);
}
std::string NFA::printableForm() {
  std::string printable;
  for(std::vector<State*>::iterator it = states.begin();
      it != states.end();
      ++it) {
    printable.append((*it)->printableForm());
    printable.append("\n\n");
  }
  return printable;
}

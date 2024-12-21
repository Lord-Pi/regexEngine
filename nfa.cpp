#include "nfa.hpp"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <stack>
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
bool Transition::characterMatches(std::string s, size_t idx) {
  if(isEpsilon()) {
    return true;
  }
  return ccd_o->characterClassContains(s[idx]);
}

void Transition::addGroupStart(size_t i) {
  groupStarts.push_back(i);
}
void Transition::addGroupEnd(size_t i) {
  groupEnds.push_back(i);
}
std::vector<size_t> Transition::getGroupStarts() {
  return groupStarts;
}
std::vector<size_t> Transition::getGroupEnds() {
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
  if(groupStarts.size() > 0) {
    printable.append(" START ");
    for(std::vector<size_t>::iterator it = groupStarts.begin();
	it != groupStarts.end();
	++it) {
      printable.append(std::to_string(*it));
      printable.append(", ");
    }
  }
  if(groupEnds.size() > 0) {
    printable.append(" END ");
    for(std::vector<size_t>::iterator it = groupEnds.begin();
	it != groupEnds.end();
	++it) {
      printable.append(std::to_string(*it));
      printable.append(", ");
    }
  }
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



ExecutionMemoryObject::ExecutionMemoryObject(State* startState,
					     size_t stringStartIdx,
					     size_t groupCount) {
  currentState = startState;
  stringIdx = stringStartIdx;
  groupRecording.clear();
  groupContents.clear();
  for(size_t i = 0; i < groupCount; i++) {
    groupRecording.push_back(false);
    groupContents.push_back("");
  }
  epsilonLoopTracker.clear();
}

ExecutionMemoryObject::ExecutionMemoryObject(const ExecutionMemoryObject &emo) {
  currentState = emo.currentState;
  stringIdx = emo.stringIdx;
  groupRecording = emo.groupRecording;
  groupContents = emo.groupContents;
  epsilonLoopTracker = emo.epsilonLoopTracker;
}

void ExecutionMemoryObject::applyTransition(std::string str, Transition* t) {
  currentState = t->getDestination();

  // First enable new groups, then consume input, then disable old groups.
  // The minimal possible relevant case I can think of is something like "(a)"
  // which could map to an NFA consisting only of two states and one transition.
  // In this case, that one transition needs to enable groups, consume the a,
  // then disable groups. No idea if this logic generalizes, YMMV.

  std::vector<size_t> groupStarts = t->getGroupStarts();
  for(std::vector<size_t>::iterator it = groupStarts.begin();
      it != groupStarts.end();
      ++it) {
    groupRecording[*it] = true;
  }
  
  if(t->isEpsilon()) {
    epsilonLoopTracker.push_back(currentState);
  } else {
    char consumedChar = str[stringIdx++];
    for(size_t i = 0; i < groupRecording.size(); i++) {
      if(groupRecording[i]) {
	groupContents[i].append(1, consumedChar);
      }
    }
  }

  std::vector<size_t> groupEnds = t->getGroupEnds();
  for(std::vector<size_t>::iterator it = groupEnds.begin();
      it != groupEnds.end();
      ++it) {
    groupRecording[*it] = false;
  }

}

bool ExecutionMemoryObject::isStateInEpsilonLoop(State* s) {
  return epsilonLoopTracker.end() != std::find(epsilonLoopTracker.begin(),
					       epsilonLoopTracker.end(),
					       s);
}

State* ExecutionMemoryObject::getCurrentState() const {
  return currentState;
}
size_t ExecutionMemoryObject::getStringIdx() const {
  return stringIdx;
}
std::vector<std::string> ExecutionMemoryObject::getGroupContents() const {
  return groupContents;
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
size_t NFA::getStartStateIdx() const {
  return startStateIdx;
}
size_t NFA::getEndStateIdx() const {
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
size_t NFA::getGroupCount() {
  std::vector<size_t> groupBegins;
  std::vector<size_t> groupEnds;
  for(std::vector<State*>::iterator it = states.begin();
      it != states.end();
      ++it) {
    std::vector<Transition*> ts = (*it)->getTransitions();
    for(std::vector<Transition*>::iterator jt = ts.begin();
	jt != ts.end();
	++jt) {
      std::vector<size_t> tgStarts = (*jt)->getGroupStarts();
      for(std::vector<size_t>::iterator kt = tgStarts.begin();
	  kt != tgStarts.end();
	  ++kt) {
	std::vector<size_t>::iterator ft = std::find(groupBegins.begin(),
						     groupBegins.end(),
						     *kt);
	if(ft == groupBegins.end()) {
	  groupBegins.push_back(*kt);
	}
      }
      std::vector<size_t> tgEnds = (*jt)->getGroupEnds();
      for(std::vector<size_t>::iterator kt = tgEnds.begin();
	  kt != tgEnds.end();
	  ++kt) {
	std::vector<size_t>::iterator ft = std::find(groupEnds.begin(),
						     groupEnds.end(),
						     *kt);
	if(ft == groupEnds.end()) {
	  groupEnds.push_back(*kt);
	}
      }      
    }
  }

  if(groupBegins.size() != groupEnds.size()) {
    throw std::length_error("Mismatch between group begin and group end transitions");
  }

  std::sort(groupBegins.begin(), groupBegins.end());
  std::sort(groupEnds.begin(), groupEnds.end());

  for(size_t i = 0; i < groupBegins.size(); i++) {
    if(i != groupBegins[i] || i != groupEnds[i]) {
      throw std::range_error("Non-consecutive groups? I guess?");
    }
  }

  return groupEnds.size();
}

std::vector<std::string> NFA::engineMatch(std::string input, size_t stringStartIdx) {
  std::vector<std::string> output;
  
  std::stack<ExecutionMemoryObject> backtrackStack;
  backtrackStack.emplace(states[startStateIdx], stringStartIdx, getGroupCount());

  while(!backtrackStack.empty()) {
    ExecutionMemoryObject snapshot = backtrackStack.top();
    backtrackStack.pop();

    State* s = snapshot.getCurrentState();
    if(s == states[endStateIdx]) {
      return snapshot.getGroupContents();
    }

    size_t stringIdx = snapshot.getStringIdx();
    std::vector<Transition*> stateTransitions = s->getTransitions();
    for(std::vector<Transition*>::iterator it = stateTransitions.begin();
	it != stateTransitions.end();
	++it) {
      if((*it)->characterMatches(input, stringIdx)) {
	backtrackStack.emplace(snapshot);
	backtrackStack.top().applyTransition(input, *it);
      }
    }
    
  }

  

  return output;
}

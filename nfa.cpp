#include "nfa.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <optional>
#include <stack>
#include <string>
#include <utility>
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
    printable.append(" (");
    for(std::string::iterator it = label.begin();
	it != label.end();
	++it) {
      printable.append(std::to_string((unsigned int)(*it)));
      printable.append(" ");
    }
    printable.append(")");
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
State::~State() {
  for(std::vector<Transition*>::iterator it = transitions.begin();
      it != transitions.end();
      ++it) {
    delete (*it);
  }
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
  groupStarts.clear();
  groupEnds.clear();
  for(size_t i = 0; i < groupCount; i++) {
    groupStarts.push_back(0);
    groupEnds.push_back(0);
    groupRecording.push_back(false);
    std::vector<size_t> recordingRow;
    recordingRow.clear();
    groupContents.push_back(recordingRow);
  }
  epsilonLoopTracker.clear();
}

ExecutionMemoryObject::ExecutionMemoryObject(const ExecutionMemoryObject &emo) {
  currentState = emo.currentState;
  stringIdx = emo.stringIdx;
  groupRecording = emo.groupRecording;
  groupContents = emo.groupContents;
  groupStarts = emo.groupStarts;
  groupEnds = emo.groupEnds;
  epsilonLoopTracker = emo.epsilonLoopTracker;
}

void ExecutionMemoryObject::applyTransition(Transition* t) {
  currentState = t->getDestination();

  // First enable new groups, then consume input, then disable old groups.
  // The minimal possible relevant case I can think of is something like "(a)"
  // which could map to an NFA consisting only of two states and one transition.
  // In this case, that one transition needs to enable groups, consume the a,
  // then disable groups. No idea if this logic generalizes, YMMV.

  std::vector<size_t> tGroupStarts = t->getGroupStarts();
  for(std::vector<size_t>::iterator it = tGroupStarts.begin();
      it != tGroupStarts.end();
      ++it) {
    groupRecording[*it] = true;
    groupStarts[*it] = stringIdx;
  }
  
  if(t->isEpsilon()) {
    epsilonLoopTracker.push_back(currentState);
  } else {
    for(size_t i = 0; i < groupRecording.size(); i++) {
      if(groupRecording[i]) {
	groupContents[i].push_back(stringIdx);
      }
    }
    stringIdx++;
  }

  std::vector<size_t> tGroupEnds = t->getGroupEnds();
  for(std::vector<size_t>::iterator it = tGroupEnds.begin();
      it != tGroupEnds.end();
      ++it) {
    groupRecording[*it] = false;
    groupEnds[*it] = stringIdx;
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
std::vector<std::vector<size_t>> ExecutionMemoryObject::getGroupContents() const {
  return groupContents;
}
std::vector<size_t> ExecutionMemoryObject::getGroupStarts() const {
  return groupStarts;
}
std::vector<size_t> ExecutionMemoryObject::getGroupEnds() const {
  return groupEnds;
}

std::string ExecutionMemoryObject::getPrintableForm() {
  std::string printableForm = "";
  
  printableForm.append("State: ");
  printableForm.append(currentState->getLabel());
  printableForm.append("\n");

  printableForm.append("stringIdx: ");
  printableForm.append(std::to_string(stringIdx));
  printableForm.append("\n");

  printableForm.append("groupData: ");
  printableForm.append("\n");
  for(int i = 0; i < groupRecording.size(); i++) {
    printableForm.append(std::to_string(groupRecording[i]));
    printableForm.append(" ");
    printableForm.append(std::to_string(groupStarts[i]));
    printableForm.append(" ");
    printableForm.append(std::to_string(groupEnds[i]));
    printableForm.append("\n");
  }

  return printableForm;
}





NFA::NFA(std::string l1, std::string l2) {
  states.clear();
  states.push_back(new State(l1, false));
  startStateIdx = 0;
  states.push_back(new State(l2, true));
  endStateIdx = 1;
}
NFA::~NFA() {
  for(std::vector<State*>::iterator it = states.begin();
      it != states.end();
      ++it) {
    delete (*it);
  }
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
void NFA::clearStates() {
  states.clear();
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

/*
std::vector<std::vector<size_t>> NFA::engineMatch(std::string input, size_t stringStartIdx) {
  std::vector<std::vector<size_t>> output;
  
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
	backtrackStack.top().applyTransition(*it);
      }
    }
    
  }

  return output;
}
*/

std::vector<std::pair<size_t, size_t>> NFA::zipGroupBounds(std::pair<
							   std::vector<size_t>,
							   std::vector<size_t>> groupBounds) {
  assert(groupBounds.first.size() == groupBounds.second.size());
  std::vector<std::pair<size_t, size_t>> zippedBounds;
  for(int i = 0; i < groupBounds.first.size(); i++) {
    std::pair<size_t, size_t> zippedPair(groupBounds.first[i], groupBounds.second[i]);
    zippedBounds.push_back(zippedPair);
  }
  return zippedBounds;
}

std::vector<std::pair<size_t, size_t>> NFA::engineMatch(std::string input,
							size_t stringStartIdx) {
  std::vector<std::pair<size_t, size_t>> output;
  
  std::stack<ExecutionMemoryObject> backtrackStack;
  backtrackStack.emplace(states[startStateIdx], stringStartIdx, getGroupCount());

  while(!backtrackStack.empty()) {
    ExecutionMemoryObject snapshot = backtrackStack.top();
    backtrackStack.pop();

    std::cout << snapshot.getPrintableForm() << std::endl;

    State* s = snapshot.getCurrentState();
    if(s == states[endStateIdx]) {
      std::pair<
	std::vector<size_t>,
	std::vector<size_t>
	> groupBounds(snapshot.getGroupStarts(), snapshot.getGroupEnds());
      return zipGroupBounds(groupBounds);
    }

    size_t stringIdx = snapshot.getStringIdx();
    std::vector<Transition*> stateTransitions = s->getTransitions();
    for(std::vector<Transition*>::iterator it = stateTransitions.begin();
	it != stateTransitions.end();
	++it) {
      if((*it)->characterMatches(input, stringIdx)) {
	backtrackStack.emplace(snapshot);
	backtrackStack.top().applyTransition(*it);
      }
    }
    
  }

  return output;
}

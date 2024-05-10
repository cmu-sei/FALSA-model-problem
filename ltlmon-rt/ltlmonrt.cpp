/*
 * FALSA Model Problem
 * 
 * Copyright 2024 Carnegie Mellon University.
 * 
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED, AS
 * TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR PURPOSE
 * OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE
 * MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND
 * WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * 
 * Licensed under a MIT (SEI)-style license, please see license.txt or contact
 * permission@sei.cmu.edu for full terms.
 * 
 * [DISTRIBUTION STATEMENT A] This material has been approved for public
 * release and unlimited distribution.  Please see Copyright notice for non-US
 * Government use and distribution.
 * 
 * This Software includes and/or makes use of Third-Party Software each subject
 * to its own license.
 * 
 * DM24-0251
 */

#include "ltlmonrt.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

const LTLMonitor::IndexT LTLMonitor::INVALID = -1;

LTLMonitor::LTLMonitor() {}

bool LTLMonitor::initialize(std::string path) { return readFile(path); }

bool LTLMonitor::readFile(std::string path) {
  const string HEADER_TAG("TLTMON:");
  const string INITIAL_STATE("(0, 0)");
  enum ReadPhase {
    HEADER,
    PROPERTY,
    STATE_COUNT,
    STATES,
    ACTION_COUNT,
    ACTIONS,
    TRANSITIONS
  };
  ReadPhase phase = HEADER;

  ifstream inputFile(path);
  if (!inputFile.is_open()) {
    cerr << "LTLMonitor: couldn't open " << path << endl;
    return false;
  }

  int stateCount = 0;
  int actionCount = 0;
  bool initialStateFound = false;

  string line;
  while (inputFile) {
    getline(inputFile, line);
    if (line.empty()) {
      continue;
    }

    switch (phase) {
    case HEADER:
      if (line.compare(0, HEADER_TAG.length(), HEADER_TAG) != 0) {
        cerr << "LTLMonitor: missing/wrong header in " << path << endl;
        return false;
      }
      phase = PROPERTY;
      break;
    case PROPERTY:
      ltlproperty = line;
      phase = STATE_COUNT;
      break;
    case STATE_COUNT:
      stateCount = atoi(line.c_str());
      if (stateCount <= 0) {
        cerr << "LTLMonitor: invalid state count in " << path << endl;
        return false;
      }
      phase = STATES;
      break;
    case STATES:
      State::StateType type;
      switch (line[0]) {
      case '+':
        type = State::ACCEPT;
        break;
      case '-':
        type = State::VIOLATION;
        break;
      case '?':
        type = State::INCONCLUSIVE;
        break;
      default:
        cerr << "LTLMonitor: invalid state type in " << path << endl;
        return false;
      }
      stateMap[line] = states.size();
      states.push_back(State{line, type});
      if (line.substr(1) == INITIAL_STATE) {
        state = states.size() - 1;
        initialStateFound = true;
      }
      if (--stateCount == 0) {
        phase = ACTION_COUNT;
      }
      break;
    case ACTION_COUNT:
      actionCount = atoi(line.c_str());
      if (actionCount <= 0) {
        cerr << "LTLMonitor: invalid action count in " << path << endl;
        return false;
      }
      phase = ACTIONS;
      break;
    case ACTIONS: {
      IndexT actionId = actionMap.size();
      actionMap[line] = actionId;
    }
      if (--actionCount == 0) {
        transitions.resize(states.size(),
                           vector<IndexT>(actionMap.size(), INVALID));
        phase = TRANSITIONS;
      }
      break;
    case TRANSITIONS: {
      vector<IndexT> components;
      // cout << "transition line: " << line << endl;
      stringstream transition(line);
      while (!transition.eof()) {
        string component;
        getline(transition, component, ',');
        // cout << "@size=" << components.size() << "component=[" << component
        // << "]" << endl;
        if (component.empty() || components.size() >= 3) {
          cerr << "LTLMonitor: invalid transition in " << path << endl;
          return false;
        }
        components.push_back(atoi(component.c_str()));
      }
      transitions[components[0]][components[1]] = components[2];
    } break;
    }
  }

  if (!initialStateFound) {
    cerr << "LTLMonitor: Initial state " << INITIAL_STATE << " not found"
         << endl;
    return false;
  }
  return true;
}

bool LTLMonitor::step(std::string action) {

  /*
   * If we already reached an accepting or violating state, stay there
   */
  if (states[state].type != State::INCONCLUSIVE) {
    return states[state].type == State::ACCEPT;
  }

  auto action_it = actionMap.find(action);
  if (action_it == actionMap.end()) {
    // cout << "Action " << action << " is unknown. Ignoring" << endl;
    return true;
  }

  auto action_idx = action_it->second;
  IndexT newState = transitions[state][action_idx];

  if (newState == INVALID) {
    cout << "Action " << action << " at state " << states[state].name
         << " is not valid" << endl;
    return false;
  }

  if (states[newState].type == State::VIOLATION) {
    cout << "Action " << action << " at state " << states[state].name
         << " violates property " << ltlproperty << endl;
    return false;
  }

  state = newState;

  return true;
}

std::string LTLMonitor::getProperty() const { return ltlproperty; }

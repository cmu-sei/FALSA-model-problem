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
#include <iostream>
#include <vector>

using namespace std;

struct Test {
  string name;
  string monFile;
  std::vector<string> events;
  bool expected;
};

std::vector<Test>
    tests({{"test1",
            "tests/prop1.mon",
            {"nop", "nop", "drop_supplies", "nop"},
            false},
           {"test2",
            "tests/prop1.mon",
            {"nop", "nop", "at_destination", "nop", "drop_supplies", "nop"},
            true}});

bool runTest(const Test &test) {
  cout << "Starting test " << test.name << endl;
  LTLMonitor monitor;
  if (!monitor.initialize(test.monFile)) {
    cout << "Could not load monitor from " << test.monFile << endl;
    return false;
  };

  cout << "Property: " << monitor.getProperty() << endl;
  bool result = true;
  for (const auto &event : test.events) {
    result = monitor.step(event);
    cout << "step[" << event << "] -> " << result << endl;
    if (!result) {
      break;
    }
  }

  bool success = (result == test.expected);
  cout << "Test " << test.name << ": " << ((success) ? "SUCCESS" : "FAILED")
       << endl;
  return success;
}

int main(int, char **) {

  for (const auto &test : tests) {
    runTest(test);
    cout << endl;
  }

  return 0;
}

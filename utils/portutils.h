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

#ifndef PORTUTILS_H
#define PORTUTILS_H

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

inline std::string &ltrim(std::string &s, const char *t = " \t\n\r\f\v") {
  s.erase(0, s.find_first_not_of(t));
  return s;
}

// trim from right
inline std::string &rtrim(std::string &s, const char *t = " \t\n\r\f\v") {
  s.erase(s.find_last_not_of(t) + 1);
  return s;
}

// trim from left & right
inline std::string &trim(std::string &s, const char *t = " \t\n\r\f\v") {
  return ltrim(rtrim(s, t), t);
}

class Ports {
private:
  std::string configFilename;
  std::map<std::string, std::string> portData;

public:
  Ports(std::string configFilenameInput) : configFilename(configFilenameInput) {
    try {
      std::ifstream infile(configFilename);
      std::string line;

      while (std::getline(infile, line)) {
        std::stringstream ss(line);
        std::string portName;
        std::string portNumberStr;
        if (!std::getline(ss, portName, ',')) {
          continue;
        }
        if (!std::getline(ss, portNumberStr, ',')) {
          continue;
        }

        trim(portName);
        trim(portNumberStr);

        if (portName.empty()) {
          continue;
        }

        if (portNumberStr.empty()) {
          continue;
        }

        std::string portAddress = "0.0.0.0:" + portNumberStr;

        portData[portName] = portAddress;
      }
    } catch (...) {
      std::cout << "ERROR Reading ports.config\n";
    }
  }

  void displayAllPorts() {
    for (const auto &port : portData) {
      std::cout << port.first << ": " << port.second << "\n";
    }
  }

  std::string getAddress(const std::string &portName,
                         const std::string &defaultAddress = "0.0.0.0:0") {
    if (portData.find(portName) != portData.end()) {
      return portData[portName];
    }

    else {
      return defaultAddress;
    }
  }
};

#endif // PORTUTILS_H

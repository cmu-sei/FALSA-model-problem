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

#include <state_control.h>

StateControl *StateControl ::instancePtr = nullptr;

StateControl::StateControl(void) {}

StateControl::~StateControl() {}

StateControl *StateControl::getInstance(void) {

  // If there is no instance of class
  // then we can create an instance.
  if (instancePtr == nullptr) {
    // We can access private members
    // within the class.
    instancePtr = new StateControl();
    // returning the instance pointer
    return instancePtr;
  } else {
    // if instancePtr != NULL that means
    // the class already has an instance.
    // So, we are returning that instance
    // and not creating a new one.
    return instancePtr;
  }
}

void StateControl::SetMissionState(const MissionState ms) {
  LockMissionState();
  mission_state = ms;
  UnlockMissionState();
}

MissionState StateControl::GetMissionState(void) {
  MissionState ret_ms;
  LockMissionState();
  ret_ms = mission_state;
  UnlockMissionState();
  return ret_ms;
}

void StateControl::SetLockedState(const LockedState ls) {
  LockLockedState();
  locked_state = ls;
  UnlockLockedState();
}

LockedState StateControl::GetLockedState(void) {
  LockedState ret_ls;
  LockLockedState();
  ret_ls = locked_state;
  UnlockLockedState();
  return ret_ls;
}

void StateControl::SetLatLonCoord(LatLonCoord llc) { lat_lon = llc; }

LatLonCoord StateControl::GetLatLonCoord() { return lat_lon; }

void StateControl::SetAltitude(double alt) { altitude = alt; }

double StateControl::GetAltitude(void) { return altitude; }

void StateControl::SetLatLonCoordDest(LatLonCoord llc) { lat_lon_dest = llc; }

LatLonCoord StateControl::GetLatLonCoordDest() { return lat_lon_dest; }

void StateControl::SetTimeDest(double t) { time_dest = t; }

double StateControl::GetTimeDest(void) { return time_dest; }

void StateControl::LockMissionState(void) { mission_state_mtx.lock(); }

void StateControl::UnlockMissionState(void) { mission_state_mtx.unlock(); }

void StateControl::LockLockedState(void) { locked_state_mtx.lock(); }

void StateControl::UnlockLockedState(void) { locked_state_mtx.unlock(); }

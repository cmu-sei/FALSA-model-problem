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

#ifndef STATE_CONTROL_H_H
#define STATE_CONTROL_H_H

#include "LatLonCoord.pb.h"
#include <mutex>

using namespace uav;

enum class MissionState {
  INITIALIZED = 0,
  PARAMETERS_SET = 1,
  TAKEOFF_STARTED = 2,
  FLYING_TO_DESTINATION = 3,
  AT_DESTINATION = 4,
  DROP_SUPPLIES = 5,
  RETURNING_TO_BASE = 6,
  LANDING_AT_BASE = 7,
  LANDED = 8
};

enum class LockedState { LOCKED = 0, UNLOCKED = 1 };

// Class that allows management of different state vatiables
class StateControl {
public:
  static StateControl *getInstance(void);
  StateControl(const StateControl &obj) = delete;
  void SetMissionState(const MissionState ms);
  MissionState GetMissionState(void);
  void SetLockedState(const LockedState ls);
  LockedState GetLockedState(void);
  void SetLatLonCoord(LatLonCoord llc);
  LatLonCoord GetLatLonCoord(void);
  void SetAltitude(double alt);
  double GetAltitude(void);
  void SetLatLonCoordDest(LatLonCoord llc);
  LatLonCoord GetLatLonCoordDest(void);
  void SetTimeDest(double t);
  double GetTimeDest(void);

private:
  MissionState mission_state;
  LockedState locked_state;
  LatLonCoord lat_lon;
  double altitude;
  LatLonCoord lat_lon_dest;
  double time_dest;
  std::mutex mission_state_mtx;
  std::mutex locked_state_mtx;
  static StateControl *instancePtr;
  StateControl(void);
  ~StateControl();
  void LockMissionState(void);
  void UnlockMissionState(void);
  void LockLockedState(void);
  void UnlockLockedState(void);
};

#endif

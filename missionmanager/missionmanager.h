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

#ifndef MISSIONMANAGER_H_H
#define MISSIONMANAGER_H_H

#include <queue>

#include "IMissionManager.h"
#include "missionmanager.h"

#include <unistd.h>

#include "LatLonCoord.pb.h"
#include "Status.pb.h"
#include "Time.pb.h"
#include "Waypoint.pb.h"
#include "client_guidance.h"
#include "state_control.h"

using namespace uav;

// Utility class, iplemented as a singleton
// Takes care of the processing needed by the IMissionManager interface messages
class ImplMissionManager : public IMissionManager {

private:
  ImplMissionManager(void);
  ~ImplMissionManager();
  static ImplMissionManager *instancePtr;
  LatLonCoord destination;
  Time startTime;
  Time endTime;
  double takeoffAltitude;
  static ClientGuidance *client1;
  static Logger *log_ptr;
  static StateControl *state_control;

public:
  static ImplMissionManager *getInstance(Logger *log);
  ImplMissionManager(const ImplMissionManager &obj) = delete;

  void setMissionParams(LatLonCoord destinationInput, Time startTimeInput,
                        Time endTimeInput, double takeoffAltitudeInput);
  void clearMissionParams(void);
  void takeOff(void);
  void abort(void);

  void saveStatus(StatusMessage statusMessage);

protected:
};

#endif

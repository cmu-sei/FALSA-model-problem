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

#ifndef IMISSIONMANAGER_H_H
#define IMISSIONMANAGER_H_H

#include "LatLonCoord.pb.h"
#include "Time.pb.h"
#include "Waypoint.pb.h"

#include "google/protobuf/empty.pb.h"
#include "google/protobuf/wrappers.pb.h"

using namespace uav;

class IMissionManager {

public:
  /**
   * Aborts the mission, commanding the UAV to return to base.
   */
  virtual void abort(void) = 0;
  /**
   * Sets the destination parameters for the mission, which include:
   * -lat/lon coordinates of the destination.
   * -time window to drop supplies at the destination, specified with two times
   * indicating the inclusive start and end of the time window.
   */
  virtual void setMissionParams(LatLonCoord destination, Time startTime,
                                Time endTime,
                                /**
                                 * Takeoff altitude (AGL)
                                 */
                                double takeoffAltitude) = 0;

  virtual void clearMissionParams(void) = 0;

  /**
   * Commands the UAV to take off and execute the mission.
   */
  virtual void takeOff(void) = 0;
};

#endif

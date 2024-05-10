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

#ifndef IGUIDANCE_H_H
#define IGUIDANCE_H_H

#include "LatLonCoord.pb.h"
#include "Waypoint.pb.h"
#include "google/protobuf/empty.pb.h"
#include "google/protobuf/wrappers.pb.h"

using namespace uav;

class IGuidance {

public:
  /**
   * Appends a waypoint to the list of waypoints to by covered by the route. A
   * waypoint is a pair of lat/lon coordinates and aIsInAirn altitude.
   */
  virtual void addWaypoint(const Waypoint *waypoint) = 0;
  /**
   * Clears the route set by previous calls to addWaypoint()
   */
  virtual void clearRoute() = 0;
  /**
   * Returns the number of waypoints in the route. This should be equivalent to
   * the number of successful calls to addWaypoint() after the later of
   * initialization or call to clearRoute().
   */
  virtual int getWaypointCount() = 0;
  /**
   * Arm
   */
  virtual void arm() = 0;
  /**
   * Disarm
   */
  virtual void disarm() = 0;
  /**
   * Land immediately.
   */
  virtual void land() = 0;
  /**
   * Returns to take off location and land, ignoring remaining waypoints in the
   * route.
   */
  virtual void returnToBase() = 0;
  /**
   * Starts flying the sequence of waypoints. When it reaches the last waypoint,
   * the UAV hovers in place.
   *
   */
  virtual void start() = 0;
  /**
   * Subscribe to receive periodic status updates.
   */
  virtual void subscribeStatus(
      /**
       * Period in milliseconds between status updates.
       */
      const unsigned int periodMsec) = 0;

  virtual void takeOff(
      /**
       * This operation consists of
       * -arming the motors
       * -taking off vertically to the altitude (AGL) specified in the argument
       * -hover in place when it reaches the altitude
       */
      const double takeoffAltitude) = 0;
};

#endif

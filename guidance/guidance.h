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

#ifndef GUIDANCE_H_H
#define GUIDANCE_H_H

#include <queue>

#include "IGuidance.h"
#include "mavsdkutils.h"

/*
   The Iguidance interface is implemented by a concrete class
   All pure virtual functions are overridden with concrete implementations
*/

class ImplGuidance : public IGuidance {

public:
  virtual ~ImplGuidance();

  static ImplGuidance &getInstance();

  /**
   * Appends a waypoint to the list of waypoints to by covered by the route. A
   * waypoint is a pair of lat/lon coordinates and an altitude.
   */
  void addWaypoint(const Waypoint *waypoint);
  /**
   * Clears the route set by previous calls to addWaypoint()
   */
  void clearRoute();
  /**
   * Returns the number of waypoints in the route. This should be equivalent to
   * the number of successful calls to addWaypoint() after the later of
   * initialization or call to clearRoute().
   */
  int getWaypointCount();
  /**
   * Land immediately.
   */
  void land();
  /**
   * Returns to take off location and land, ignoring remaining waypoints in the
   * route.
   */
  void returnToBase();
  /**
   * Starts flying the sequence of waypoints. When it reaches the last waypoint,
   * the UAV hovers in place. MAVSDKUtils
   */
  void start();
  /**
   * Subscribe to receive periodic status updates.
   */
  void subscribeStatus(
      /**
       * Period in milliseconds between status updates.
       */
      const unsigned int periodMsec);
  void takeOff(
      /**
       * This operation consists of
       * -arming the motors
       * -taking off vertically to the altitude (AGL) specified in the argument
       * -hover in place when it reaches the altitude
       */
      const double takeoffAltitude);

  /*
   *  Arm the drone
   */
  void arm(void);

  /*
   *  Disarm the drone
   */
  void disarm(void);

  unsigned int GetStatusPeriod(void);

protected:
  ImplGuidance(void); // protected for singleton

  /* Utility functions that are not part of the IGuidance interface */
  void WaitToLand(void);

private:
  MAVSDKUtils *mavsdkUtils;
  std::deque<Waypoint> wp_queue;
  unsigned int statusMsec;
};

#endif

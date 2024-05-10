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

#include "guidance.h"
#include "Waypoint.pb.h"
#include <unistd.h>

using namespace uav;

ImplGuidance::ImplGuidance(void) {
  mavsdkUtils = MAVSDKUtils::getInstance(nullptr);
  mavsdkUtils->Init();
  statusMsec = 0;
}

ImplGuidance::~ImplGuidance() {}

/**
 * Appends a waypoint to the list of waypoints to by covered by the route. A
 * waypoint is a pair of lat/lon coordinates and an altitude.
 */
void ImplGuidance::addWaypoint(const Waypoint *waypoint) {
  /* Add the waypoint to the list */
  Waypoint wp;
  wp.mutable_latlon()->set_latitude(waypoint->latlon().latitude());
  wp.mutable_latlon()->set_longitude(waypoint->latlon().longitude());
  wp.set_altitude(waypoint->altitude());
  wp_queue.push_back(wp);
  std::cout << "Waypoint received and stored." << std::endl;
}

ImplGuidance &ImplGuidance::getInstance() {
  static ImplGuidance instance;
  return instance;
}

/**
 * Clears the route set by previous calls to addWaypoint()
 */
void ImplGuidance::clearRoute() { wp_queue.clear(); }

/**
 * Returns the number of waypoints in the route. This should be equivalent to
 * the number of successful calls to addWaypoint() after the later of
 * initialization or call to clearRoute().
 */
int ImplGuidance::getWaypointCount() { return wp_queue.size(); }

/**
 * Land immediately.
 */
void ImplGuidance::land() {
  /* Send a MAVSDK command */
  mavsdkUtils->Land();
}

/**
 * Returns to take off location and land, ignoring remaining waypoints in the
 * route.
 */
void ImplGuidance::returnToBase() {
  /* Send a MAVSDK command */
  /* Use baseWP variable where we stored the waypoint for the base */
  mavsdkUtils->ReturnToBase();
}

/**
 * Starts flying the sequence of waypoints. When it reaches the last waypoint,
 * the UAV hovers in place.
 *
 */
void ImplGuidance::start() {
  /* Send a MAVSDK command */
  Waypoint wp = wp_queue.back();
  std::cout << "lat: " << wp.latlon().latitude() << std::endl;
  std::cout << "lon: " << wp.latlon().longitude() << std::endl;
  std::cout << "alt: " << wp.altitude() << std::endl;
  mavsdkUtils->Start(&wp);
  wp_queue.pop_back();
}

/**
 * Subscribe to receive periodic status updates.
 */
void ImplGuidance::subscribeStatus(
    /* Period in milliseconds between status updates.*/ const unsigned int
        periodMsec) {

  /* Start a timer in a thread and send back status mesages every periodMsec */
  statusMsec = periodMsec;
}

void ImplGuidance::arm(void) { mavsdkUtils->Arm(); }

void ImplGuidance::disarm(void) { mavsdkUtils->Disarm(); }

/**
 * This operation consists of
 * -arming the motors
 * -taking off vertically to the altitude (AGL) specified in the argument
 * -hover in place when it reaches the altitude
 */
void ImplGuidance::takeOff(const double takeoffAltitude) {
  /* Send a MAVSDK command */
  mavsdkUtils->TakeOff(takeoffAltitude);
}

unsigned int ImplGuidance::GetStatusPeriod(void) { return statusMsec; }

/* Utility Methods */

void ImplGuidance::WaitToLand(void) {
  while (mavsdkUtils->IsInAir() == true) {
    sleep(1);
  }
}

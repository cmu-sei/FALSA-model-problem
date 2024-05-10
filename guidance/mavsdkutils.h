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


#ifndef MAVSDKUTILS_H_H
#define MAVSDKUTILS_H_H

#include "Poco/Logger.h"
#include <chrono>
#include <cmath>
#include <future>
#include <iostream>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/offboard/offboard.h>
#include <mavsdk/plugins/param/param.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mutex>
#include <thread>

#include "LatLonCoord.pb.h"
#include "Status.pb.h"
#include "Waypoint.pb.h"
#include "vehicleState.h"

using namespace uav;
using namespace mavsdk;
using Poco::Logger;

/*
   A singleton pattern is applied for thi class
   This assures one instance of the class in the project.
   The class encapsulates all Mavlink communications with PX4 or
   another autopilot that supports PX4.
*/

class MAVSDKUtils {

public:
  static MAVSDKUtils *getInstance(Logger *logger);
  MAVSDKUtils(const MAVSDKUtils &obj) = delete;

  bool Init(void);
  void GotoWayPoint(const Waypoint *waypoint);
  bool Land(void);
  bool Start(const Waypoint *waypoint);
  bool TakeOff(double takeoffAlt);
  bool IsInAir(void);
  bool Arm(void);
  bool Disarm(void);
  bool ReturnToBase(void);
  void SubscribePosition(void);
  void SubscribePositionVelocityNED(void);
  void SubscribeAttitudeQuaternion(void);
  void SubscribeAttitudeEuler(void);
  void SubscribeAngularVelocityBody(void);
  void SubscribeOdometry(void);
  static StatusMessage
      statusMessage; // The message we send back to the mission manager
  static VehicleState
      vehicleState; // The vehicle state we use to store all info regarding the
                    // vehicle, coming from PX4
  static Logger *mavsdk_logger;
  static void LockStatus(void);
  static void UnlockStatus(void);

private:
  MAVSDKUtils();
  ~MAVSDKUtils();
  // The following callback functions are used to update variables in
  // vehicleState automatically and periodically
  static void PositionCallback(Telemetry::Position position);
  static void
  PositionVelocityNEDCallback(Telemetry::PositionVelocityNed velocity);
  static void AttitudeQuaternionCallback(Telemetry::Quaternion attitude);
  static void AttitudeEulerCallback(Telemetry::EulerAngle attitude);
  static void
  AngularVelocityBodyCallback(Telemetry::AngularVelocityBody angularVelBody);
  static void OdometryCallback(Telemetry::Odometry odometry);
  static MAVSDKUtils *instancePtr;
  static std::mutex status_mtx;

  mavsdk::Mavsdk *mavsdk;
  std::optional<std::shared_ptr<mavsdk::System>> system;
  mavsdk::Action *action;
  mavsdk::Offboard *offboard;
  mavsdk::Telemetry *telemetry;
  static Waypoint dest_waypoint;
  static Waypoint base_waypoint;
  static double takeoffAltitude;
};

#endif

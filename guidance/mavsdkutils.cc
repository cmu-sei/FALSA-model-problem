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


#include "mavsdkutils.h"
#include <string.h>
#include <time.h>

using namespace mavsdk;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::this_thread::sleep_for;

// initializing static variables needs to happen in the .cc file
MAVSDKUtils *MAVSDKUtils ::instancePtr = nullptr;
StatusMessage MAVSDKUtils ::statusMessage;
VehicleState MAVSDKUtils ::vehicleState;
std::mutex MAVSDKUtils ::status_mtx;
Logger *MAVSDKUtils ::mavsdk_logger = nullptr;
Waypoint MAVSDKUtils ::dest_waypoint;
Waypoint MAVSDKUtils ::base_waypoint;
double MAVSDKUtils ::takeoffAltitude;

// Return one instance of the class to the caller
MAVSDKUtils *MAVSDKUtils::getInstance(Logger *logger) {

  if (logger != nullptr) {
    instancePtr->mavsdk_logger = logger;
  }
  // If there is no instance of class
  // then we can create an instance.
  if (instancePtr == nullptr) {
    // We can access private members
    // within the class.
    instancePtr = new MAVSDKUtils();
    // returning the instance pointer
    return instancePtr;
  } else {
    // if instancePtr != NULL that means
    // the class already have an instance.
    // So, we are returning that instance
    // and not creating new one.
    return instancePtr;
  }
}

MAVSDKUtils::MAVSDKUtils() {
  mavsdk = nullptr;
  action = nullptr;
  offboard = nullptr;
  telemetry = nullptr;
}

MAVSDKUtils::~MAVSDKUtils() {
  if (mavsdk != nullptr) {
    delete mavsdk;
    if (mavsdk_logger != nullptr) {
      // mavsdk_logger->information(total_string);
    }
  }
  if (system != nullptr) {
    system->reset();
  }
  if (action != nullptr) {
    delete action;
  }

  if (offboard != nullptr) {
    delete offboard;
  }
  if (telemetry != nullptr) {
    delete telemetry;
  }
}

bool MAVSDKUtils::Init(void) {
  bool status = true;
  mavsdk =
      new Mavsdk(Mavsdk::Configuration(Mavsdk::ComponentType::GroundStation));
  ConnectionResult connection_result =
      mavsdk->add_any_connection("udp://:14540");
  if (connection_result != ConnectionResult::Success) {
    std::cerr << "Connection failed: " << connection_result << '\n';
    status = false;
    exit(-1);
  }

  do {
    std::cout << "Waiting for an autopilot system...\n";
    system = mavsdk->first_autopilot(3.0);
  } while (!system);
  std::cout << "Connected to autopilot.\n";

  // Set this parameter to avoid failure mode
  auto param = Param(std::shared_ptr<System>(system.value()));
  param.set_param_int("COM_RCL_EXCEPT", 4);

  action = new Action{system.value()};
  offboard = new Offboard{system.value()};
  telemetry = new Telemetry{system.value()};

  return status;
}

void MAVSDKUtils::GotoWayPoint(const Waypoint *waypoint) {
  std::cout << "GotoWayPoint() called\n";
  char total_string[256];
  dest_waypoint = *waypoint;
  sprintf(total_string,
          "Waypoint coordinates set:: Latitude: %lf Longitude: %lf Altitude "
          "%lf .\n",
          waypoint->latlon().latitude(), waypoint->latlon().longitude(),
          waypoint->altitude());
  if (mavsdk_logger != nullptr) {
    mavsdk_logger->information(total_string);
  }
  Offboard::PositionGlobalYaw wp{
      waypoint->latlon().latitude(), waypoint->latlon().longitude(),
      (float)waypoint->altitude(), 100.0f,
      Offboard::PositionGlobalYaw::AltitudeType::RelHome};
  offboard->set_position_global(wp);
}

bool MAVSDKUtils::ReturnToBase(void) {
  bool status;
  std::cout << "ReturnToBase() called\n";
  GotoWayPoint(&base_waypoint);
  statusMessage.set_state(FLYINGTOBASE);
  Offboard::Result offboard_result = offboard->start();
  return status;
}

bool MAVSDKUtils::Arm(void) {
  std::cout << "Arm() called\n";
  bool status = true;
  // Check until vehicle is ready to arm"Rel. Altitude: " <<
  // position.relative_altitude_m << " Latitude: " << position.latitude_deg << "
  // Longitude: " << position.longitude_deg
  while (telemetry->health_all_ok() != true) {
    std::cout << "Vehicle is getting ready to arm\n";
    sleep_for(seconds(1));
  }

  Action::Result arm_result = action->arm();
  if (arm_result != Action::Result::Success) {
    std::cout << "Vehicle failed to arm\n";
    status = false;
  } else {
    std::cout << "Vehicle armed\n";
  }
  return status;
}

bool MAVSDKUtils::Disarm(void) {
  std::cout << "Disarm() called\n";
  bool status = true;
  // Check if vehicle is still in air"Rel. Altitude: " <<
  // position.relative_altitude_m << " Latitude: " << position.latitude_deg << "
  // Longitude: " << position.longitude_deg
  while (telemetry->in_air()) {
    std::cout << "Vehicle is landing...\n";
    sleep_for(seconds(1));
  }

  Action::Result disarm_result = action->disarm();
  if (disarm_result != Action::Result::Success) {
    std::cout << "Vehicle failed to disarm\n";
    status = false;
  } else {
    std::cout << "Vehicle disarmed\n";
  }
  statusMessage.set_state(LANDED);
  return status;
}

bool MAVSDKUtils::Land(void) {
  std::cout << "Arm() called\n";
  bool status = true;
  const auto land_result = action->land();
  if (land_result != Action::Result::Success) {
    std::cerr << "Landing failed: " << land_result << '\n';
    status = false;
  }
  statusMessage.set_state(LANDING);
  while (IsInAir()) {
    sleep(1);
  }
  statusMessage.set_state(LANDED);
  return status;
}

bool MAVSDKUtils::Start(const Waypoint *waypoint) {
  std::cout << "Arm() called\n";
  bool status = true;
  GotoWayPoint(waypoint);
  Offboard::Result offboard_result = offboard->start();
  statusMessage.set_state(FLYING);
  if (offboard_result != Offboard::Result::Success) {
    std::cerr << "Offboard start failed: " << offboard_result << '\n';
    status = false;
    if (mavsdk_logger != nullptr) {
      mavsdk_logger->information("Offboard start command");
    }
  }
  return status;
}

bool MAVSDKUtils::TakeOff(double takeoffAlt) {
  std::cout << "Arm() called\n";
  bool status = true;
  takeoffAltitude = takeoffAlt;
  if (Arm()) {
    const auto takeoff_result = action->takeoff();
    if (takeoff_result != Action::Result::Success) {
      std::cerr << "Takeoff failed: " << takeoff_result << '\n';
      status = false;
      statusMessage.set_state(TAKEOFFFAILED);
    } else {
      statusMessage.set_state(TAKINGOFF);
    }
  } else {
    statusMessage.set_state(TAKEOFFFAILED);
  }
  return status;
}

bool MAVSDKUtils::IsInAir(void) {
  std::cout << "Arm() called\n";
  return telemetry->in_air();
}

void MAVSDKUtils::LockStatus(void) { status_mtx.lock(); }

void MAVSDKUtils::UnlockStatus(void) { status_mtx.unlock(); }

// Callbacks for data coming periodically from PX4

void MAVSDKUtils::PositionCallback(Telemetry::Position position) {
  static bool first_call = true;
  char total_string[256];
  sprintf(total_string,
          "Position :: Rel. Altitude: %lf Latitude: %lf Longitude: %lf \n",
          position.relative_altitude_m, position.latitude_deg,
          position.longitude_deg);
  if (mavsdk_logger != nullptr) {
    // mavsdk_logger->information(total_string);
  }
  if (statusMessage.state() == TAKINGOFF) {
    if (position.relative_altitude_m > takeoffAltitude) {
      statusMessage.set_state(FLYING);
    }
  } else if (statusMessage.state() == FLYING) {
    const double delta = 0.0001;
    if ((abs(dest_waypoint.latlon().latitude() - position.latitude_deg) <
         delta) &&
        (abs(dest_waypoint.latlon().longitude() - position.longitude_deg) <
         delta)) {
      statusMessage.set_state(WAYPOINTREACHED);
      mavsdk_logger->information("State changed to WAYPOINREACHED");
    }
  } else if (statusMessage.state() == FLYINGTOBASE) {
    const double delta = 0.0001;
    if ((abs(base_waypoint.latlon().latitude() - position.latitude_deg) <
         delta) &&
        (abs(base_waypoint.latlon().longitude() - position.longitude_deg) <
         delta)) {
      statusMessage.set_state(BASEREACHED);
      mavsdk_logger->information("State changed to BASEREACHED");
    }
  }
  if (first_call) {
    base_waypoint.mutable_latlon()->set_latitude(position.latitude_deg);
    base_waypoint.mutable_latlon()->set_longitude(position.longitude_deg);
    base_waypoint.set_altitude(1.5); // Standard altitude
    sprintf(total_string,
            "Base Latitude: %lf and Base Longitude: %lf stored.\n",
            position.latitude_deg, position.longitude_deg);
    if (mavsdk_logger != nullptr) {
      mavsdk_logger->information(total_string);
    }
    first_call = false;
  }
  // Need to use a lock, because the cleint can be using the data to
  // send back status to the mission manager. The lock gurantees atomicity
  LockStatus();
  vehicleState.pos_lat = position.latitude_deg;
  vehicleState.pos_lon = position.longitude_deg;
  vehicleState.pos_alt = position.relative_altitude_m;
  statusMessage.mutable_position()->set_latitude(position.latitude_deg);
  statusMessage.mutable_position()->set_longitude(position.longitude_deg);
  statusMessage.set_altitude(position.relative_altitude_m);
  unsigned long t = time(NULL);
  statusMessage.mutable_time()->set_epoch(t);
  UnlockStatus();
}

void MAVSDKUtils::SubscribePosition(void) {
  std::cout << "Arm() called\n";
  // We want to listen to the altitude of the drone at 0.3 Hertz.
  const auto set_rate_result = telemetry->set_rate_position(0.3);
  if (set_rate_result != Telemetry::Result::Success) {
    std::cerr << "Setting position rate failed: " << set_rate_result << '\n';
  }

  // Set up callback to monitor altitude while the vehicle is in flight
  telemetry->subscribe_position(PositionCallback);
}

void MAVSDKUtils::PositionVelocityNEDCallback(
    Telemetry::PositionVelocityNed posvel) {
  char total_string[256];
  Telemetry::VelocityNed velNed = posvel.velocity;
  Telemetry::PositionNed posNed = posvel.position;
  LockStatus();
  vehicleState.pos_ned_north = posNed.north_m;
  vehicleState.pos_ned_east = posNed.east_m;
  vehicleState.pos_ned_down = posNed.down_m;
  vehicleState.vel_ned_north = velNed.north_m_s;
  vehicleState.vel_ned_east = velNed.east_m_s;
  vehicleState.vel_ned_down = velNed.down_m_s;
  UnlockStatus();

  sprintf(total_string, "Position Ned :: N: %lf E: %lf D: %lf \n",
          posNed.north_m, posNed.east_m, posNed.down_m);
  if (mavsdk_logger != nullptr) {
    // mavsdk_logger->information(total_string);
  }
  sprintf(total_string, "Velocity Ned :: N: %lf E: %lf D: %lf \n",
          velNed.north_m_s, velNed.east_m_s, velNed.down_m_s);
  if (mavsdk_logger != nullptr) {
    // mavsdk_logger->information(total_string);
  }
}

void MAVSDKUtils::SubscribePositionVelocityNED(void) {
  std::cout << "Arm() called\n";
  // We want to listen to the altitude of the drone at 0.3 Hertz.
  const auto set_rate_result = telemetry->set_rate_position_velocity_ned(0.3);
  if (set_rate_result != Telemetry::Result::Success) {
    std::cerr << "Setting velocity rate failed: " << set_rate_result << '\n';
  }

  // Set up callback to monitor velocity while the vehicle is in flight
  telemetry->subscribe_position_velocity_ned(PositionVelocityNEDCallback);
}

void MAVSDKUtils::AttitudeQuaternionCallback(Telemetry::Quaternion attitude) {
  char total_string[256];
  LockStatus();
  vehicleState.att_quat_w = attitude.w;
  vehicleState.att_quat_x = attitude.x;
  vehicleState.att_quat_y = attitude.y;
  vehicleState.att_quat_z = attitude.z;
  UnlockStatus();
  sprintf(total_string, "Quaternion Attitude :: W: %lf X: %lf Y: %lf Z: %lf \n",
          attitude.w, attitude.x, attitude.y, attitude.z);
  if (mavsdk_logger != nullptr) {
    // mavsdk_logger->information(total_string);
  }
}

void MAVSDKUtils::SubscribeAttitudeQuaternion(void) {
  std::cout << "Arm() called\n";
  // We want to listen to the altitude of the drone at 0.3 Hertz.
  const auto set_rate_result = telemetry->set_rate_attitude_quaternion(0.3);
  if (set_rate_result != Telemetry::Result::Success) {
    std::cerr << "Setting attitude rate failed: " << set_rate_result << '\n';
  }

  // Set up callback to monitor attitude while the vehicle is in flight
  telemetry->subscribe_attitude_quaternion(AttitudeQuaternionCallback);
}

void MAVSDKUtils::AttitudeEulerCallback(Telemetry::EulerAngle attitude) {
  char total_string[256];
  LockStatus();
  vehicleState.ang_vel_roll = attitude.roll_deg;
  vehicleState.ang_vel_pitch = attitude.pitch_deg;
  vehicleState.ang_vel_yaw = attitude.yaw_deg;
  vehicleState.timestamp = attitude.timestamp_us;
  UnlockStatus();
  sprintf(total_string,
          "Euler Attitude :: Roll: %lf Pitch: %lf Yaw: %lf Time: %ld \n",
          attitude.roll_deg, attitude.pitch_deg, attitude.yaw_deg,
          attitude.timestamp_us);
  if (mavsdk_logger != nullptr) {
    // mavsdk_logger->information(total_string);
  }
}

void MAVSDKUtils::SubscribeAttitudeEuler(void) {
  std::cout << "Arm() called\n";
  // We want to listen to the altitude of the drone at 0.3 Hertz.
  const auto set_rate_result = telemetry->set_rate_attitude_euler(0.3);
  if (set_rate_result != Telemetry::Result::Success) {
    std::cerr << "Setting attitude rate failed: " << set_rate_result << '\n';
  }

  // Set up callback to monitor attitude while the vehicle is in flight
  telemetry->subscribe_attitude_euler(AttitudeEulerCallback);
}

void MAVSDKUtils::AngularVelocityBodyCallback(
    Telemetry::AngularVelocityBody angularVelBody) {
  char total_string[256];
  LockStatus();
  vehicleState.ang_vel_roll = angularVelBody.roll_rad_s;
  vehicleState.ang_vel_pitch = angularVelBody.pitch_rad_s;
  vehicleState.ang_vel_yaw = angularVelBody.yaw_rad_s;
  UnlockStatus();
  sprintf(total_string, "Angular velocity :: Roll: %lf Pitch: %lf Yaw: %lf \n",
          angularVelBody.roll_rad_s, angularVelBody.pitch_rad_s,
          angularVelBody.yaw_rad_s);
  if (mavsdk_logger != nullptr) {
    // mavsdk_logger->information(total_string);
  }
}

void MAVSDKUtils::SubscribeAngularVelocityBody(void) {
  std::cout << "Arm() called\n";
  // Set up callback to monitor attitude while the vehicle is in flight
  telemetry->subscribe_attitude_angular_velocity_body(
      AngularVelocityBodyCallback);
}

void MAVSDKUtils::OdometryCallback(Telemetry::Odometry odometry) {
  LockStatus();
  vehicleState.pos_x = odometry.position_body.x_m;
  vehicleState.pos_y = odometry.position_body.y_m;
  vehicleState.pos_z = odometry.position_body.z_m;
  vehicleState.vel_x = odometry.velocity_body.x_m_s;
  vehicleState.vel_y = odometry.velocity_body.y_m_s;
  vehicleState.vel_z = odometry.velocity_body.z_m_s;
  UnlockStatus();
  if (mavsdk_logger != nullptr) {
    // mavsdk_logger->information("OdometryCallback called");
  }
}

void MAVSDKUtils::SubscribeOdometry(void) {
  std::cout << "Arm() called\n";
  // Subscribe for updates at 3Hz frequency
  const auto set_rate_result = telemetry->set_rate_odometry(3);
  if (set_rate_result != Telemetry::Result::Success) {
    std::cerr << "Subscribe odometry failed: " << set_rate_result << '\n';
  }
  // Set up callback to monitor attitude while the vehicle is in flight
  telemetry->subscribe_odometry(OdometryCallback);
}

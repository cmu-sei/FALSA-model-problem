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

#include "client.h"
#include "guidance.h"
#include "mavsdkutils.h"
#include <iostream>
#include <unistd.h>

void ClientStatus::saveStatus(StatusMessage statusMessage) {
  ClientContext context;
  ::google::protobuf::Empty reply;
  Status status = stub_->saveStatus(&context, statusMessage, &reply);
  // log_ptr->information("Status message sent.");
}

void ClientStatus::logState(VehicleState vehicleState) {
  // log_ptr->information(std::string("Position :: lat: ") +
  // std::to_string(vehicleState.pos_lat) + std::string(" lon ") +
  // std::to_string(vehicleState.pos_lon) + std::string(" alt ") +
  // std::to_string(vehicleState.pos_alt));
  // log_ptr->information(std::string("Position NED :: North: ") +
  // std::to_string(vehicleState.pos_ned_north) + std::string(" East ") +
  // std::to_string(vehicleState.pos_ned_east) + std::string(" Down ") +
  // std::to_string(vehicleState.pos_ned_down));
  // log_ptr->information(std::string("Velocity NED :: North: ") +
  // std::to_string(vehicleState.vel_ned_north) + std::string(" East ") +
  // std::to_string(vehicleState.vel_ned_east) + std::string(" Down ") +
  // std::to_string(vehicleState.vel_ned_down));
  // log_ptr->information(std::string("Attitude Quaternion :: W: ") +
  // std::to_string(vehicleState.att_quat_w) + std::string(" X: ") +
  // std::to_string(vehicleState.att_quat_x) + std::string(" Y: ") +
  // std::to_string(vehicleState.att_quat_y) + std::string(" Z: ") +
  // std::to_string(vehicleState.att_quat_z));
  // log_ptr->information(std::string("Attitude Euler :: Roll: ") +
  // std::to_string(vehicleState.att_euler_roll) + std::string(" Pitch: ") +
  // std::to_string(vehicleState.att_euler_pitch) + std::string(" Yaw: ") +
  // std::to_string(vehicleState.att_euler_yaw) + std::string(" Timestamp: ") +
  // std::to_string(vehicleState.timestamp));
}

// This function is in a run forever thread
void RunClient(Logger &logger, std::string client_addr_port) {
  logger.information("RunClient() starting on port: " + client_addr_port);
  MAVSDKUtils *mavsdkUtils = MAVSDKUtils::getInstance(&logger);
  sleep(5); // Wait until things get settled
  mavsdkUtils->SubscribePosition();
  mavsdkUtils->SubscribePositionVelocityNED();
  mavsdkUtils->SubscribeAttitudeQuaternion();
  mavsdkUtils->SubscribeAttitudeEuler();
  mavsdkUtils->SubscribeAngularVelocityBody();
  mavsdkUtils->SubscribeOdometry();
  ClientStatus &client1 = ClientStatus::getInstance(&logger, client_addr_port);

  // Run forever and periodically send the status back to the mission manager
  while (1) {
    unsigned statusPeriod = ImplGuidance::getInstance().GetStatusPeriod();
    if (statusPeriod == 0) { // status not subscribed
      // wait for a second to check again
      sleep(1);
      continue;
    }

    // Need to read to data with a lock, because the MAVSDK
    // may be updating it asynchronously. The lock gurantees atomicity
    mavsdkUtils->LockStatus();
    client1.logState(MAVSDKUtils ::vehicleState);
    client1.saveStatus(MAVSDKUtils ::statusMessage);
    mavsdkUtils->UnlockStatus();
    sleep(statusPeriod / 1000); // mSecs to Sec
  }
}

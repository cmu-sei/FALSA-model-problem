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

#include "governor.h"
#include "mavsdkutils.h"
#include <array>
#include <cmath>
#include <eigen3/Eigen/Dense>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace Eigen;

static MAVSDKUtils *mavsdkUtils = NULL;

// This function is in a run forever thread
void RunGovernor(Logger &logger) {
  mavsdkUtils = MAVSDKUtils::getInstance(&logger);
  logger.information("RunGovernor() starting ");

  // Run forever and periodically send the status back to the mission manager
  while (1) {
    sleep(5);
    // Call the governor functions below as needed
  }
}

Matrix<float, 12, 1> getCurrentState(attitude_quaternion_t *v_att,
                                     vehicle_local_position_t *local_pos) {
  Matrix<float, 12, 1> _x;

  Quaternion<float> q(v_att->q1, v_att->q2, v_att->q3, v_att->q4);
  q.normalize();

  _x(0, 0) = local_pos->vx;
  _x(1, 0) = local_pos->x;
  _x(2, 0) = local_pos->vy;
  _x(3, 0) = local_pos->y;
  _x(4, 0) = local_pos->vz;
  _x(5, 0) = local_pos->z;

  auto euler = q.toRotationMatrix().eulerAngles(2, 1, 0);
  float roll_angle = euler[2];
  float pitch_angle = euler[1];
  float yaw_angle = euler[0];

  _x(6, 0) = v_att->rollspeed;
  _x(7, 0) = roll_angle;
  _x(8, 0) = v_att->pitchspeed;
  _x(9, 0) = pitch_angle;
  _x(10, 0) = v_att->yawspeed;
  _x(11, 0) = yaw_angle;

  return _x;
}

Matrix<float, 12, 1> getWayPoint(float x_d, float y_d, float z_d) {
  Matrix<float, 12, 1> _x_f;

  _x_f(0, 0) = 0.0f;
  _x_f(1, 0) = x_d;
  _x_f(2, 0) = 0.0f;
  _x_f(3, 0) = y_d;
  _x_f(4, 0) = 0.0f;
  _x_f(5, 0) = z_d;

  _x_f(6, 0) = 0.0f;
  _x_f(7, 0) = 0.0f;
  _x_f(8, 0) = 0.0f;
  _x_f(9, 0) = 0.0f;
  _x_f(10, 0) = 0.0f;
  _x_f(11, 0) = 0.0f;

  return _x_f;
}

Matrix<float, 12, 1> ergf(Matrix<float, 12, 12> P, Matrix<float, 12, 1> x_f,
                          Matrix<float, 12, 1> x_a, Matrix<float, 12, 1> x_sp,
                          float m, float s) {
  // Inputs:
  // P: symmetric positive definite matrix (12x12)
  // x_f: final position/next waypoint (12x1)
  // x_a: current state (12x1)
  // x_sp: current setpoint (12x1)
  // m: size of the external ellipsoid
  // s: size of the internal ellipsoid
  // Outputs:
  // new_x_sp: new setpoint

  Matrix<float, 12, 1> new_x_sp; // Assuming new_x_sp is a 12x1 vector

  float Vc = (x_a - x_sp).transpose() * P * (x_a - x_sp);

  if (Vc <= s) {
    double Vf = (x_f - x_sp).transpose() * P * (x_f - x_sp);
    Matrix<float, 12, 1> v = (x_f - x_sp) / Vf;
    new_x_sp = (sqrt(m) - sqrt(s)) * v;
  } else {
    new_x_sp = x_sp;
  }

  return new_x_sp;
}

Matrix<float, 12, 12> readMatrixP(const char *filename) {

  static Matrix<float, 12, 12> result;
  static int rows = 12;
  static int cols = 12;
  std::ifstream infile;
  infile.open(filename);
  if (infile.is_open()) {
    for (int i = 0; i < rows; i++) {
      std::string line;
      getline(infile, line);
      std::stringstream stream(line);
      for (int j = 0; j < cols; j++) {
        stream >> result(i, j);
      }
    }
    infile.close();
  } else {
    std::cout << "Unable to open file";
  }
  return result;
}

int test_governor() {
  float x_d = 0;
  float y_d = 0;
  float z_d = 0;
  attitude_quaternion_t att_quaternion;
  vehicle_local_position_t local_position;

  mavsdkUtils->LockStatus();
  // Get the info for the quaternion struct
  att_quaternion.q1 = MAVSDKUtils::vehicleState.att_quat_w;
  att_quaternion.q2 = MAVSDKUtils::vehicleState.att_quat_x;
  att_quaternion.q3 = MAVSDKUtils::vehicleState.att_quat_y;
  att_quaternion.q4 = MAVSDKUtils::vehicleState.att_quat_z;
  att_quaternion.rollspeed = MAVSDKUtils::vehicleState.ang_vel_roll;
  att_quaternion.pitchspeed = MAVSDKUtils::vehicleState.ang_vel_pitch;
  att_quaternion.yawspeed = MAVSDKUtils::vehicleState.ang_vel_yaw;
  // Get the info for the local position structure
  local_position.x = MAVSDKUtils::vehicleState.pos_x;
  local_position.y = MAVSDKUtils::vehicleState.pos_y;
  local_position.z = MAVSDKUtils::vehicleState.pos_z;
  local_position.vx = MAVSDKUtils::vehicleState.vel_x;
  local_position.vy = MAVSDKUtils::vehicleState.vel_y;
  local_position.vz = MAVSDKUtils::vehicleState.vel_z;
  mavsdkUtils->UnlockStatus();

  Matrix<float, 12, 12> P; // Initialize and load P matrix from your data
  Matrix<float, 12, 1> x_a = getCurrentState(&att_quaternion, &local_position);
  Matrix<float, 12, 1> x_f =
      getWayPoint(x_d, y_d, z_d); // the parameters should be initialized
  Matrix<float, 12, 1> x_sp;      // Initialize and load P matrix from your data
  float m;
  float s;

  P = readMatrixP("./new_pe.txt");
  Matrix<float, 12, 1> new_x_sp =
      ergf(P, x_f, x_a, x_sp, m, s); // Assuming x_a as x_sp initially

  // Print or use new_x_sp as needed
  std::cout << "New Setpoint: " << new_x_sp << std::endl;

  return 0;
}

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


#ifndef VEHICLE_STATE_H_H
#define VEHICLE_STATE_H_H

// This sturcture is used to store all data that comes from the vehicle
struct VehicleState {
  /* Position */
  double pos_lat;
  double pos_lon;
  double pos_alt;

  /* Position NED */
  double pos_ned_north;
  double pos_ned_east;
  double pos_ned_down;

  /* Velocity NED */
  double vel_ned_north;
  double vel_ned_east;
  double vel_ned_down;

  /* Position x,y,z */
  double pos_x;
  double pos_y;
  double pos_z;

  /* Velocity vx,vy,vz */
  double vel_x;
  double vel_y;
  double vel_z;

  /* Attitude Quaternion */
  double att_quat_w;
  double att_quat_x;
  double att_quat_y;
  double att_quat_z;

  /* Attitude Euler */
  double att_euler_roll;
  double att_euler_pitch;
  double att_euler_yaw;
  long timestamp;

  /* Angular Velocity */
  double ang_vel_roll;
  double ang_vel_pitch;
  double ang_vel_yaw;
};

#endif

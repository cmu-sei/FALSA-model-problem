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


#ifndef GOVERNOR_H_H
#define GOVERNOR_H_H

#include "Poco/Logger.h"
#include <unistd.h>
using Poco::Logger;

struct attitude_quaternion_t {
  float q1;         /*<  Quaternion component 1*/
  float q2;         /*<  Quaternion component 2*/
  float q3;         /*<  Quaternion component 3*/
  float q4;         /*<  Quaternion component 4*/
  float rollspeed;  /*< [rad/s] Roll angular speed*/
  float pitchspeed; /*< [rad/s] Pitch angular speed*/
  float yawspeed;   /*< [rad/s] Yaw angular speed*/
};

struct vehicle_local_position_t {
  float x;
  float y;
  float z;
  float vx;
  float vy;
  float vz;
};

// Runs the governor in a separate thread
void RunGovernor(Logger &logger);

#endif

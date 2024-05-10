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

#ifndef TIMER_UTIL_H_H
#define TIMER_UTIL_H_H

#include "Poco/Logger.h"
#include "Poco/Timer.h"
#include "client_assurance.h"
#include "client_guidance.h"
#include "client_payload.h"
#include "state_control.h"
#include <iostream>
#include <unistd.h>

using Poco::Logger;
using Poco::Timer;
using Poco::TimerCallback;

// Timer class that emulates a periodic task
class TimerUtil {
public:
  TimerUtil(std::string guidance_client_port, std::string payload_client_port,
            std::string assurancebrkr_client_port);
  ~TimerUtil();
  void periodicTimerCall(Timer &timer);
  Timer *launchTimer(void);

private:
  ClientPayload *clientPayload;
  ClientAssurance *clientAssurance;
  StateControl *state_control;
  ClientGuidance *clientGuidance;
  double elapsed_time;
  bool subscribed_to_status = false;
  static const unsigned STATUS_UPDATE_PERIOD_MSEC;
};

#endif

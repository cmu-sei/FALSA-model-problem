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

#include "timer_util.h"

const unsigned TimerUtil::STATUS_UPDATE_PERIOD_MSEC = 2000;
static bool already_released = false;

void TimerUtil::periodicTimerCall(Timer &timer) {
  const double delta = 0.001;

  // if we haven't successfully subscribed to status yet, try again
  if (!subscribed_to_status) {
    clientGuidance->subscribeStatus(STATUS_UPDATE_PERIOD_MSEC);
    subscribed_to_status = clientGuidance->getLastGrpcStatus().ok();
  }

  MissionState mission_state = state_control->GetMissionState();
  LockedState locked_state = state_control->GetLockedState();
  LatLonCoord lat_lon = state_control->GetLatLonCoord();
  LatLonCoord lat_lon_dest = state_control->GetLatLonCoordDest();
  double time_dest = state_control->GetTimeDest();
  elapsed_time += 0.5; // We are called every 500ms

  switch (mission_state) {
  case MissionState::AT_DESTINATION:
    std::cout << "State is AT_DESTINATION" << std::endl;
    clientAssurance->checkState("at_destination");
    if ((locked_state == LockedState::UNLOCKED) &&
        (mission_state != MissionState::DROP_SUPPLIES)) {
      std::cout << std::endl
                << "*** RELEASING PAYLOAD ***" << std::endl
                << std::endl;
      clientPayload->releasePayload();
      state_control->SetMissionState(MissionState::DROP_SUPPLIES);
      state_control->SetLockedState(LockedState::LOCKED);
      clientPayload->lockReleaseMechanism();
      already_released = true;
    }
    break;
  case MissionState::DROP_SUPPLIES:
    clientAssurance->checkState("drop_supplies");
    break;
  case MissionState::FLYING_TO_DESTINATION:
    // Check if we are close to the destination
    // If so then unlock the release mechanism
    if (!already_released) {
      if ((abs(lat_lon.latitude() - lat_lon_dest.latitude()) < delta) &&
          (abs(lat_lon.longitude() - lat_lon_dest.longitude()) < delta) &&
          (locked_state == LockedState::LOCKED)) {
        std::cout << std::endl
                  << "*** UNLOCKING RELEASE MECHANISM ***" << std::endl
                  << std::endl;
        clientPayload->unlockReleaseMechanism();
        state_control->SetLockedState(LockedState::UNLOCKED);
      }
    }
    break;
  }
}

TimerUtil::TimerUtil(std::string guidance_client_port,
                     std::string payload_client_port,
                     std::string assurancebrkr_client_port) {
  clientPayload = ClientPayload::getInstance(payload_client_port);
  clientPayload->lockReleaseMechanism();
  state_control = StateControl::getInstance();
  clientAssurance = ClientAssurance::getInstance(assurancebrkr_client_port);
  clientGuidance = ClientGuidance::getInstance(guidance_client_port);
  clientGuidance->subscribeStatus(STATUS_UPDATE_PERIOD_MSEC);
  subscribed_to_status = clientGuidance->getLastGrpcStatus().ok();
  elapsed_time = 0.0;
}

TimerUtil::~TimerUtil() { delete clientPayload; }

Timer *TimerUtil::launchTimer(void) {
  TimerCallback<TimerUtil> timerCallback(*this, &TimerUtil::periodicTimerCall);
  Timer *timer = new Timer(0, 500);
  timer->start(timerCallback);
  return timer;
}

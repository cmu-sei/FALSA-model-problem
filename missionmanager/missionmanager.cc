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

#include "missionmanager.h"

// Static variable instance
ImplMissionManager *ImplMissionManager::instancePtr = nullptr;
ClientGuidance *ImplMissionManager::client1 = nullptr;
Logger *ImplMissionManager::log_ptr = nullptr;
StateControl *ImplMissionManager::state_control = nullptr;

ImplMissionManager::ImplMissionManager(void) {
  std::cout << "ImplMissionManager called" << std::endl;
  if (client1 == nullptr) {
    client1 = ClientGuidance::getInstance();
  }
  // initialize default values;
  takeoffAltitude = 2.0;
  if (state_control == nullptr) {
    state_control = StateControl::getInstance();
  }
}

ImplMissionManager::~ImplMissionManager() {}

ImplMissionManager *ImplMissionManager::getInstance(Logger *log) {

  // If there is no instance of class
  // then we can create an instance.
  if (instancePtr == nullptr) {
    // We can access private members
    // within the class.
    instancePtr = new ImplMissionManager();
    if (log != nullptr) {
      instancePtr->log_ptr = log;
    }
    instancePtr->state_control->SetMissionState(MissionState::INITIALIZED);
    instancePtr->state_control->SetLockedState(LockedState::LOCKED);
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

void ImplMissionManager::setMissionParams(LatLonCoord destinationInput,
                                          Time startTimeInput,
                                          Time endTimeInput,
                                          double takeoffAltitudeInput) {
  destination = destinationInput;
  startTime = startTimeInput;
  endTime = endTimeInput;
  takeoffAltitude = takeoffAltitudeInput;
  if (state_control->GetMissionState() != MissionState::PARAMETERS_SET) {
    Waypoint wp;
    wp.mutable_latlon()->set_latitude(destination.latitude());
    wp.mutable_latlon()->set_longitude(destination.longitude());
    wp.set_altitude(takeoffAltitude);
    client1->addWaypoint(&wp);
    log_ptr->information("Sending Waypoint to guidance component.");
    state_control->SetLatLonCoordDest(destinationInput);
    state_control->SetTimeDest(endTimeInput.epoch() - startTimeInput.epoch());
    state_control->SetMissionState(MissionState::PARAMETERS_SET);
    log_ptr->information(
        "Mission parameters were set. State is now PARAMETERS_SET");
  } else {
    log_ptr->information(
        "Already in PARAMETERS_SET state. New parameters were NOT set");
  }
}

void ImplMissionManager::clearMissionParams(void) {
  if (state_control->GetMissionState() != MissionState::INITIALIZED) {
    LatLonCoord latlon;
    latlon.set_latitude(0);
    latlon.set_longitude(0);
    state_control->SetLatLonCoordDest(latlon);
    state_control->SetTimeDest(0);
    state_control->SetMissionState(MissionState::INITIALIZED);
    log_ptr->information(
        "Mission parameters were cleared. State is now INITIALIZED");
  } else {
    log_ptr->information("Already in INITIALIZED state. Nothing to clear.");
  }
}

void ImplMissionManager::takeOff(void) {
  if (state_control->GetMissionState() == MissionState::PARAMETERS_SET) {
    client1->takeOff(takeoffAltitude);
    state_control->SetMissionState(MissionState::TAKEOFF_STARTED);
    log_ptr->information("Takeoff Initiated. State is now TAKEOFF_STARTED");
  } else {
    log_ptr->information("Need to be in PARAMETERS_SET state. Not taking off.");
  }
}

void ImplMissionManager::abort(void) {
  MissionState missionState = state_control->GetMissionState();
  if (missionState != MissionState::INITIALIZED &&
      missionState != MissionState::PARAMETERS_SET &&
      missionState != MissionState::LANDED &&
      missionState != MissionState::LANDING_AT_BASE) {
    client1->clearRoute();
    client1->returnToBase();
    state_control->SetMissionState(MissionState::RETURNING_TO_BASE);
    log_ptr->information("Abort initiated. State is now RETURNING_TO_BASE");
  } else {
    log_ptr->information("Current state does not allow abort operation.");
  }
}

void ImplMissionManager::saveStatus(StatusMessage statusMessage) {
  // process status message;
  State st = statusMessage.state();
  static State previous_state = INITIALIZED;
  LatLonCoord lat_lon = statusMessage.position();
  double altitude = statusMessage.altitude();
  state_control->SetAltitude(altitude);
  state_control->SetLatLonCoord(lat_lon);
  log_ptr->information("Setting new drone coordinates:: Altitude " +
                       std::to_string(altitude) +
                       " Latitude: " + std::to_string(lat_lon.latitude()) +
                       " Longitude: " + std::to_string(lat_lon.longitude()));
  switch (st) {
  case TAKINGOFF:
    log_ptr->information("State is now TAKEOFF_STARTED");
    state_control->SetMissionState(MissionState::TAKEOFF_STARTED);
    break;
  case TAKEOFFFAILED:
    log_ptr->information("State is now LANDED");
    state_control->SetMissionState(MissionState::LANDED);
    break;
  case FLYING:
    log_ptr->information("State is now FLYING_TO_DESTINATION");
    state_control->SetMissionState(MissionState::FLYING_TO_DESTINATION);
    if ((previous_state == TAKINGOFF) || (previous_state == INITIALIZED)) {
      log_ptr->information("Sending start command");
      client1->start();
    }
    break;
  case FLYINGTOBASE:
    log_ptr->information("State is now RETURNING_TO_BASE");
    state_control->SetMissionState(MissionState::RETURNING_TO_BASE);
    break;
  case WAYPOINTREACHED:
    if (state_control->GetMissionState() != MissionState::AT_DESTINATION) {
      log_ptr->information("State is now AT_DESTINATION");
      state_control->SetMissionState(MissionState::AT_DESTINATION);
    } else {
      log_ptr->information("ReturnToBase() sent.");
      client1->returnToBase();
    }
    break;
  case BASEREACHED:
    log_ptr->information("State is now LANDING_AT_BASE");
    state_control->SetMissionState(MissionState::LANDING_AT_BASE);
    client1->land();
    break;
  case LASTWAYPOINTUNREACHABLE:
    log_ptr->information("State is now RETURNING_TO_BASE");
    state_control->SetMissionState(MissionState::RETURNING_TO_BASE);
    break;
  case LANDING:
    log_ptr->information("State is now LANDING_AT_BASE");
    state_control->SetMissionState(MissionState::LANDING_AT_BASE);
    break;
  case LANDED:
    log_ptr->information("State is now LANDED");
    state_control->SetMissionState(MissionState::LANDED);
    break;
  }
  previous_state = st;
}

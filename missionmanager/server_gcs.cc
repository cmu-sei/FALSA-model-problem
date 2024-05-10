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

#include "server_gcs.h"

MissionManagerServiceGcsImplementation::MissionManagerServiceGcsImplementation(
    Logger *log) {
  missionmanager = nullptr;
  log_ptr = log;
}

Status MissionManagerServiceGcsImplementation::abort(
    ServerContext *context, const ::google::protobuf::Empty *request,
    ::google::protobuf::Empty *response) {
  log_ptr->information("abort received");
  missionmanager->abort();
  return Status::OK;
}

Status MissionManagerServiceGcsImplementation::setMissionParams(
    ServerContext *context, const MissionParams *request,
    ::google::protobuf::Empty *response) {
  log_ptr->information("setMissionParams received: ");
  log_ptr->information(
      "Altitude: " + std::to_string(request->takeoffaltitude()) +
      " Latitude: " + std::to_string(request->destination().latitude()) +
      " Longitude: " + std::to_string(request->destination().longitude()) +
      " Start: " + std::to_string(request->starttime().epoch()) +
      " End: " + std::to_string(request->endtime().epoch()));
  missionmanager->setMissionParams(request->destination(), request->starttime(),
                                   request->endtime(),
                                   request->takeoffaltitude());
  return Status::OK;
}

Status MissionManagerServiceGcsImplementation::clearMissionParams(
    ServerContext *context, const ::google::protobuf::Empty *request,
    ::google::protobuf::Empty *response) {
  log_ptr->information("clearMissionParams received");
  missionmanager->clearMissionParams();
  return Status::OK;
}

Status MissionManagerServiceGcsImplementation::takeOff(
    ServerContext *context, const ::google::protobuf::Empty *request,
    ::google::protobuf::Empty *response) {
  log_ptr->information("takeOff received");
  missionmanager->takeOff();
  return Status::OK;
}

void MissionManagerServiceGcsImplementation::init(void) {
  missionmanager = ImplMissionManager::getInstance(log_ptr);
}

void MissionManagerServiceGcsImplementation::test(void) {
  if (missionmanager != nullptr) {
    LatLonCoord destination;
    Time startTime;
    Time endTime;
    double takeoffAltitude;

    missionmanager->setMissionParams(destination, startTime, endTime,
                                     takeoffAltitude);
    missionmanager->takeOff();
    sleep(10);
    missionmanager->abort();
  }
}

// Task entry point
void RunServerGcs(Logger &logger, std::string server_address) {
  MissionManagerServiceGcsImplementation service(&logger);

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  service.init();

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::string txt =
      std::string("MissionManager GCS Server listening on port: ") +
      server_address;
  logger.information(txt);

  server->Wait();
}

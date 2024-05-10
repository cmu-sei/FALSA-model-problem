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

#include "server_guidance.h"

MissionManagerServiceStatusImplementation::
    MissionManagerServiceStatusImplementation(Logger *log) {
  missionmanager = nullptr;
  log_ptr = log;
}

Status MissionManagerServiceStatusImplementation::saveStatus(
    ServerContext *context, const StatusMessage *request,
    ::google::protobuf::Empty *response) {
  log_ptr->information("saveStatus()::status message received");
  missionmanager->saveStatus(*request);
  return Status::OK;
}

void MissionManagerServiceStatusImplementation::init(void) {
  missionmanager = ImplMissionManager::getInstance(log_ptr);
}

void MissionManagerServiceStatusImplementation::test(void) {
  if (missionmanager != nullptr) {
  }
}

// Task entry point
void RunServerStatus(Logger &logger, std::string server_address) {
  MissionManagerServiceStatusImplementation service(&logger);

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  service.init();

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::string txt =
      std::string("MissionManager Status Server listening on port: ") +
      server_address;
  logger.information(txt);

  server->Wait();
}

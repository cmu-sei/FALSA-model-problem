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

#ifndef SERVER_GCS_H_H
#define SERVER_GCS_H_H

#include <grpcpp/grpcpp.h>

#include <string>

#include "Poco/Logger.h"

#include "IMissionManager.grpc.pb.h"
#include "missionmanager.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using Poco::Logger;
using namespace uav;

// This class implements a GRPC service to receive messages from GCS
class MissionManagerServiceGcsImplementation final
    : public MissionManager::Service {

public:
  Status abort(ServerContext *context, const ::google::protobuf::Empty *request,
               ::google::protobuf::Empty *response);

  Status setMissionParams(ServerContext *context, const MissionParams *request,
                          ::google::protobuf::Empty *response);

  Status clearMissionParams(ServerContext *context,
                            const ::google::protobuf::Empty *request,
                            ::google::protobuf::Empty *response);

  Status takeOff(ServerContext *context,
                 const ::google::protobuf::Empty *request,
                 ::google::protobuf::Empty *response);

  void init(void);

  void test(void);

  MissionManagerServiceGcsImplementation(Logger *log);

private:
  ImplMissionManager *missionmanager;
  Logger *log_ptr;
};

void RunServerGcs(Logger &logger, std::string server_address);

#endif

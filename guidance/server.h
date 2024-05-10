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

#ifndef SERVER_H_H
#define SERVER_H_H

#include <grpcpp/grpcpp.h>

#include <string>

#include "Poco/Logger.h"
#include "Poco/Semaphore.h"

#include "IGuidance.grpc.pb.h"
#include "guidance.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using Poco::Logger;

using namespace uav;

/* GRPC service class for receiving messages from the iGuidance interface */
class GuidanceServiceImplementation final : public Guidance::Service {
  Status addWaypoint(ServerContext *context, const Waypoint *request,
                     ::google::protobuf::Empty *response);
  // void clearRoute( )
  Status clearRoute(ServerContext *context,
                    const ::google::protobuf::Empty *request,
                    ::google::protobuf::Empty *response);
  // int getWaypointCount( )
  Status getWaypointCount(ServerContext *context,
                          const ::google::protobuf::Empty *request,
                          ::google::protobuf::Int32Value *response);
  // void land( )
  Status land(ServerContext *context, const ::google::protobuf::Empty *request,
              ::google::protobuf::Empty *response);
  // void returnToBase( )
  Status returnToBase(ServerContext *context,
                      const ::google::protobuf::Empty *request,
                      ::google::protobuf::Empty *response);
  // void start( )
  Status start(ServerContext *context, const ::google::protobuf::Empty *request,
               ::google::protobuf::Empty *response);
  // void subscribeStatus(int periodMsec )
  Status subscribeStatus(ServerContext *context,
                         const ::google::protobuf::Int32Value *request,
                         ::google::protobuf::Empty *response);
  // void takeOff(double takeoffAltitude )
  Status takeOff(ServerContext *context,
                 const ::google::protobuf::DoubleValue *request,
                 ::google::protobuf::Empty *response);
  ImplGuidance *guidance;
  Logger *log_ptr;

public:
  void init(void);
  void test(void);
  GuidanceServiceImplementation(Logger *log);
};

void RunServer(Logger &logger, std::string server_address,
               Poco::Semaphore &server_ready_sem);

#endif

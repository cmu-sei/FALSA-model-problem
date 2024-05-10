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

#ifndef CLIENT_GUIDANCE_H_H
#define CLIENT_GUIDANCE_H_H

#include <string>

#include "Poco/Logger.h"

using Poco::Logger;

#include <iostream>
#include <string>
#include <unistd.h>

#include "IGuidance.grpc.pb.h"
#include "IGuidance.h"
#include "IMissionManager.grpc.pb.h"
#include "IMissionManager.h"
#include "Waypoint.pb.h"
#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace uav;

// Client GRPC class that talks to the Guidance component
class ClientGuidance : public IGuidance {

private:
  static ClientGuidance *instance;
  std::unique_ptr<Guidance::Stub> stub_;

  ClientGuidance(const std::string &client_addr_port)
      : stub_(Guidance::NewStub(grpc::CreateChannel(
            client_addr_port, grpc::InsecureChannelCredentials()))) {}

  grpc::Status last_status;

public:
  static ClientGuidance *
  getInstance(const std::string &client_addr_port = "0.0.0.0:50052");

  ClientGuidance(ClientGuidance const &) = delete;
  void operator=(ClientGuidance const &) = delete;

  void addWaypoint(const Waypoint *waypoint);

  void clearRoute(void);

  int getWaypointCount(void);

  void arm(void);

  void disarm(void);

  void land(void);

  void returnToBase(void);

  void start(void);

  void subscribeStatus(const unsigned int periodMsec);

  void takeOff(const double takeoffAltitude);

  grpc::Status getLastGrpcStatus();
};

#endif

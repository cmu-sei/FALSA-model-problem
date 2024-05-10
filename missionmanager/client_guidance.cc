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

#include "client_guidance.h"

#include "IGuidance.grpc.pb.h"
#include "IGuidance.h"
#include "IMissionManager.grpc.pb.h"
#include "IMissionManager.h"
#include "Waypoint.pb.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <unistd.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace uav;

ClientGuidance *ClientGuidance ::instance = nullptr;

// TODO: can the port change? Should there be a version that doesn't take a
// port?
ClientGuidance *
ClientGuidance ::getInstance(const std::string &client_addr_port) {
  if (instance == nullptr) {
    instance = new ClientGuidance(client_addr_port);
  }
  return instance;
}

void ClientGuidance ::addWaypoint(const Waypoint *waypoint) {
  ClientContext context;
  Waypoint request;
  request = *waypoint;
  ::google::protobuf::Empty reply;
  last_status = stub_->addWaypoint(&context, request, &reply);
}

void ClientGuidance ::clearRoute(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::Empty reply;
  last_status = stub_->clearRoute(&context, request, &reply);
}

int ClientGuidance ::getWaypointCount(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::Int32Value reply;
  last_status = stub_->getWaypointCount(&context, request, &reply);
  return reply.value();
}

void ClientGuidance ::arm(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::Empty reply;
  last_status = stub_->arm(&context, request, &reply);
}

void ClientGuidance ::disarm(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::Empty reply;
  last_status = stub_->disarm(&context, request, &reply);
}

void ClientGuidance ::land(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::Empty reply;
  last_status = stub_->land(&context, request, &reply);
}

void ClientGuidance ::returnToBase(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::Empty reply;
  last_status = stub_->returnToBase(&context, request, &reply);
}

void ClientGuidance ::start(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::Empty reply;
  last_status = stub_->start(&context, request, &reply);
}

void ClientGuidance ::subscribeStatus(const unsigned int periodMsec) {
  ClientContext context;
  ::google::protobuf::Int32Value request;
  ::google::protobuf::Empty reply;
  request.set_value(periodMsec);
  last_status = stub_->subscribeStatus(&context, request, &reply);
}

void ClientGuidance ::takeOff(const double takeoffAltitude) {
  ClientContext context;
  ::google::protobuf::DoubleValue request;
  ::google::protobuf::Empty reply;
  request.set_value(takeoffAltitude);
  last_status = stub_->takeOff(&context, request, &reply);
}

grpc::Status ClientGuidance::getLastGrpcStatus() { return last_status; }
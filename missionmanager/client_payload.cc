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

#include "client_payload.h"

#include "IMissionManager.grpc.pb.h"
#include "IMissionManager.h"
#include "IPayload.grpc.pb.h"
#include "IPayload.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <unistd.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace uav;

bool ClientPayload::getLockStatus(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::BoolValue reply;
  Status status = stub_->getLockStatus(&context, request, &reply);
  return reply.value();
}

bool ClientPayload::hasReleased(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::BoolValue reply;
  Status status = stub_->hasReleased(&context, request, &reply);
  return reply.value();
}

void ClientPayload::lockReleaseMechanism(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::Empty reply;
  Status status = stub_->lockReleaseMechanism(&context, request, &reply);
}

bool ClientPayload::releasePayload(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::BoolValue reply;
  Status status = stub_->releasePayload(&context, request, &reply);
  return reply.value();
}

void ClientPayload::unlockReleaseMechanism(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::Empty reply;
  Status status = stub_->unlockReleaseMechanism(&context, request, &reply);
}

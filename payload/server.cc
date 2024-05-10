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

#include "server.h"

static ImplPayload payload;

PayloadServiceImplementation::PayloadServiceImplementation(Logger *log) {
  log_ptr = log;
}

Status PayloadServiceImplementation::getLockStatus(
    ServerContext *context, const ::google::protobuf::Empty *request,
    ::google::protobuf::BoolValue *response) {
  log_ptr->information("PayloadServiceImplementation::getLockStatus() invoked");
  response->set_value(payload.getLockStatus());
  return Status::OK;
}

// boolean hasReleased( )
Status PayloadServiceImplementation::hasReleased(
    ServerContext *context, const ::google::protobuf::Empty *request,
    ::google::protobuf::BoolValue *response) {
  log_ptr->information("PayloadServiceImplementation::hasReleased() invoked");
  response->set_value(payload.hasReleased());
  return Status::OK;
}

// void lockReleaseMechanism( )
Status PayloadServiceImplementation::lockReleaseMechanism(
    ServerContext *context, const ::google::protobuf::Empty *request,
    ::google::protobuf::Empty *response) {
  log_ptr->information(
      "PayloadServiceImplementation::lockReleaseMechanism() invoked");
  payload.lockReleaseMechanism();
  std::cout << std::endl
            << "*** LOCKED RELEASE MECHANISM ***" << std::endl
            << std::endl;
  return Status::OK;
}

// boolean releasePayload( )
Status PayloadServiceImplementation::releasePayload(
    ServerContext *context, const ::google::protobuf::Empty *request,
    ::google::protobuf::BoolValue *response) {
  log_ptr->information(
      "PayloadServiceImplementation::releasePayload() invoked");
  response->set_value(payload.releasePayload());
  std::cout << std::endl
            << "*** PAYLOAD RELEASED ***" << std::endl
            << std::endl;
  return Status::OK;
}

// void unlockReleaseMechanism( )
Status PayloadServiceImplementation::unlockReleaseMechanism(
    ServerContext *context, const ::google::protobuf::Empty *request,
    ::google::protobuf::Empty *response) {
  log_ptr->information(
      "PayloadServiceImplementation::unlockReleaseMechanism() invoked");
  payload.unlockReleaseMechanism();
  std::cout << std::endl
            << "*** UNLOCKED RELEASE MECHANISM ***" << std::endl
            << std::endl;
  return Status::OK;
}

void RunServer(Logger &logger, std::string server_address) {
  PayloadServiceImplementation service(&logger);

  ServerBuilder builder;

  logger.information("RunServer() starting on port: " + server_address);
  // Listen on the given address without any authentication mechanism
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which
  // communication with client takes place
  builder.RegisterService(&service);

  // Assembling the server
  std::unique_ptr<Server> server(builder.BuildAndStart());
  server->Wait();
}

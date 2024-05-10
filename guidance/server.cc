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

// Constructor
GuidanceServiceImplementation::GuidanceServiceImplementation(Logger *log) {
  guidance = nullptr;
  log_ptr = log;
}

/* GRPC service class methods */
Status GuidanceServiceImplementation::addWaypoint(
    ServerContext *context, const Waypoint *request,
    ::google::protobuf::Empty *response) {
  log_ptr->information("GuidanceServiceImplementation::addWaypoint() invoked");
  guidance->addWaypoint(request);
  return Status::OK;
}

Status GuidanceServiceImplementation::clearRoute(
    ServerContext *context, const ::google::protobuf::Empty *request,
    ::google::protobuf::Empty *response) {
  log_ptr->information("GuidanceServiceImplementation::clearRoute() invoked");
  guidance->clearRoute();
  return Status::OK;
}
// int getWaypointCount( )
Status GuidanceServiceImplementation::getWaypointCount(
    ServerContext *context, const ::google::protobuf::Empty *request,
    ::google::protobuf::Int32Value *response) {
  log_ptr->information(
      "GuidanceServiceImplementation::getWaypointCount() invoked");
  response->set_value(guidance->getWaypointCount());
  return Status::OK;
}
// void land( )
Status
GuidanceServiceImplementation::land(ServerContext *context,
                                    const ::google::protobuf::Empty *request,
                                    ::google::protobuf::Empty *response) {
  log_ptr->information("GuidanceServiceImplementation::land() invoked");
  guidance->land();
  return Status::OK;
}
// void returnToBase( )
Status GuidanceServiceImplementation::returnToBase(
    ServerContext *context, const ::google::protobuf::Empty *request,
    ::google::protobuf::Empty *response) {
  log_ptr->information("GuidanceServiceImplementation::returnToBase() invoked");
  guidance->returnToBase();
  return Status::OK;
}
// void start( )
Status
GuidanceServiceImplementation::start(ServerContext *context,
                                     const ::google::protobuf::Empty *request,
                                     ::google::protobuf::Empty *response) {
  log_ptr->information("GuidanceServiceImplementation::start() invoked");
  guidance->start();
  return Status::OK;
}
// void subscribeStatus(int periodMsec )
Status GuidanceServiceImplementation::subscribeStatus(
    ServerContext *context, const ::google::protobuf::Int32Value *request,
    ::google::protobuf::Empty *response) {
  log_ptr->information(
      "GuidanceServiceImplementation::subscribeStatus() invoked");
  guidance->subscribeStatus(request->value());
  return Status::OK;
}
// void takeOff(double takeoffAltitude )
Status GuidanceServiceImplementation::takeOff(
    ServerContext *context, const ::google::protobuf::DoubleValue *request,
    ::google::protobuf::Empty *response) {
  log_ptr->information("GuidanceServiceImplementation::takeOff() invoked");
  guidance->takeOff(request->value());
  return Status::OK;
}

void GuidanceServiceImplementation::init(void) {
  // ImplGuidance has the reference to MAVSDK and calls the right methods from
  // the MAVSDKUtils class
  guidance = &ImplGuidance::getInstance();
}

/* A test method showing how we can control the drone */
void GuidanceServiceImplementation::test(void) {
  if (guidance != nullptr) {
    log_ptr->information("GuidanceServiceImplementation::test() invoked");
    // guidance->arm();
    guidance->takeOff(1);
    sleep(10);
    guidance->land();
    guidance->disarm();
  }
}

void RunServer(Logger &logger, std::string server_address,
               Poco::Semaphore &server_ready_sem) {
  GuidanceServiceImplementation service(&logger);

  ServerBuilder builder;

  logger.information("RunServer() starting on port: " + server_address);
  // Listen on the given address without any authentication mechanism
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which
  // communication with client takes place
  builder.RegisterService(&service);

  service.init();

  /* Unit test for MAVSDK
   *  Uncomment if you want to test this component's interaction with PX4
   */
  // service.test();

  // Assembling the server
  std::unique_ptr<Server> server_guidance(builder.BuildAndStart());
  std::string txt = std::string("Server listening on port: ") + server_address;
  logger.information(txt);

  server_ready_sem.set();

  server_guidance->Wait();
}

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

#include "client.h"
#include <iostream>
#include <unistd.h>

void GCSClient::setMissionParams(LatLonCoord destination, Time startTime,
                                 Time endTime, double takeoffAltitude) {
  ClientContext context;
  MissionParams request;
  ::google::protobuf::Empty reply;
  request.mutable_destination()->set_latitude(destination.latitude());
  request.mutable_destination()->set_longitude(destination.longitude());
  request.mutable_starttime()->set_epoch(startTime.epoch());
  request.mutable_endtime()->set_epoch(endTime.epoch());
  request.set_takeoffaltitude(takeoffAltitude);
  // Actual Remote Procedure Call
  // Function name must match proto file rpc name
  Status status = stub_->setMissionParams(&context, request, &reply);
}

void GCSClient::clearMissionParams(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::Empty reply;
  // Actual Remote Procedure Call
  // Function name must match proto file rpc name
  Status status = stub_->clearMissionParams(&context, request, &reply);
}

void GCSClient::abort(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::Empty reply;
  // Actual Remote Procedure Call
  // Function name must match proto file rpc name
  Status status = stub_->abort(&context, request, &reply);
}

void GCSClient::takeOff(void) {
  ClientContext context;
  ::google::protobuf::Empty request;
  ::google::protobuf::Empty reply;
  // Actual Remote Procedure Call
  // Function name must match proto file rpc name
  Status status = stub_->takeOff(&context, request, &reply);
}

// This function is in a run forever thread
void RunClient(Logger &logger, std::string client_addr_port, std::string cmd) {
  // Instantiates the client
  GCSClient client(
      // Channel from which RPCs are made - endpoint i
      // request.set_takeoffaltitude(takeoffAltitude);s the target_address
      grpc::CreateChannel(client_addr_port,
                          // Indicate when channel is not authenticated
                          grpc::InsecureChannelCredentials()));
  logger.information("RunClient starting on port: " + client_addr_port);
  /* Initialize from command line params or a configuration file */
  LatLonCoord dest = LatLonCoord();
  Time start = Time();
  Time end = Time();
  double altitude;

  if (!strcmp(cmd.c_str(), "setmissionparams")) {
    double d;
    std::cout << "Type altitude : ";
    std::cin >> altitude;
    std::cout << "Type latitude : ";
    std::cin >> d;
    dest.set_latitude(d);
    std::cout << "Type longitude : ";
    std::cin >> d;
    dest.set_longitude(d);
    ::uint64_t v;
    std::cout << "Type start time : ";
    std::cin >> v;
    start.set_epoch(v);
    std::cout << "Type end time : ";
    std::cin >> v;
    end.set_epoch(v);
    logger.information("Sending command: " + cmd);
    logger.information("Parameters:: Altitude: " + std::to_string(altitude) +
                       " Latitude: " + std::to_string(dest.latitude()) +
                       " Longitude: " + std::to_string(dest.longitude()) +
                       " Start: " + std::to_string(start.epoch()) +
                       " End: " + std::to_string(end.epoch()));
    client.setMissionParams(dest, start, end, altitude);
  } else if (!strcmp(cmd.c_str(), "clearmissionparams")) {
    logger.information("Sending command: " + cmd);
    client.clearMissionParams();
  } else if (!strcmp(cmd.c_str(), "takeoff")) {
    logger.information("Sending command: " + cmd);
    client.takeOff();
  } else if (!strcmp(cmd.c_str(), "abort")) {
    logger.information("Sending command: " + cmd);
    client.abort();
  } else {
    logger.information("Unknown client command: " + cmd);
  }
}

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
#include "ltlmonrt.hpp"

static LTLMonitor monitor;
static std::string monFile = "../ltlmon-rt/tests/prop1.mon";

AssuranceBrokerServiceImplementation::AssuranceBrokerServiceImplementation(
    Logger *log) {
  log_ptr = log;
  if (!monitor.initialize(monFile)) {
    std::cout << "Could not load monitor from " << monFile << std::endl;
  };
}

Status AssuranceBrokerServiceImplementation::checkState(
    ServerContext *context, const ::google::protobuf::StringValue *request,
    ::google::protobuf::BoolValue *response) {
  bool result = true;
  result = monitor.step(request->value());
  std::cout << "Result of step[" << request->value() << "] -> " << result
            << std::endl;
  response->set_value(result);
  return Status::OK;
}

void RunServer(Logger &logger, std::string server_address) {
  AssuranceBrokerServiceImplementation service(&logger);

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

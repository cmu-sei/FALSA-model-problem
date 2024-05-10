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


#ifndef CLIENT_H_H
#define CLIENT_H_H

#include <string>

#include "IStatus.h"
#include "Poco/Logger.h"
#include "Status.grpc.pb.h"
#include "Status.pb.h"
#include "vehicleState.h"
#include <grpcpp/grpcpp.h>

using Poco::Logger;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace uav;

/*
 *  This class implements the IMissionManagerStatus interface
 *  Calling saveStatus() periodically sends the information
 *  we receive from PX4 back to the mission manager
 */
class ClientStatus : public IMissionManagerStatus {

private:
  std::unique_ptr<MissionManagerStatus::Stub> stub_;
  Logger *log_ptr;

  ClientStatus(const std::string &client_addr_port)
      : stub_(MissionManagerStatus::NewStub(grpc::CreateChannel(
            client_addr_port, grpc::InsecureChannelCredentials()))) {}

public:
  static ClientStatus &
  getInstance(Logger *log, const std::string &client_addr_port = "0.0.0.0:0") {
    static ClientStatus *instance = nullptr;
    if (instance == nullptr) {
      instance = new ClientStatus(client_addr_port);
      instance->log_ptr = log;
    }
    return *instance;
  }

  ClientStatus(ClientStatus const &) = delete;
  void operator=(ClientStatus const &) = delete;

  void saveStatus(StatusMessage statusMessage);
  void logState(VehicleState vehicleState);
};

void RunClient(Logger &logger, std::string client_addr_port);

#endif

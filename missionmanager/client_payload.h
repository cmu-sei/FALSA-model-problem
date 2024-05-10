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

#ifndef CLIENT_PAYLOAD_H_H
#define CLIENT_PAYLOAD_H_H

#include <string>

#include "Poco/Logger.h"

using Poco::Logger;

#include <iostream>
#include <string>
#include <unistd.h>

#include "IMissionManager.grpc.pb.h"
#include "IMissionManager.h"
#include "IPayload.grpc.pb.h"
#include "IPayload.h"
#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace uav;

// Client GRPC class that talks to the Payload component
class ClientPayload : public IPayload {

private:
  std::unique_ptr<Payload::Stub> stub_;

  ClientPayload(const std::string &client_addr_port)
      : stub_(Payload::NewStub(grpc::CreateChannel(
            client_addr_port, grpc::InsecureChannelCredentials()))) {}

public:
  static ClientPayload *
  getInstance(const std::string &client_addr_port = "0.0.0.0:0") {
    static ClientPayload *instance = nullptr;
    if (instance == nullptr) {
      instance = new ClientPayload(client_addr_port);
    }
    return instance;
  }

  ClientPayload(ClientPayload const &) = delete;
  void operator=(ClientPayload const &) = delete;

  bool getLockStatus(void);
  bool hasReleased(void);
  void lockReleaseMechanism(void);
  bool releasePayload(void);
  void unlockReleaseMechanism(void);
};

#endif

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

#include <grpcpp/grpcpp.h>

#include <string>

#include "Poco/Logger.h"

#include "IPayload.grpc.pb.h"
#include "payload.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using Poco::Logger;

using namespace uav;

// This class the IPayload GRPC interface
class PayloadServiceImplementation final : public Payload::Service {
public:
  virtual ~PayloadServiceImplementation() {}
  PayloadServiceImplementation(Logger *log);

private:
  Logger *log_ptr;
  virtual Status getLockStatus(ServerContext *context,
                               const ::google::protobuf::Empty *request,
                               ::google::protobuf::BoolValue *response);
  // boolean hasReleased( )
  virtual Status hasReleased(ServerContext *context,
                             const ::google::protobuf::Empty *request,
                             ::google::protobuf::BoolValue *response);
  // void lockReleaseMechanism( )
  virtual Status lockReleaseMechanism(ServerContext *context,
                                      const ::google::protobuf::Empty *request,
                                      ::google::protobuf::Empty *response);
  // boolean releasePayload( )
  virtual Status releasePayload(ServerContext *context,
                                const ::google::protobuf::Empty *request,
                                ::google::protobuf::BoolValue *response);
  // void unlockReleaseMechanism( )
  virtual Status
  unlockReleaseMechanism(ServerContext *context,
                         const ::google::protobuf::Empty *request,
                         ::google::protobuf::Empty *response);
};

void RunServer(Logger &logger, std::string server_address);

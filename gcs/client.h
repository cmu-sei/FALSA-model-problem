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


#include "IMissionManager.grpc.pb.h"
#include "IMissionManager.h"
#include <grpcpp/grpcpp.h>
#include <string>

#include "Poco/Logger.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace uav;
using Poco::Logger;

class GCSClient : public IMissionManager {
public:
  GCSClient(std::shared_ptr<Channel> channel)
      : stub_(MissionManager::NewStub(channel)) {}

  void setMissionParams(LatLonCoord destination, Time startTime, Time endTime,
                        double takeoffAltitude);

  void clearMissionParams(void);

  void abort(void);

  void takeOff(void);

private:
  std::unique_ptr<MissionManager::Stub> stub_;
};

void RunClient(Logger &logger, std::string client_addr_port, std::string cmd);

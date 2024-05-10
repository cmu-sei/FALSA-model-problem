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

#include "Poco/AutoPtr.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/Logger.h"
#include "Poco/SimpleFileChannel.h"
#include "Poco/SplitterChannel.h"
#include "Poco/Task.h"
#include "Poco/TaskManager.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/ServerApplication.h"

#include "portutils.h"

#include "client_guidance.h"
#include "client_payload.h"
#include "server_gcs.h"
#include "server_guidance.h"
#include "timer_util.h"

using Poco::AutoPtr;
using Poco::ConsoleChannel;
using Poco::DateTimeFormatter;
using Poco::Logger;
using Poco::SimpleFileChannel;
using Poco::SplitterChannel;
using Poco::Task;
using Poco::TaskManager;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

class ServerStatusTask : public Task {
public:
  ServerStatusTask(std::string srv_addr_port)
      : Task("MissionManagerAppServerStatusTask") {
    _logger.information("Server Status task starting");
    server_addr_port = srv_addr_port;
  }

  void runTask() {
    Application &app = Application::instance();
    RunServerStatus(_logger, server_addr_port); // Runs forever
    _logger.information("Exiting server status task");
  }

private:
  Logger &_logger = Logger::get("Application");
  std::string server_addr_port;
};

class ServerGcsTask : public Task {
public:
  ServerGcsTask(std::string srv_addr_port)
      : Task("MissionManagerAppServerGcsTask") {
    _logger.information("Server gcs task starting");
    server_addr_port = srv_addr_port;
  }

  void runTask() {
    Application &app = Application::instance();
    RunServerGcs(_logger, server_addr_port); // Runs forever
    _logger.information("Exiting server gcs task");
  }

private:
  Logger &_logger = Logger::get("Application");
  std::string server_addr_port;
};

class MissionManagerApp : public ServerApplication {
public:
  MissionManagerApp() : _helpRequested(false) {}

  ~MissionManagerApp() {}

protected:
  void initialize(Application &self) {
    loadConfiguration(); // load default configuration files, if present
    ServerApplication::initialize(self);
    AutoPtr<ConsoleChannel> pChannel(new ConsoleChannel);
    AutoPtr<SimpleFileChannel> pChannel2(new SimpleFileChannel);
    AutoPtr<SplitterChannel> pSC(new SplitterChannel);
    pChannel2->setProperty("path", "missionmanager.log");
    pChannel2->setProperty("rotation", "2 K");
    pSC->addChannel(pChannel);
    pSC->addChannel(pChannel2);
    logger().setChannel(pSC);
    logger().information("MissionManager app starting up");
    std::cout << "Logger Name: " << logger().name() << std::endl;
  }

  void uninitialize() {
    logger().information("MissionManager app shutting down");
    ServerApplication::uninitialize();
  }

  void defineOptions(OptionSet &options) {
    ServerApplication::defineOptions(options);

    options.addOption(
        Option("help", "h",
               "display help information on command line arguments")
            .required(false)
            .repeatable(false)
            .callback(OptionCallback<MissionManagerApp>(
                this, &MissionManagerApp::handleHelp)));
  }

  void handleHelp(const std::string &name, const std::string &value) {
    _helpRequested = true;
    displayHelp();
    stopOptionsProcessing();
  }

  void displayHelp() {
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader(
        "A sample server application that demonstrates some of the features of "
        "the Util::ServerApplication class.");
    helpFormatter.format(std::cout);
  }

  int main(const ArgVec &args) {
    if (!_helpRequested) {
      Ports ports("../configs/ports.cfg");

      TaskManager tm;

      std::string gcs_server_port = ports.getAddress("MISSIONMANAGER_GCS_PORT");
      std::cout << "Missionmanager gcs_server address: " << gcs_server_port
                << std::endl;

      std::string guidance_client_port = ports.getAddress("GUIDANCE_PORT");
      std::cout << "Missionmanager guidance_client address: "
                << guidance_client_port << "\n";

      std::string status_server_port =
          ports.getAddress("MISSIONMANAGER_STATUS_PORT");
      std::cout << "Missionmanager status_server address: "
                << status_server_port << "\n";

      std::string payload_client_port = ports.getAddress("PAYLOAD_PORT");
      std::cout << "Missionmanager payload_client address: "
                << payload_client_port << "\n";

      std::string assurancebrkr_client_port =
          ports.getAddress("ASSURANCEBRKR_PORT");
      std::cout << "Missionmanager assurancebrkr_client address: "
                << assurancebrkr_client_port << "\n";

      // Server threads
      tm.start(new ServerGcsTask(gcs_server_port));
      sleep(1); // Wait for first thread to be establsihed
      tm.start(new ServerStatusTask(status_server_port));

      // Periodic timer thread
      TimerUtil timerUtil(guidance_client_port, payload_client_port,
                          assurancebrkr_client_port);
      Timer *timer = timerUtil.launchTimer();

      waitForTerminationRequest();
      tm.cancelAll();
      tm.joinAll();
      delete timer;
    }
    return Application::EXIT_OK;
  }

private:
  bool _helpRequested;
};

// This is a substitute for the main program in C++
POCO_SERVER_MAIN(MissionManagerApp)

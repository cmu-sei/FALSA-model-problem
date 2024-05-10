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
#include "Poco/Semaphore.h"
#include "Poco/SimpleFileChannel.h"
#include "Poco/SplitterChannel.h"
#include "Poco/Task.h"
#include "Poco/TaskManager.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/ServerApplication.h"
#include "client.h"
#include "governor.h"
#include "server.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>

#include "portutils.h"

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

/*
 * Client thread class
 * RunClient() is the function that contains the logic for the client thread
 * The thread is used to send back status to the mission manager
 */

class ClientTask : public Task {
public:
  ClientTask(std::string clnt_addr_port) : Task("GuidanceAppClientTask") {
    _logger.information("Client task starting");
    client_addr_port = clnt_addr_port;
  }

  void runTask() {
    Application &app = Application::instance();
    RunClient(_logger, client_addr_port);
    _logger.information("Exiting client task");
  }

private:
  Logger &_logger = Logger::get("Application");
  std::string client_addr_port;
};

/*
 * Server thread class
 * RunServer() is the function that contains the logic for the server thread
 * The thread is used to process the GRPC messages that comply to the IGuidance
 * interface
 */
class ServerTask : public Task {
public:
  ServerTask(std::string srv_addr_port, Poco::Semaphore &server_ready_sem)
      : Task("GuidanceAppServerTask"), server_ready_sem(server_ready_sem) {
    _logger.information("Server task starting");
    server_addr_port = srv_addr_port;
  }

  void runTask() {
    Application &app = Application::instance();
    RunServer(_logger, server_addr_port, server_ready_sem); // Runs forever
    _logger.information("Exiting server task");
  }

private:
  Logger &_logger = Logger::get("Application");
  std::string server_addr_port;
  Poco::Semaphore &server_ready_sem; // to signal the main thread that it is
                                     // connected to an autopilot
};

/*
 * Reference Governor thread class
 * RunGovernor() is the function that contains the logic for the reference
 * governor thread The thread is used to calculate the intermediate waypoints
 */
class GovernorTask : public Task {
public:
  GovernorTask() : Task("GuidanceAppGovernorTask") {
    _logger.information("Governor task starting");
  }

  void runTask() {
    Application &app = Application::instance();
    RunGovernor(_logger); // Runs forever
    _logger.information("Exiting governor task");
  }

private:
  Logger &_logger = Logger::get("Application");
};

class GuidanceApp : public ServerApplication {
public:
  GuidanceApp() : _helpRequested(false) {}

  ~GuidanceApp() {}

protected:
  void initialize(Application &self) {
    loadConfiguration(); // load default configuration files, if present
    ServerApplication::initialize(self);
    AutoPtr<ConsoleChannel> pChannel(new ConsoleChannel);
    AutoPtr<SimpleFileChannel> pChannel2(new SimpleFileChannel);
    AutoPtr<SplitterChannel> pSC(new SplitterChannel);
    pChannel2->setProperty("path", "guidanceapp.log");
    pChannel2->setProperty("rotation", "10 K");
    pSC->addChannel(pChannel);
    pSC->addChannel(pChannel2);
    logger().setChannel(pSC);
    logger().information("Guidance app starting up");
    std::cout << "Logger Name: " << logger().name() << std::endl;
  }

  void uninitialize() {
    logger().information("Guidance app shutting down");
    ServerApplication::uninitialize();
  }

  void defineOptions(OptionSet &options) {
    ServerApplication::defineOptions(options);

    options.addOption(
        Option("help", "h",
               "display help information on command line arguments")
            .required(false)
            .repeatable(false)
            .callback(
                OptionCallback<GuidanceApp>(this, &GuidanceApp::handleHelp)));
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

      // Read the ports we need to use for the client and server threads
      Ports ports("../configs/ports.cfg");

      TaskManager tm;
      std::string server_addr_port = ports.getAddress("GUIDANCE_PORT");
      std::cout << "Guidance server address: " << server_addr_port << std::endl;

      std::string client_addr_port =
          ports.getAddress("MISSIONMANAGER_STATUS_PORT");
      std::cout << "Guidance client address: " << client_addr_port << std::endl;

      // ports.displayAllPorts();

      Poco::Semaphore server_ready_sem(0, 1);

      tm.start(new ServerTask(server_addr_port, server_ready_sem));

      // Wait for the server to be ready before starting the other tasks
      server_ready_sem.wait();
      tm.start(new ClientTask(client_addr_port));
      tm.start(new GovernorTask());

      waitForTerminationRequest();
      tm.cancelAll();
      tm.joinAll();
    }
    return Application::EXIT_OK;
  }

private:
  bool _helpRequested;
};

// This is a substitute for the main program in C++
POCO_SERVER_MAIN(GuidanceApp)

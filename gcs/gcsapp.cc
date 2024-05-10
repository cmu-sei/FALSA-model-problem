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
#include "client.h"
#include <fstream>
#include <iostream>
#include <sstream>
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

class ClientTask : public Task {
public:
  ClientTask(std::string clnt_addr_port, std::string c)
      : Task("GcsAppClientTask") {
    _logger.information("Client task starting");
    client_addr_port = clnt_addr_port;
    cmd = c;
  }
  void runTask() {
    Application &app = Application::instance();
    RunClient(_logger, client_addr_port, cmd);
    _logger.information("Exiting client task");
  }

private:
  Logger &_logger = Logger::get("Application");
  std::string client_addr_port;
  std::string cmd;
};

class GcsApp : public ServerApplication {
public:
  GcsApp() : _helpRequested(false) {}

  ~GcsApp() {}

protected:
  void initialize(Application &self) {
    loadConfiguration(); // load default configuration files, if present
    ServerApplication::initialize(self);
    AutoPtr<ConsoleChannel> pChannel(new ConsoleChannel);
    AutoPtr<SimpleFileChannel> pChannel2(new SimpleFileChannel);
    AutoPtr<SplitterChannel> pSC(new SplitterChannel);
    pChannel2->setProperty("path", "gcsapp.log");
    pChannel2->setProperty("rotation", "2 K");
    pSC->addChannel(pChannel);
    pSC->addChannel(pChannel2);
    logger().setChannel(pSC);
    logger().information("Gcs app starting up");
    std::cout << "Logger Name: " << logger().name() << std::endl;
  }

  void uninitialize() {
    logger().information("Gcs app shutting down");
    ServerApplication::uninitialize();
  }

  void defineOptions(OptionSet &options) {
    ServerApplication::defineOptions(options);

    options.addOption(
        Option("help", "h",
               "display help information on command line arguments")
            .required(false)
            .repeatable(false)
            .callback(OptionCallback<GcsApp>(this, &GcsApp::handleHelp)));
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
      std::string client_addr_port =
          ports.getAddress("MISSIONMANAGER_GCS_PORT");

      if (args.size() != 1) {
        std::cout << "One argument is required! Exiting..." << std::endl;
        std::cout
            << "gcs setmissionparams | clearmissionparams | takeoff | abort"
            << std::endl;
        return Application::EXIT_USAGE;
      }
      std::string command = *args.begin();
      std::cout << command << std::endl;
      TaskManager tm;
      tm.start(new ClientTask(client_addr_port, command));
      tm.joinAll();
    }
    return Application::EXIT_OK;
  }

private:
  bool _helpRequested;
};

// This is a substitute for the main program in C++
POCO_SERVER_MAIN(GcsApp)

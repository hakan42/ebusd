/*
 * Copyright (C) Roland Jax 2014 <roland.jax@liwest.at>
 *
 * This file is part of ebusd.
 *
 * ebusd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ebusd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ebusd. If not, see http://www.gnu.org/licenses/.
 */

#include "libebus.hpp"
#include "logger.hpp"
#include "daemon.hpp"
#include "appl.hpp"
#include "network.hpp"
#include "ebusloop.hpp"
#include "cycdata.hpp"
#include "baseloop.hpp"
#include <iostream>
#include <memory>
#include <csignal>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace libebus;

Appl& A = Appl::Instance();
Daemon& D = Daemon::Instance();
LogDivider& L = LogDivider::Instance();

Network* network;
Commands* commands;
CYCData* cycdata;
EBusLoop* ebusloop;


void define_args()
{
	A.addItem("p_area", Appl::Param(All), "a", "area", "\tlogging area (Base=1, Comm=2, eBus=4, cycD=8, All=15)", Appl::type_int, Appl::opt_mandatory);
	A.addItem("p_level", Appl::Param(Debug), "l", "level", "\tlogging level (Error=1, Event=2, Trace=3, Debug=4)\n", Appl::type_int, Appl::opt_mandatory);
	A.addItem("p_foreground", Appl::Param(false), "f", "foreground", "run in foreground", Appl::type_bool, Appl::opt_none);
	A.addItem("p_localhost", Appl::Param(false), "", "localhost", "listen localhost only", Appl::type_bool, Appl::opt_none);
	A.addItem("p_port", Appl::Param(8888), "p", "port", "\tlisten port\n", Appl::type_int, Appl::opt_mandatory);
	A.addItem("p_help", Appl::Param(false), "h", "help", "\tprint this message", Appl::type_bool, Appl::opt_none);
}

void shutdown()
{
	// free Network
	if (network != NULL)
		delete network;

	// free CYCData
	if (cycdata != NULL)
		delete cycdata;

	// free EBusLoop
	if (ebusloop != NULL)
		delete ebusloop;
	
	// free Commands DB
	if (commands != NULL)
		delete commands;

	// reset all signal handlers to default
	signal(SIGHUP, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);

	// delete Daemon pid file
	if (D.status() == true)
		D.stop();

	// stop Logger
	L.log(Base, Event, "ebusd stopped");
	L.stop();

	exit(EXIT_SUCCESS);
}

void signal_handler(int sig)
{
	switch(sig) {
	case SIGHUP:
		L.log(Base, Event, "SIGHUP received");
		break;
	case SIGINT:
		L.log(Base, Event, "SIGINT received");
		shutdown();
		break;
	case SIGTERM:
		L.log(Base, Event, "SIGTERM received");
		shutdown();
		break;
	default:
		L.log(Base, Event, "undefined signal %s", strsignal(sig));
		break;
	}
}

int main(int argc, char* argv[])
{
	// define Arguments and Application variables
	define_args();

	// parse Arguments
	if (A.parse(argc, argv) == false) {
		A.printArgs();
		exit(EXIT_FAILURE);
	}
	
	// print Help
	if (A.getParam<bool>("p_help") == true) {
		A.printArgs();
		exit(EXIT_SUCCESS);
	}
	
	// make me Daemon
	if (A.getParam<bool>("p_foreground") == true) {
		L += new LogConsole(A.getParam<int>("p_area"), static_cast<const Level>(A.getParam<int>("p_level")), "logConsole");
	} else {
		D.run("/var/run/ebusd.pid");
		L += new LogFile(A.getParam<int>("p_area"), static_cast<const Level>(A.getParam<int>("p_level")), "logFile", "/tmp/test.txt");
	}

	// trap Signals that we expect to receive
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	// start Logger
	L.start("logDivider");
	L.log(Base, Event, "ebusd started");

	// print Daemon status
	if (D.status() == true)
		L.log(Base, Event, "change to daemon");

	// create Commands DB
	commands = ConfigCommands("contrib/csv/vaillant", CSV).getCommands();
	L.log(Base, Event, "commands DB created");

	// create EBusLoop
	ebusloop = new EBusLoop();
	ebusloop->start("ebusloop");

	// create CYCData
	cycdata = new CYCData();
	cycdata->start("cycdata");

	// create Network
	if (A.getParam<bool>("p_localhost") == true)
		network = new Network(A.getParam<int>("p_port"), "127.0.0.1");
	else
		network = new Network(A.getParam<int>("p_port"), "0.0.0.0");

	// create BaseLoop
	BaseLoop baseloop;
	network->addQueue(baseloop.getQueue());

	// start Network
	network->start("netListener");

	// start Baseloop
	baseloop.start();

	// search command
	//~ std::size_t index = commands->findCommand("get vr903 RaumTempSelfHeatingOffset");
	//~ L.log(Base, Event, "found at index: %d", index);

	shutdown();
}

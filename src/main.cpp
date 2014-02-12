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
#include <iostream>
#include <memory>
#include <csignal>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace libebus;

const char *progname;
Commands* commands;

LogDivider & L = LogDivider::Instance();

void shutdown()
{
	// free commands DB
	delete commands;

	// Reset all signal handlers to default
	signal(SIGHUP, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);

	// print end message and stop logger
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

int main(int argc, char *argv[])
{
	// set progname
	progname = (const char*) strrchr(argv[0], '/');
	progname = progname ? (progname + 1) : argv[0];

	// make me daemon
	Daemon daemon("/var/run/ebusd.pid");

	// Trap signals that we expect to receive
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	// start Logger
	//~ L += new LogConsole(Base | User, Error, "LogConsole");
	L += new LogFile(Base, Debug, "LogFile", "/tmp/test.txt");
	L.start("LogDivider");
	L.log(Base, Event, "ebusd started");

	// print info from daemon
	if (daemon.status())
		L.log(Base, Event, "change to daemon");

	// create commands DB
	commands = Config("vaillant", CSV).getCommands();
	L.log(Base, Event, "commands DB created");

	// search command
	//~ std::size_t index = commands->findCommand("get vr903 RaumTempSelfHeatingOffset");
	//~ L.log(Base, Event, "found at index: %d", index);
	
	for (int i = 0; i < 5; i++) {
		sleep(1);
		L.log(Base, Event, "Loop");
	}

	shutdown();

}

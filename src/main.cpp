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
#include <iostream>
#include <csignal>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace libebus;

const char *progname;

LogDivider & L = LogDivider::Instance();

std::string pidfile("/var/run/ebusd.pid");
static int pidfd = 0;

int pid_file_open(const char* file, int& fd)
{
	char pid[10];

	fd = open(file, O_RDWR|O_CREAT, 0600);
	if(fd < 0)
		return -1;

	if(lockf(fd, F_TLOCK, 0) < 0)
		return -1;

	sprintf(pid, "%d\n", getpid());
	if(write(fd, pid, strlen(pid)) < 0)
		return -1;

	return 0;
}

int pid_file_close(const char* file, const int fd)
{
	if(close(fd) < 0)
		return -1;

	if(remove(file) < 0)
		return -1;

	return 0;
}

void shutdown()
{
	// delete PID file
	pid_file_close(pidfile.c_str(), pidfd);

	// Reset all signal handlers to default
	signal(SIGCHLD, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
	signal(SIGTTIN, SIG_DFL);
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

void daemonize()
{
	pid_t pid;

	// fork off the parent process
	pid = fork();
	
	if (pid < 0) {
		std::cerr << "ebusd fork() failed." << std::endl;
		exit(EXIT_FAILURE);
	}

	// If we got a good PID, then we can exit the parent process
	if (pid > 0) {
		// printf("Child process created: %d\n", pid);
		exit(EXIT_SUCCESS);
	}

	// At this point we are executing as the child process

	// Set file permissions 750
	umask(027);

	// Create a new SID for the child process and
	// detach the process from the parent (normally a shell)
	if (setsid() < 0) {
		std::cerr << "ebusd setsid() failed." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Change the current working directory. This prevents the current
	// directory from being locked; hence not being able to remove it.
	if (chdir("/tmp") < 0) {  //DAEMON_WORKDIR
		std::cerr << "ebusd chdir() failed." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Close stdin, stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	// write pidfile and try to lock it
	if (pid_file_open(pidfile.c_str(), pidfd) == -1) {
		std::cerr << "can't open pidfile: %s" << pidfile.c_str() << std::endl;
		exit(EXIT_FAILURE);
	}

	// Cancel certain signals
	signal(SIGCHLD, SIG_DFL); // A child process dies
	signal(SIGTSTP, SIG_IGN); // Various TTY signals
	signal(SIGTTOU, SIG_IGN); // Ignore TTY background writes
	signal(SIGTTIN, SIG_IGN); // Ignore TTY background reads

	// Trap signals that we expect to receive
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
}

int main(int argc, char *argv[])
{
	// set progname
	progname = (const char *)strrchr(argv[0], '/');
	progname = progname ? (progname + 1) : argv[0];

	daemonize();
	
	//~ L += new LogConsole(Base | User, Error, "LogConsole");
	L += new LogFile(Base, Debug, "LogFile", "/tmp/test.txt");
	L.start("LogDivider");
	L.log(Base, Event, "ebusd started");

	while (1) {
		sleep(1);
		L.log(Base, Event, "Loop");
	}


	shutdown();

}

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
#include <sys/types.h>
#include <sys/stat.h>

using namespace libebus;

LogDivider & L = LogDivider::Instance();

void shutdown()
{
	//~ if (foreground == NO) {

	// delete PID file
	//~ if (pidfile_locked)
	//~ if (pid_file_close(pidfile, pidfd) == -1)
	//~ log_print(L_INF, "%s deleted", pidfile);

	// Reset all signal handlers to default
	signal(SIGCHLD, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
	signal(SIGTTIN, SIG_DFL);
	signal(SIGHUP, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);

	// print end message
	L.log(Base, Event, "ebusd stopped");
	//~ }	
	
	sleep(1);
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
	//~ if (pid_file_open(pidfile, &pidfd) == -1) {
		//~ log_print(L_ERR, "can't open pidfile: %s\n", pidfile);
		//~ cleanup(EXIT_FAILURE);
	//~ } else {
		//~ pidfile_locked = YES;
		//~ log_print(L_INF, "%s created.", pidfile);
	//~ }

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

int main ()
{
	
	daemonize();
	
	//~ L += new LogConsole(Base | User, Error, "LogConsole");
	L += new LogFile(Base, Debug, "LogFile", "/tmp/test.txt");
	
	L.start("LogDivider");

	while (1) {
		sleep(1);
		L.log(Base, Event, "Loop");
	}


	shutdown();
		
	return 0;
}

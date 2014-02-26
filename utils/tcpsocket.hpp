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

#ifndef TCPSOCKET_HPP__
#define TCPSOCKET_HPP__

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

class TCPSocket
{

public:
	friend class TCPListener;

	~TCPSocket() { close(m_sfd); }

	ssize_t recv(char* buffer, size_t len) { return read(m_sfd, buffer, len); }
	ssize_t send(const char* buffer, size_t len) { return write(m_sfd, buffer, len); }

	int getPort() const { return m_port; }
	std::string getIP() const { return m_ip; }

	int getFD() const { return m_sfd; }
	
private:
	int m_sfd;
	int m_port;
	std::string  m_ip;

	TCPSocket(int sd, struct sockaddr_in* address);

};

class TCPListener
{

public:
	TCPListener(int port, std::string address)
		: m_lfd(0), m_port(port), m_address(address), m_listening(false) {}

	~TCPListener() { if (m_lfd > 0) {close(m_lfd);} }

	int start();
	TCPSocket* newSocket();

	int getFD() const { return m_lfd; }

private:
	int m_lfd;
	int m_port;
	std::string m_address;
	bool m_listening;

};

#endif // TCPSOCKET_HPP__


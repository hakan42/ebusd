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

#include "socket.hpp"
#include <arpa/inet.h>
#include <string.h>

TCPSocket::~TCPSocket()
{
	close(m_sd);
}

ssize_t TCPSocket::recv(char* buffer, size_t len) 
{
	return ::read(m_sd, buffer, len);
}

ssize_t TCPSocket::send(const char* buffer, size_t len) 
{
	return ::write(m_sd, buffer, len);
}

TCPSocket::TCPSocket(int sd, struct sockaddr_in* address) : m_sd(sd)
{
	char ip[50];
	inet_ntop(PF_INET, (struct in_addr*)&(address->sin_addr.s_addr), ip, sizeof(ip)-1);
	m_ip = ip;
	m_port = ntohs(address->sin_port);
}



TCPListener::~TCPListener()
{
	if (m_lsd > 0)
		close(m_lsd);
}

int TCPListener::start()
{
	if (m_listening == true)
		return 0;

	m_lsd = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in address;

	memset(&address, 0, sizeof(address));
	
	address.sin_family = PF_INET;
	address.sin_port = htons(m_port);
    
	if (m_address.size() > 0)
		inet_pton(PF_INET, m_address.c_str(), &(address.sin_addr));
	else
		address.sin_addr.s_addr = INADDR_ANY;
    
	int optval = 1;
	setsockopt(m_lsd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); 
    
	int result = bind(m_lsd, (struct sockaddr*) &address, sizeof(address));
	if (result != 0) {
		//~ perror("bind() failed");
		return result;
	}
    
	result = listen(m_lsd, 5);
	if (result != 0) {
		//~ perror("listen() failed");
		return result;
	}
	
	m_listening = true;
	return result;
}

TCPSocket* TCPListener::accept() 
{
	if (m_listening == false)
		return NULL;

	struct sockaddr_in address;
	socklen_t len = sizeof(address);
	
	memset(&address, 0, sizeof(address));
	
	int sd = ::accept(m_lsd, (struct sockaddr*) &address, &len);
	
	if (sd < 0) {
		//~ perror("accept() failed");
		return NULL;
	}
	
	return new TCPSocket(sd, &address);
}

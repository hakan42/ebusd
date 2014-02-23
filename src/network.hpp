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

#ifndef NETWORK_HPP__
#define NETWORK_HPP__

#include "tcpsocket.hpp"
#include "wqueue.hpp"
#include "thread.hpp"

class ConnectionHandler : public Thread
{

public:
	ConnectionHandler(WQueue<TCPConnection*>& queue) : m_queue(queue) {}
 
	void* run();

private:
	WQueue<TCPConnection*>& m_queue;

};

class Network : public Thread
{

public:
	Network(int port, std::string ip, int maxConnections);
	
	void* run();

private:
	int m_port;
	std::string m_ip;
	int m_maxConnections;
	
	WQueue<TCPConnection*> m_queue;
	TCPListener* m_Listener;

};

#endif // NETWORK_HPP__

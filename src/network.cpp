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

#include "network.hpp"
//~ #include <iostream>
#include <sstream>
#include <sys/select.h>

void* TCPBroker::run()
{
	TCPConnection* connection = m_queue.remove();
	m_socket = connection->getSocket();

	m_running = true;
	
	int maxfd;
	fd_set checkfds;
	struct timeval timeout;

	FD_ZERO(&checkfds);
	FD_SET(m_notify.notifyFD(), &checkfds);
	FD_SET(m_socket->getFD(), &checkfds);
	
	(m_notify.notifyFD() > m_socket->getFD()) ?
		(maxfd = m_notify.notifyFD()) : (maxfd = m_socket->getFD());

	for (;;) {
		fd_set readfds;
		int ret;

		// set select timeout 10 secs
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;		

		// set readfds to inital checkfds
		readfds = checkfds;

		ret = select(maxfd + 1, &readfds, NULL, NULL, &timeout);
		if (ret == 0) {
			//~ std::cout << "timeout: " << timeout.tv_sec << " reached" << std::endl;
			continue;
		}
			
		// new data from notify
		if (FD_ISSET(m_notify.notifyFD(), &readfds)) {
			//~ std::cout << "notified" << std::endl;
			delete connection;
			break;
		}

		// new data from socket
		if (FD_ISSET(m_socket->getFD(), &readfds)) {
			char data[256];
			int datalen;
			
			datalen = m_socket->recv(data, sizeof(data)-1);
			
			// removed closed socket
			if (datalen <= 0) {
				delete connection;
				m_running = false;
				break;
			}
			
			data[datalen] = '\0';
			m_socket->send(data, datalen);
		}

	}

	return NULL;
}


Network::Network(int port, std::string ip) : m_listening(false)
{
	// Start Listener
	m_Listener = new TCPListener(port, ip.c_str());
	if (m_Listener && m_Listener->start() == 0)
		m_listening = true;

}

Network::~Network()
{
	while (!m_brokers.empty()) {
		TCPBroker* broker = m_brokers.back();
		m_brokers.pop_back();
		broker->stop();
		broker->join();
		delete broker;
	}
	delete m_Listener;
	sleep(1);
}

void* Network::run()
{
	if (!m_listening)
		return NULL;

	int i = 1;
	int maxfd;
	fd_set checkfds;
	struct timeval timeout;

	FD_ZERO(&checkfds);
	FD_SET(m_notify.notifyFD(), &checkfds);
	FD_SET(m_Listener->getFD(), &checkfds);
	
	(m_notify.notifyFD() > m_Listener->getFD()) ?
		(maxfd = m_notify.notifyFD()) : (maxfd = m_Listener->getFD());

	for (;;) {
		fd_set readfds;
		int ret;

		// set select timeout 1 secs
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;		

		// set readfds to inital checkfds
		readfds = checkfds;

		ret = select(maxfd + 1, &readfds, NULL, NULL, &timeout);
		if (ret == 0) {
			//~ std::cout << "timeout: " << timeout.tv_sec << " reached" << std::endl;
			wipeBrokers();
			continue;
		}
			
		// new data from notify
		if (FD_ISSET(m_notify.notifyFD(), &readfds)) {
			//~ std::cout << "notified" << std::endl;
			break;
		}

		// new data from socket
		if (FD_ISSET(m_Listener->getFD(), &readfds)) {
			TCPSocket* socket = m_Listener->newSocket();
			if (!socket)
				continue;
				
			TCPBroker* broker = new TCPBroker(m_queue);
			if (!broker)
				continue;

			std::ostringstream name;
			name << "Broker " << i++;
			broker->start(name.str().c_str());
			m_brokers.push_back(broker);
				
			TCPConnection* connection = new TCPConnection(socket);
			if (!connection)
				continue;
			
			m_queue.add(connection);
		}

	}

	return NULL;
}

void Network::wipeBrokers()
{
	std::list<TCPBroker*>::iterator b_it;
	for (b_it = m_brokers.begin(); b_it != m_brokers.end(); b_it++) {
		if (!(*b_it)->isRunning()) {
			TCPBroker* broker = *b_it;
			b_it = m_brokers.erase(b_it);
			delete broker;
		}
	}
}


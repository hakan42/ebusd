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
#include <sstream>

void* ConnectionHandler::run()
{

	for (int i = 0;; i++) {
		
		TCPConnection* connection = m_queue.remove();
		TCPSocket* socket = connection->getSocket();

		char data[256];
		int datalen;
		
		while ((datalen = socket->recv(data, sizeof(data)-1)) != 0) {
			data[datalen] = '\0';
			socket->send(data, datalen);
		}
		
		delete connection; 
	}

	return NULL;
}


Network::Network(int port, std::string ip, int maxConnections)
	: m_port(port), m_ip(ip), m_maxConnections(maxConnections)
{
	// Create ConnectionHandlers
	for (int i = 0; i < m_maxConnections; i++) {
		
		ConnectionHandler* handler = new ConnectionHandler(m_queue);
		if (!handler) {
		    //~ std::cout << "Could not create ConnectionHandler " << i << std::endl;
		    //~ exit(EXIT_FAILURE);
		}

		std::ostringstream name;
		name << "ConHandler " << i;
		handler->start(name.str().c_str());
		//~ std::cout << name.str().c_str() << " started." << std::endl;
	}

	// Start Listener
	m_Listener = new TCPListener(port, ip.c_str());
	if (!m_Listener || m_Listener->start() != 0) {
		//~ std::cout << "Could not create an connectionListener" << std::endl;
		//~ exit(EXIT_FAILURE);
	}
}

void* Network::run()
{
	TCPConnection* connection;
	
	while (1) {
		TCPSocket* socket = m_Listener->accept(); 
		if (!socket) {
			//~ std::cout << "Could not accept a connection" << std::endl;
			continue;
		}
		
		connection = new TCPConnection(socket);
		if (!connection) {
			//~ std::cout << "Could not open new connection" << std::endl;
			continue;
		}
		
		m_queue.add(connection);
	}
	
	return NULL;
}



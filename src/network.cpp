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

void* ConnectionHandler::run() {

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


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

#include "baseloop.hpp"
#include "logger.hpp"
#include "network.hpp"
#include <algorithm>
#include <sstream>
#include <unistd.h>

extern LogDivider& L;

Message::Message(std::string data, void* source)
	: m_data(data), m_source(source) {}

Message::Message(const Message& src)
	: m_data(src.m_data), m_source(src.m_source) {}

std::string Message::getData() const { return m_data.c_str(); }

void* Message::getSource() const { return m_source; }

void BaseLoop::start()
{
	for (;;) {
		Message* message = m_queue.remove();
		std::string data = message->getData();
		data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
		
		L.log(Base, Trace, "message: %s", data.c_str());

		// check command
		// ...

		// save result
		std::ostringstream result;
		result << data.substr(0, data.size()-1) << " done" << std::endl;

		Connection* connection = static_cast<Connection*>(message->getSource());
		connection->addResult(Message(result.str().c_str()));

		delete message;
	}
}


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

#ifndef BASE_HPP__
#define BASE_HPP__

#include "wqueue.hpp"
#include <string>

class Connection;

class Command
{

public:
	Command(const char* data, Connection* connection);

	const char* getData() const;
	Connection* getConnection() const;

private:
	const char* m_data;
	Connection* m_connection; 

};

class BaseLoop
{

public:
	void start();
	
	WQueue<Command*>* getQueue() { return &m_queue; }
	void addCommand(Command* command) { m_queue.add((command)); }

private:
	WQueue<Command*> m_queue;

};

#endif // BASE_HPP__

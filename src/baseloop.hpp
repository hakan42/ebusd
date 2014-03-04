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

class Message
{

public:
	Message(std::string data, void* source = NULL);
	Message(const Message& src);

	std::string getData() const;
	void* getSource() const;

private:
	std::string m_data;
	void* m_source; 

};

class BaseLoop
{

public:
	void start();
	
	WQueue<Message*>* getQueue() { return &m_queue; }
	void addMessage(Message* message) { m_queue.add((message)); }

private:
	WQueue<Message*> m_queue;

};

#endif // BASE_HPP__

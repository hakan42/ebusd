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

#ifndef CYCDATA_HPP__
#define CYCDATA_HPP__

//~ #include "wqueue.hpp"
#include "thread.hpp"
//~ #include <string>

class CYCData : public Thread
{

public:
	void* run();
	//~ void start();
	
	//~ WQueue<Telegram*>* getQueue() { return &m_queue; }
	//~ void addMessage(Telegram* telegram) { m_queue.add(telegram); }

private:
	//~ WQueue<Telegram*> m_queue;

};

#endif // CYCDATA_HPP__

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

#ifndef THREAD_HPP__
#define THREAD_HPP__

#include <pthread.h>

class Thread
{
   
public:
	Thread();
	virtual ~Thread();

	int start(const char * name);
	int join();
	int detach();
	pthread_t self();

	virtual void * run() = 0;

private:
	pthread_t m_threadid;
	bool m_running;
	bool m_detached;
	
};

#endif // THREAD_HPP__

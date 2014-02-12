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

#ifndef DAEMON_HPP__
#define DAEMON_HPP__

class Daemon
{
   
public:
	Daemon(const char* file);
	~Daemon() { pidfile_close(); }

	bool status() { return m_status; }

private:
	bool m_status;
	const char* m_pidfile;
	int m_pidfd;

	bool pidfile_open();
	bool pidfile_close();
	
};

#endif // DAEMON_HPP__

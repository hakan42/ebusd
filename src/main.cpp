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

#include "libebus.hpp"
#include "utils/logger.hpp"
//~ #include <iostream>
//~ #include <iomanip>

using namespace libebus;

int main (void)
{
	LogDivider & L = LogDivider::Instance();
	L += new LogConsole(Base | User, Error, "LogConsole");
	L += new LogFile(Base, Error, "LogFile", "./test.txt");

	L.start("LogDivider");

	L.log(Base, Error, "BASE ERROR %s", "abc");
	L.log(Base, Event, "BASE EVENT %s", "def");
	L.log(Base, Trace, "BASE TRACE %s", "ghi");
	L.log(Base, Debug, "BASE DEBUG %s", "jkl");
	
	L.log(Comm, Error, "COMM ERROR %s", "abc");
	L.log(Comm, Event, "COMM EVENT %s", "def");
	L.log(Comm, Trace, "COMM TRACE %s", "ghi");
	L.log(Comm, Debug, "COMM DEBUG %s", "jkl");
	
	L.log(User, Error, "USER ERROR %s", "abc");
	L.log(User, Event, "USER EVENT %s", "def");
	L.log(User, Trace, "USER TRACE %s", "ghi");
	L.log(User, Debug, "USER DEBUG %s", "jkl");


	L.stop();
	
	return 0;

}

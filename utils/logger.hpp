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

#ifndef LOGGER_HPP__
#define LOGGER_HPP__

#include "wqueue.hpp"
#include "thread.hpp"
#include <string>
#include <functional>
#include <algorithm>
#include <vector>
#include <cstdarg>

enum Area  { Base=0x01, Comm=0x02, User=0x04 };
enum Level { Error, Event, Trace, Debug };

class LogMessage
{
	
public:
	enum Status { Run, End };
	
	LogMessage(const Area area, const Level level, const std::string text, const Status status);
	
	~LogMessage() {}

	LogMessage(const LogMessage& src)
		: m_area(src.m_area), m_level(src.m_level), m_text(src.m_text),
		  m_status(src.m_status), m_time(src.m_time) {}

	void operator= (const LogMessage& src)
		{ m_area = src.m_area; m_level = src.m_level; m_text = src.m_text;
		  m_status = src.m_status; m_time = src.m_time; }
	
	Area getArea() const { return (m_area); }
	Level getLevel() const { return(m_level); }
	std::string getText() const { return (m_text.c_str()); }
	Status getStatus() const { return (m_status); }
	std::string getTime() const { return (m_time.c_str()); }

private:
	Area m_area;
	Level m_level;
	std::string m_text;
	Status m_status;
	std::string m_time;

};

enum Type { Console, Logfile };

class LogObserver : public Thread
{
	
public:
	LogObserver(const unsigned char areas, const Level level, const Type type, const char* name)
		: m_areas(areas), m_level(level), m_type(type), m_name(name) {}

	virtual ~LogObserver() {}
	
	void addMessage(const LogMessage& message);

	void* run();

	unsigned char getAreas() const { return (m_areas); }
	void setAreas(const unsigned char areas) { m_areas = areas; }
	
	Level getLevel() const { return (m_level); }
	void setLevel(const Level level) { m_level = level; }
	
	Type getType() const { return (m_type); }
	const char* getName() const { return (m_name.c_str()); }

protected:
	WQueue<LogMessage*> m_queue;

private:
	unsigned char m_areas;
	Level m_level;
	Type m_type;
	std::string m_name;
	
	virtual void Write(const LogMessage& message) const = 0;

};

class LogConsole : public LogObserver
{
	
public:
	LogConsole(const unsigned char areas, const Level level, const char* name)
		: LogObserver(areas, level, Console, name), m_instance(++m_numInstance)
		{ this->start(name); }

	~LogConsole() {}

private:
	const int m_instance;
	static int m_numInstance;

	void Write(const LogMessage& message) const;
	
};

class LogFile : public LogObserver
{
	
public:
	LogFile(const unsigned char areas, const Level level, const char* name, const char* filename)
		: LogObserver(areas, level, Logfile, name), m_filename(filename), m_instance(++m_numInstance)
		{ this->start(name); }

	~LogFile() {}
	
private:
	std::string m_filename;
	const int m_instance;
	static int m_numInstance;

	void Write(const LogMessage& message) const;
	
};

class LogDivider : public Thread
{
	
public:
	static LogDivider& Instance();

	~LogDivider();

	LogDivider& operator+= (LogObserver* Observer);
	LogDivider& operator-= (const LogObserver* Observer);

	char* format(const std::string& format, va_list arguments);
	void log(const Area area, const Level level, const char* text, ...);

	int getNumberOfObservers() const { return(m_Observer.size()); }
	LogObserver* getObserver(const int Index) const { return(m_Observer[Index]); }

	void* run();

	void stop();

private:	
	LogDivider() {}
	LogDivider(const LogDivider&);
	LogDivider& operator= (const LogDivider&);
	
	typedef std::vector<LogObserver*> obs_t;
	typedef std::vector<LogObserver*>::iterator obsCI_t;

	obs_t m_Observer;

	WQueue<LogMessage*> m_queue;

	bool m_running;

};

#endif // LOGGER_HPP__

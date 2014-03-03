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

#include "logger.hpp"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <sys/time.h>
#include <unistd.h>

static const char* AreaNames[Size_of_Area] = { "", "Base", "Conn", "User" };
static const char* LevelNames[Size_of_Level] = { "Error", "Event", "Trace", "Debug" };

LogMessage::LogMessage(const Area area, const Level level, const std::string text, const Status status)
	: m_area(area), m_level(level), m_text(text), m_status(status)
{
	char time[24];
	struct timeval tv;
	struct timezone tz;
	struct tm* tm;
	
	gettimeofday(&tv, &tz);
	tm = localtime(&tv.tv_sec);
	
	sprintf(&time[0], "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
		tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec/1000);

	m_time = std::string(time);
}



void LogObserver::addMessage(const LogMessage& message)
{
	LogMessage* tmp = new LogMessage(LogMessage(message));
	m_queue.add((tmp));
}

void* LogObserver::run()
{
	while (1) {
		LogMessage* message = m_queue.remove();
			if (message->getStatus() == LogMessage::End) {
				delete message;
				while (m_queue.size() == true) {
					LogMessage* message = m_queue.remove();
					Write(*message);
					delete message;		
				}
				return NULL;
			}
			
		Write(*message);
		delete message;
	}
	return NULL;
}



int LogConsole::m_numInstance = 0;

void LogConsole::Write(const LogMessage& message) const
{
	std::cout << message.getTime() << " ["
		  << AreaNames[message.getArea()] << " "
		  << LevelNames[message.getLevel()] << "] "
		  << message.getText() << std::endl;
}



int LogFile::m_numInstance = 0;

void LogFile::Write(const LogMessage& message) const
{
	std::fstream file(m_filename.c_str(), std::ios::out | std::ios::app);

	if (file.is_open() == true) {
		file << message.getTime() << " ["
		     << AreaNames[message.getArea()] << " "
		     << LevelNames[message.getLevel()] << "] "
		     << message.getText() << std::endl;
		file.close();
	}
}



LogDivider& LogDivider::Instance()
{
	static LogDivider instance;
	return (instance);
}

LogDivider::~LogDivider()
{
	while (m_Observer.empty() == false)
		*this -= *(m_Observer.begin());
}

LogDivider& LogDivider::operator+= (LogObserver* Observer)
{
	obsCI_t itEnd = m_Observer.end();
	obsCI_t it = std::find(m_Observer.begin(), itEnd, Observer);

	if (it == itEnd)
		m_Observer.push_back(Observer);

	return (*this);
}
	
LogDivider& LogDivider::operator-= (const LogObserver* Observer)
{
	obsCI_t itEnd = m_Observer.end();
	obsCI_t it = std::find(m_Observer.begin(), itEnd, Observer);

	if (it == itEnd)
		return (*this);

	m_Observer.erase(it);

	delete (Observer);

	return (*this);
}

char* LogDivider::format(const std::string& format, va_list arguments)
{
	int size = format.size() * 2;
	char* buffer = new char[size];

	while (vsprintf(buffer, format.c_str(), arguments) < 0) {
		delete[] buffer;
		size *= 2;
		buffer = new char[size];
	}

	return buffer;
}

void LogDivider::log(const Area area, const Level level, const char* text, ...)
{
	if (m_running == true) {
		va_list arguments;
		
		va_start(arguments, text);
		
		char* buffer = format(text, arguments);
		std::string string(buffer);
		delete[] buffer;
		
		va_end(arguments);
		
		m_queue.add(new LogMessage(LogMessage(area, level, string.c_str(), LogMessage::Run)));
	}

}

void* LogDivider::run()
{
	m_running = true;
	
	while (m_running == true) {
		LogMessage* message = m_queue.remove();

		obsCI_t iter = m_Observer.begin();
		
		for (; iter != m_Observer.end(); ++iter) {
			if (*iter != 0) {
			
				if (((*iter)->getAreas() & message->getArea()
				&& (*iter)->getLevel() >= message->getLevel())
				&& message->getStatus() == LogMessage::Run) {
					(*iter)->addMessage(*message);
				} else if (message->getStatus() == LogMessage::End) {
					(*iter)->addMessage(*message);
					m_running = false;
				}
					
			}
		}
		
		delete message;

	}
	return NULL;
}

void LogDivider::stop()
{
	m_queue.add(new LogMessage(LogMessage(Base, Error, "", LogMessage::End)));
	// TODO: Improve this method
	sleep(1);
}

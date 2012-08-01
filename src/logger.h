/*
 * logger.h
 *
 *  Created on: Apr 24, 2010
 *      Author: piersaj
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>

class LoggerJP {
private:
	long timeL;
	char stringTime[255];
	char fileName[255];

//	std::fstream str;

public:
	LoggerJP();
	LoggerJP(int ileNeuronow);

	LoggerJP & operator<<(int val);
	LoggerJP & operator<<(unsigned int val);
	LoggerJP & operator<<(long long val);
	LoggerJP & operator<<(long val);
	LoggerJP & operator<<(unsigned long val);
	LoggerJP & operator<<(float val);
	LoggerJP & operator<<(double val);
	LoggerJP & operator<<(const std::string val);
	LoggerJP & operator<<(const char * val);

	LoggerJP & operator<<(char val);
	LoggerJP & operator<<(bool val);

	LoggerJP & timestamp();
	LoggerJP & timestamp_nl();
};


void initLog();

extern LoggerJP logJP;
#endif /* LOGGER_H_ */

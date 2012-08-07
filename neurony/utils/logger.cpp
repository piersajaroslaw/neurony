/*
 * logger.cpp
 *
 *  Created on: Apr 24, 2010
 *      Author: piersaj
 */


#include <ctime>
#include <iostream>
#include "logger.h"


LoggerJP logJP;

std::_Ios_Openmode logJP_file_flags;

LoggerJP::LoggerJP(){
	if (1+1==2)
		return;
}	// this()

LoggerJP::LoggerJP(int ileNeuronow){
	logJP_file_flags = std::fstream::app | std::fstream::out;

	timeL = time(NULL);
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	char buff[80];
	strftime (buff, 80,"%Y_%m_%d_%H_%M_%S",timeinfo);
	sprintf(fileName, "./logNeurony_%s__%d_n.txt",buff, ileNeuronow);

	std::fstream str;
	str.open(fileName , logJP_file_flags );

	if (str == NULL){
		std::cerr<< "Logger() can't open file\n\n";
		return;
	}	// if

	str << fileName << "\n";
	str.close();

}	// this

LoggerJP & LoggerJP::operator<<(int val){
	std::cout << val;
	std::cout.flush();
	std::fstream str;
	str.open(fileName , logJP_file_flags );
	str << val;
	str.close();
	return (*this);
}	// operator<<


LoggerJP & LoggerJP::operator<<(unsigned int val){
	std::cout << val;
	std::cout.flush();
	std::fstream str;
	str.open(fileName , logJP_file_flags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

LoggerJP & LoggerJP::operator<<(long val){
	std::cout << val;
	std::cout.flush();
	std::fstream str;
	str.open(fileName , logJP_file_flags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

LoggerJP & LoggerJP::operator<<(long long val){
	std::cout << val;
	std::cout.flush();
	std::fstream str;
	str.open(fileName , logJP_file_flags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

LoggerJP & LoggerJP::operator<<(unsigned long val){
	std::cout << val;
	std::cout.flush();
	std::fstream str;
	str.open(fileName , logJP_file_flags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

LoggerJP & LoggerJP::operator<<(float val){
	std::cout << val;
	std::fstream str;
	str.open(fileName , logJP_file_flags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

LoggerJP & LoggerJP::operator<<(double val){
	std::cout << val;
	std::cout.flush();
	std::fstream str;
	str.open(fileName , logJP_file_flags );
	str.precision(6);
	str << val;
	str.precision(4);
	str.close();
	return (*this);
}	// operator<<

LoggerJP & LoggerJP::operator<<(const char * val){
	std::cout << val;
	std::cout.flush();
	std::fstream str;
	str.open(fileName , logJP_file_flags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

LoggerJP & LoggerJP::operator<<(const std::string val){
	std::cout << val;
	std::fstream str;
	str.open(fileName , logJP_file_flags );
	str << val.data();
	str.close();
	return (*this);
}	// operator<<

LoggerJP & LoggerJP::operator<<(char val){
	std::cout << val;
	std::cout.flush();
	std::fstream str;
	str.open(fileName , logJP_file_flags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

LoggerJP & LoggerJP::operator<<(bool val){
	std::cout << val;
	std::cout.flush();
	std::fstream str;
	str.open(fileName , logJP_file_flags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

LoggerJP & LoggerJP::timestamp(){
	char val[80];

	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	strftime (val, 80,"Log %Y-%m-%d-%H-%M-%S :",timeinfo);

	std::cout << val;
	std::cout.flush();
	std::fstream str;
	str.open(fileName , logJP_file_flags );
	str << val;
	str.close();
	return (*this);
}	// timestamp()

LoggerJP & LoggerJP::timestamp_nl(){
	char val[80];

	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	strftime (val, 80,"Log %Y-%m-%d-%H-%M-%S :\n",timeinfo);

	std::cout << val;
	std::cout.flush();
	std::fstream str;
	str.open(fileName , logJP_file_flags );
	str << val;
	str.close();
	return (*this);
}	// timestamp()

void initLog(){
	logJP = LoggerJP();
}	// initLog

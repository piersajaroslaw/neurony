/*
 * fileWriter.cpp
 *
 *  Created on: Feb 18, 2011
 *      Author: piersaj
 *
 * 2011-02-18
 * 		General purpose file appender,
 * 		just specify the path, hold a reference / pointer and print text
 * 		NOTE: it saves file after each print!
 * 2011-02-20
 * 		*** debugging a cyclic import
 *
 */
#include <iostream>

#include "fileWriter.h"

NNFileWrter::NNFileWrter(char * _pathname):
	LEN(255),
	fileName ((char *) _pathname),
	author((char *) "Jaroslaw Piersa"),
	lineCommentBegin((char *) "\% "),
	fileFlags (std::fstream::app | std::fstream::out),
	str_p (new std::fstream (fileName, fileFlags)) ,
	str( (*str_p))
{
	buff = new char[LEN];
	if (str_p == NULL){
		std::cerr<< "NNFW() can't open file\n\n";
		return;
	}	// if

}	// this()

NNFileWrter::NNFileWrter(const NNFileWrter & from):
	LEN(255),
	fileName ((char *) from.fileFlags),
	author((char *) from.author),
	lineCommentBegin((char *)from.lineCommentBegin),
	fileFlags ( from.fileFlags),
	str_p (new std::fstream (fileName, fileFlags)) ,
	str( (*str_p))
{
	std::cout<< "NNFW.this(NNFW& ) called!\n\n";
	buff = new char[LEN];
	if (str_p == NULL){
		std::cerr<< "NNFW() can't open file\n\n";
		return;
	}	// if
}	// this( copy) << konstruktor kopiujacy


NNFileWrter::~NNFileWrter(){
	str_p->close();
	delete str_p;
	delete buff;
}	// ~this()

NNFileWrter & NNFileWrter::operator<<(int val){
	str.open(fileName , fileFlags );
	str << val;
	str.close();
	return (*this);
}	// operator<<


NNFileWrter & NNFileWrter::operator<<(unsigned int val){
	str.open(fileName , fileFlags );
	str << val;
	str.close();
	return (*this);
}	// operator<<


NNFileWrter & NNFileWrter::operator<<(long val){
	str.open(fileName , fileFlags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

NNFileWrter & NNFileWrter::operator<<(unsigned long val){
	str.open(fileName , fileFlags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

NNFileWrter & NNFileWrter::operator<<(float val){
	str.open(fileName , fileFlags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

NNFileWrter & NNFileWrter::operator<<(double val){
	str.open(fileName , fileFlags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

NNFileWrter & NNFileWrter::operator<<(const char * val){
	str.open(fileName , fileFlags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

NNFileWrter & NNFileWrter::operator<<(const std::string val){
	str.open(fileName , fileFlags );
	str << val.data();
	str.close();
	return (*this);
}	// operator<<

NNFileWrter & NNFileWrter::operator<<(char val){
	str.open(fileName , fileFlags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

NNFileWrter & NNFileWrter::operator<<(bool val){
	str.open(fileName , fileFlags );
	str << val;
	str.close();
	return (*this);
}	// operator<<

NNFileWrter & NNFileWrter::timestamp(){
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	strftime(buff, LEN-1,"TS %Y-%m-%d-%H-%M-%S :", timeinfo);

	str.open(fileName , fileFlags );
	str << buff;
	str.close();

	return (*this);
}	// timestamp()

NNFileWrter & NNFileWrter::timestampNl(){
	timestamp().nl();
	return (*this);
}	// timestamp()


NNFileWrter & NNFileWrter::nl(){
	(*this) << '\n';
	return (*this);
}	// nl

NNFileWrter & NNFileWrter::comment(){
	(*this) << lineCommentBegin;
	return (*this);
}	// comment

NNFileWrter & NNFileWrter::printAuthor(){
	comment() << author << "\n";
	return (*this);
}	// printauthor

NNFileWrter & NNFileWrter::ind1(){
	(*this) << "  ";
	return (*this);
}	// indent

NNFileWrter & NNFileWrter::ind2(){
	(*this) << "    ";
	return (*this);
}	// indent

NNFileWrter & NNFileWrter::ind3(){
	(*this) << "      ";
	return (*this);
}	// indent


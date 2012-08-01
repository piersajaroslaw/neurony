#ifndef FILEWRITER_H_
#define FILEWRITER_H_

/*
 * fileWriter.h
 *
 *  Created on: Feb 19, 2011
 *      Author: piersaj
 */


#include <fstream>

class NNFileWrter {

private:
	int LEN;
	// timestamp
	long timeL;
	// formatted time
	char * stringTime;
	// fileName
	char * fileName;
	// buffer
	char * buff;


	// author
	char * author;
	// single line comments
	char * lineCommentBegin;

	std::_Ios_Openmode fileFlags;

	// stream pointer
	std::fstream * str_p;
	// stream reference for sanity purpose :P
	std::fstream & str;

public:

	NNFileWrter(char * _fileName);
	NNFileWrter(const NNFileWrter &);

	~NNFileWrter();


	// printing
	NNFileWrter & operator<<(int val);
	NNFileWrter & operator<<(unsigned int val);
	NNFileWrter & operator<<(long val);
	NNFileWrter & operator<<(unsigned long val);
	NNFileWrter & operator<<(float val);
	NNFileWrter & operator<<(double val);
	NNFileWrter & operator<<(const std::string val);
	NNFileWrter & operator<<(const char * val);
	NNFileWrter & operator<<(char val);
	NNFileWrter & operator<<(bool val);


	NNFileWrter&  operator=(const NNFileWrter&)	{
	   return *this;
	}	// =


	// timestamps
	NNFileWrter & timestamp();
	NNFileWrter & timestampNl();


	// newline
	NNFileWrter & printAuthor();
	NNFileWrter & nl();
	NNFileWrter & ind1();
	NNFileWrter & ind2();
	NNFileWrter & ind3();
	NNFileWrter & comment();

	/*
	 *  SETTERY I GETTERY
	 */

	inline char *getAuthor() const {
        return author;
    }

	inline char *getFileName() const {
        return fileName;
    }

	inline int getLEN() const {
        return LEN;
    }

	inline char *getLineCommentBegin() const{
        return lineCommentBegin;
    }

	inline char *getStringTime() const {
        return stringTime;
    }

	inline void setAuthor(char *author) {
        this->author = author;
    }

	inline void setLineCommentBegin(char *lineCommentBegin)   {
        this->lineCommentBegin = lineCommentBegin;
    }

};	// class

#endif /* FILEWRITER_H_ */


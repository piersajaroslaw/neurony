/*
 * NN_DegreeCorelationTester.h
 *
 *  Created on: Aug 3, 2012
 *      Author: piersaj
 *
 * aka Assortativity
 *
 * TODO:
 *  - assortativity = degree correlation
 *  - avg clustering vs degree
 *  - neighbourdegree vs degree
 *  - maybe other avg stats vs degree functions as well
 *
 *
 * degree of k say = <k>    or   d_k
 *
 * Assortativity =  E(X - EX)(Y-EY) / \sqrt(D^2X  D^2Y)
 *
 *
 *
 *  2012-08-03
 *  	Created
 *
 */

#ifndef NN_DEGREECORELATIONTESTER_H_
#define NN_DEGREECORELATIONTESTER_H_

#include "../../network/siecNeuronowa.h"


class NN_DegreeCorrelationTester{

private:
	SiecNeuronowa * n;

public:
	NN_DegreeCorrelationTester(SiecNeuronowa *);
	~NN_DegreeCorrelationTester();

	void calculateDegreeCorrelation();

private:
	void saveToMFile();

};	// class


#endif /* NN_DEGREECORELATIONTESTER_H_ */

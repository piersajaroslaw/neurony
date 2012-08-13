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
 * Corr(X,Y) =  E(X - EX)(Y-EY) / \sqrt(D^2X  D^2Y)
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
	int * degreeInFunctionalGraph;
	int * degreeVsAvgDegree_Deg;
	double * degreeVsAvgDegree_Avg;

	int edgesNum;

	std::fstream str;

	// TODO reconsider...
	// distribution od degrees
	// for each neuron n_i: ( deg(n_i), avg(deg(neighbours of n_i)))
	// and correlation of this data

public:
	NN_DegreeCorrelationTester(SiecNeuronowa *);
	~NN_DegreeCorrelationTester();




	void calculateDegreeCorrelation();

private:

	/*       1/M * \sum_i j_i * k*i  - (1/M 1/2 \sum_i (j_i + k_i) )^2
	 *  r = ----------------------------------------------------------
	 *       1/M 1/2 \sum_i (j_i^2 + k_i^2) - (1/M 1/2 \sum_i (j_i + k_i)^2 )^2
	 *
	 *   i = edge / sum over all edges
	 *   j_i, k_i = degree of the vertices incident to edge i (in functional graph)
	 *
	 */
	double calculatePearsonDegreeCorrelationCoefficient();

	double calculateDegVsAverageDegree();


	void recalculateDegreesInFunctionalGraph();

	void saveToMFile();

	void saveToMFile_generateHeader();
	void saveToMFile_generateData();
	void saveToMFile_generateFooter();


};	// class


#endif /* NN_DEGREECORELATIONTESTER_H_ */

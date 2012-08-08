/*
 * NN_DegreeCorrelationTester.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: piersaj
 */

#include "NN_DegreeCorrelationTester.h"


NN_DegreeCorrelationTester::NN_DegreeCorrelationTester(SiecNeuronowa * n_):
	n(n_)
{


	degreeInFunctionalGraph = NULL;
	recalculateDegreesInFunctionalGraph();
}	// this()


NN_DegreeCorrelationTester::~NN_DegreeCorrelationTester(){
	if (degreeInFunctionalGraph!= NULL){
		delete[] degreeInFunctionalGraph;
	}	// fi
}	// ~this()

void NN_DegreeCorrelationTester::recalculateDegreesInFunctionalGraph(){
	if (degreeInFunctionalGraph == NULL){
		degreeInFunctionalGraph = new int[n->getIloscNeuronow()];
		for (int i=0; i< n->getIloscNeuronow(); i++){
			degreeInFunctionalGraph[i] = 0;
		}	// for i
	}	// fi

	for (int i=0; i<n->getIloscKrawedzi(); i++){
		Polaczenie & e = n->getKrawedz(i);
		// if flow is above threshold value than incident vertices have increased degree
		// note,
		//ATM it is increase by +1, in multigraph case it should be by +|flow|
		if (e.getSumaLadunku()>= n->getConfig()->getKohonenChargeThreshold()){
			degreeInFunctionalGraph[e.getIndeks1()] += 1;
			degreeInFunctionalGraph[e.getIndeks2()] += 1;
		}	// fi
	}	// for all edges

}	// recalculateDegreesInFunctionalGraph()


double NN_DegreeCorrelationTester::calculatePearsonDegreeCorrelationCoefficient(){
	if (degreeInFunctionalGraph == NULL){
		recalculateDegreesInFunctionalGraph();
	}	// fi

	double m1 = 1.0 / n->getIloscKrawedzi();

	double sjk = 0, sj2k2=0, spjk = 0;
	// sum (j+k) ; sum (j^2 + k^2) ; sum j*p respectively

	int ij = -1, ik =-1;
	// index of j and k
	for (int i=0; i< n->getIloscNeuronow(); i++){
		Polaczenie & e = n->getKrawedz(i);
		if (e.getSumaLadunku()< n->getConfig()->getKohonenChargeThreshold() ){
			continue;
		}	// fi

		ij = e.getIndeks1();
		ik = e.getIndeks2();

		sjk += degreeInFunctionalGraph[ij] + degreeInFunctionalGraph[ik];
		sj2k2 += degreeInFunctionalGraph[ij] * degreeInFunctionalGraph[ij]
		         + degreeInFunctionalGraph[ik] * degreeInFunctionalGraph[ik];;
		spjk += degreeInFunctionalGraph[ij] * degreeInFunctionalGraph[ik];
	}	// for i

	// after Newman, Assortative Mixing in Networks
	// Physical Review Letters, 2002, Nov
	double ret = (m1 * spjk - m1 * m1 * 0.25 * sjk * sjk )
			/ (m1 *.5 * sj2k2 - m1 * m1 * 0.25 * sjk * sjk );

	logJP << "\nsjk = " << sjk << "\nsj2k2 = "<< sj2k2 <<"\nspjk = "<< spjk << "\n";

	return ret;
}	// calculatePearsonDegreeCorrelationCoefficient()


void NN_DegreeCorrelationTester::calculateDegreeCorrelation(){
	logJP.timestamp() << "  Calculating degree correlation = ";
	double corr = calculatePearsonDegreeCorrelationCoefficient();
	logJP<< corr << "\n" ;
}	// calculateDegreeCorrelation()

/*
 * NN_Failure_resiliency_path_length.h
 *
 *  Created on: Jul 11, 2012
 *      Author: piersaj
 *
 *
 * This class is to analyse the path length of the maximum component in the damaged network
 *
 *  2012-07-11
 *  	created, basic sketch of the class
 *  2012-07-12
 *  	added path length + some code
 *
 *
 */

#ifndef NN_FAILURE_RESILIENCY_PATH_LENGTH_H_
#define NN_FAILURE_RESILIENCY_PATH_LENGTH_H_

#include <pthread.h>

#include "../network/siecNeuronowa.h"
#include "NN_FailureResiliancyTester.h"
#include "NN_Failure_resiliency_path_len_thread.h"


void * FailureResileincyPathLength_threadStartupFunction(void *);


class NN_FailureResiliancyPathLength{
	SiecNeuronowa * n;
	NN_FailureResiliancyTester * frt;

	std::vector<int> neuronsInConnectedComponent;

	bool * isNeuronRemoved;
	int stepSkip, minStepskip, maxStepskip;

	double * pathLengthAtRandomFailure;
	double * pathLengthAtAttack;


	pthread_t * threads;
	FailureResiliencyPathLength_Thread ** threadObjects;



	std::fstream str;


public:
	NN_FailureResiliancyPathLength(SiecNeuronowa *n_, NN_FailureResiliancyTester * frt_);
	~NN_FailureResiliancyPathLength();


	void calculateAPL_FaultTolerance();




private:

	void saveToMfile();

	void  collectCalculationThreadsResults();


//	void calculateAPL_vs_Random_damage();
//	void calculateAPL_vs_Attack_damage();

//	void calculateAPL_vs_Attack_or_Random(bool random = true);


	void saveToMfile_generateHeader();
	void saveToMfile_writeData();
	void saveToMfile_generateFooter();

//	int calculatePathLengthBetween(int i1, int i2);
//	void removeNeuronFromConnectedComponentVector(int index);
//	void findConnectedComponentVector(int index);

//	double calculateAPL_MC(int iters);
};


#endif /* NN_FAILURE_RESILIENCY_PATH_LENGTH_H_ */

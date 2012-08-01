/*
 * NN_Failure_resiliency_path_len_thread.h
 *
 *  Created on: Jul 20, 2012
 *      Author: piersaj
 */

#ifndef NN_FAILURE_RESILIENCY_PATH_LEN_THREAD_H_
#define NN_FAILURE_RESILIENCY_PATH_LEN_THREAD_H_

#include"../network/siecNeuronowa.h"
#include "NN_FailureResiliancyTester.h"

class FailureResiliencyPathLength_Thread{
	SiecNeuronowa * n;
	NN_FailureResiliancyTester * frt;


	int tid;
	int t_num;
	int startingIndex;
	int endingIndex;



	std::vector<int> neuronsInConnectedComponent;

	bool * isNeuronRemoved;
	int stepSkip, minStepskip, maxStepskip;

	double * pathLengthAtRandomFailure;
	double * pathLengthAtAttack;

public:
	FailureResiliencyPathLength_Thread(SiecNeuronowa *n_, NN_FailureResiliancyTester * frt_,
		int tid_, int maxTid_, int startingVertex_, int endingVertex_, int stepskip_);

	~FailureResiliencyPathLength_Thread();

	void run();

	double getAplAtRandomDamage(int i);
	double getAplAtAttack(int i);

private:

	void calculateAPL_vs_Attack_or_Random(bool flagRandom);

	int calculatePathLengthBetween(int i1, int i2);
	void findConnectedComponentVector(int index);
	double calculateAPL_MC(int iters);
};	// class


#endif /* NN_FAILURE_RESILIENCY_PATH_LEN_THREAD_H_ */

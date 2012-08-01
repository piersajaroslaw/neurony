/*
 * GraphErPaths.h
 *
 *  Created on: Jul 14, 2011
 *      Author: piersaj
 *
 *  to be clear - this class runs Dijkstra for ErGraph, instead SiecNeuronowa,
 *  and leaves no logs
 *
 * 2011-07-14
 * 		created / copied
 */

#ifndef GRAPHERPATHS_H_
#define GRAPHERPATHS_H_

#include "../../GraphER.h"
#include "GraphErDijkstra.h"

class GraphErPaths{

private:

	int NUM_THREADS;
	GraphER * graph;

	pthread_t * threads;
	GraphErDijkstra ** threadObjects;

	// length of the distances table
	int pldlen;
	// distances table
	long long * pldTab;

public:
	GraphErPaths(GraphER *, int threads);
	~GraphErPaths();

	void calculatePldDijkstra();
	double calculateAPL();


private:
//	void calculateAPLDijkstra();
//	void runCalculationThreads();
	void collectCalculationThreadsResults();
	int startingIndexForTid(int tid);
	int endingIndexForTid(int tid);

};	// class

#endif /* GRAPHERPATHS_H_ */

/*
 * SiecNeuronowaPldDijkstra.h
 *
 *  Created on: Mar 25, 2011
 *      Author: piersaj
 *
 *  2011-03-25
 *  	Yet another version of path length distribution
 *  	this time n times Dijkstra, which should be quite fast and accurate
 *  	to make this even better i am going to write it in threads :P
 *  2011-03-29
 *  	the dijkstra itself on threads has gone to another header file...
 *  	this one is now supposed to run x threads, join them, collect results
 *  	and print a plot (Octave)
 *  2011-03-30
 *  	added idndef sentry :P
 *		few fixes
 *	2011-04-06
 *		Implemented thread launchs
 *
 */

#ifndef SIECNEURONOWAPLDDIJKSTRA_H_
#define SIECNEURONOWAPLDDIJKSTRA_H_

#include <pthread.h>
#include "../../dynamics/siecNeuronowaDynamika.h"
#include "pldDijkstraThread.h"


class SiecNeuronowaPldDijkstra{
	SiecNeuronowaDynamika * n;

	pthread_t * threads;
	PldDijkstraThread ** threadObjects;

	// length of the distances table
	int pldlen;
	// distances table
	long long * pldTab;

public:
	SiecNeuronowaPldDijkstra(SiecNeuronowaDynamika *);
	~SiecNeuronowaPldDijkstra();

	void saveToFile(bool useTimestampInFilename = true);
	void calculatePldDijkstra();


private:
	void calculateAPL();
//	void runCalculationThreads();
	void collectCalculationThreadsResults();

	int startingIndexForTid(int tid);
	int endingIndexForTid(int tid);
};



#endif /* SIECNEURONOWAPLDDIJKSTRA_H_ */

/*
 * pldDijkstraThread.h
 *
 *  Created on: Mar 29, 2011
 *      Author: piersaj
 *
 * this class is to perform a dijkstra calculation(s) as a single thread
 * one thread = one object, along with registers, counters, locks, etc
 *
 * This should be able tpo be dividable in a way, that it doesn't collide with other threads
 * i hope so...
 *
 * 2011-03-29
 * 		created / refactored from another class
 * 2011-03-30
 * 		swaped array queue for a binary heap
 * 2011-03-31
 * 		dijskstra algorithm tests / fixes
 */

#ifndef PLDDIJKSTRATHREAD_H_
#define PLDDIJKSTRATHREAD_H_

#include "../../network/siecNeuronowa.h"
#include "../../utils/BinaryHeap.h"


/*
 */
class PldDijkstraThread{
	SiecNeuronowa * n;
	int tid;
	int t_num;
	int startingVertex;
	int endingVertex;
	long long * distances;
	BinaryHeap * queue;
	const int infinity;
	int chargeThreshold;

	long long  * distancesCount;
	int numberOfEntries;
public:
	PldDijkstraThread(SiecNeuronowa *n_, int tid_, int tid_max, int svertex, int evertex);
	~PldDijkstraThread();

	/* perform a calculation
	 * aka Dijkstra Algorithm + recording results
	 */
	void caclucate();
	/* perform a calculation
	 * aka Dijkstra Algorithm + recording results
	 *
	 * naming suggests the method should be launched in a body of a function launched by a pthread :P
	 */
	void run();

	/* number of shortest paths starting in given vertices range
	 * which have path length == d
	 */
	long long getNumberOfDistances(int d);

private:
	/* calculates Dijkstra algoritm for a given vertice!
	 */
	void calculatePathLengthsForNeuron(int i);
	/* builds a queue (binary heap) of startnig distances originating in
	 * starting distances
	 */
	void buildHeap(int startingVertex);
};

#endif /* PLDDIJKSTRATHREAD_H_ */

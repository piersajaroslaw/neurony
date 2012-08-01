/*
 * GraphErDijkstra.h
 *
 *  Created on: Jul 14, 2011
 *      Author: piersaj
 */

#ifndef GRAPHERDIJKSTRA_H_
#define GRAPHERDIJKSTRA_H_

#include "../../GraphER.h"
#include "../../../src/BinaryHeap.h"


/* This class is ctrl+c-ctrl+v'ed from Dijkstra calcluation thread.
 * Operates on sole graph, not embedded into a neural network
 *
 * 2011-07-14
 * 		created / copied
 *
 */
class GraphErDijkstra{
	GraphER* g;
	int tid;
	int t_num;
	int startingVertex;
	int endingVertex;
	long long * distances;
	BinaryHeap * queue;
	const int infinity;
	int chargeThreshold;

	long long * distancesCount;
	int numberOfEntries;
public:
	GraphErDijkstra( GraphER *g_, int tid_, int tid_max, int svertex, int evertex);
	~GraphErDijkstra();

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

#endif /* GRAPHERDIJKSTRA_H_ */

/*
 * GraphErPaths.cpp
 *
 *  Created on: Jul 14, 2011
 *      Author: piersaj
 *
 *
 */

#include "GraphErPaths.h"
#include <cstdlib>
#include <pthread.h>
#include "../../utils/logger.h"

void * threadStartupErGraphs(void *);

/* a class for calculating a path length distribution for general graph
 * copied form SN_PLD_Dijkstra
 *
 */
GraphErPaths::GraphErPaths(GraphER *g_, int num_threads):
	NUM_THREADS(num_threads),
	graph(g_)
{
	pldTab = NULL;

	threads = new pthread_t[NUM_THREADS];
	threadObjects = (GraphErDijkstra **) malloc(NUM_THREADS * sizeof(GraphErDijkstra *));


	for (int i=0; i<NUM_THREADS; i++){
		int start = startingIndexForTid(i);
		int stop = endingIndexForTid(i);
		threadObjects[i] = new GraphErDijkstra(graph, i, NUM_THREADS, start, stop);
//		logJP << "DijkstraThr #"<< i << " at nodes  "
//					<< start << " .. "<< stop <<"\n";
	}	// for
}	// this

GraphErPaths::~GraphErPaths(){
	delete[] threads;
	threads = NULL;
	for (int i=0; i<NUM_THREADS; i++)
		delete threadObjects[i];

	if (pldTab !=NULL){
		delete[] pldTab;
		pldTab = NULL;
	}


	free(threadObjects);
	threadObjects = NULL;
}	// this


/* preforms calculation of PLD (n times Dijkstra)
 * using threads
 * also saves results to a file
 */
void GraphErPaths::calculatePldDijkstra(){


	logJP.timestamp_nl()
			<<"GraphErPaths: starting with "<< NUM_THREADS << "threads\n";
	for(long t=0; t<NUM_THREADS; t++){
		int rc = pthread_create(&threads[t], NULL, threadStartupErGraphs, (void *) threadObjects[t]);
		if (rc){
			std::cerr << "GraphERPaths.calculatePldDijkstra(): ERROR return code from pthread_create() is "
					<< rc <<"\n";
			return;
		}	// if

	}	// for

	for(long t=0; t<NUM_THREADS; t++){
		pthread_join(threads[t], NULL);
	}	// for

	collectCalculationThreadsResults();
	calculateAPL();

}	// calculatePldDijkstra()




/* this local function is used to run
 * PldDijkstraThread.run();
 *
 * Do not use outside!
 */
void * threadStartupErGraphs(void * th){
	GraphErDijkstra * t = (GraphErDijkstra *) th;
	t->run();
	return NULL;
}	// threadStartup

/* returns a starting index of neuron of the block given for a specified tid
 *
 * Note the ranges:
 * -  tids = 0...numThread-1
 * -  indices = 0...n->neuronsNumber()-1
 */
int GraphErPaths::startingIndexForTid(int tid){
	int max = graph->getNeuronsCount();
	int basePerNeuron = max / NUM_THREADS;
	int restOfNeurons = max - basePerNeuron * NUM_THREADS;
	int min = tid < restOfNeurons? tid : restOfNeurons;

	return tid * basePerNeuron + min;
}	// startingIndexForTid()


/* returns a ending index (== including last index) of neuron of the block given for a specified tid
 *
 * Note the ranges:
 * -  tids = 0...numThread-1
 * -  indices = 0...n->neuronsNumber()-1
 */
int GraphErPaths::endingIndexForTid(int tid){
	int max = graph->getNeuronsCount();
	int basePerNeuron = max / NUM_THREADS;
	int restOfNeurons = max - basePerNeuron * NUM_THREADS;
	int min = (tid+1) < restOfNeurons? (tid+1) : restOfNeurons;

	return (tid+1) * basePerNeuron + min -1;
}	// startingIndexForTid()


/* collects results of caculations of the threads
 */
void GraphErPaths::collectCalculationThreadsResults(){
	pldlen = 100;
	pldTab = new long long[pldlen];

	for (int i=0; i<pldlen; i++){
		pldTab[i] = 0;
	}	// for

	for (int t=0; t<NUM_THREADS; t++){
		for (int i=0; i<pldlen; i++){
			pldTab[i] += threadObjects[t]->getNumberOfDistances(i);
		}	// for
	}	// for t


	for (int t=0; t<10; t++){
		logJP << "pld["<< t<< "] = "<< pldTab[t] <<"\n";
	}	// for t


}	// collectCalculationThreadsResults

double GraphErPaths::calculateAPL(){
	if (pldTab == NULL){
		return -1;
	}	// fi

	// this can be crush on really large graphs
	long double s = 0;
	for (int i=0; i< pldlen; i++){
		s += (pldTab[i] / graph->getNeuronsCount() ) * i;
	}	// for

//	for (int i=0; i<pldlen; i++){
//		std::cout << "t[" << i << "] = "<< pldTab[i] << "\n";
//	}	// for

	double APL_real = ((double)s) / graph->getNeuronsCount();
	return APL_real;
}	// calculate APL

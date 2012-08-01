/*
 * GraphErDijkstra.cpp
 *
 *  Created on: Jul 14, 2011
 *      Author: piersaj
 */

#include "GraphErDijkstra.h"
#include "../../../src/logger.h"

/* This class is ctrl+c-ctrl+v'ed from Dijkstra calcluation thread.
 * Operates on sole graph, not embedded into a neural network
 *
 * 2011-07-15
 * 		created / copied
 *
 */
GraphErDijkstra::GraphErDijkstra(GraphER *g_, int tid_, int t_num_, int svertex_, int evertex_):
	g(g_),
	tid(tid_),
	t_num(t_num_),
	startingVertex(svertex_),
	endingVertex(evertex_),
	// should be enough :P
	infinity(g->getNeuronsCount()+100),
	chargeThreshold(1)
{
	queue = new BinaryHeap(g->getNeuronsCount()+10);
	distances = new long long [g->getNeuronsCount()];

	distancesCount = new long long[g->getNeuronsCount()];
	for (int i=0; i<g->getNeuronsCount(); i++){
		distancesCount[i] = 0;
	}	//
	numberOfEntries = 0;

}	// this()

void GraphErDijkstra::run(){
	this->caclucate();
}

void GraphErDijkstra::caclucate(){
	char *buff = new char[255];
	sprintf(buff, "GeaphERDijkstra starting thread [%d]\n", tid);
	logJP << buff;



	double sumDist=0;
	double sumCounts=0;

	for (int i=startingVertex; i<=endingVertex; i++){
//		if (i==startingVertex){
//			sprintf(buff, "calculating PL for starting neuron %d\n", startingVertex);
//			logJP << buff;
//		}

		calculatePathLengthsForNeuron(i);



//		if (i==endingVertex){
//			sprintf(buff, "calculating PL for starting neuron %d\n", endingVertex);
//			logJP << buff;
//		}

		for (int j=0; j<g->getNeuronsCount(); j++){
			sumDist += distances[j];
		}	// for
		sumCounts += g->getNeuronsCount()-1;
	}	// for

	delete buff;
}	// start

void GraphErDijkstra::calculatePathLengthsForNeuron(int index){
	char * buff = new char[255];

	/*
	 *
	 * STH is crushing in here...
	 *
	 */

	distances[startingVertex] = 0;


	bool * presentInQueue = new bool[g->getNeuronsCount()];
	for (int i=0; i<g->getNeuronsCount(); i++){
		presentInQueue[i] = true;
	}	// for


	buildHeap(startingVertex);


	int ik = -1, is = -1;
	while (queue->isEmpty()==false){
		int i1 = queue->popMinKey();
		presentInQueue[i1] = false;

		if (i1 > g->getNeuronsCount()){
			logJP << "Dijkstea.cplfn() error at index = " << index << "    i1 = " << i1<<"\n";
		}

		for (int i=0; i< g->getNeuron(i1).getIloscSasiadow(); i++){
			ik = g->getNeuron(i1).getIndeksKrawedzi(i);
			is = g->getIthNeighbourIndexOfNeuron(i1, i);

			// this is on ER no charge threshold!
			if ( presentInQueue[is] == true   /* && g->getEdge(ik).getSumaLadunku() >= chargeThreshold */){
				if (distances[is] > distances[i1]+1){
					distances[is] = distances[i1]+1;
					queue->changeWeigth(is, distances[i1]+1 );
				}	// if dist
			}	// if charge
		}	// for
	}	// while


	for (int i=0; i< g->getNeuronsCount(); i++){
		distancesCount[distances[i]]++;
	}	// for

	delete [] presentInQueue;
	delete [] buff;
}	// calculate for one neuron


/* builds a queue (binary heap) of startnig distances originating in
 * starting distances
 */
void GraphErDijkstra::buildHeap(int start){
	int len = g->getNeuronsCount();
	int * keys = new int[len];
	int * wei = new int[len];

	for (int i=0; i<len; i++){
		// key = index of the vertice = 0..number of neurons
		keys[i] = i;
		// weigth = 0 if starting v,
		// weigth = \infty otherwise
		if (i==start){
			wei[i] = distances[i] =  0;
		} else{
			wei[i] = distances[i] =  infinity;
		}	// fi
	}	// for
	queue->buildHeap(len, keys, wei);
	delete [] keys;
	delete [] wei;
}	// build heap


long long GraphErDijkstra::getNumberOfDistances(int d){
	if (distancesCount == NULL){
		std::cerr<<"GraphER_Dij::getNumberOfDistances(): table is NULL\n";
		return -1;
	}	// fi

	if (d<0 || d >= g->getNeuronsCount()){
		std::cerr<<"GraphER_Dij::getNumberOfDistances(): key out of bounds " << d << "\n";
		return 0;
	}	// fi

	return distancesCount[d];
}	// getNumberofDistances



GraphErDijkstra::~GraphErDijkstra(){
	queue->clear();
	delete queue;
	delete [] distances;
	delete [] distancesCount;
}	// ~this()

/*
 * pldDijkstraThread.cpp
 *
 *  Created on: Mar 29, 2011
 *      Author: piersaj
 *
 *
 *  Wszystko się wywołuje w obrębie jednego wątku, na poziomie wyżej trzeba stworzyć z THREADS_NUM takich wątków i obiektów,
 *  podzielić i odpacać każdy wątek z osobnym obiektem!
 *  Why this description is in polish??
 *
 *
 *  2011-03-29
 *  	utworzenie / przeniesienie z innego pliku
 *  2011-03-30
 *  	added a priority queue
 *  2011-03-31
 *  	continue building / testing
 *  	bux fixes
 *  2011-04-06
 *  	run method added for thread sanity
 *  2011-07-14
 *  	now it only requires a SN instead of SND
 */
#include "pldDijkstraThread.h"
#include "../../utils/BinaryHeap.h"

PldDijkstraThread::PldDijkstraThread(SiecNeuronowa *n_, int tid_, int t_num_, int svertex_, int evertex_):
	n(n_),
	tid(tid_),
	t_num(t_num_),
	startingVertex(svertex_),
	endingVertex(evertex_),
	// should be enough :P
	infinity(n->getIloscNeuronow()+100),
	chargeThreshold(1)
{
	queue = new BinaryHeap(n->getIloscNeuronow()+10);
	distances = new long long [n->getIloscNeuronow()];



	distancesCount = new long long [n->getIloscNeuronow()];
	for (int i=0; i<n->getIloscNeuronow(); i++){
		distancesCount[i] = 0;
	}	//
	numberOfEntries = 0;

}	// this()

void PldDijkstraThread::run(){
	this->caclucate();
}

void PldDijkstraThread::caclucate(){
	double sumDist=0;
	double sumCounts=0;

	for (int i=startingVertex; i<=endingVertex; i++){
		calculatePathLengthsForNeuron(i);

		for (int j=0; j<n->getIloscNeuronow(); j++){
			sumDist += distances[j];
		}	// for
		sumCounts += n->getIloscNeuronow()-1;
	}	// for
}	// start

void PldDijkstraThread::calculatePathLengthsForNeuron(int index){
//	logJP << "Dijkstra started at node "<< index << " ";
	distances[startingVertex] = 0;

	bool * presentInQueue = new bool[n->getIloscNeuronow()];
	for (int i=0; i<n->getIloscNeuronow(); i++){
		presentInQueue[i] = true;
	}	// for

	buildHeap(startingVertex);

//	int num = 0;
	int ik = -1, is = -1;
	while (queue->isEmpty()==false){
		int i1 = queue->popMinKey();
		presentInQueue[i1] = false;

		for (int i=0; i< n->getNeuron(i1).getIloscSasiadow(); i++){
			ik = n->getNeuron(i1).getIndeksKrawedzi(i);
			is = n->getIndeksItegoSasiada(i1, i);
			if ( presentInQueue[is] == true   &&
					n->getKrawedz(ik).getSumaLadunku() >= chargeThreshold){
				if (distances[is] > distances[i1]+1){
					distances[is] = distances[i1]+1;
					queue->changeWeigth(is, distances[i1]+1 );
				}	// if dist
			}	// if charge
		}	// for
	}	// while

	for (int i=0; i< n->getIloscNeuronow(); i++){
		distancesCount[distances[i]]++;
	}	// for

	delete [] presentInQueue;
//	logJP << "... end\n";
}	// calculate for one neuron


/* builds a queue (binary heap) of startnig distances originating in
 * starting distances
 */
void PldDijkstraThread::buildHeap(int start){
	int len = n->getIloscNeuronow();
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


long long PldDijkstraThread::getNumberOfDistances(int d){
	if (distancesCount == NULL){
		logJP<<"PldDijkstraThread::getNumberOfDistances(): table is NULL\n";
		return -1;
	}	// fi

	if (d<0 || d >= n->getIloscNeuronow()){
		logJP<<"PldDijkstraThread::getNumberOfDistances(): key out of bounds " << d << "\n";
		return 0;
	}	// fi

	return distancesCount[d];
}	// getNumberofDistances



PldDijkstraThread::~PldDijkstraThread(){
	queue->clear();
	delete queue;
	delete [] distances;
	delete [] distancesCount;
}	// ~this()

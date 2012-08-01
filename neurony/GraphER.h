/*
 * GraphER.h
 *
 *  An abstract graph without geometrical nor NN  embedding.
 *  Maybe it will be part of nn architecture, as the SN class is far mote than the graph?
 *
 *  Think:
 *  	Extract a (sub) graph from NN as this
 *		TODO remove charge from the neurons / edges?
 *
 *
 *
 *  Created on: Jul 14, 2011
 *  Author: piersaj
 *
 *  2011-07-14
 *  	Created
 *
 */

#ifndef GRAPHER_H_
#define GRAPHER_H_

#include "network/neuron.h"
#include "network/polaczenie.h"

#include <vector>


class GraphER {
private:
	// zbiory neuronow i polaczen
	std::vector<Neuron> neurons;
	std::vector<Polaczenie> edges;


	int neuronsCount;
	int edgesCount;
	int charge;
	double prob;

public:
	GraphER(int neurons, double p,  int numThreads);

	Neuron & getNeuron(int i);
	Polaczenie& getEdge(int i);

	inline int getNeuronsCount(){
		return neuronsCount;
	}	// get neuronsCount

	inline int getEdgesCount(){
		return edgesCount;
	}	// get edgesCount




	// is there an edge between a pair of neuron
	bool isThereEdgeBetweenNeurons(int, int);
	int getEdgeIdBetweenNeurons(int, int);

	// funkcjonalnosc, ktora powinienem dopisac latatemu :P
	int getIthNeighbourIndexOfNeuron(int index, int i);

	Neuron & getIthNeighbourOfNeuron(int index, int i);


private:
	void fillEdges();
	void addEdge(int i, int j);

private:
	/*
	 * deprecated version of getEdgeIdBetweenNeurons(int, int)
	 */
	int getEdgeIdBetweenNeurons_OLD(int, int);
};	// class

#endif /* GRAPHER_H_ */

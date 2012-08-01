/*
 * NN_FailureResiliancyTester.h
 *
 *  Created on: Apr 11, 2012
 *      Author: piersaj
 *
 *  Analyser of the resiliency of the network
 *  Random / orchestrated attacks on the predefined fraction of nodes
 *  Check the stability of the network
 *
 *  As a part of Polish Ministry of Science Research Grant
 *  DEC-2011/01/N/ST6/01931
 *
 *
 *  2012-04-11
 *  	Created
 *
 */

#ifndef NN_FAILURERESILIANCYTESTER_H_
#define NN_FAILURERESILIANCYTESTER_H_

#include <fstream>
#include "../network/siecNeuronowa.h"


const int RESILIENCY_MODE_RANDOM = 0;
const int RESILIENCY_MODE_LARGEST_DEGREE = 1;
const int RESILIENCY_MODE_GREATEST_ACTIVITY = 2;

class NN_FailureResiliancyTester {
private:

	const int mode;

	SiecNeuronowa * n;
	float failureRate;

	int removedNodes;
	int removedEdges;
	int chargeThreshold;
	bool * isNodeRemoved;
	char * modeString;
	std::fstream str;


	// also they will allow us to replicate the test wit the same order of deletion / attack
	int * indexOfRemovedNode__Random;
	int * indexOfRemovedNode__Attack;
	int * indexOfLargestComponent__Random;
	int * indexOfLargestComponent__Attack;



public:
	NN_FailureResiliancyTester(SiecNeuronowa * n, float fraction, const int mode);
	~NN_FailureResiliancyTester();

	inline int getNumberOfRemovedNodes(){
		return removedNodes;
	}
	inline int getNumberOfRemovedEdges(){
		return removedEdges;
	}
	inline int getFailureMode(){
		return mode;
	}
	inline int getChargeThreshold(){
		return chargeThreshold;
	}
	void setChargeThreshold(int newThres){
		this -> chargeThreshold = newThres;
	}
	inline float getFailureRate(){
		return failureRate;
	}

	/*returns a table of length n->neurons_count with the order of neuron removal
	 *  -1 indicates end of the test
	 */
	inline int* getIndexOfRemovedNode_Random(){
		return indexOfRemovedNode__Random;
	}

	/*returns a table of length n->neurons_count with the order of neuron removal
	 *  -1 indicates end of the attack
	 */
	inline int* getIndexOfRemovedNode_Attack(){
		return indexOfRemovedNode__Attack;
	}

	/* returns the table with an index of neuron belonging to the largest component
	 * -1 indicates end of test
	 *
	 */
	inline int* getIndexOfLargestComponent_Random(){
		return indexOfLargestComponent__Random;
	}

	/* returns the table with an index of neuron belonging to the largest component
	 * -1 indicates end of test
	 *
	 */
	inline int* getIndexOfLargestComponent_Attack(){
		return indexOfLargestComponent__Attack;
	}

	void calculateResiliency();

private:
	/* removes one node at random
	 */
	int removeRandomNode();

	/* removes one node at random
	 */
	int removeHighestDegreeNode();

	/* recalculates min and max cluster sizes, writes tham into in-out arguments
	 * Also stores the index of a vertex in the largest component, so a DFS can be started directly
	 *
	 * some ugly BFS goes here :P
	 */
	void calculateMaxMinClusterSize(int & maxClusterSize, int & minClusterSize,  int & indexOfLargestCluster);


	/* performs a DFS (or BFS -- not certain :) from vertex index, marks visited nodes into array
	 * returns a component size,
	 *
	 * note: preferably visited == [false...false]
	 *
	 * may (and will) terminate prematurely if some nodes in analysed component are marked asvisited
	 */
	int runDfsAtVertexI(int index, bool * visited);

	// saving mfile footer and header
	void saveTo_M_File_generateHead();
	void saveTo_M_File_generateFooter();
};


#endif /* NN_FAILURERESILIANCYTESTER_H_ */

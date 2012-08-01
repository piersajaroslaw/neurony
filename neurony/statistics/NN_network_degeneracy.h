/*
 * NN_networks_degeneracy.h
 *
 *  Created on: Jul 17, 2012
 *      Author: piersaj
 *
 *  2012-07-17
 *  	created
 */

#ifndef NN_NETWORKS_DEGENERACY_H_
#define NN_NETWORKS_DEGENERACY_H_

#include "../network/siecNeuronowa.h"
#include <fstream>

/*
 * Network degeneracy = smallest k such that the networks has a k-degenerate
 *  (every subgraph has a node of degree <=k)
 *
 * Algorithm:
 * iteratively remove degree with lowest degrees.
 *
 * The connected component which remain are k-cores
 *
 * max k such that nonempty k-core exists is the degeneracy value.
 *
 *
 */


class NN_DegeneracyTester{
	SiecNeuronowa * n;


	bool * isVertexRemoved;
	int * currentDegree;

	int networkDegeneracy;
	int * degeneracyDistribution;
	int * corenness;
	std::fstream str;


public:
	NN_DegeneracyTester(SiecNeuronowa *);
	~NN_DegeneracyTester();

	int calculateDegeneracy();


private:

	int calculateDegeneracy_atCurrentSetup();
	int calculateDegeneracy_ofTheNetwork();

	// this function iteratively runs the graph removes all vertices of _remaining_ degree less than k (k excluded)
	// until nothing is removed
	void removeVerticesWithDegreeLowerThanK(int k);
	// this function runs the graph once and removes vertices of _remaining_ degree less than k (k excluded)
	int removeVerticesWithDegreeLowerThanK_1run(int k);
	bool isRemainingNetworkEmpty();

	void saveToMfile();

	void saveToMfile_generateHeader();
	void saveToMfile_writeData();
	void saveToMfile_generateFooter();
};	// class


#endif /* NN_NETWORKS_DEGENERACY_H_ */

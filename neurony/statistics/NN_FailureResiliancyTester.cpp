/*
 * NN_FailureResiliancyTester.cpp
 *
 *  Created on: Apr 11, 2012
 *      Author: piersaj
 *
 * 2012-04-11
 * 		created, constructor
 * 2012-04-13
 * 		implementations of dfs etc...
 * 2012-04-16
 * 		saving to m file - at least random version :))
 * 2012-07-11
 * 		Added a recording of the ndex in the largest component
 * 		APL analysis started... in another file
 */

#include <stack>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <fstream>
#include <iostream>

#include "NN_FailureResiliancyTester.h"
#include "../../src/logger.h"
/*
 */

NN_FailureResiliancyTester::NN_FailureResiliancyTester(SiecNeuronowa *n_, float fraction_, int mode_):
	mode(mode_),
	n(n_),
	failureRate(fraction_)
{

//	if (mode == RESILIENCY_MODE_RANDOM){
//		modeString = (char*) "Random_failure";
//	} else if (mode == RESILIENCY_MODE_LARGEST_DEGREE) {
//		modeString = (char*)"Largest_degree_nodes";
//	} else /*  remaing condition */{
//		modeString = (char*)"UNK_(possible_bug)";
//	}	// if else


	isNodeRemoved = new bool[n->getIloscNeuronow()];

	chargeThreshold = n->getConfig()->getKohonenChargeThreshold();
	removedEdges = 0;
	removedNodes = 0;


	indexOfRemovedNode__Random = new int[n->getIloscNeuronow()];
	indexOfRemovedNode__Attack = new int[n->getIloscNeuronow()];
	indexOfLargestComponent__Random = new int[n->getIloscNeuronow()];
	indexOfLargestComponent__Attack = new int[n->getIloscNeuronow()];

	for (int i=0; i<n->getIloscNeuronow(); i++){
		isNodeRemoved[i] = false;
		indexOfRemovedNode__Random[i]=-1;
		indexOfRemovedNode__Attack[i]=-1;
		indexOfLargestComponent__Attack[i]=-1;
		indexOfLargestComponent__Random[i]=-1;
	}	// for
}	// this


NN_FailureResiliancyTester::~NN_FailureResiliancyTester(){
	delete[] isNodeRemoved;
	delete[] indexOfRemovedNode__Random;
	delete[] indexOfRemovedNode__Attack;
	delete[] indexOfLargestComponent__Random;
	delete[] indexOfLargestComponent__Attack;
}	// ~this


void NN_FailureResiliancyTester::calculateResiliency(){

	saveTo_M_File_generateHead();

	int maxClusterSize = n->getIloscNeuronow();
	int minClusterSize = maxClusterSize;
	int minComponentSize = 4;
	int indexOfMemberOfLargestComponent = 0;
	int counter = 0, lastRemoved = -1;

	removedNodes = removedEdges = 0;
	logJP.timestamp_nl() << "Resiliency: rand...";


	// RANDOM
	// repeat until all nodes are removed / hit max cluster == 1
	// 		remove one random node
	// 		calculate apl / max & min cluster size / connectedness
	while (maxClusterSize >=minComponentSize){
		lastRemoved = removeRandomNode();

		indexOfRemovedNode__Random[counter] = lastRemoved;



		calculateMaxMinClusterSize(maxClusterSize, minClusterSize, indexOfMemberOfLargestComponent);

		indexOfLargestComponent__Random[counter] = indexOfMemberOfLargestComponent;

		counter ++ ;

		// i, removed index, totalNodesRemoved (==i), totalEdgesRemoved, MaxComponent, MinComponent
		str << "  " << counter << ", "
			<< lastRemoved <<", "
			<< removedNodes << ", "
			<< removedEdges << ", "
			<< maxClusterSize << ", "
			<< minClusterSize << "\n";

	}	// while

	str << "];\n\n Y_ATT = [ \n";
	logJP << "done; attack...";



	for (int i=0; i<n->getIloscNeuronow(); i++){
		isNodeRemoved[i] = false;
	}	// fi
	removedEdges = removedNodes = 0;
	maxClusterSize = n->getIloscNeuronow();
	minClusterSize = maxClusterSize;
	counter = 0;
	lastRemoved = -1;

	// ATTACK
	// repeat until all nodes are removed / hit max cluster == 1
	// 		remove one best node
	// 		calculate apl / max & min cluster size / connectedness
	while (maxClusterSize >=minComponentSize){
		lastRemoved = removeHighestDegreeNode();


		indexOfRemovedNode__Attack[counter] = lastRemoved;

		calculateMaxMinClusterSize(maxClusterSize, minClusterSize, indexOfMemberOfLargestComponent);

		indexOfLargestComponent__Attack[counter] = indexOfMemberOfLargestComponent;

		counter ++ ;

		// i, removed index, totalNodesRemoved (==i), totalEdgesRemoved, MaxComponent, MinComponent
		str << "  " << counter << ", "
			<< lastRemoved <<", "
			<< removedNodes << ", "
			<< removedEdges << ", "
			<< maxClusterSize << ", "
			<< minClusterSize << "\n";

	}	// while

	logJP << "  done\n";
	saveTo_M_File_generateFooter();
	logJP.timestamp_nl();

}	// calculateResiliency()


/* removes one node at random
 * marks it in array
 * increases removedNodes and Edges
 *
 * if everything is removed does nothing
 *
 * returns removed index
 * Note: quite ugly in ending phases when most of things is removed...
 */
int NN_FailureResiliancyTester::removeRandomNode(){
	if (removedNodes >= n->getIloscNeuronow()){
		logJP << "NN_FRT.removeNode(): all nodes are already removed\n";
		return -1;
	}	// fi

	int ri = -1;
	do {
		ri = losujRInt(n->getIloscNeuronow());
	} while (isNodeRemoved[ri] == true);

	isNodeRemoved[ri] = true;
	removedNodes++;

	for (int i=0; i< (n->getNeuron(ri)).getIloscSasiadow() ; i++ ){
		int in = n->getItySasiad(ri, i).getIndeks();
		if (isNodeRemoved[in] == false){
			removedEdges++;
		}	// fi
	}	// for

	return ri;
}	// removeRandomNode()


int NN_FailureResiliancyTester::removeHighestDegreeNode(){
	int ret = -1;
	int maxNb = -1;

	int currentDeg = 0;
	int in =-1, ik = -1;


	for (int i=0; i< n->getIloscNeuronow(); i++){
		if (isNodeRemoved[i] == true){
			continue;
		}	// fi

		currentDeg = 0;

		// calculate number of valid not-removed neighbours
		for (int j=0; j<n->getNeuron(i).getIloscSasiadow(); j++){
			in = n->getIndeksItegoSasiada(i, j);

			// if neighbour removed
			if (isNodeRemoved[in] == true){
				continue;
			}	// fi

			// if edge _not_ included in spike-flow graph
			ik = n->getIndeksKrawedziMiedzyNeuronami(i, in);
			if (n->getKrawedz(ik).getSumaLadunku() < n->getConfig()->getKohonenChargeThreshold()){
				continue;
			}	// fi

			//now the neighbour is valid :)

			currentDeg ++;
		}	// for j / neighbours


		if (currentDeg > maxNb){
			maxNb = currentDeg;
			ret = i;
		}	// fi
	}	// for i

	isNodeRemoved[ret] = true;
	removedNodes++;

	for (int i=0; i< (n->getNeuron(ret)).getIloscSasiadow() ; i++ ){
		int in = n->getItySasiad(ret, i).getIndeks();
		if (isNodeRemoved[in] == false){
			removedEdges++;
		}	// fi
	}	// for

	// logJP << "removed node = "<< ret << " with (remaining) degree = "<< maxNb <<"\n";

	return ret;
}	// removeHighestDegreeNode

void NN_FailureResiliancyTester::calculateMaxMinClusterSize(int & maxClusterSize, int & minClusterSize, int & indexOfLargestCluster){
	int newMax = 0;
	int newMin = n->getIloscNeuronow();

	bool * visited = new bool[n->getIloscNeuronow()];
	for (int i=0; i< n->getIloscNeuronow(); i++){
		visited[i] = false;
	}	// for


	for (int i=0; i<n->getIloscNeuronow(); i++){
		if ( (visited[i] == false) && (isNodeRemoved[i] == false)){
			int size = runDfsAtVertexI(i, visited);
			// update sizes

			if (size > newMax){
				indexOfLargestCluster = i;
				newMax = size;
			}	// if
			newMin = size < newMin ? size : newMin;
		}	// fi
	}	// for

	maxClusterSize = newMax;
	minClusterSize = newMin;
}	// calculateMaxMinClusterSize()

int NN_FailureResiliancyTester::runDfsAtVertexI(int index, bool * visited){
	int size = 1;
	std::stack<int> BFSstack = std::stack<int>();
	BFSstack.push(index);
	visited[index] = true;

	while (BFSstack.empty() == false){
		// pushed on stack == visited
		int i1 = BFSstack.top();
		BFSstack.pop();

		// for all neighbours loop
		for (int i=0; i<(n->getNeuron(i1)).getIloscSasiadow(); i++){
			int in = n->getItySasiad(i1, i).getIndeks();

			// in not visited -- visit it
			if ((isNodeRemoved[in]==false) && (visited[in] == false)){
				int edgeCharge = n->getKrawedz( n->getIndeksKrawedziMiedzyNeuronami(in, i1)).getSumaLadunku();
				if (edgeCharge >= chargeThreshold){
					visited[in] = true;
					BFSstack.push(in);
					size++;
				}	// if edge is valid
			}	// if
		}	// for neighnours loop
	}	// while stack not empty loop

	return size;
}	// runDfsAtVertexI


void NN_FailureResiliancyTester::saveTo_M_File_generateHead(){
	char filename[255];

	if (n->getConfig()->getUseTimestampInFilenames() == true) {
		sprintf(filename, "%s/r_%s__%d_kn_%d_Me_%ldM_it_resiliency_test.m",
				n->getConfig()->getResultsFolder(),
				n->getTimestamp(),
				n->getIloscNeuronow()/1000,
				n->getIloscKrawedzi()/1000000,
				n->getIloscIteracji() / 1000000
				);
	} else {
		sprintf(filename, "%s/resiliency.m", n->getConfig()->getResultsFolder());
	}	// if .. else

	str.open(filename, std::fstream::out );

	if (str == NULL){
		std::cerr<< "NNFRT.save() str.open Error\n\n";
		return;
	}	// fi

//	if (mode == RESILIENCY_MODE_RANDOM){
//		str << "# mode RANDOM\n mode = \"RANDOM\"\n";
//	} else if (mode == RESILIENCY_MODE_LARGEST_DEGREE) {
//		// NYI
//		str << "# mode LARGEST_DEGREE\n mode = \"LARGEST_DEGREE\"\n";
//	} else if (mode == RESILIENCY_MODE_GREATEST_ACTIVITY ) {
//		// NYI
//		str << "# mode GREATEST_ACTIVITY\n mode = \"GREATEST_ACTIVITY\"\n";
//	}	else {
//		// WTF?
//		str << "# mode ??? (bug?)\n mode = \"??? (possible bug)\"\n";
//	}	// if ..  else

	str << "# timestamp  "<< n->getTimestamp() << '\n';
	str << "# neurons    "<< n->getIloscNeuronow() << '\n';
	str << "# edges      "<< n->getIloscKrawedzi() << '\n';
	str << "# geometry   "<< n->getTypString() << '\n';
	str << "# radius     "<< n->getMaxPos() << '\n';
	str << "# iterations "<< n->getIloscIteracji() << "\n\n";



//	str << "removedNodes = " << removedNodes << ";\n";
//	str << "removedEdges = " << removedEdges << ";\n\n";

	str << "% i, removed index, totalNodesRemoved (==i), totalEdgesRemoved, MaxComponent, MinComponent \n";
	str << "Y_RAND = [\n";

}	// saveTo_m_File()


void NN_FailureResiliancyTester::saveTo_M_File_generateFooter(){
	str << "]; % Ydata\n\n"

		<< "plot(Y_RAND(:,1), Y_RAND(:,5), \"b\", \"linewidth\", 2);\n"
		<< "hold on;\n"
		<< "plot(Y_ATT(:,1), Y_ATT(:,5), \"r\", \"linewidth\", 2);\n"
		<< "plot( [1,"<< n->getIloscNeuronow() <<"], ["<< n->getIloscNeuronow() <<",1], \"g\", \"linewidth\", 1.5);\n"
		<< "legend(\""<< "Max cluster size (random)\", \" Max cluster size (attack)\" ,\"Remaining nodes\", \"fontsize\", \"28\");\n"
//		<< "text(1,1, \"" << modeString << "\" );\n"
		<< "hold off;\n"
		<< "grid on;\n"
		<< "xlabel(\"Number of removed nodes\", \"fontsize\", 28);\n"
		<< "ylabel(\"Cluster size \", \"fontsize\", 28);\n";



//	time_t rawtime;
//	struct tm * timeinfo;
//	time( &rawtime );
//	timeinfo = localtime ( &rawtime );


	str << "% print( \"/home/piersaj/Desktop/nn/resilience_"<< modeString << "_"  << n->getTimestamp();

		char buffer[80];
		sprintf(buffer, "_%dk_%dM_%ldM---XXX.svg",
			n->getIloscNeuronow()/1000,
			n->getIloscKrawedzi()/1000000,
			n->getIloscIteracji() / 1000000
		);

		str << buffer << "\" , \"-color\"); \n\n";

	str.close();
}	// save to m file footer

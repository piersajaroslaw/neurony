/*
 * GraphER.cpp
 *
 *  Created on: Jul 14, 2011
 *      Author: piersaj
 */


#include "GraphER.h"

/* this()
 */
GraphER::GraphER(int n, double p, int numThreads):
	neuronsCount(n),
	charge(100),
	prob(p)
{

	double x=0,y=0,z=0;

	for (int i=0;i< neuronsCount; i++){
		Neuron n = Neuron(i, x, y, z, +1);
		neurons.push_back(n);
	}	// for i

	fillEdges();
}	// konstruktor()


/* thread-wise edges filing
 *
 */
void GraphER::fillEdges(){
	for (int i=0; i< neuronsCount; i++){
		for (int j=i; j<neuronsCount; j++){

			if (losujRBool(prob)){
				addEdge(i,j);
			}
		} 	// for j
	}	// for i
}	// fillEdges()

void GraphER::addEdge(int i, int j){
	edgesCount++;
	int ik = edgesCount-1;
	Polaczenie k =  Polaczenie(ik, i, j);

	k.zwiekszLadunekDo1(charge);
	k.zwiekszLadunekDo2(charge);
	edges.push_back(k);


	getNeuron(i).dodajIndeksKrawedzi(ik);
	getNeuron(j).dodajIndeksKrawedzi(ik);

}	// addEdge


Neuron& GraphER::getNeuron(int i){
	if (i<0 || i>neuronsCount){
		std::cerr << "GER::getNeuron(): wrong index i = " << i <<"\n";
	}	// fi

	return  neurons[i];
}	// getNeuron()

Polaczenie& GraphER::getEdge(int i){
	if (i<0 || i>= edgesCount){
			std::cerr << "GER::getEdge(): wrong index i = " << i <<"\n";
	}	// fi

	return edges[i];
}	// getKrawedz()


/* ij - neuron indices
 * binary search
 *
 * but requires the neighbour list to be sorted by id of second edge
 */
bool GraphER::isThereEdgeBetweenNeurons(int i, int j){
	if (getEdgeIdBetweenNeurons(i,j)>=0)
		return true;
	else
		return false;
}	// isThereEdgeBetweenNeurons


/* returns id of edfe between neurons of id i j
 * binary search
 *
 * but requires the neighbour list to be sorted by id of second edge
 *
 * if no such edge exist -1 is returned
 */
int GraphER::getEdgeIdBetweenNeurons_OLD(int i , int j ){
	if (i<0 || j<0 || i>= neuronsCount || j>=neuronsCount)
		return -1;


	int l=0;
	int p= getNeuron(i).getIloscSasiadow()-1;

	while (l<=p){
		int s = (l+p)/2;

		if (s<0){
			return -1;
		}
		if (s >= getNeuron(i).getIloscSasiadow()){
			return -1;
		}

		int i1 = getEdge( getNeuron(i).getIndeksKrawedzi(s)).getIndeks1();
		int i2 = getEdge( getNeuron(i).getIndeksKrawedzi(s)).getIndeks2();

		// ten drugi neuron
		int k = (i1 ==i ) ? i2 : i1;
		if (k == j)
			return getNeuron(i).getIndeksKrawedzi(s);

		if (k < j){
			l = s+1;
		} else {
			p = s-1;
		}	// if

	}	// while
	return -1;
} 	// getEdgeIdBetweenNeurons(int, int)


/* returns id of edfe between neurons of id i j
 * binary search
 *
 * this one does not require sorting the edges
 *
 * if no such edge exist -1 is returned
 */
int GraphER::getEdgeIdBetweenNeurons(int i1 , int i2 ){
	if (i1<0 || i2<0 || i1>= neuronsCount || i2>=neuronsCount)
		return -1;


	int adj_no= getNeuron(i1).getIloscSasiadow()-1;

	for (int i=0; i< adj_no; i++){
		int ik = getNeuron(i1).getIndeksKrawedzi(i);

		if (getEdge(ik).getIndeks1() == i2 || getEdge(ik).getIndeks2() == i2){
			return ik;
		}	// fi
	}	// for
	return -1;
} 	// getEdgeIdBetweenNeurons(int, int)

int GraphER::getIthNeighbourIndexOfNeuron(int i1, int i){
	int ik = getNeuron(i1).getIndeksKrawedzi(i);
	if (getEdge(ik).getIndeks1() == i1)
		return getEdge(ik).getIndeks2();
	else
		return getEdge(ik).getIndeks1();
}	// getIndeksItegoSasiada

Neuron & GraphER::getIthNeighbourOfNeuron(int index, int i){
	return getNeuron( getIthNeighbourIndexOfNeuron(index, i));
}

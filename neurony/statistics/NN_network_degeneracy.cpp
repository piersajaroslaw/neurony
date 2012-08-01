/*
 * NN_network_degeneracy.cpp
 *
 *  Created on: Jul 17, 2012
 *      Author: piersaj
 *
 *  2012-07-17
 *  	created
 *  2012-07-18
 *  	implemented counting of this coreness
 *  2012-07-19
 *  	testing
 *  	fixed very ugly bug :P I hope so...
 */




#include "NN_network_degeneracy.h"

#include "../../src/logger.h"

NN_DegeneracyTester::NN_DegeneracyTester(SiecNeuronowa * n_):
	n(n_)
{

	isVertexRemoved = new bool[n->getIloscNeuronow()];
	currentDegree = new int[n->getIloscNeuronow()];

	degeneracyDistribution = new int[n->getIloscNeuronow()];
	corenness = new int[n->getIloscNeuronow()];


	for (int i=0; i<n->getIloscNeuronow(); i++){
		isVertexRemoved[i] = false;
		currentDegree[i] = 0;
		degeneracyDistribution[i] = 0;
		corenness[i] = 0;
	}	// for

	networkDegeneracy = 0;

}	// this

NN_DegeneracyTester::~NN_DegeneracyTester(){
	delete[] currentDegree;
	delete[] isVertexRemoved;
	delete[] corenness;
	delete[] degeneracyDistribution;
}	// ~this

int NN_DegeneracyTester::calculateDegeneracy(){
	int val = calculateDegeneracy_ofTheNetwork();
	saveToMfile();
	return val;
}	// calcDegeneracy()

int  NN_DegeneracyTester::calculateDegeneracy_ofTheNetwork(){

	logJP.timestamp_nl() << "Degeneracy Test\n";

	networkDegeneracy = 0;

	// enable all vertices
	for (int i=0; i<n->getIloscNeuronow(); i++){
		isVertexRemoved[i] = false;
		corenness[i] =0;
		currentDegree[i] = 0;
	}	// for

	// init te edge degrees
	for (int i=0; i<n->getIloscNeuronow(); i++){
		for (int j=0; j<n->getNeuron(i).getIloscSasiadow(); j++){
			int ie = n->getNeuron(i).getIndeksKrawedzi(j);
			Polaczenie &e = n->getKrawedz(ie);

			if (e.getSumaLadunku() >= n->getConfig()->getKohonenChargeThreshold() ){
				currentDegree[i]++;
			}	// fi
		}	// for
	}	// for


	int core = 0;
	// loop
	bool someRunningFlag = true;
	while (someRunningFlag){
		core ++;

		removeVerticesWithDegreeLowerThanK(core);


		if (isRemainingNetworkEmpty()){
			someRunningFlag = false;
		}


		// update coreness
		for (int i=0; i<n->getIloscNeuronow(); i++){
			if (isVertexRemoved[i] == false){
				corenness[i] = core;
			}	// fi
		}	// for

	}	// while

	// debug printout
	//	for (int i=0; i<n->getIloscNeuronow(); i++){
	//		logJP << "coreness["<<i<< "] = " <<corenness[i] <<"\n";
	//	}	// for



	logJP << "network degeneracy = " << core <<"\n";

	networkDegeneracy = core;

	return core;
}	// calc_degeneracy

bool NN_DegeneracyTester::isRemainingNetworkEmpty(){
	for (int i=0; i<n->getIloscNeuronow(); i++){
		if (isVertexRemoved[i] == false){
			return false;
		}	// fi
	}	// for
	return true;
}	// isRemainingNetworkEmpty()



void NN_DegeneracyTester::removeVerticesWithDegreeLowerThanK(int k){
	int val = 0;

	do {
		val = removeVerticesWithDegreeLowerThanK_1run(k);
	} while (val>=1);

}	// removeVerticesWithDegreeLowerThanK(int k)


int NN_DegeneracyTester::removeVerticesWithDegreeLowerThanK_1run(int k){

//	int deg = 0;

	int numberOfRemovedNodes = 0;

	// for every neuron
	for (int i=0; i<n->getIloscNeuronow(); i++){

		if (currentDegree[i] < k && currentDegree[i] >=0 && isVertexRemoved[i] == false){
			isVertexRemoved[i] = true;
			currentDegree[i] = -10;
			numberOfRemovedNodes++;


//			deg = 0;

			// for all of its neighbours reduce their degree
			for (int j=0; j<n->getNeuron(i).getIloscSasiadow(); j++){
				int ie = n->getNeuron(i).getIndeksKrawedzi(j);
				Polaczenie &e = n->getKrawedz(ie);

				int in = (e.getIndeks1() != i ? e.getIndeks1() : e.getIndeks2());

				// if connected by valid edge
				if (e.getSumaLadunku() >= n->getConfig()->getKohonenChargeThreshold() && isVertexRemoved[in]==false){
					currentDegree[in]--;

					if (currentDegree[in]<0){
						logJP << "while removing neighbours: "<< in
								<< " has its deg <0  deg == " <<  currentDegree[in]<<"\n";
					}	// fi

				}	// fi





				// TODO cos tu nie do konca gra...

			}	// for
		}	// if
	}	// for


	// safety check
	// if Current degree <0 and  != -10
	for (int i=0; i<n->getIloscNeuronow(); i++){
		if (currentDegree[i] != -10 && currentDegree[i] < 0 ){
			// something is terribly wrong
			std::cerr << "NDegeneracy.removeVertices(): something is wrong with currentDegree["<< i
					<<"] = " <<currentDegree[i] <<"  isremoved[] = " << isVertexRemoved[i] <<  "!\n";
		}	// fi
	}	// for


	return numberOfRemovedNodes;
}	// removeVerticesWithDegreeLowerThanK(int k)





void NN_DegeneracyTester::saveToMfile(){
	char filename[255];

	if (n->getConfig()->getUseTimestampInFilenames() == true) {
		sprintf(filename, "%s/r_%s__%d_kn_%d_Me_%ldM_it_degeneracy_coreness.m",
				n->getConfig()->getResultsFolder(),
				n->getTimestamp(),
				n->getIloscNeuronow()/1000,
				n->getIloscKrawedzi()/1000000,
				n->getIloscIteracji() / 1000000
		);
	} else {
		sprintf(filename, "%s/degeneracy_coreness.m", n->getConfig()->getResultsFolder());
	}	// if .. else


	logJP.timestamp_nl() << "degeneracy_: path length: saving results to file "<< filename << " ... ";

	str.open(filename, std::fstream::out );

	if (str == NULL){
		logJP << "NN_Degeneracy.save() str.open Error\n\n";
		return;
	}	// fi


	saveToMfile_generateHeader();
	saveToMfile_writeData();
	saveToMfile_generateFooter();


	logJP <<" done\n";
}	// save to m file


void NN_DegeneracyTester::saveToMfile_generateHeader(){

	str << "# timestamp  "<< n->getTimestamp() << '\n';
	str << "# neurons    "<< n->getIloscNeuronow() << '\n';
	str << "# edges      "<< n->getIloscKrawedzi() << '\n';
	str << "# geometry   "<< n->getTypString() << '\n';
	str << "# radius     "<< n->getMaxPos() << '\n';
	str << "# iterations "<< n->getIloscIteracji() << "\n\n";

	str << "NEURONS = " << n->getIloscNeuronow() << ";\n";

}	// saveToMfile_generateHeader


void NN_DegeneracyTester::saveToMfile_writeData(){
	str << "\ncoreness = [\n";

	for (int i=0; i<n->getIloscNeuronow(); i++){
		str << "  " << corenness[i] <<";\n";
	}	// for

	str << "]; % coreness\n";


}	// saveToMfile_writeData


void NN_DegeneracyTester::saveToMfile_generateFooter(){

	str << "hold off;\n"
		<<  "hist(coreness, 20);\n"
		<< "grid on;\n"
		<< "xlabel(\"Coreness\", \"fontsize\", 24);\n"
		<< "ylabel(\"Counts\", \"fontsize\", 24);\n";


	str << "% print( \"/home/piersaj/Desktop/nn/coreness_histogram"<<  n->getTimestamp();

	char buffer[80];
	sprintf(buffer, "_%dk_%dM_%ldM_.svg",
		n->getIloscNeuronow()/1000,
		n->getIloscKrawedzi()/1000000,
		n->getIloscIteracji() / 1000000
	);

	str << buffer << "\" , \"-color\"); \n\n";

	str.close();
}	// saveToMfile_generateFooter()

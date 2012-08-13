/*
 * NN_Failure_resiliency_path_length.cpp
 *
 *  Created on: Jul 12, 2012
 *      Author: piersaj
 *
 *  2012-07-12
 *  	created,
 *  	written implementation of DFS (leasuring the path length),
 *  	written MC (average path length),
 *  	written sampling of nodes only in the connected component (i hope so)
 *  	testing
 *  2012-07-13
 *  	written attack mode to resiliency
 *  	written savineg of the results to m file
 *  	fixed few bugs...
 *  	... introduced a handful of another
 *  	heavy testing :P
 *  2012-07-14
 *  	Testing  + minor improvments
 *  	Attack and random tests rewritten into a single function
 *  2012-07-16
 *  	testing...
 *  2012-07-17
 *
 *	2012-07-20
 *		Splitting into  threads
 *
 *
 *
 *  	TODO - see at kohonen synapses saver - there are sum segfults on destructor...
 *
 */

#include <cstdlib>
#include "NN_Failure_resiliency_path_length.h"
#include "NN_Failure_resiliency_path_len_thread.h"


NN_FailureResiliancyPathLength::NN_FailureResiliancyPathLength(SiecNeuronowa *n_, NN_FailureResiliancyTester * frt_):
	n(n_),
	frt(frt_)

{

	NUM_THREADS = n->getConfig()->getThreads_number();


	stepSkip = 19;
	minStepskip = 2;
	maxStepskip = 50;

	pathLengthAtRandomFailure = new double[n->getIloscNeuronow()];
	pathLengthAtAttack = new double[n->getIloscNeuronow()];


	isNeuronRemoved = new bool[n->getIloscNeuronow()];
	for (int i=0; i<n->getIloscNeuronow(); ++i){
		isNeuronRemoved[i] = false;
		neuronsInConnectedComponent.push_back(i);
		pathLengthAtRandomFailure[i] = -1;
		pathLengthAtAttack[i] = -1;
	}	// for


	threads = new pthread_t[NUM_THREADS];
	threadObjects = (FailureResiliencyPathLength_Thread **) malloc(NUM_THREADS * sizeof(FailureResiliencyPathLength_Thread *));


	int expectedNumberOfTics = 100;

	for (int i=0; i<NUM_THREADS; i++){
		int stepskip = n->getIloscNeuronow() / expectedNumberOfTics;
		threadObjects[i] = new FailureResiliencyPathLength_Thread(n , frt, i, NUM_THREADS, 0, 0, stepskip);

	}	// for


}	// this(NN, NN_FRT, int x 4)


NN_FailureResiliancyPathLength::~NN_FailureResiliancyPathLength(){
	delete[] isNeuronRemoved;

	delete[] pathLengthAtAttack;
	delete[] pathLengthAtRandomFailure;
	neuronsInConnectedComponent.clear();

	for (int i=0; i<NUM_THREADS; i++)
		delete threadObjects[i];

	free(threadObjects);
}	// ~this()




void NN_FailureResiliancyPathLength::calculateAPL_FaultTolerance(){


	for(long t=0; t<NUM_THREADS; t++){
		int rc = pthread_create(&threads[t], NULL, FailureResileincyPathLength_threadStartupFunction, (void *) threadObjects[t]);
		if (rc){
			std::cerr << "FR_PL.calcAPL_FaultFolerance(): ERROR return code from pthread_create() is "
					<< rc <<"\n";
			return;
		}	// if
	}	// for
	std::cerr <<"\n";

	for(long t=0; t<NUM_THREADS; t++){
		pthread_join(threads[t], NULL);
	}	// for


	for(long t=0; t<NUM_THREADS; t++){
		for (int j=0; j< n->getIloscNeuronow(); j++){
			if (threadObjects[t]->getAplAtAttack(j) >0){
				pathLengthAtAttack[j] = threadObjects[t]->getAplAtAttack(j);
			}	// if

			if (threadObjects[t]->getAplAtRandomDamage(j) >0){
				pathLengthAtRandomFailure[j] = threadObjects[t]->getAplAtRandomDamage(j);
			}	// if

		}	// for
	}	// for

//	collectCalculationThreadsResults();

	saveToMfile();
}	// calculateAPL_FaultTolerance()


void NN_FailureResiliancyPathLength::saveToMfile(){
	char filename[255];

	if (n->getConfig()->getUseTimestampInFilenames() == true) {
		sprintf(filename, "%s/r_%s__%d_kn_%d_Me_%ldM_it_resiliency_path_length.m",
				n->getConfig()->getResultsFolder(),
				n->getTimestamp(),
				n->getIloscNeuronow()/1000,
				n->getIloscKrawedzi()/1000000,
				n->getIloscIteracji() / 1000000
		);
	} else {
		sprintf(filename, "%s/resiliency_path_length.m", n->getConfig()->getResultsFolder());
	}	// if .. else


	logJP.timestamp() << ": Failure resiliency: path length: saving results to file "<< filename << " ... ";

	str.open(filename, std::fstream::out );

	if (str == NULL){
		logJP << "NNFRPL.save() str.open Error\n\n";
		return;
	}	// fi


	saveToMfile_generateHeader();
	saveToMfile_writeData();
	saveToMfile_generateFooter();


	logJP <<" done\n";
}	// save to m file


void NN_FailureResiliancyPathLength::saveToMfile_generateHeader(){

	str << "# timestamp  "<< n->getTimestamp() << '\n';
	str << "# neurons    "<< n->getIloscNeuronow() << '\n';
	str << "# edges      "<< n->getIloscKrawedzi() << '\n';
	str << "# geometry   "<< n->getTypString() << '\n';
	str << "# radius     "<< n->getMaxPos() << '\n';
	str << "# iterations "<< n->getIloscIteracji() << "\n\n";


	str << "% number of removed nodes, apl in largest component on random removal \n";

	str << "NEURONS = " << n->getIloscNeuronow() << ";\n";

}	// saveToMfile_generateHeader


void NN_FailureResiliancyPathLength::saveToMfile_writeData(){
	str << "\nY_RAND = [\n";

	for (int i=0; i<n->getIloscNeuronow(); i++){
		if (pathLengthAtRandomFailure[i] >=0){
			str << "  " << i << " , " << pathLengthAtRandomFailure[i] <<";\n";
		} else {
			// continue
		}	// if .. else
	}	// for

	str << "]; % Y_RAND \n\n"
		<< "Y_ATTACK = [\n";

	for (int i=0; i<n->getIloscNeuronow(); i++){
		if (pathLengthAtAttack[i] >=0){
			str << "  " << i << " , " << pathLengthAtAttack[i] <<";\n";
		} else {
			// continue
		}	// if .. else
	}	// for
	str << "]; % Y_ATTACK\n\n";

	str.flush();
}	// saveToMfile_writeData


void NN_FailureResiliancyPathLength::saveToMfile_generateFooter(){

	str << "plot(Y_RAND(:,1), Y_RAND(:,2), \"bs\", \"markersize\", 5);\n"
		<< "hold on;\n"
		<< "plot(Y_ATTACK(:,1), Y_ATTACK(:,2), \"r*\", \"markersize\", 5);\n"
	//	<< "plot( [1,"<< n->getIloscNeuronow() <<"], ["<< n->getIloscNeuronow() <<",1], \"g\", \"linewidth\", 1.5);\n"
		<< "legend(\""<< "APL in largest cluster on random failures\", \" APL in largest cluster on attack\", \"fontsize\", \"28\");\n"
		<< "plot(Y_RAND(:,1), Y_RAND(:,2), \"b\", \"markersize\", 5);\n"
		<< "plot(Y_ATTACK(:,1), Y_ATTACK(:,2), \"r\", \"markersize\", 5);\n"
		<< "hold off;\n"
		<< "grid on;\n"
		<< "xlabel(\"Number of removed nodes\", \"fontsize\", 24);\n"
		<< "ylabel(\"Path length in largest cluster\", \"fontsize\", 24);\n";


	str << "% print( \"/home/piersaj/Desktop/nn/resilience_path_length_"<<  n->getTimestamp();

	char buffer[80];
	sprintf(buffer, "_%dk_%dM_%ldM_.svg",
		n->getIloscNeuronow()/1000,
		n->getIloscKrawedzi()/1000000,
		n->getIloscIteracji() / 1000000
	);

	str << buffer << "\" , \"-color\"); \n\n";

	str.close();
}	// saveToMfile_generateFooter()

/*
 * This function is used to start threads - fdo not touch!
 *
 */
void * FailureResileincyPathLength_threadStartupFunction(void * th){
	FailureResiliencyPathLength_Thread * t = (FailureResiliencyPathLength_Thread *) th;
	t->run();
	return NULL;
}	// thread startup


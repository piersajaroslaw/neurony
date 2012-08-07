/*
 * SiecNeuronowaPldDijkstra.cpp
 *
 *  Created on: Mar 29, 2011
 *      Author: piersaj
 *
 *
 *  2011-03-25
 *  	Yet another version of path length distribution
 *  	this time n times Dijkstra, which should be quite fast and accurate
 *  	to make this even better i am going to write it in threads :P
 *  2011-03-29
 *  	the dijkstra itself on threads has gone to another header file...
 *  	this one is now supposed to run x threads, join them, collect results
 *  	and print a plot (Octave)
 *  2011-03-30
 *  	added idndef sentry :P
 *		few fixes
 *	2011-04-06
 *		Implemented thread launchs
 *	2011-04-07
 *		saving to file / ctrl+c-and-ctrl+v'ed from another file
 */

#include "SiecNeuronowaPldDijkstra.h"
#include <cstdlib>
#include "ctime"
#include <pthread.h>
#include "GraphErPaths.h"

void * threadStartup(void *);

SiecNeuronowaPldDijkstra::SiecNeuronowaPldDijkstra(SiecNeuronowaDynamika *n_):
	n(n_)
{

	NUM_THREADS = n->getN().getConfig()->getThreads_number();
	logJP.timestamp_nl()
			<<"SiecNeuronowaPldDijkstra: starting with "<< NUM_THREADS << "threads\n";

	threads = new pthread_t[NUM_THREADS];
	threadObjects = (PldDijkstraThread **) malloc(NUM_THREADS * sizeof(PldDijkstraThread *));

//	logJP << "SiecNeuronowaPldDijkstra.this(): allocated "
//			<< NUM_THREADS * sizeof(PldDijkstraThread *)
//				<< " memory\n";

	for (int i=0; i<NUM_THREADS; i++){
		int start = startingIndexForTid(i);
		int stop = endingIndexForTid(i);
		threadObjects[i] = new PldDijkstraThread(&(n->getN()), i, NUM_THREADS, start, stop);
//		logJP << "DijkstraThr #"<< i << " at nodes  "
//				<< start << " .. "<< stop <<"\n";
	}	// for

}	// this

SiecNeuronowaPldDijkstra::~SiecNeuronowaPldDijkstra(){
	delete[] threads;
	threads = NULL;
	for (int i=0; i<NUM_THREADS; i++)
		delete threadObjects[i];

	free(threadObjects);
	threadObjects = NULL;
}	// this


/* preforms calculation of PLD (n times Dijkstra)
 * using threads
 * also saves results to a file
 */
void SiecNeuronowaPldDijkstra::calculatePldDijkstra(){

	for(long t=0; t<NUM_THREADS; t++){
		int rc = pthread_create(&threads[t], NULL, threadStartup, (void *) threadObjects[t]);
		if (rc){
			logJP << "SN_PldDijkstra.calculatePldDijkstra(): ERROR return code from pthread_create() is "
					<< rc <<"\n";
			return;
		}	// if
	}	// for
	logJP <<"\n";

	for(long t=0; t<NUM_THREADS; t++){
		pthread_join(threads[t], NULL);
	}	// for

	collectCalculationThreadsResults();

	calculateAPL();

	saveToFile(n->getN().getConfig()->getUseTimestampInFilenames());
	logJP.timestamp_nl()
			<<"SiecNeuronowaPldDijkstra: end\n\n";

}	// calculatePldDijkstra()



/* this local function is used to run
 * PldDijkstraThread.run();
 *
 * Do not use outside!
 */
void * threadStartup(void * th){
	PldDijkstraThread * t = (PldDijkstraThread *) th;
	t->run();
	return NULL;
}	// threadStartup

/* returns a starting index of neuron of the block given for a specified tid
 *
 * Note the ranges:
 * -  tids = 0...numThread-1
 * -  indices = 0...n->neuronsNumber()-1
 */
int SiecNeuronowaPldDijkstra::startingIndexForTid(int tid){
	int max = n->getIloscNeuronow();
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
int SiecNeuronowaPldDijkstra::endingIndexForTid(int tid){
	int max = n->getIloscNeuronow();
	int basePerNeuron = max / NUM_THREADS;
	int restOfNeurons = max - basePerNeuron * NUM_THREADS;
	int min = (tid+1) < restOfNeurons? (tid+1) : restOfNeurons;

	return (tid+1) * basePerNeuron + min -1;
}	// startingIndexForTid()


/* collects results of caculations of the threads
 */
void SiecNeuronowaPldDijkstra::collectCalculationThreadsResults(){
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

void SiecNeuronowaPldDijkstra::calculateAPL(){
	if (pldTab == NULL){
		logJP << "SNPldDijkstra::calculateAPL(): distances not ready!\n";
		return;
	}	// fi

	// this should no longer crush on large graphs
	long long s = 0;
	for (int i=0; i< pldlen; i++){
		s +=  i * pldTab[i];
	}	// for

	double APL_real =  (double) ((long double )s) / (n->getIloscNeuronow()) / ((n->getIloscNeuronow()));

	int edgesNumber = 0;
	for (int i=0; i< n->getIloscKrawedzi(); i++){
		if (n->getKrawedz(i).getSumaLadunku() >=1){
			edgesNumber ++;
		}	// fi
	}	// for

	double p = (edgesNumber * 2.0)  / n->getIloscNeuronow() /(n->getIloscNeuronow() -1.0);
	char buff[32];
	sprintf(buff, "%0.5lf", APL_real);

	logJP.timestamp_nl();
	logJP << "APL_TEO_connp =            = "<< p << "\n";
	logJP << "APL_real                   = "<<buff <<"\n";

	GraphER * gr = new GraphER(n->getIloscNeuronow(), p, NUM_THREADS);
	GraphErPaths * gr_apl = new GraphErPaths(gr, n->getN().getConfig()->getThreads_number());
	gr_apl->calculatePldDijkstra();


	double APL_teo = gr_apl->calculateAPL();
	sprintf(buff, "%0.5lf", APL_teo);
	logJP << "APL_teo                    = "<< buff << "\n";
	sprintf(buff, "%0.5lf", APL_real /  APL_teo);
	logJP << "lambda (aplReal / aplTeo)  = "<< buff <<"\n\n";

}	// calculate APL


void SiecNeuronowaPldDijkstra::saveToFile(bool withTimestamp){
	logJP << "SNPLDD.zapisWynikow().. ";

	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	char timestamp[100];
	strftime (timestamp,100,"%Y_%m_%d_%H_%M_%S",timeinfo);


	char filename[255];
	if (withTimestamp){
		// TODO add ts_flag
		sprintf(filename, "%s/path_length_distribution_%s__%d_K_n_%d_K_e_%ld_M_it.m",
			n->getN().getConfig()->getResultsFolder(),
			timestamp,
			n->getN().getIloscNeuronow() / 1000,
			n->getN().getIloscKrawedzi() / 1000,
			n->getIlIteracji() / 1000000
		); // sprintf
	} else {
		sprintf(filename, "%s/path_length_distribution.m",
				n->getN().getConfig()->getResultsFolder());
	}	// fi
	logJP << " results file = "<< filename << "\n";

	std::fstream str;
	str.open(filename, std::fstream::out );

	if (str.is_open() == false){
		logJP << "\nERROR: SNPLDD.saveToFile() str.fstream.open()\n";
		return;
	}	// fi

	str << "# timestamp   "<< timestamp << '\n';
	str << "# Il neuronow "<< n->getN().getIloscNeuronow() << '\n';
	str << "# Il krawedzi "<< n->getN().getIloscKrawedzi() << '\n';
	str << "# Il iteracji "<< (int) n->getIloscIteracji() << '\n';

	str << "Y = [\n";

	for (int i=0; i<pldlen ; i++){
		str <<  i << ' ' << pldTab[i] << ";\n";
	}	// for
	str << "];\n\n";

	str << "plot(Y(:,1), Y(:,2) / sum(Y(:,2)), \"r\");\n";

	str << "# loglog(Y(:,1), Y(:,2) / sum(Y(:,2)), \"b+\");\n"
		<< "# bar(Y(:,1), Y(:,2) / sum(Y(:,2)));\n"
		<< "grid(\'on\');\n"
		<< "legend(\"Path Length Distribution\");\n"
		<< "S = sprintf(\"neurons = %d\", "<<  n->getN().getIloscNeuronow() << ");\n"
		<< "text(4, .70, S, \'fontsize\', 15);\n"
		<< "xlabel(\"path length\");\n"
		<< "ylabel(\"freq\");\n"
		<< "axis([-1,8, 0, 1]);\n"
	    << "av = sum(Y(:,1) .* (Y(:,2) / sum(Y(:,2))));\n"
		<< "printf(\"avg = %.6f\\n\", av);\n";
	str << "% print( \"/home/piersaj/Desktop/nn/path_length_distribution_" << timestamp;

	char buffer[80];
	sprintf(buffer, "_%dk_%dM_%ldM.svg",
		n->getN().getIloscNeuronow()/1000,
		n->getN().getIloscKrawedzi()/1000000,
		n->getN().getIloscIteracji() / 1000000
	);

	str << buffer << "\" , \"-color\"); \n\n";
	str.close();
	logJP << "SNPLDD.zapisWynikow(): ok\n";
}	// save

//============================================================================
// Name        : Neurony_PHD.cpp
// Author      : Jaroslaw Piersa
// Version     :
// Copyright   : JP. PHD simulation
// Description : Geometrically embeddend multi-valued neural network
//             : with Boltzmann-like dynamics
//============================================================================

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "../neurony/utils/logger.h"


//#include "wyswietlanie.h"

#include "../neurony/dynamics/siecNeuronowaDynamika.h"

#include "../neurony/statistics/siecNeuronowaRozkladStopni.h"
#include "../neurony/statistics/siecNeuronowaCC.h"
#include "../neurony/statistics/NN_FailureResiliancyTester.h"
#include "../neurony/statistics/NN_Failure_resiliency_path_length.h"
#include "../neurony/statistics/NN_network_degeneracy.h"
#include "../neurony/statistics/degree_corelation/NN_DegreeCorrelationTester.h"


#include "../neurony/GraphER.h"

#include "../neurony/statistics/path_length/siecNeuronowaPLD.h"
#include "../neurony/statistics/path_length/SiecNeuronowaPldDijkstra.h"
#include "../neurony/statistics/path_length/GraphErPaths.h"

#include "../neurony/statistics/spectrum/spektrumGrafuLapack.h"


#include "../neurony/config/parserKonfiguracji.h"

#ifdef WITH_XML_LIB2
	#include "../neurony/config/simulationConfigParser.h"
#else
	#include "../neurony/config/simulationConfigTxtParser.h"
#endif



/*
 * 2008-12-??
 * 		napisanie
 * 2008-12-17
 * 		zczytywanie parametrow z argv
 * 2009-03-21
 *		parametry zczytywane z pliku konfiguracyjnego zamiast argv
 * 2009-05-31
 *		dwa dodatkowe parametry
 *		histogram zmian
 * 2011-04-11
 * 		dodane flaki odpalania fearurów z konfiguracji
 * 2011-07-14
 * 		dodany abstrakcyjny graf
 * 2011-07-15
 * 		zmiana parametrow z xmla na liste "param=val"
 * 		program nie wymaga libxml2 do dzialania (mam nadzieje)
 */

int main(int argc, char ** argv) {

#ifdef WITH_XML_LIB2
	SimulationConfigParser * confP = new SimulationConfigParser();
	SimulationConfig * conf = confP->getSimulationConfig();
#else

	SimulationConfigTxtParser* confTP = new SimulationConfigTxtParser();
	SimulationConfig * conf = confTP->getSimulationConfig();
#endif



//	wczytajKonfiguracje();
	SiecNeuronowa n = SiecNeuronowa(conf);

	/*
	 */


	n.setIloscIteracji(conf->getIterationsNumber());


	SiecNeuronowaDynamika nd = SiecNeuronowaDynamika(n);
	siecNeuronowaDynamikaGlobalna = & nd;

	// wydruk na ekran
	n.opiszStanSieci();

	// symulacja
	nd.rozpocznijSymulacjeNaWatkach();

	// wyniki

	// Deg Distr
	if (n.getConfig()->getCalculateDegreeDistribution()){
		SiecNeuronowaRozkladStopni sndd = SiecNeuronowaRozkladStopni( nd);
		sndd.obliczTablicePrzplywu();
	//	sndd.obliczNajlepszeDopasowanieMNK();
		sndd.zapiszRozkladStopniDoPliku();
	}	else {
		logJP << "DD: Degree Distribution calculation was disabled\n";
	}	// if


	// CC
	if (n.getConfig()->getCalculateClasteringCoeffitientDistribution()){
		SiecNeuronowaCC sncc = SiecNeuronowaCC(nd);
		sncc.obliczRozkladCCDlaGrafu();
		sncc.zapiszRozkladCCStopniDoPliku();
	} else {
		logJP << "CC: Clustering Coeffitient Distribution calculation was disabled\n";
	}	// if


	// dijkstra
	if (n.getConfig()->getCalculatePathLengthDistribution()){
		SiecNeuronowaPldDijkstra * snpld = new  SiecNeuronowaPldDijkstra(& nd);
		snpld->calculatePldDijkstra();
		delete snpld;
	}	else {
		logJP << "PLD: PLD calculation was disabled\n";
	}	// if


	if (n.getConfig()->getCalculatePathLengthDistributionMC()){
		SiecNeuronowaPLD snpl = SiecNeuronowaPLD(nd);
		snpl.obliczRozkladDlugosciSciezek();
		snpl.obliczRozkladDlugosciSciezekMC();
	}	else {
		logJP << "PLD_MC: calculation MC version of PLD was disabled\n";
	}	// if

	if ( n.getConfig()->getCalculateFailureResiliency() ){
		NN_FailureResiliancyTester * nnfrt =  new NN_FailureResiliancyTester(&n, .1, RESILIENCY_MODE_RANDOM);
		nnfrt->calculateResiliency();


		NN_FailureResiliancyPathLength * nnfrpl = new NN_FailureResiliancyPathLength(&n, nnfrt);
		nnfrpl ->calculateAPL_FaultTolerance();

		delete nnfrpl;
		delete nnfrt;
	}	else {
		logJP << "NN_FRT: Resiliency test was disabled\n";
	}	// if


	if ( n.getConfig()->getCalculateNetworkDegeneracy() ){
		NN_DegeneracyTester * nndt =  new NN_DegeneracyTester(&n);
		nndt->calculateDegeneracy();

		delete nndt;
	}	else {
		logJP << "NN_DegeneracyTester: Degeneracy test was disabled\n";
	}	// if

	if ( n.getConfig()->getCalculateDegreeCorrelation() ){
		NN_DegreeCorrelationTester * nndc =  new NN_DegreeCorrelationTester(&n);
		nndc -> calculateDegreeCorrelation();

		delete nndc;
	}	else {
		logJP << "NN_DegreeCorrelationTester: Degree Correlation Test was disabled\n";
	}	// if

	// Eigs
#ifdef WITH_LAPACK
	// lapack requires -llapack / -lmkl / or any other implementation to run...
	//zapiszMacierzGrafuPrzeplywuM(n);
	if (n.getConfig()->getCalculateSpectrum()){
		policzWartosciWlasneLapack(n);
		zapiszWartosciWlasneLapack(n);
	}	else {
		logJP << "Lapack: Eigenvalues calculation was disabled\n";
	}	// if
#endif

#ifdef WITH_GSL
	if (n.getConfig()->getCalculateSpectrum()){
		policzRozkladSpektralny(n);
		zapiszWartosciWlasneM(n);
	}	else {
		logJP << "GSL: Eigenvalues calculation was disabled\n";
	}	// if
#endif



#ifdef WITH_GL
	// GL can be switched on in wyswietlanie.h
	//  it will require to add -lGL -lGLU -lSDL -lSDL_ttf
	// and they are currently DISABLED!!!
	if (n.getConfig()->getUseGLmode()){
		setupGL(n);
		main_loop(n);
	}	else {
		logJP << "GL mode disabled\n";
	}	// if
#endif




	logJP << "\n\n";
	logJP.timestamp_nl() << "*** END OF SIMULATION ***\n\n";
	return 0;
}	// main

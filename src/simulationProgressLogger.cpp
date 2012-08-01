/*
 * simulationProgressLogger.cpp
 *
 *  Created on: Aug 26, 2010
 *      Author: piersaj
 *
 *  2010-08-26
 *  	utworzenie
 *  2010-08-29
 *  	korekty do zapisow, wywalenie fragmentow kodu, ktore robily to samo
 *  2011-11-14
 *  	Added useTimeStampInFilename handling
 */

#include <fstream>
#include <ctime>

#include "simulationProgressLogger.h"

#ifdef SIMMULATIONPROGRESSLOGGER_H_

SimulationProgresLogger * simulationProgresLog = NULL;

std::_Ios_Openmode splogger_file_flags;

void initProgresLoger(SiecNeuronowaDynamika * n){
	simulationProgresLog = new SimulationProgresLogger(n);
}	// initProgresLog()

SimulationProgresLogger::SimulationProgresLogger(SiecNeuronowaDynamika *n){
	splogger_file_flags = std::fstream::app | std::fstream::out;

	iterations = 0;
	numberOfLogs = 0;
	numberOfDrainedNeurons = numberOfEdgeDeletions = numberOfTransfers = 0;

	timePrev = time(NULL);
	timeCurr = timePrev;

	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	strftime (stringTime, 255,"%Y_%m_%d_%H_%M_%S",timeinfo);


	if (n->getN().getConfig()->getUseTimestampInFilenames() == true) {
		sprintf(fileName, "%s/r_%s__%d_kn_%d_Me_%ldM_it_logProgress.m",
				n->getN().getConfig()->getResultsFolder(),
				n->getN().getTimestamp(),
				n->getN().getIloscNeuronow() / 1000,
				n->getN().getIloscKrawedzi() / 1000000,
				n->getIlIteracji() / 1000000
		);
	} else {
		sprintf(fileName, "%s/logProgress.m", n->getN().getConfig()->getResultsFolder());
	}	// if .. else


	std::fstream str;
	str.open(fileName , splogger_file_flags );

	if (str == NULL){
		std::cerr<< "SPLogger() can't open file: " << fileName << "\n";
		return;
	}	// if

	str <<"% " << fileName << "\n";

	str <<"SIMULATION_LOG =  [ \n";
	str << "% time, iterations , IPS, Energy, edgesRemoved, neuronsDrained, transfersAccepted;\n";


	str.close();
}	// this()


SimulationProgresLogger::SimulationProgresLogger(){
	SimulationProgresLogger(bierzacaSiecDlaWatkowGenerujacych);
}	// this(void)

void SimulationProgresLogger::logProgress(SiecNeuronowaDynamika *n){
	timeCurr = time(NULL);
	long dtime = timeCurr - timePrev;
	timePrev = timeCurr;
	numberOfLogs++;

	std::fstream str;
	str.open(fileName , splogger_file_flags);


	long iterationsLastStep = n->getNumerBiezacejIteracji() - iterations;
	iterations = n->getNumerBiezacejIteracji();

	double ips = (dtime == 0 ? 0 : (iterationsLastStep / (1.0 * dtime)));
//	long numberOfEdgeDeletionsDuringLastStep = n.getIloscUsuniecKrawedzi() - numberOfEdgeDeletions;
//	numberOfEdgeDeletions = n.getIloscUsuniecKrawedzi();
	numberOfEdgeDeletions = n->getIloscUsuniecKrawedzi();
	numberOfDrainedNeurons = n->getN().obliczIloscNeuronowBezLadunku() /* - numberOfDrainedNeurons */;


	str << "   " << dtime << ",\t";
	str << iterationsLastStep << ",\t" << ips << ",\t"<< n->getN().getEnergia() << " ,\t";
	str << /* numberOfEdgeDeletionsDuringLastStep */ numberOfEdgeDeletions << ",\t"
		<< numberOfDrainedNeurons
		<< " ,\t"<< n->getIloscEfektywnychZmian() <<" ; \n";

	str.close();
}	// logProgress;

void SimulationProgresLogger::endLog(SiecNeuronowaDynamika *n){
	std::fstream str;
	str.open(fileName , splogger_file_flags );


	str << "];\n\n";

	str << "N = "<< n->getIloscNeuronow() << ";\n";
	str << "E = " << n->getIloscKrawedzi() << ";\n";

	str << "hold off;\n"
		<< "plot((1:50).*2, SIMULATION_LOG(:,1), \"b\" )\n"
		<< "# print (\"/home/piersaj/Desktop/nn/"
		<< stringTime;

	char buffer[80];
	sprintf(buffer, "_simmulation_progress_%.0fk_%.0fM_%dM.pdf",
			n->getN().getIloscNeuronow()/ 1000.0,
			n->getN().getIloscKrawedzi() / 1000000.0,
			(int) n->getIloscIteracji() / 1000000);

	str << buffer << "\" , \"-color\", \"-dpdf\"); \n";

	str.close();
}	// logProgress;

#endif

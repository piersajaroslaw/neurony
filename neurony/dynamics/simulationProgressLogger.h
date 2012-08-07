/*
 * simmulationProgressLogger.h
 *
 *  Created on: Aug 26, 2010
 *      Author: piersaj
 *
 * this class implement progress logger: every small fraction of iterations (read 2%)
 * writes current parameters (time, iterations, energy, etc.) to logfile
 *
 * 2010-08-26
 * 		created
 * 		somehow i switched the langiage to english... whatever... who cares?
 */

#ifndef SIMMULATIONPROGRESSLOGGER_H_
#define SIMMULATIONPROGRESSLOGGER_H_


#include "siecNeuronowaDynamika.h"


class SimulationProgresLogger{
private:
	long timeCurr;
	long timePrev;

	char stringTime[255];
	char fileName[255];
	long iterations;
	double energy, iterationsPerSec;
	long numberOfEdgeDeletions, numberOfDrainedNeurons, numberOfTransfers;

	int numberOfLogs;

public:
	SimulationProgresLogger(SiecNeuronowaDynamika *);
	SimulationProgresLogger();
	void logProgress(SiecNeuronowaDynamika *);
	void endLog(SiecNeuronowaDynamika *);
};


//void initProgresLoger();
void initProgresLoger(SiecNeuronowaDynamika *);

extern SimulationProgresLogger * simulationProgresLog;


#endif /* SIMMULATIONPROGRESSLOGGER_H_ */

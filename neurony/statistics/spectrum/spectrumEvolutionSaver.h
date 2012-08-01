/*
 * SpectrumEvolutionSaver.h
 *
 *  Created on: Mar 10, 2011
 *      Author: piersaj
 *
 *  2011-03-10
 *  	this piece of code is to save graph spectrum every tic = 2% of the simulation
 */

#ifndef SPECTRUMEVOLUTIONSAVER_H_
#define SPECTRUMEVOLUTIONSAVER_H_

#include "../../network/siecNeuronowa.h"
#include "../../fileWriter.h"

class SpectrumEvolutionSaver{
private:
	SiecNeuronowa * n;
	char * filename;
	NNFileWrter * writer;
	int currentSnapshot;
	int maxSnapshot;


public:
	SpectrumEvolutionSaver(  SiecNeuronowa * );
	~SpectrumEvolutionSaver();

	void generateHeader();
	void generateCalculationTic();
	void generateFooter();

};

#endif /* SPECTRUMEVOLUTIONSAVER_H_ */

/*
 * verticesEvolutionSaver.h
 *
 *  Created on: Feb 18, 2011
 *      Author: piersaj
 *
 *  2011-02-18
 *  	first version
 *  2011-02-20
 *  	debugging
 *  2011-02-22
 *  	second version
 */

#ifndef VERTICESEVOLUTIONSAVER_H_
#define VERTICESEVOLUTIONSAVER_H_

#include "fileWriter.h"
#include "network/siecNeuronowa.h"

class VerticesEvolutionSaver{

private:
	SiecNeuronowa * n;
	char * filename;
	NNFileWrter * writer;
	int currentSnapshot;
	int maxSnapshot;


public:
	VerticesEvolutionSaver(  SiecNeuronowa * );
	~VerticesEvolutionSaver();

	void generateHeader();
	void generateCalculationTic();
	void generateFirstCalculationTic();
	void generateFooter();

};


#endif /* VERTICESEVOLUTIONSAVER_H_ */

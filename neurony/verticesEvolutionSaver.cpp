/*
 * verticesEvolutionSaver.cpp
 *
 *  Created on: Feb 22, 2011
 *      Author: piersaj
 */

#include<ctime>
#include<cstdlib>

#include "verticesEvolutionSaver.h"
#include "network/siecNeuronowa.h"
#include "../src/logger.h"

#define LEN 255

VerticesEvolutionSaver::VerticesEvolutionSaver(SiecNeuronowa *n_){
	n  = n_;
	filename = new char [255];
	currentSnapshot = 0;



	if (n->getConfig()->getUseTimestampInFilenames()) {
		sprintf(filename, "%s/r_%s__%d_kn_%d_Me_%ldM_it__nodesSnapshot.m",
			 n->getConfig()->getResultsFolder(),
			 n->getTimestamp(),
			 n->getIloscNeuronow()/1000,
			 n->getIloscKrawedzi()/1000000,
			 n->getIloscIteracji()/1000000
		);
	} else {
		sprintf(filename, "%s/nodesSnapshot.m", n->getConfig()->getResultsFolder());
	}	// else

	logJP << "nodes file:       " << filename<< "\n";


	writer = new NNFileWrter(filename);
	writer->comment().printAuthor().nl();
	writer->comment().timestampNl();

}	// this

VerticesEvolutionSaver::~VerticesEvolutionSaver(){
	delete writer;
	delete filename;
}	// ~this

void VerticesEvolutionSaver::generateHeader(){
	writer->comment() <<" skrypt w Octave do rysowania sieci\n\n"
		<< "iloscNeuronow = " << n->getIloscNeuronow()
		<< ";\niloscKrawedzi = " << n->getIloscKrawedzi()
		<< ";\niloscIteracji = " << n->getIloscIteracji() << ";\n\n";

	writer->comment() << "a teraz piszemy nasze neurony\n";
	writer->nl()<< "progres = [];\n";
	currentSnapshot = 0;
}	// generateHeader();


void VerticesEvolutionSaver::generateCalculationTic(){
	if (currentSnapshot == 0){
		generateFirstCalculationTic();
	}	// fi

	writer->ind1();
	for (int i=0; i<n->getIloscNeuronow(); i++){
		Neuron & m = n->getNeuron(i);
		(*writer) << m.getLadunek() << " , ";
	}	// for i;
	(*writer) << ";\n\n";
	currentSnapshot++;
}	// generateHeader();

void VerticesEvolutionSaver::generateFirstCalculationTic(){
	writer->ind1();

	*writer << "progres = [\n" ;
	writer->ind1();
	for (int i=0; i<n->getIloscNeuronow(); i++){
		(*writer) << n->getNeuron(i).getX() << " , ";
	}	// for i;
	*writer << ";\n\n";
	writer->ind1();
	for (int i=0; i<n->getIloscNeuronow(); i++){
		(*writer) << n->getNeuron(i).getY() << " , ";
	}	// for i;

	*writer << ";\n\n";
	writer->ind1();
	for (int i=0; i<n->getIloscNeuronow(); i++){
		(*writer) << n->getNeuron(i).getZ() << " , ";
	}	// for i;
	(*writer) << ";\n\n";
}	// generateHeader();


void VerticesEvolutionSaver::generateFooter(){
	(*writer) << "];\n";

	writer->comment()<< " end of snapshots \n"
		<< "numberOfSteps = " << currentSnapshot << ";\n"
		<< "function printNN3(progres, i, savePNG = false)\n";

	(*writer)<<"  sleeptime = 2;\n"
			<<"  offset = 3;\n"
			<<"  sleeptime = .5;\n"
			<<"  th0 = find(progres(offset+i, :)< 1000);\n"
			<<"  hold off;\n"
			<<"  pause(sleeptime);\n"
			<<"  if length(th0>0)\n"
			<<"      plot3( progres(1, th0), progres(2, th0), progres(3, th0), \"b.\");\n"
			<<"      pause(sleeptime);\n"
			<<"      hold on;\n"
			<<"      pause(sleeptime);\n"
			<<"  end;\n"
			<<"  th1 = find(abs(progres(offset+i, :) -3000)<= 2000);\n"
			<<"  if length(th1>0)\n"
			<<"      plot3( progres(1, th1), progres(2, th1), progres(3, th1), \"co\");\n"
			<<"      pause(sleeptime);\n"
			<<"      hold on;\n"
			<<"      pause(sleeptime);\n"
			<<"  end;\n"
			<<"  th2 = find(abs(progres(offset+i, :)-10000)<= 5000);\n"
			<<"  if length(th2>0)\n"
			<<"      plot3( progres(1, th2), progres(2, th2), progres(3, th2), \"gd\");\n"
			<<"      pause(sleeptime);\n"
			<<"      hold on;\n"
			<<"      pause(sleeptime);\n"
			<<"  end\n"
			<<"  th3 = find(progres(offset+i, :)>= 15000);\n"
			<<"  if length(th3>0)\n"
			<<"      plot3( progres(1, th3), progres(2, th3), progres(3, th3), \"rd\");\n"
			<<"      pause(sleeptime);\n"
			<<"      hold on;\n"
			<<"      pause(sleeptime);\n"
			<<"  end\n"
			<<"  view (i, 30);\n"
			<<"  refresh()\n"
			<<"  pause(sleeptime);\n"
			<<"\n"
			<<"%  legend(\"0-1k\", \"1k-5k\", \"5k-15k\", \"15k+\");\n"

			<<"  if savePNG\n"
			<<"        filename = sprintf(\"/home/piersaj/Desktop/delme/fig3d_%04d.png\", i);\n"
			<<"  	print(filename, \"-dpng\");\n"
			<<"  	pause(sleeptime);\n"
			<<"  end;\n"

			<<"end; %function\n"

			<<"for i=(1:numberOfSteps)\n"
			<<"  printNN3(progres, i, false);\n"
			<<"end;\n";
}	// generateHeader();

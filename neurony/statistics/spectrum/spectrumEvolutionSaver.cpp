/*
 * spectrumEvolutionSaver.cpp
 *
 *  Created on: Mar 10, 2011
 *      Author: piersaj
 */


#include<ctime>
#include<cstdlib>

#include "spectrumEvolutionSaver.h"
#include "../../network/siecNeuronowa.h"
#include "../../utils/logger.h"
#include "spektrumGrafuLapack.h"

#define LEN 255

SpectrumEvolutionSaver::SpectrumEvolutionSaver(SiecNeuronowa *n_){
	n  = n_;
	filename = new char [255];
	currentSnapshot = 0;

	char buff[LEN];

	time_t rawtime;
	time( &rawtime );
	struct tm * timeinfo;
	timeinfo = localtime ( &rawtime );
	strftime(buff, LEN,"%Y_%m_%d__%H_%M_%S", timeinfo);


	sprintf(filename, "./wyniki/r_%s__%dk_n_%dk_k_%ldM_i__spectrumSnapshot.m",
		 buff, n->getIloscNeuronow()/1000, n->getIloscKrawedzi()/1000, n->getIloscIteracji()/1000000
	);


	writer = new NNFileWrter(filename);
	writer->comment().printAuthor().nl();
	writer->comment().timestampNl();
	writer->comment() << " Auto generated Octave script\n";
	writer->comment() << " Evolving spectrum of the graph\n";

}	// this

SpectrumEvolutionSaver::~SpectrumEvolutionSaver(){
	delete writer;
	delete filename;
}	// ~this

void SpectrumEvolutionSaver::generateHeader(){
	(*writer)<<"iloscNeuronow = " << n->getIloscNeuronow()
		<< ";\niloscKrawedzi = " << n->getIloscKrawedzi()
		<< ";\niloscIteracji = " << n->getIloscIteracji() << ";\n\n";

	writer->nl()<< "Eigs = [\n";
	currentSnapshot = 0;

}	// generateHeader();

void SpectrumEvolutionSaver::generateCalculationTic(){
	writer->ind1();


#ifdef WITH_LAPACK
	policzWartosciWlasneLapackSilent(*n, true);

	float * f = getWartosciWlasneLapack();

	for (int i=0; i<n->getIloscNeuronow(); i++){
		(*writer) << f[i] << " , ";
	}	// for i;

	f = NULL;

#else
	std::cerr << "SES.genCT(): Compiled without Lapack!\n\n";
#endif

	(*writer) << ";\n";
	currentSnapshot++;
}	// generateHeader();



void SpectrumEvolutionSaver::generateFooter(){
	(*writer) << "];\n";

	writer->comment()<< " end of eigs \n"
		<< "numberOfSteps = " << currentSnapshot << ";\n\n"
		<< "function [s,e]=findBestRange(Eigs, row, slope, Err1, Err2)\n"
		<< "        E = Eigs(row,:);\n"
		<< "        E = sort((E))(end:-1:1);\n"
		<< "        E = E(find(E>0));\n"
		<< "        s = 1;\n"
		<< "        e = 2;\n"
		<< "        best = -1000;\n"
		<< "        bestlen = 0;\n"
		<< "        step=10;\n"
		<< "        besterr = 1000000;\n"
		<< "        for i = 1:step:length(E)-step;\n"
		<< "                % printf(\"%d\n\", i);\n"
		<< "                for j=(i+1:step:length(E))\n"
		<< "                        a = polyfit(log(i:j), log(E(i:j)), 1);\n"
		<< "                        er = abs(slope - a(1));\n"
		<< "                        x = a(1)*log(i:j)+a(2) - log(E(i:j));\n"
		<< "                        sumerr =  sum(abs(x))/ (j-i)/(j-i);\n"
		<< "                        if (er < Err1 &&  sumerr<Err2 && bestlen<=(j-i))\n"
		<< "                                s=i;\n"
		<< "                                e=j;\n"
		<< "                                bestlen=j-i;\n"
		<< "                                besterr=sumerr;\n"
		<< "                        end\n"
		<< "               end\n"
		<< "        end\n"
		<< "        besterr;\n"
		<< "end\n\n\n"


		<< "function printSpectrumEvolution(E, i, savePNG = false)\n"
		<< "        [s,e] = findBestRange(E, i, -2, 1*10^-1, 3.7* 10^-5);\n"
		<< "        E = E(i,:);\n"
		<< "        E = sort((E))(end:-1:1);\n"
		<< "        E = E(find(E>0));\n"
		<< "        sleeptime = .1;\n"
		<< "        hold off;\n"
		<< "        loglog((1:s), E(1:s), \"b\", \"linewidth\", 2);\n"
		<< "        hold on;\n"
		<< "        plot((s:e), E(s:e) , \"r\", \"linewidth\", 2);\n"
		<< "        plot((e:length(E)), E(e: length(E)) , \"b\", \"linewidth\", 2);\n"
		<< "        a = polyfit(log(s:e), log(E(s:e)), 1);\n"
		<< "%       plot(exp(log(s:e)),  exp(a(1).*log(s:e) .+ a(2)), \"g\");\n"
		<< "        t = sprintf(\"range = %d .. %d = %d\", s, e, e-s );\n"
		<< "        text(10^1, 10^1, t);\n"
		<< "        t = sprintf(\"step = %d\", 2*i );\n"
		<< "        text(10^1, 10^0, t);\n"
		<< "%       t = sprintf(\"a = %1.3f\", a(1) );\n"
		<< "%       text(10^1, 10^-1, t);\n"
		<< "        axis([10^0, 10^4, 10^-3, 10^6] );\n"
		<< "        pause(sleeptime);\n"
		<< "        grid(\"on\");\n"
		<< "        % legend(\""<< n->getIloscNeuronow()<< " neurons \");\n"
		<< "        if savePNG\n"
		<< "                filename = sprintf(\"/home/piersaj/Desktop/delme/spectrum12530n_%04d.png\", i);\n"
		<< "                print(filename, \"-dpng\");\n"
		<< "                filename = sprintf(\"/home/piersaj/Desktop/delme/spectrum12530n_%04d.svg\", i);\n"
		<< "                print(filename);\n"
		<< "                pause(sleeptime);\n"
		<< "        end;\n"
		<< "end; %function\n";
}	// generateHeader();

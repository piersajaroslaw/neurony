#if 0

#include "../neurony/spektrumGrafuGsl.h"
#include "../neurony/spektrumGrafuLapack.h"
#include "wyswietlanie.h"

#include "../neurony/parserKonfiguracji.h"
#include "../neurony/zapisWynikow.h"

#include "../neurony/siecNeuronowaDynamika.h"
#include "../neurony/siecNeuronowaRozkladStopni.h"
#include "../neurony/siecNeuronowaCC.h"
#include "../neurony/siecNeuronowaAPL.h"
#include "../neurony/siecNeuronowaPLD.h"
//#include "../neurony/fileWriter.h"
#include "logger.h"
#include <cstdio>

using namespace std;
/*
 * 2008-12-??
 * 		napisanie
 * 2008-12-17
 * 		zczytywanie parametrow z argv
 * 2009-03-21
 *		parametry zczytywane z pliku konfiguracyjnego zamiast argv
 * 2009-05-31
 *		dwa dodatkowe parametr
 *		histogram zmian
 */
int main____(int argc, char ** argv) {
	printf("main\n\n");
	srand(time(NULL));


	wczytajKonfiguracje();

//	NNFileWrter fw = NNFileWrter((char*)"/home/piersaj/Desktop/delme.txt");
//	fw.timestampNl();

//	fw.comment() << "ala ma kota\n";
//	fw.timestamp();
//	fw.timestamp();
//	fw.timestamp();
//	fw.timestampNl();

	SiecNeuronowa n = SiecNeuronowa(parametry[0], parametry[1], parametry[2], parametry[4]);

	n.setBeta(parametry[3]);
	n.setIloscDrzewek((int)parametry[6]);
	n.setWielkoscDrzewka((int) parametry[7]);
	n.setIloscIteracji((int)parametry[5]);


	SiecNeuronowaDynamika nd = SiecNeuronowaDynamika(n);
	siecNeuronowaDynamikaGlobalna = & nd;

	nd.setProgDlugosciKrawedzi( parametry[8]);
	nd.setPrawdopodobienstwoUsunieciaLadunku(parametry[9]);
	nd.setIloscIteracji((int)parametry[5]);

	NUM_THREADS = (int)parametry[12];

	// wydruk na ekran
	n.opiszStanSieci();


	// symulacja
	nd.rozpocznijSymulacjeNaWatkach();

	// wyniki

	// Deg Distr
	SiecNeuronowaRozkladStopni sndd = SiecNeuronowaRozkladStopni( nd);
	sndd.obliczTablicePrzplywu();
	sndd.obliczNajlepszeDopasowanieMNK();
	sndd.zapiszRozkladStopniDoPliku();


	// CC
	SiecNeuronowaCC sncc = SiecNeuronowaCC(nd);
	sncc.obliczRozkladCCDlaGrafu();
	sncc.zapiszRozkladCCStopniDoPliku();

	SiecNeuronowaPLD snpl = SiecNeuronowaPLD(nd);
	snpl.obliczRozkladDlugosciSciezek();
	snpl.obliczRozkladDlugosciSciezekMC();

	// Eigs
#ifdef WITH_LAPACK
	//zapiszMacierzGrafuPrzeplywuM(n);
	policzWartosciWlasneLapack(n);
	zapiszWartosciWlasneLapack(n);
#endif

#ifdef WITH_GSL
	policzRozkladSpektralny(n);
	zapiszWartosciWlasneM(n);
#endif


#ifdef WITH_GL
	// GL can be switched on in wyswietlanie.h
	//  it will require to add -lGL -lGLU -lSDL -lSDL_ttf
	// and they are currently DISABLED!!!
	setupGL(n);
	main_loop(n);
#endif



	return 0;
}	// main

#endif

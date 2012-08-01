/*
 * parserKonfiguracji.h
 *
 *  Created on: 21-Mar-2009
 *      Author: piersaj
 *
 *  2009-03-21
 *  	plik do wczytywania konfigu z pliku
 *  	a moze również i do zapisywania wynikow do pliku :)
 *  2009-03-22
 *  	zapis tablicy do pliku
 *  2009-05-31
 *  	zapis histogramu przyjmowanyuch zmian do pliku
 *  	zapis wspolczynnika skupienia (clustering coeffitian) do pliku
 *  2009-06-19
 *  	prog dlugosci krawedzi powyzej ktorego akceptujemy krawedz
 *  2009-06-27
 *  	zapis histogramu czasu liczenia danej epoki symulacji
 *  2009-07-05
 *  	zapisy polecialy do oddzielnego pliku
 *  2010-07-09
 *  	dodany parametr kontrolujacy ilosc waatkow
 */

#include <fstream>
#include <cstdlib>
#include "parserKonfiguracji.h"


float parametry[iloscParametrow];




void wczytajKonfiguracje(){

	inicjalizujParametry();

	std::fstream str;

	str.open(plikKonfiguracyjny, std::fstream::in );

	if (str == NULL){
		std::cerr << "wczytajKonfiguracje(): brak pliku wejsciowego " << plikKonfiguracyjny << "\n\n";
		return;
	}	// fi

	char bufor[255];


	int i=0;

	while (i< iloscParametrow){
		//str >> bufor;

		str.getline(bufor , 255);

		if (bufor[0] == '#')
			continue;

		// else

		float v = atof(bufor);
		parametry[i] = v;
		i++;

	}	// while

	str.close();

	return;
}	// wczytajKonfigurace()



void inicjalizujParametry(){
	parametry[0] = 2.5;
	parametry[1] = 10;
	parametry[2] = 3;
	parametry[3] = 1000;
	parametry[4] = 10;
	parametry[5] = 10000000;
	parametry[6] = 4;
	parametry[7] = 50;
	parametry[8] = -1;
	parametry[9] = .01;
	parametry[10] = 10;
	parametry[11] = 10;
	parametry[12] = 2;
}	// initParametry


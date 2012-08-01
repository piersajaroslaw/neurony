#ifndef PARSERKONFIGURACJI_H_
#define PARSERKONFIGURACJI_H_

/*
 * parserKonfiguracji.h
 *
 *  Created on: 21-Mar-2009
 *      Author: piersaj
 *
 *  plik do wczytywania konfigu z pliku
 *
 *  2009-03-21
 *  	pliczek do wczytywania parametrow z pliku txt zamiast szukania w kodzie albo z linii polecen
 *  	również do zapisywania wyników do pliku
 *  2009-05-31
 *  	dodano zrzut wspolczynnikow skupienia do pliku
 *  	dodano zrzut histogramu przyjetych iteracji do pliku
 *  2009-06-19
 *  	dorzucony nowy parametr - prog dlugosci krawedzi powyzej ktorego przeplyw jest zawsze akceptowany
 *  2009-06-27
 *  	zapis histogramu czasu liczenia danej epoki symulacji
 *  2009-07-05
 *  	zapoisy polecialy do osobnego pliku
 *  2010-07-09
 *  	dodany parametr kontrolujacy ilosc waatkow
 */

#include "../network/siecNeuronowa.h"




const char  plikKonfiguracyjny[] = "./konfig/konfig.txt";
const int iloscParametrow = 13;


// alpha, maxPos, typ, beta, gestosc, ilIteracji, ilDrzewek, wielkoscDrzewka progKrawedz
// P-o usuniecia ladunku przy przeplywie przez dluga krawedz!
// sredniLadunek, wariancjaLadunku
// NUM_THREADS - ilosc watkow
extern float parametry[];

void inicjalizujParametry();

void wczytajKonfiguracje();



#endif /* PARSERKONFIGURACJI_H_ */

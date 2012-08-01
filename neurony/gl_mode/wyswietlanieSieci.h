/*
 * wyswietlanieSieci.h
 *
 *
 * wyswietlanie sieci w gl wrzucone tu zeby w innym miejscu lepiej sie kod czytalo :))
 *
 *
 *  2008-12-07
 *  	utworzenie,
 *  	wyswietlanie grafu, krawedzi, siatki szescianu
 *  2008-12-17
 *  	dorzucone kontrolowanie progu odcinajacego
 *  	rysowanie poprzez DisplayList
 *  	nieudane proby narysowania tekstu na ekranie :(
 *  2009-01-12
 *  	zamiana malych nieznaczacych sfer na glPoints - naprawde widac te poprawe, ilosc poligonow spadla 10 razy
 *  2009-01-18
 *		dorzucono wyswietlanie wartosci przeplywu ladunku przez siec (argx, argy)
 *  2009-??-??
 *  	dodano wyswietlanie prostej mnk
 *  2009-04-19
 *  	dodano progi gorny i dolny sterujacy mnk - progi sa kontrolowane klawiatura (wyswietlanie.h)
 *  2009-04-26
 *		dodano wyswietlanie tekstu 2d (statystyki)
 *	2009-05-04
 *		wyświetlanie drzewek w grafie
 *  2009-05-09
 *  	wyrzucono kod do oddzielnego pliku
 *  2009-06-12
 *  	dorzucony tryb wyswietlania krawedzi
 *  2009-07-09
 *  	dodano tryb pokazywania wykresow
 *
 *  Created on: 07-Dec-2008
 *      Author: piersaj
 */



#ifndef WYSWIETLANIESIECI_H_
#define WYSWIETLANIESIECI_H_

#ifdef WITH_GL

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

#include "../neurony/siecNeuronowa.h"
#include "../neurony/siecNeuronowaRozkladStopni.h"

/* update listy po zmianie sieci
 * do uzytku z zwenatrz
 */
void uaktualnijListe(SiecNeuronowaRozkladStopni & );


/*
 * metoda z zewnatrz
 */
void rysujGraf(SiecNeuronowa&);

/* rysowanie poszczegolnych skladowych grafu
 * do uzytku wewnetrznego
 */
void rysujKlatke(float);
void rysujWierzcholki(SiecNeuronowa& , bool);
void rysujKrawedzie(SiecNeuronowa& siec);
void rysujWykresPrzplywuLadunku(SiecNeuronowa& siec);
void rysujProstaMNK(SiecNeuronowa& siec);
void rysujWykresWspolczynnikaKlasteryzacji(SiecNeuronowa& siec);
void rysujHistogramZmian(SiecNeuronowa& siec);

/* kontrola progu rysowania krawedzi
 */
void zwiekszProg();
void zmniejszProg();

/* kontrola progu wykresu do liczenia mnk
 */
void zmniejszProgMin();
void zwiekszProgMin();
void zwiekszProgMax();
void zmniejszProgMax();

/* flagi i opcje przy rysowaniu
 *
 * czy pokazywac neurony bez ladunku
 * (tj 10k+ niebieskich kropek)
 */
void pokazujZeroweNeurony();
void ukryjZeroweNeurony();


/* zmienne globalne - nie dotykac z zewnatrz!
 */
bool czyPokazywacZeroweNeurony();


/* dotyczy trybu pokazywania krawedzi
 */
int getPokazkrawedzie();
void setPokazkrawedzie(int val);

/* dotyczy opcji pokazywania lub nie wykresow
 *
 */
void setPokazWykresy(bool val);
bool czyPokazywacWykresy();

const int KRAWEDZIE_ZADNE = 0;
const int KRAWEDZIE_DRZEWKA = 1;
const int KRAWEDZIE_PRZEPLYW = 2;

const int MAX_PROG = 10;



#endif /* WYSWIETLANIESIECI_H_ */
#endif // ZERO

/*
 *  siecNeuronowa.h
 *
 * Struktury danych do pracy, docelowo:
 *  neurony, polaczenia miedzy neuronami, siec, generowanie sieci, ewolucja sieci
 *
 *
 *
 *  Author: Piersa Jaroslaw
 *  Created on: 30-Nov-2008
 *
 *  2008-11-30
 *  	przypomnienie sobie jak sie pisze w c++,
 *  	zmuszenie do wspolpracy eclipsa i cdt,
 *  	utworzenie,
 *  	klasa SiecNeuronowa, Polaczenie
 *
 *  2008-12-05
 *  	generowanie neuronow,
 *  	generowanie krawedzi,
 *  	ogarniecie std::vector,
 *  	pomniejsze korekty: dodanie kilku pol do klas, deklaracji na gorze pliku...
 *
 *	2008-12-06
 *		gettery do klasy SiecNeuronowa,
 *		liczenie energii,
 *		symulacja dynamiki sieci
 *		przerzucono losowania, class Neuron do innych plikow.
 *
 *	2008-12-07
 *		korekty do getNeuron() / getKrawedz() - teraz zwraca referencje
 *		liczenie maksymalnego ladunku i przeplywu dla wszystkich neuronow / krawedzi
 *	2009-01-12
 *		korekty do generowania sieci - zapamietywanie listy krawedzi dla neuronu
 *		korekta do symulacji --- liczenie energii (bardzo powazna korekta :P)
 *		losowanie ze sfery dla dim==3 i okregu dla dim==2
 *		liczenie zachowania sie ladunku w grafie - zachowuje sie dziwnie...
 *	2009-01-18
 *		bardzo istotna korekta do dynamiki (zmiana o +/-1, a nie wszystko)
 *		przeliczenia ladunku do log-log + przechowywanie w klasie
 *	2009-02-07
 *		dodany typ sieci (sfera/kostka)
 *		zmiana alpha z int na float
 *		kilka innych korekt
 *  2009-02-08
 *  	rozbito na sn.h i sn.cpp - nadal jest balagan...
 *  	dodano metode najmniejszych kwadratow do liczenia jak wyglada nasza prosta
 *  2009-03-21
 *  	(sesja, plan i podobne)
 *  	dodany parametr zageszczenia neuronow w sieci
 *  2009-03-22
 *  	dodano getter do calej tabeli
 *  2009-04-03
 *  	dorzucono wsparcia
 *	2009-04-05
 *		Liczenie na wsparciach - maksymalne, minimalne takie ze ladunek dodatni
 *		Usuwanie krawedzi miedzy weslami bez ladunku, o wsparciu mniejszym niz
 *		minimalne wsparcie wezla który MA ladunek (powyzej 25%)
 *	2009-05-04
 *  2009-05-09
 *  	dodano szukanie i kolorowanie drzewek przeplywu w sieci
 *  2009-05-21
 *		Drzewka liczone w kolejce priorytetowej
 *		rozbino kod na sn.h snd.cpp snw.cpp (nadal balagan)
 *  2009-05-30
 *		ilosc drzewek i wielkosc Drzewek sa parametrami sieci neuronowej wczytywanymi z konfigu
 *		szybki dostep do krawedzi miedzy neuronami
 *	2009-05-31
 *		histogram przyjmowanych zmian w danym etapie symulacji
 *	2009-06-19
 *		wykres cc w zaleznosci od progu
 * 2009-06-21
 * 		dodano zapis czasu jaki zajeła dana epoka (== 1/100 całości) symulacji
 * 2009-07-05
 * 		dodano prawdopodobienstwo usuniecia ladunku przy przeplywie przez dluga krawedz
 * 2010-07-06
 * 		dodane generowanie sieci na watkach
 * 2010-07-09
 *  	dodany parametr kontrolujacy ilosc waatkow
 * 2010-08-26
 * 		korekty do usuwania jalowych krawedzi - robione przez zerowy watek
 * 2010-08-28
 * 		funkcjonalnosc odpowiadajaca za dynamike zostala (zostanie!) fizycznie wywalona
 * 		do innej klasy, (a nie tylko pliku)
 * 		to samo w blizszej przyszlosci czeka  liczenie DegDistr, CC, Eig, Drzewek, itd
 * 2010-08-29
 * 		przeniesiona czesc kodu liczaca DegDist
 * 2010-08-30
 * 		przeniesiona czesc liczaca CC (i usuniety stary kod)
 * 2010-09-04
 * 		fragmenty kodu przechowywyjace histogramy zmian i czasu zostaly wywalone
 * 		ich funkcjonalnosc przejal ProgresLogger
 *
 * ???: wrzucanie migawki sieci do pliku (aka. serializacja) -
 * 			500000 krawedzi * 10B ~= 5MB - ma to sens?
 * 			a bywa z 10x - 100x tyle,
 * 			zdecydowanie nie, m1gawka zajmuje z 30MB a bywa i wiecej
 *
 * TODO: refactoring klasy - niemilosiernie sie rozrosla,
 * 			zostaly jeszcze drzewka przeplywu... które nie sa liczone
 *
 */



#ifndef SIECNEURONOWA_H_
#define SIECNEURONOWA_H_


#include <pthread.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <deque>
#include <queue>


#include "neuron.h"
#include "polaczenie.h"
#include "../utils/pairOfInts.h"
#include "../utils/logger.h"
#include "../config/simulationMasterConfig.h"

extern int NUM_THREADS;



// typ topologii sieci
const int KOSTKA_2D=0;
const int KOSTKA_3D=1;
const int SFERA_1D=2;
const int SFERA_2D=3;
const int KOLO_2D=4;
const int KULA_3D=5;
const int RANDOM_ER=6;
const int LATTICE_2D=7;
const int LATTICE_3D=8;



/* finkcja okresla kolor dla drzewka
 * int numer drzewka
 * rgb - zwracane kolory [0-1]^3
 */
void okreslKolorDrzewka(int i, float &r, float &g, float &b);

extern bool czyUsuwacKrawedzie;
extern bool czyUsuwacLadunek;

class SiecNeuronowa{
private:

	SimulationConfig * conf;

	// podstawowe parametry
	const float alpha;
	const int maxPos;
	int lambda;
	int dim;
	int iloscNeuronow;
	int iloscKrawedzi;
	int typ;
	float gestosc;

	float maxWsparcie;


	// energia neuronow
	long sumaLadunku;
	float energia;
	float beta;

	// zbiory neuronow i polaczen
	std::vector<Neuron> neurony;
	std::vector<Polaczenie> krawedzie;

	int iloscDrzewek;
	int wielkoscDrzewka;

	int wspSkupieniaProgKrawedzi;

	long ilIteracji;

	char * timestamp;

	// mutex na watki i energie,
	pthread_mutex_t mutexSN, mutex_energia;

	/* ************************************
	 *  konstruktory i destruktoru
	 * ***********************************/
public:
	SiecNeuronowa(SimulationConfig * conf_);
	~SiecNeuronowa();
	void opiszStanSieci();

	/* ************************************
	 *
	 *
	 *  generowanie neuronow i krawedzi w watkach
	 *
	 *
	 * ***********************************/

public:
	// do generowania krawedzi na watkach, mimo wszystko nie dotykac bez pozwolenia
	void wypelnijPolaczenia(int odIndeks, int doIndeks);

	// sortuje rosnaco (wg indeksu neuronu) krawedzie w neuronie
	void sortujKrawedzieWNeuronie(int indeks);
	int obliczPrzedzialKrawedziDlaWatku(int numer);

private:
	void losujPozycjeSzescian(float &x, float &y, float &z);
	void losujPozycjeSfera(float &x, float &y, float &z);
	void losujPozycjeKula(float &x, float &y, float &z);
	int obliczLambdaKostka();
	int obliczLambdaSfera();
	int obliczLambdaKula();

private:
	void testujSortowanieKrawedzi(int i);


	/* ************************************
	 *  dostep do wierzcholkow i krawedzi
	 * ***********************************/
public:

	Neuron& getNeuron(int);
	Polaczenie& getKrawedz(int i);

	// czy istnieje krawedz miedzy neuronami
	bool czyIstniejeKrawedzMiedzyNeuronami(int, int);
	int getIndeksKrawedziMiedzyNeuronami(int, int);

	// funkcjonalnosc, ktora powinienem dopisac latatemu :P
	int getIndeksItegoSasiada(int index, int i);

	Neuron & getItySasiad(int index, int i);

	SimulationConfig * getConfig();

	/* ***********************************************************
	 *
	 *  settery i gettery do pol klasy
	 *  inline
	 *
	 * ***********************************************************/

	inline float getAlpha(){
		return alpha;
	}

	inline int getMaxPos(){
		return maxPos;
	}

	inline int getLambda(){
		return lambda;
	}

	inline int getDim(){
		return dim;
	}

	inline int getIloscNeuronow(){
		return iloscNeuronow;
	}

	/* troche brzydkie ale detal
	 */
	inline long getIloscIteracji(){
		return ilIteracji;
	}

	inline void setIloscIteracji(long il){
		if (il>0)
			ilIteracji = il;
	}

	inline int getIloscKrawedzi(){
		return iloscKrawedzi;
	}

	inline long getSumaLadunku(){
		return sumaLadunku;
	}

	inline float getEnergia(){
		return energia;
	}
	inline void setEnergia(float val){
		energia = val;
	}

	inline float getBeta(){
		return beta;
	}

	inline float getGestosc(){
		return gestosc;
	}

	inline void setBeta(float beta_){
		if (beta_ >= 0)
			beta = beta_;
	}	// setBeta()

	inline const char* getTypString(){
		switch ( typ){
			case KOSTKA_3D:
				return "kostka3d";
				break;
			case KOSTKA_2D:
				return "kostka2d";
				break;
			case SFERA_1D:
				return "sfera1d";
				break;
			case SFERA_2D:
				return "sfera2d";
				break;
			case KOLO_2D:
				return "kolo2d";
				break;
			case KULA_3D:
				return "kula3d";
				break;
			case RANDOM_ER:
				return "random_ER";
				break;
		}	// switch
		return "";
	}	// getTyp

	inline int getTyp(){
		return typ;
	}

	inline float getMaksymalneWsparcie(){
		return maxWsparcie;
	}

	inline void mutexLock(){
		pthread_mutex_lock(& mutexSN);
	}

	inline void mutexUnLock(){
		pthread_mutex_unlock(& mutexSN);
	}

	inline void mutexEnergiaLock(){
		pthread_mutex_lock(& mutex_energia);
	}

	inline void mutexEnergiaUnLock(){
		pthread_mutex_unlock(& mutex_energia);
	}

	inline char* getTimestamp(){
		return timestamp;
	}

	/*
	 *  Drzewka i inne raczej do wywalenia...
	 */

	inline int getIloscDrzewek(){
		return iloscDrzewek;
	}

	inline void setIloscDrzewek(int il){
		iloscDrzewek = il;
	}

	inline int getWielkoscDrzewka(){
		return wielkoscDrzewka;
	}

	inline void setWielkoscDrzewka(int w){
		wielkoscDrzewka = w;
	}

//	inline float getWspSkupieniaProgKrawedzi(){
//		return wspSkupieniaProgKrawedzi;
//	}
//
//	inline void setWspSkupieniaProgKrawedzi(int wsp){
//		wspSkupieniaProgKrawedzi = wsp;
//	}


//	inline std::deque<pairIntInt> & getWspolczynnikSkupienia(){
//		return wspolczynnikSkupienia;
//	}

/*
	inline float getprogDlugosciKrawedzi(){
		if (progDlugosciKw <=0)
			return progDlugosciKw;
		else
			return sqrt(progDlugosciKw);
	}

	inline void setProgDlugosciKrawedzi(float val){
		if (val <=0)
			progDlugosciKw = val;
		else
			progDlugosciKw = val * val;
	}


*/

//	inline float * getCcRozklad(){
//		return ccRozklad;
//	}
//
//	inline int getCcRozkladLen(){
//		return ccRozkladLen;
//	}


	/* *************************************************
	 *
	 * uslugi sieci - nie sa inline
	 *
	 * *************************************************/

public:
	int obliczIloscNeuronowBezLadunku();

	long znajdzMaksymalnyLadunekNeuronu();

	int znajdzNeuronOMaksymalnymWsparciu();

	/* zwraca (szukajac!) maksymalny przeplyw przez krawedz
	 */
	long znajdzMaksymalnyPrzelpywKrawedzi();



	/* statystyki z symulacji
	 */
	void podsumujSymulacje();


public:
	/* liczy energie od zera
	 */
	float obliczEnergie();

private:
	/* zwraca losowa krawedz w grafie
	 *
	 */
	Polaczenie& wylosujKrawedz();

public:
	long znajdzMaksymalnyLadunekPrzeplywajacyPrzezNeurony();

public:
	float znajdzMinimalneWsparcieODodatnimLadunku();




private:
	long obliczIloscKrawedziPowyzejProgu();

	/* ************************************************
	 *
	 * Drzewka
	 *
	 * ***********************************************/


	/* wyszukuje drzewka i je koloruje
	 *
	 * przerywa po maxKrawedziach
	 *
	 */
public:
	void kolorujDrzewka();

private:
	/* koloruje drzewko wchodzace do neuronu o indeksie i
	 *
	 */
	void kolorujDrzewko(int);

	/* znajduje niepokolorowany wierzcholek o najwiekszym ladunku/wsparciu
	 *
	 */
	int znajdzNiepokolorowanyWierzcholek();



	/* ****************************************************
	 *
	 * Wspolczynnik skupienia << zostal wywalony...
	 *
	 * ****************************************************/
//private:
	/* liczenie wsp skupienia neuronu itego
	 * WS = CC = Clustering Coeficiant
	 */
//	float obliczWspolczynnikSkupieniaNeuronu(int );
//	float obliczWspolczynnikSkupieniaNeuronuTylkoKolorowaneKrawedzie(int );
//	float obliczWspolczynnikSkupieniaNeuronu(int , bool );
//
//	float obliczCCtylkoKrawedzieWychodzace(int i);

//public:

	/* liczenie cc dla grafu powstalego z krawedzi pokolorowanych
	 * patrz SN::kolorujDrzewka()
	 */
//	float obliczWspolczynnikSkupieniaGrafuPrzeplywuLadunku();

	/* liczenie sredniego wsp skupienia dla calej sieci
	 */
//	float obliczWspolczynnikSkupienia();

	/* liczenie cc dla sieci losowej o odpowiadajacej ilosci krawedzi
	 *
	 * deprecated
	 */
//	float obliczWspolczynnikSkupieniaOdpowiadajacejSieciLosowej();


//	void obliczWykresWspolczynnikowSkupienia();

public:
	int stopienWejsciowy(int indeks);
	int inDeg(int indeks);

	int stopienWyjsciowy(int indeks);
	int outDeg(int indeks);

private:
	int maksymalnyStopienWejsciowy();
	int maksymalnyStopienWyjsciowy();

};	// class SiecNeuronowa


/*
 * siec na ktorej dziala cala symulacja, czasem sie nadaje, dostep z dowolnego miejsca
 * w programie wiec nie dotykac jak nie jest absolutnie potrzebna
 */
extern SiecNeuronowa * bierzacaSiecDlaWatkowGenerujacych;

#endif /* SIECNEURONOWA_H_ */

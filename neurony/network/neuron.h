/*
 * neuron.h
 *
 *
 *  Created on: 06-Dec-2008
 *  Author: piersaj
 *
 * 2008-11-30
 *      klasa Neuron
 * 2008-12-06
 * 		przeniesiony do oddzielnego pliku, kolejne 100 linii mniej w innych plikach
 * 2009-01-10
 * 		dodano liste <indeksow> sasiadow - bez komentarza, ech ten swiateczny / noworoczny nastroj
 */

#ifndef NEURON_H_
#define NEURON_H_

#include <cstdio>
#include <iostream>
#include <vector>
#include <cmath>

#include <pthread.h>


/* Neuron
 *
 * pamieta: indeks, pozycje w R^3, ladunek biezacy, ilosc sasiadow, indeksy krawedzi
 * nie pamieta: sasiadow
 *
 */
class Neuron{

private:
	// consty pojechaly na wakacje
	int indeks;
	float x, y, z;
	long ladunek;
	int iloscSasiadow;
	long ladunekWejsciowy, ladunekWyjsciowy;
	std::vector<int> sasiedzi;
	float wsparcie;
	// flaga do dsf-u
	bool czyOdwiedzony;
	pthread_mutex_t mutex;
public:

	Neuron(int indeks_, float x_, float y_, float z_, long ladunek_):
		indeks(indeks_),
		x(x_),
		y(y_),
		z(z_),
		ladunek(ladunek_),
		iloscSasiadow(0),
		ladunekWejsciowy(0),
		ladunekWyjsciowy(0),
		wsparcie(0),
		czyOdwiedzony(false)
	{
		pthread_mutex_init(&mutex, NULL);
	}	// konstruktor(int, 3x float, long)


	~Neuron(){
		sasiedzi.clear();
	}	// ~this()

	/* *********************************************************
	 *
	 *  Dostep do pol (tylko do odczytu)
	 *
	 * *********************************************************/

	inline int getIndeks(){
		return indeks;
	}

	inline float getX(){
		return x;
	}

	inline float getY(){
		return y;
	}

	inline float getZ(){
		return z;
	}

	inline int getIloscSasiadow(){
		return iloscSasiadow;
	}

	inline long getLadunekWejsciowy()  {
        return ladunekWejsciowy;
    }

	inline long getLadunekWyjsciowy()  {
		return ladunekWyjsciowy;
	}

	inline long getLadunek(){
		return ladunek;
	}

	inline float getWsparcie(){
		return wsparcie;
	}

	inline void setWsparcie(float arg){
		wsparcie = arg;
	}

	inline bool getCzyOdwiedzony(){
		return czyOdwiedzony;
	}

	inline void setCzyOdwiedzony(bool arg){
		czyOdwiedzony = arg;
	}

private:
	inline pthread_mutex_t & getMutex(){
		return mutex;
	}

public:
	inline void mutexLock(){
		pthread_mutex_lock(&mutex);
	}

	inline void mutexUnLock(){
		pthread_mutex_unlock(&mutex);
	}

	/* ********************************************************
	 *
	 * Zaawansowane metody
	 *
	 * ********************************************************/
private:
	void zerujLadunek(){
		ladunekWyjsciowy += 1;
		ladunek = 0;
	}

public:
	void zwiekszLadunek(){
		ladunek += 1;
		ladunekWejsciowy += 1;

	}	// zwiekszLadunek()

	void zmniejszLadunek(){
		if (ladunek>=1){
			ladunek -= 1;
			ladunekWyjsciowy += 1;
		}	// if

	}	// zwiekszLadunek()

private:
	void zwiekszLadunek(long oIle){
		if (ladunek + oIle >= 0){
			ladunek += oIle;
			ladunekWejsciowy += 1;
		} else {
			std::cerr << "Neuron.zwiekszLadunkek(): wartosc jest ujemna "<< ladunek << " + "
				<< oIle << "\n";
		}	// if
	}	// zwiekszLadunek()

private:
	void zwiekszLadunekWejsciowy(long oIle){
		if (oIle >= 0){
			ladunekWejsciowy += oIle;
		}
	}	// zwiekszLadunekWejsaciowy()

	void zwiekszLadunekWyjsciowy(long oIle){
		if (oIle >= 0){
			ladunekWyjsciowy += oIle;
		}
	}	// zwiekszLadunekWyjsciowy()



public:
	inline float odleglosc(Neuron a){
		float d = (x-a.x)*(x-a.x) + (y-a.y)*(y-a.y) + (z-a.z)*(z-a.z);
		return sqrt(d);
	}	// odleglosc()

	inline float odlegloscKw(Neuron a){
		float d = (x-a.x)*(x-a.x) + (y-a.y)*(y-a.y) + (z-a.z)*(z-a.z);
		return d;
	}	// odlegloscKw()

	/* dodaje krawedz do listy krawedzi incydentnych z tym wezlem
	 */
	void dodajIndeksKrawedzi(int indeks){
		sasiedzi.push_back(indeks);
		iloscSasiadow++;
	}	// dodajSasiada

	int getIndeksKrawedzi(int numer){
		if (numer>=0 && numer< iloscSasiadow)
			return sasiedzi[numer];

		std::cerr << "Neuron.getSasiad(): niepoprawny numer sasiada = " << numer << "\n";
		return -1;
	}	// getIndeksKrawedzi

	void zamienSasiadow(int i, int j){
			int t = sasiedzi[i];
			sasiedzi[i] = sasiedzi[j];
			sasiedzi[j] = t;
	}	//  zamienSasiadow(int i, int j

};	// class Neuron

#endif /* NEURON_H_ */

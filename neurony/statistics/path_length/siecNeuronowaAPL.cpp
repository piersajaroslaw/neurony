/*
 * siecNeuronowaAPL.cpp
 *
 *  Created on: Sep 12, 2010
 *      Author: piersaj
 *
 *  2010-09-12
 *  	utworzenie
 */
#include <cstdio>

#include "siecNeuronowaAPL.h"
#include "../../../src/logger.h"


#ifdef WITH_APL

SiecNeuronowaAPL::SiecNeuronowaAPL(SiecNeuronowa & n_):
		n(n_)
	{
		aplFaktyczna = 0;
		aplPelnaSiec = 0;
		czyOdwiedzony = new bool[n.getIloscNeuronow()];
		ustawWszystkimNieodwiedzony();
		flagaTylkoKrawedzieZDodatnimPrzeplywem = true;
	}	// this(SN &)

SiecNeuronowaAPL::~SiecNeuronowaAPL(){
	if (czyOdwiedzony != NULL){
		delete [] czyOdwiedzony;
	}	// fi
}	// ~this()

void SiecNeuronowaAPL::ustawWszystkimNieodwiedzony(){
	for (int i=0; i< getIloscNeuronow(); ++i){
		czyOdwiedzony[i] = false;
	}	// for
}	// ustawWszystkimNieodwiedzony()

void SiecNeuronowaAPL::obliczAPL(){
	flagaTylkoKrawedzieZDodatnimPrzeplywem = true;

	double suma = 0;
	logJP << "liczenie APL ";

	double ilosc = getIloscNeuronow() * (getIloscNeuronow()-1) / 2;
	for (int i=0; i<getIloscNeuronow()-1; i++){
		for (int j=i+1; j< getIloscNeuronow(); j++){
			suma += obliczDlugoscSciezki(i, j);
		}	// for i
		if (i%1000 == 0)
			std::cout << '.';
			std::cout.flush();
	}	// for j


	logJP << " ok\n";
	aplFaktyczna = suma / ilosc;

	logJP << "APL graf przeplywu = " << aplFaktyczna << "\n";
}	// obliczAPL()

int SiecNeuronowaAPL::obliczDlugoscSciezki(int i1, int i2){
	if (i1==i2)
		return 0;

	ustawWszystkimNieodwiedzony();
	std::queue<int> kolejka  = std::queue<int>();
	kolejka.push(i1);
	int glebokosc = 0;
	int nastepny = i1;
	czyOdwiedzony[i1] = true;
	bool flagaTrzebaUstawicNastepnego = false;

	// BFS
	while (kolejka.empty() == false){
		int wierzcholek = kolejka.front();
		kolejka.pop();

		if (wierzcholek == nastepny){
			glebokosc++;
			flagaTrzebaUstawicNastepnego = true;
		}	// fi

		// dodajemy Sasiadow
		Neuron & nw = getNeuron(wierzcholek);
		for (int j=0; j< nw.getIloscSasiadow(); j++){
			int sasiad = nw.getIndeksKrawedzi(j);
			Polaczenie & k = getKrawedz(sasiad);

			if (flagaTylkoKrawedzieZDodatnimPrzeplywem && k.getSumaLadunku() == 0){
				continue;
			}	// fi

			sasiad = k.getIndeks1() == wierzcholek ? k.getIndeks2() : k.getIndeks1();

			// znalezlismy
			if (sasiad == i2 ){
				// kolejka.clear();
				while (kolejka.empty()== false)
					kolejka.pop();
				return glebokosc+1;
			}	// fi

			// pierwszy nieodwiedzony wierzcholek na nastepnym poziomie
			if (flagaTrzebaUstawicNastepnego && !czyOdwiedzony[sasiad]){
				flagaTrzebaUstawicNastepnego = false;
				nastepny = sasiad;
			}	// fi

			if (czyOdwiedzony[sasiad] == false){
				czyOdwiedzony[sasiad] = true;
				kolejka.push(sasiad);
			}	// fi
		}	// for

	}	// while

	printf("SNAPL.BFS(): brak sciezki pomiedzy %d %d <- dziwne...\n", i1, i2);
	// Situation Normal All
	return glebokosc+100;
}	// obliczDlugoscSciezki

#endif // ZERO

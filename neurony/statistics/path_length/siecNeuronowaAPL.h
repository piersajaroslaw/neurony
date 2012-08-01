/*
 * siecNeuronowaAPL.h
 *
 *  Created on: Sep 12, 2010
 *      Author: piersaj
 *
 *  kod do liczenie sredniej dlugosci sciezki
 *  puki co w wersji wyjatkowo glupiej tj kazdy z kazdym + bfs
 *
 *  2010-09-12
 *  	utworzony
 */

#ifndef SIECNEURONOWAAPL_H_
#define SIECNEURONOWAAPL_H_

#define WITH_APL
#ifdef WITH_APL

#include "../../network/siecNeuronowa.h"


class SiecNeuronowaAPL {
private:
	SiecNeuronowa &n;
	double aplFaktyczna;
	double aplPelnaSiec;
	bool * czyOdwiedzony;
	bool flagaTylkoKrawedzieZDodatnimPrzeplywem;

public:

	SiecNeuronowaAPL(SiecNeuronowa &);

	~SiecNeuronowaAPL();

	inline double getAplFaktyczna() const    {
        return aplFaktyczna;
    }

    inline double getAplPelnaSiec() const    {
        return aplPelnaSiec;
    }

    inline SiecNeuronowa & getN() const    {
        return n;
    }

    inline Neuron & getNeuron(int i){
    	return n.getNeuron(i);
    }

    inline Polaczenie & getKrawedz(int i){
		return n.getKrawedz(i);
	}

    inline int getIloscNeuronow(){
    	return n.getIloscNeuronow();
    }

    inline int getIloscKrawedzi(){
    	return n.getIloscKrawedzi();
    }

public:
	void obliczAPL();
	void obliczAPLPelnaSiec();

private:
	// BFS
	// mozniej moze zrobimy floyda-warshalla (kwadratowa macierz!)
	int obliczDlugoscSciezki(int i1, int i2);

	void ustawWszystkimNieodwiedzony();

};
#endif // zero

#endif /* SIECNEURONOWAAPL_H_ */

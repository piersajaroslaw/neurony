/*
 * siecNeuronowaCC.h
 *
 *  Created on: Aug 30, 2010
 *      Author: piersaj
 */

#ifndef SIECNEURONOWACC_H_
#define SIECNEURONOWACC_H_

#include "../dynamics/siecNeuronowaDynamika.h"

class SiecNeuronowaCC{
private:
	SiecNeuronowaDynamika &n;

	int ccRozkladLen;
	float *ccRozklad;
	double ccFaktyczny;
	double ccTeoretyczny;

public:
	SiecNeuronowaCC(SiecNeuronowaDynamika &);
	~SiecNeuronowaCC();

/* ********************************************
 *
 * gettery
 *
 * ********************************************/

	inline SiecNeuronowaDynamika & getN(){
		return n;
	}

	inline int getCCRozladLen(){
		return ccRozkladLen;
	}

	inline float * getCCRozlad(){
		return ccRozklad;
	}

	inline Neuron & getNeuron(int i){
		return n.getNeuron(i);
	}

	inline int getIloscNeuronow(){
		return n.getIloscNeuronow();
	}
	inline Polaczenie & getKrawedz(int i){
		return n.getKrawedz(i);
	}

	inline int getIloscKrawedzi(){
		return n.getIloscKrawedzi();
	}



	/* ****************************************************
	 *
	 * Wspolczynnik skupienia
	 *
	 * ****************************************************/

public:
	float ccDlaStopniaX(int stopien);
	void obliczRozkladCCDlaGrafu();
	void zapiszRozkladCCStopniDoPliku();



private:
	float obliczCCtylkoKrawedzieWychodzace(int i);
	void obliczFaktycznyITeoretycznyCCdlaCalejSieci();

//	/* liczenie wsp skupienia neuronu itego
//	 * WS = CC = Clustering Coeficiant
//	 */
//	float obliczWspolczynnikSkupieniaNeuronu(int );
//	float obliczWspolczynnikSkupieniaNeuronu(int , bool );
//
//	float obliczWspolczynnikSkupieniaNeuronuTylkoKolorowaneKrawedzie(int );

//
//public:
//
//	/* liczenie cc dla grafu powstalego z krawedzi pokolorowanych
//	 * patrz SN::kolorujDrzewka()
//	 */
//	float obliczWspolczynnikSkupieniaGrafuPrzeplywuLadunku();
//
//	/* liczenie sredniego wsp skupienia dla calej sieci
//	 */
//	float obliczWspolczynnikSkupieniaSieci();
//	void obliczRozkladWspolczynnikaSkupienia();

};

#endif /* SIECNEURONOWACC_H_ */

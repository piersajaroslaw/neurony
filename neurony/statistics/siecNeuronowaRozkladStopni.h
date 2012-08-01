/*
 * siecNeuronowaRozkladStopni.h
 *
 *  Created on: Aug 28, 2010
 *      Author: piersaj
 */

#ifndef SIECNEURONOWAROZKLADSTOPNI_H_
#define SIECNEURONOWAROZKLADSTOPNI_H_

#include "../siecNeuronowaDynamika.h"


class SiecNeuronowaRozkladStopni {
private:
	SiecNeuronowaDynamika & n;
	/* ilosc wpisow w tablicy
	 */
	int len;

	float mnkA;
	float mnkB;

	// ro liczenia rozkladu
	float *rozkladStopniX, *rozkladStopniY;

	// a to do rysowania w podzialce log-log
	float *ddLogX, *ddLogY;

	/*
	 *  Konstruktory
	 */

public:
	SiecNeuronowaRozkladStopni(SiecNeuronowaDynamika &n_);
	~SiecNeuronowaRozkladStopni();

	/*
	 * settery i gettery
	 */

	float *getDdLogX() const{
		return ddLogX;
	}

	float *getDdLogY() const{
		return ddLogY;
	}

	float getDdLogX(int i) const{
		return ddLogX[i];
	}

	float getDdLogY(int i) const{
		return ddLogY[i];
	}



	int getLen() const{
		return len;
	}

	float getMnkA() const{
		return mnkA;
	}

	float getMnkB() const{
		return mnkB;
	}

	SiecNeuronowaDynamika & getN() const{
		return n;
	}

	float *getRozkladStopniX() const{
		return rozkladStopniX;
	}

	float *getRozkladStopniY() const{
		return rozkladStopniY;
	}


	/* **********************************
	 *
	 * Liczenie rozkladu
	 *
	 * **********************************/


	/* przelicza koncowe tablice
	 */
	void obliczTablicePrzplywu();

	/*
	 * Liczy parametry prostej Metoda najmniejszych kwadratow
	 */
	void obliczProstaMNK(double, double);

	/*
	 *
	 */
	void obliczNajlepszeDopasowanieMNK();

	/*
	 *
	 */
	void zapiszRozkladStopniDoPliku();
};

#endif /* SIECNEURONOWAROZKLADSTOPNI_H_ */

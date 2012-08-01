/*
 * pairOfInts.h
 *
 *  Created on: May 21, 2009
 *      Author: piersaj
 *
 *  para intow do kolejki priorytetowej
 *  jeden jest kluczem a drugi wartoscia (indeksem tego co ma dany klucz)
 *
 *  2009-05-21
 *  	utworzenie
 */

#ifndef PAIROFINTS_H_
#define PAIROFINTS_H_

struct pairIntInt{
	int klucz, wartosc;

	pairIntInt(int _klucz, int _wartosc){
		klucz = _klucz;
		wartosc = _wartosc;
	}	// this

	inline bool operator<(const pairIntInt &y) const{
		return klucz < y.klucz;
	}



};	// struct


//inline bool porownaj(const pairIntInt &x, const pairIntInt &y){
//	return x.klucz < y.klucz;
//}



#endif /* PAIROFINTS_H_ */

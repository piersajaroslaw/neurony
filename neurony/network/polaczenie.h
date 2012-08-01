/* polaczenie.h
 *
 *  Created on: 07-Feb-2009
 *  	przerzucono z innego pliku
 *  2009-04-03
 *  	dodana flaga czy krawedz jest w objeta losowaniem
 *  2009-05-21
 *  	dodano indeks krawedzi i porownywanie abs(przeplyw przez krawedz)
 *  2009-05-29
 *  	korekty do koloru - wyrzucone 3 floaty (12B), zamiast nich jest jeden short (2B) :))
 *
 *  Author: piersaj
 */

#ifndef POLACZENIE_H_
#define POLACZENIE_H_

#include "../../src/funkcjeLosowe.h"

/* Polaczenie miedzy dwoma neuronami
 * neurony pamietane jako indeksy (a nie jako wskazniki / referencje)
 * do tego pamieta rowniez ladunek, jaki przeplynal p krawedzi (w obie strony oddzielnie)
 * dorzuciny kolor drzewka do ktorego krawedz nalezy
 *
 * nie pamieta - wskaznikow na neurony, dlugosci polaczenia
 *
 * przed skorzystaniem zainicjowac generator liczb losowych
 *
 * ze wzgledu na liczebnosc krawedzi powinno byc wzglednie pamieciooszczedne
 *
 *
 * short ma max 32k  (ushort 64k) - za malo na indeksy neuronow i krawedzi - o ladunek tez sie torche boje
 *
 */

inline int myAbs(int a){
	if (a >=0)
		return a;
	else
		return -a;
}	// abs


class Polaczenie{
private:
	// indeks krawedzi
	int i;
	// indeksy wezlow koncowych
	int indeks1, indeks2;
	// ladunek jaki przeplywal w obie strony (to chyba mozna zrobic na short)
	int ladunekDo2, ladunekDo1;
	// waga krawedzi
	float waga;
	// kolor krawedzi (char??)
	short int kolor;
	// czy krawedz moze byc wylosowana w symulacji do przeplywu ladunku
	bool czyWPuliLosowanych;

public:

	Polaczenie(int i_, int indeks1_, int indeks2_):
		i(i_),
		indeks1(indeks1_),
		indeks2(indeks2_),
		ladunekDo2(0),
		ladunekDo1(0),
		waga(losujRNorm()),
		kolor(0),
		czyWPuliLosowanych(0)

	{
		if (indeks1_ == indeks2_)
			waga = 0;
	}	// konstruktor(int x3)

	inline int getIndeks1(){
		return indeks1;
	}

	inline int getIndeks2(){
		return indeks2;
	}

	inline int getladunekDo1() const{
		return ladunekDo1;
	}

	inline int getladunekDo2() const{
		return ladunekDo2;
	}

	inline void zwiekszLadunekDo1(long oIle){
		ladunekDo1 += oIle;
	}

	inline void zwiekszLadunekDo2(long oIle){
		ladunekDo2 += oIle;
	}

	inline float getWaga(){
		return waga;
	}

	inline bool getCzyWPuliLosowanychKrawedzi() const{
		return czyWPuliLosowanych;
	}

	inline void setCzyWPuliLosowanychKrawedzi(bool val){
		 czyWPuliLosowanych = val;
	}

	/* kolor 0 <==> niepokolorowana!!!
	 */
	inline void setKolor(short k){
		kolor = (short) k;
	}	// setKolor

	/* kolor 0 <==> niepokolorowana!!!
	 */
	inline int getKolor(){
		return (int) kolor;
	}

	inline int getI(){
		return i;
	}

	inline void setI(int i_){
		i = i_;
	}

	inline int getRoznicaLadunku(){
		return myAbs(ladunekDo1 - ladunekDo2);
	}

	inline int getSumaLadunku(){
		return ladunekDo1 + ladunekDo2;
	}
};	// class Polaczenie

#endif /* POLACZENIE_H_ */

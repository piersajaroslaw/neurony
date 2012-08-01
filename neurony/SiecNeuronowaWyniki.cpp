/*
 * SiecNeuronowaWyniki.cpp
 *
 *  Created on: May 21, 2009
 *      Author: piersaj
 *
 *
 * 2009-03-21
 *  	korekty do mnk
 * 2009-04-17
 *		korekty do MNK: ignorowanie wpisow skrajnych (pozniej usuniete)
 * 2009-04-19
 * 		korekty do mnk: dodane progi startowy i koncowy, wpisy w teblicy juz nie sa zerowane
 * 		gdy sa poza progiem (usuniete)
 * 2009-05-04
 * 		szukanie drzewek w grafie
 * 2009-05-21
 * 		MNK i drzewka polecialy do osobnego (znaczy sie tego!) pliku -
 * 		w starym zostala sie tylko dynamika
 * 		Drzewka liczone za pomca kolejki priorytetowej (zamiast stosu / kolejki)
 * 		korekty do funkcji kolorujacej drzewka
 * 2009-05-22
 * 		dlugawe i malo produktywne "popraw, skomplikuj, uruchom, poczekaj, obejzyj, znajdz blad, popraw..."
 * 		korekty do drzewek
 * 2009-05-30
 * 		wspolczynniki skupienia aka clustering coefitions
 * 		testowanie istnienia i zwracanie krawedzi miedzy zadana para wezlow
 * 		z tablic przeplywu usunieto wpisy z zerami :))
 * 		funkcja okreslajaca kolor drzewka wyrzucona poza klase SN
 * 2009-06-01
 * 		malutka korekta do wspolczynnika sklasteryzowania - liczona klika _nie_uwzglednia_ centralnego wezla
 * 2009-06-19
 * 		liczenie wykresu cc w zaleznosci od progu
 * 2009-07-09
 * 		liczenie cc na grafie przeplywu impulsow
 * 2009-12-07
 * 		:-))
 * 		to nie jest 5 miechow nic-nie-robienia, ot po prostu nie mialem czasu uzupelnic tych wpisow
 * 		kilka korekt do liczenia wsp klasteryzacji, to srednia, to tylko wyzsze, to szybsze, to gorsze itp.
 * 2010-08-29
 * 		to tez nie jest 8 miesiecy nic-nie-robienia (chociaz moze...) po co porawiac cos co dziala
 * 		... zeby bylo czytelniejsze - czesc zadac pliku przechodzi do innej klasy!
 * 		liczenie DegDistr <- na poczatek
 */

#include <cstdio>

#include "network/siecNeuronowa.h"
#include "pairOfInts.h"
#include "../src/logger.h"


/* ******************************************************
 *
 * MNK
 *
 * ******************************************************/


long SiecNeuronowa::znajdzMaksymalnyLadunekPrzeplywajacyPrzezNeurony(){
	long max = 0;

	for (int i=0;i< iloscNeuronow; i++){
		long c = getNeuron(i).getLadunekWejsciowy() /* + getNeuron(i).getLadunekWyjsciowy() */;
		if (max < c){
			max = c;
		}	// if
	}	// for i

	return max;
}	// znajdzMaksymalnyLadunekNeuronu()



/* ********************************************
 *
 *  Drzewka
 *
 * **************************************/

// numer <==> kolor drzewka
// nie dotykać
int in = 1;


/* kolorowanie drzewka kolorem wynikajacy z indeks (tj numer drzewka)
 *
 */
void SiecNeuronowa::kolorujDrzewko(int indeks){
	if (getNeuron(indeks).getCzyOdwiedzony()){
		// juz pokolorowany
		return;
	}	// fi

	int il=0;
	in++ ;

	std::priority_queue< pairIntInt, std::vector<pairIntInt >  > kolejka;
	Neuron & n = getNeuron(indeks);

	// na poczatek wrzucamy wszystko co wchodzi do neuronu startowego
	for (int j=0; j< n.getIloscSasiadow(); j++){
		// to nie jest indeks sasiada ale indeks krawedzi
		int p = n.getIndeksKrawedzi(j);
		Polaczenie & k = getKrawedz(p);

		if (k.getKolor()!=0 /* || k.getKolorG()!=0 ||  k.getKolorB()!=0 */)
			continue;

		// jezeli przeplyw przez krawedz jest DO neuronu to wrzucamy do kolejki
		if (   (k.getIndeks1() == indeks && k.getladunekDo1() > k.getladunekDo2())
				|| (k.getIndeks2() == indeks && k.getladunekDo2() > k.getladunekDo1()) ){


			kolejka.push( pairIntInt( k.getRoznicaLadunku(), k.getI()) );
		}	// fi
	}	// for


	while (! kolejka.empty()){
		pairIntInt p = kolejka.top();
		kolejka.pop();

		Polaczenie &k = getKrawedz( p.wartosc );

		if (p.wartosc != k.getI()){
			std::cerr << "kolorowanie(): niezgodne indeksy" << p.wartosc << " " << k.getI() << "\n";
		}	// if

		// logJP << "pair " << p.klucz << " " << p.wartosc << "\n";
		// logJP << p.klucz << " " << kolejka.top().klucz << "\n";

		if (k.getKolor()!=0){
			continue;
		}	// fi

		//kolorujemy krawedz
		if (in-1 == 0){
			// takie zabezpieczenie
			std::cerr << "kolorujDrzewko(): kolor (in-1) == 0\n";
		}	// if

		k.setKolor(in-1);
		il++;

		// jezeli koniec to czyscimy kolejke
		if (il > wielkoscDrzewka){
			while (!kolejka.empty())
				kolejka.pop();
			return;
		}	// if


		// 2 <--k-- 1
		// szukamy sasiada tj 1
		int n1 = (k.getladunekDo1() > k.getladunekDo2() ? k.getIndeks2() : k.getIndeks1());
		Neuron & s1 = getNeuron(n1);

		// dodajemy krawedzie _do_ sasiada (tj do s1) do kolejki
		for (int j=0; j< s1.getIloscSasiadow(); j++){

			// to nie jest indeks sasiada ale indeks krawedzi
			int p = s1.getIndeksKrawedzi(j);
			Polaczenie & ks = getKrawedz(p);

			if (ks.getKolor()!=0  /* || ks.getKolorG()!=0 ||  ks.getKolorB()!=0 */ )
				continue;

			// jezeli przeplyw przez krawedz jest DO neuronu n1 to...
			if (   (ks.getIndeks1() == n1 && ks.getladunekDo1() > ks.getladunekDo2())
					|| (ks.getIndeks2() == n1 && ks.getladunekDo2() > ks.getladunekDo1()) ){
				if (ks.getKolor()<=0 /* && ks.getKolorG()<=0 &&  ks.getKolorB()<=0 */){
					kolejka.push( pairIntInt( ks.getRoznicaLadunku(), ks.getI()) );
				}	// if krawedz niepokolorowana(jeszcze)
			}	// fi
		}	// for
	}	// while
}	// kolorujDrzewko


/* kolorowanie drzewek
 *
 */
void SiecNeuronowa::kolorujDrzewka(){
	int tab[iloscDrzewek];

	for (int i=0; i<iloscDrzewek; i++){
		tab[i] = -1;
	}	// for

	for (int i=0; i<iloscDrzewek; i++){
		int wierzcholek = znajdzNiepokolorowanyWierzcholek();
		kolorujDrzewko(wierzcholek);
		getNeuron(wierzcholek).setCzyOdwiedzony(true);
		tab[i] = wierzcholek;
	}	// for i
}	// void kolorujDrzewka();


/* znajduje niepokolorowanue wierzcholek o najwiekszym ladunku
 *
 */
int SiecNeuronowa::znajdzNiepokolorowanyWierzcholek(){
	int ret = 0;

	double najlepszy = -100000;

	for (int i=0; i<iloscNeuronow; i++){
		if (!getNeuron(i).getCzyOdwiedzony() && getNeuron(i).getLadunek()> najlepszy){
			najlepszy = getNeuron(i).getLadunek();
			ret = i;
		}	// fi
	}	// for

	return ret;
}	// znajdzNiepokolorowanyWierzcholek()



/* taki dosc naiwny generator kolorow
 *
 * 2009-05-30
 * 		funkcja wyrzucona poza klase SiecNeuronowa
 */
void okreslKolorDrzewka(int indeks, float &r, float &g, float &b){
	r = g = b = 1;
	int d = 1;

	if (indeks & d)
		r -= .5;
	d <<= 1;

	if (indeks & d)
		g -= .5;
	d <<= 1;

	if (indeks & d)
		b -= .5;
	d <<= 1;

	if (indeks & d)
		r -= .25;
	d <<= 1;

	if (indeks & d)
		g -= .25;
	d <<= 1;

	if (indeks & d)
		b -= .25;

}	// okreslKolorDrzewka(int, float&, float&, float&)

/* ******************************************************************
 *
 * Wspolczynnik skuipienia
 *
 * ***************************************************************/

//float SiecNeuronowa::obliczWspolczynnikSkupieniaNeuronuTylkoKolorowaneKrawedzie(int i){
//	return obliczWspolczynnikSkupieniaNeuronu(i, true );
//}
//
//float SiecNeuronowa::obliczWspolczynnikSkupieniaNeuronu(int i){
//	return obliczWspolczynnikSkupieniaNeuronu(i, false );
//}
//
//
///* cc dna neuronu
// */
//float SiecNeuronowa::obliczWspolczynnikSkupieniaNeuronu(int i, bool tylkoPokolorowaneKrawedzie ){
//	Neuron& n1 = getNeuron(i);
//	int t = 0;
//
//	for (int l=0; l< n1.getIloscSasiadow(); l++){
//		// marker ze tutaj ostatnio _cos_ majstrowalismy
//		// konkretniej dorzucilismy obsluge flagi czy zbierac dane z kawedzi kolorowanych
//		// czy o wysokim przeplywie
//
//		// czy krawedz bedzie liczona do wspolczynnika skupienia
//		if (! tylkoPokolorowaneKrawedzie ) {
//			if	( getKrawedz(n1.getIndeksKrawedzi(l)).getSumaLadunku() >= wspSkupieniaProgKrawedzi){
//				t++;
//			}	// if
//		} else {
//			if ( getKrawedz(n1.getIndeksKrawedzi(l)).getKolor() >= 1 ){
//				t++;
//			}	// if
//		}	// if .. else
//	}	// for
//
//	// warunki brzegowe
//	// brak sasiadow - nie liczymy
//	if (t==0){
//		return -1.0;
//	}	// if
//
//	// jeden sasiad - zero
//	// inne opcje:: -1 - zignoruj neuron // 0 - niesklasteryzowany // +1 całkowicie sklasteryzowany
//	if (t==1){
//		return 0.0;
//	}	// fi
//
//	// tu beda indeksy tych neuronow - sasiadow
//	int indeksySasiadow[t];
//	int it = 0;
//
//	for (int l=0; l< n1.getIloscSasiadow(); l++){
//		Polaczenie & k = getKrawedz(n1.getIndeksKrawedzi(l));
//		if (  (! tylkoPokolorowaneKrawedzie && k.getSumaLadunku() >= wspSkupieniaProgKrawedzi)
//				|| ( tylkoPokolorowaneKrawedzie &&  k.getKolor()>=1 )){
//			indeksySasiadow[it] = k.getIndeks1() == i ? k.getIndeks2() : k.getIndeks1();
//			it++;
//		}	// if
//	}	// for
//
//	// liczymy ile jest krawedzi w klice
//	int ilosc = 0;
//	int iloscTemp1 = 0, iloscTemp2 = 0;
//	// korekta liczymy tylko sasiadow ze sasiadow
//	for (int l=0; l<t; l++){
//		for (int k = l+1; k<t; k++){
//			iloscTemp1++;
//
//			if (czyIstniejeKrawedzMiedzyNeuronami(indeksySasiadow[l] , indeksySasiadow[k] )){
//
//				iloscTemp2 ++;
//
//				int ik = getIndeksKrawedziMiedzyNeuronami(indeksySasiadow[l] , indeksySasiadow[k] );
//				Polaczenie &k = getKrawedz(ik);
//
//				if (( (! tylkoPokolorowaneKrawedzie) && k.getSumaLadunku() >= wspSkupieniaProgKrawedzi)
//					|| (tylkoPokolorowaneKrawedzie &&  k.getKolor() >=1 )) {
//					ilosc ++;
//				}	// if
//			}	// if
//
//		}	// for k
//	}	// for l
//
//	/*  ilosc
//	 * --------
//	 *  t(t-1) / 2
//	 */
//
////	if (tylkoPokolorowaneKrawedzie == false && wspSkupieniaProgKrawedzi == 0 )
////		logJP << i << " " << ilosc << " " << iloscTemp1 << " "<< iloscTemp2 << " " << t << " " << ilosc *2.0 / t / (t-1)  << "\n";
//
//	return (ilosc * 2.0) / t / (t-1);
//}	// obliczWspolczynnikSkupieniaNeuronu
//
//
//
///* liczenie sredniego wsp skupienia dla calej sieci
// *
// * z wszystkich krawedzi o jakimkolwiek przeplywie??
// */
//float SiecNeuronowa::obliczWspolczynnikSkupienia(){
//	float suma = 0;
//	int ilosc = 0;
//
//	// wersja inteligentniejsza - omijamy to co daje zero bo tak naprawde _nie_ma_
//	// krawedzi o tak wysokim przeplywie (jest ich ~ prog^(-2), czytaj malo)
//	for (int i=0; i<iloscNeuronow; i++){
//		float a = obliczWspolczynnikSkupieniaNeuronu( i );
//		if (a>=0){
//			suma +=a;
//			ilosc++;
//		}	// if
//	}	// for i
//
//	float ret = 0.0;
//	if (ilosc == 0){
//		ret = -1.0;
//	} else {
//		ret = suma / ilosc;
//	}	// if .. else
//
//	return ret;
//}	// obliczWspolczynnikSkupienia()


/* liczenie cc dla sieci losowej o odpowiadajacej ilosci krawedzi
 */

 /* nieaktualne
float SiecNeuronowa::obliczWspolczynnikSkupieniaOdpowiadajacejSieciLosowej(){
	logJP << "SN::obliczWspolczynnikSkupieniaOdpowiadajacejSieciLosowej(): a jednak zostalam wywolana - usunac mnie\n";
	setWspSkupieniaProgKrawedzi(0);

	int max = 1000;

	float suma = 0;
	for (int i=0; i<max; i++){
		suma += obliczWspolczynnikSkupieniaNeuronu( losujRInt( iloscNeuronow ) );
	}	// for i

	logJP << "wspSkupienia W Sieci Losowej = " << wspSkupieniaProgKrawedzi  << "\n";

	return suma / max;
}	// obliczWspolczynnikSkupieniaOdpowiadajacejSieciLosowej()
 */

//void SiecNeuronowa::obliczWykresWspolczynnikowSkupienia(){
//	int tmp = getWspSkupieniaProgKrawedzi();
//	float x;
//
//	int i=0;
//
//
//	printf("obliczWykresWspolczynnikowSkupienia(): zakomentowalismy liczenie cc\n");
//	while (i>=0){
//		setWspSkupieniaProgKrawedzi(i);
//		// TODO odznaczyc gdy wrocimy do rzadkich grafow
////		x = obliczWspolczynnikSkupienia();
//		x = i<10 ? 1 : -1;
//
//		if (x < 0 ){
//			break;
//		}	// if
//
//		// klucz jest floatem miedzy 0 a 1!!!
//		wspolczynnikSkupienia.push_back(pairIntInt(i, 1000*x));
//
//		if (i<40)
//			i++;
//		else if (i<100)
//			i+=2;
//		else if (i<100)
//			i+=5;
//		else if (i<500)
//			i+=10;
//		else if (i<2000)
//			i+=100;
//		else
//			i+=500;
//
//	}	// while
//
//	setWspSkupieniaProgKrawedzi(tmp);
//}	// obliczWykresWspolczynnikowSkupienia

/* liczenie cc na grafie powstalym z najbardziej aktywnych krawedzi, ktorymi plynie
 * ladunek do elity (patrz kolorowanie drzewek - te krawedzie ktore sa pokolorowane)
 */
//float SiecNeuronowa::obliczWspolczynnikSkupieniaGrafuPrzeplywuLadunku(){
//	if (1==1){
//		printf("obliczWspolczynnikSkupieniaGrafuPrzeplywuLadunku(): pomijamy te obliczenia\n");
//		return -1;
//	}	// fi
//
//	int ilosc = 0;
//	float suma = 0.0;
//
//	for (int i=0; i< iloscNeuronow; i++){
//		float v = obliczWspolczynnikSkupieniaNeuronuTylkoKolorowaneKrawedzie(i);
//		if (v >=0){
//			ilosc++;
//			suma += v;
//		}	// if
//	}	// for i
//
//	if (ilosc == 0){
//		return -1;
//	}	// if
//
//	// liczenie cc dla grafu losowego o takowej strukturze.
//	// tyle jest krawedzi w grafie
//	int iloscKr = 0;
//	for (int i=0; i< iloscKrawedzi; i++){
//		if (getKrawedz(i).getKolor() >=1 )
//			iloscKr ++;
//	}	// for i
//
//	float p = iloscKr * 2.0  / ilosc /(ilosc -1);
//	logJP<< "CC teoretyczny      == "  <<  p << "\n";
//	logJP<< "CC faktyczny        == "  << (suma / ilosc) << "\n";
//
//	return suma / ilosc;
//}	// obliczWspolczynnikSkupieniaGrafuPrzeplywuLadunku();

/* **************************************************************************
 *
 * wsp skupienia dotyczacy stopni
 *
 *
 *
 * ************************************************************************/
int SiecNeuronowa::stopienWejsciowy(int indeks){
	return inDeg(indeks);
}

int SiecNeuronowa::inDeg(int indeks){
	int ret = 0;
	Neuron n = getNeuron(indeks);

	for (int i=0; i < n.getIloscSasiadow(); i++){
		/* uwaga moze byc 1000:1 a i tak bedzie liczone jako 1 wiec ostroznie z tym!!
		 */
		Polaczenie p  = getKrawedz( n.getIndeksKrawedzi(i) );
		if (p.getIndeks1() == indeks && p.getladunekDo1() >=1 ){
			ret ++;
		} else if (p.getIndeks2() == indeks && p.getladunekDo2() >=1 ){
			ret ++;
		}	// if

	}	// for

	return ret;
}	// inDeg

int SiecNeuronowa::stopienWyjsciowy(int indeks){
	return outDeg(indeks);
}

int SiecNeuronowa::maksymalnyStopienWejsciowy(){
	int ret = 0;

	for (int i=0; i< iloscNeuronow; ++i){
		int temp = inDeg(i);
		if (temp >ret)
			ret  = temp;
	}	// for
	return ret;
}	// maksymalnyStopienWejsciowy()

int SiecNeuronowa::maksymalnyStopienWyjsciowy(){
	int ret = 0;

	for (int i=0; i< iloscNeuronow; ++i){
		int temp = outDeg(i);
		if (temp >ret)
			ret  = temp;
	}	// for
	return ret;
}	// maksymalnyStopienWejsciowy()

/* stopień wyjściowy wierzchołka ale liczony wg przepływu w symulacji tj przez ile krawedzi wyplynal
 * ladunek
 */
int SiecNeuronowa::outDeg(int indeks){
	int ret = 0;
	Neuron n = getNeuron(indeks);

	for (int i=0; i < n.getIloscSasiadow(); i++){
		Polaczenie p  = getKrawedz( n.getIndeksKrawedzi(i) );
		/*  patrz uwaga wyzej
		 */
		if (p.getIndeks1() == indeks && p.getladunekDo2() >=1 ){
			ret ++;
		} else if (p.getIndeks2() == indeks && p.getladunekDo1() >=1 ){
			ret ++;
		}	// if
	}	// for

	return ret;
}	// inDeg

/* zwraca średni stopień współczynnika klasteryzacji dla neuronów o stopniu st
 * -1 jeżeli takowych nie ma
 *
 * nie korzystac w przypadk obliczen hurtowych
 * zrobic to inteligentniej tj przejechac po neuronach zapamietac ilosci i sume cc!!!
 * patrz void ccRozkladDlaStopniX(void)
 */
//float SiecNeuronowa::ccDlaStopniaX(int stopien){
//
//	// depracated
//	printf("\nSN.ccDlaStopniaX(int): ktos mnie wywołał nie wiem po co\n");
//
//
//	float suma = 0;
//	int ilosc = 0;
//
////	setWspSkupieniaProgKrawedzi(1);
//	for (int i=0; i< iloscNeuronow; i++){
//		if (outDeg(i) == stopien){
////			suma += obliczWspolczynnikSkupieniaNeuronu(i , false);
//			float ret = obliczCCtylkoKrawedzieWychodzace(i);
//
//			if (ret >=0){
//				ilosc ++;
//				suma += ret;
//			}	// if
//
//		}	// fi
//	}	// for i
//
//	if (ilosc == 0){
//		return -1;
//	} else {
//		return suma / ilosc;
//	}	// fi
//}	// ccDlaStopniaX


/* liczenie sredniego cc dla neuronow o stopniu >= s=1..max - wszystkich na raz!
 *
 * tu gdzies trzeba bedzie wkleic liczenie ilosci wezlow w zaleznosci od cc!
 *
 * wypelnia tablice ccRozkladLen!
 * -1 stopien nie istnieje w sieci albo nie da sie policzyc cc!
 */
//void SiecNeuronowa::ccRozkladDlaStopniX(){
//
//	printf("ccRozkladDlaStopniX()\n");
//
//	ccRozkladLen = 1000;
//	ccRozklad = new float[ccRozkladLen];
////	int * ccIlosc = new int[ccRozkladLen];
//
//
//	for (int i=0; i<ccRozkladLen; i++){
//		ccRozklad[i] = 0;
////		ccIlosc[i] = 0;
//	}	// for
//
//	for (int i=0; i< iloscNeuronow; ++i){
//		float cc = obliczCCtylkoKrawedzieWychodzace(i);
//		int val = round(cc * ccRozkladLen);
//		// taki failsafe - moze trochę kontrowersyjny
//		if (val < 0) {
//			val = 0;
//		} else if (val >= ccRozkladLen){
//			val = ccRozkladLen-1;
//		}	// fi
//
//		ccRozklad[val]++;
//	}	// for i neurony
//
//	// ewentualnie zrobic wersje kumulatywna i loglog
//
//	printf("ccRozkladDlaStopniX(): koniec!\n");
//}	// ccRozkladDlaStopniX()



/* NIEAKTUALNE!!!
 */
/*
void SiecNeuronowa::ccRozkladDlaStopniX___OLD(){

	printf("ccRozkladDlaStopniX()\n");

	ccRozkladLen = maksymalnyStopienWyjsciowy()+1;
	ccRozklad = new float[ccRozkladLen];
	int * ccIlosc = new int[ccRozkladLen];

	ccRozklad[0] = 0;
	ccRozklad[1] = 0;

	for (int i=0; i<ccRozkladLen; i++){
//		ccRozklad[i] = ccDlaStopniaX(i);
		ccRozklad[i] = 0;
		ccIlosc[i] = 0;
	}	// for

	for (int i=0; i< iloscNeuronow; ++i){
		int stopien = outDeg(i);
		float cc = obliczCCtylkoKrawedzieWychodzace(i);

		if (cc >=0){
			ccIlosc[stopien] ++;
			ccRozklad[stopien] += cc;
		}	// if
	}	// for i neurony


	// Tu korekta - liczymy sredni wspolczynnik ale dla wszystkiego co jest powyzej i


	float suma = 0;
	int ilosc = 0;
	for (int i=ccRozkladLen; i>=0; i--){

//		if (ccIlosc[i] > 0){
//			ccRozklad[i] = (ccRozklad[i]  + suma ) / (ilosc + ccIlosc[i]);
//		} else {
//			ccRozklad[i] = ccRozklad[i+1];
//		}	// fi

		suma += ccRozklad[i];
		ilosc += ccIlosc[i];

		if (ilosc >0){
			ccRozklad[i] = suma / ilosc;
		} else {
			ccRozklad[i] = 0;
		}
	}	// for i

//	for (int i=1; i<ccRozkladLen; i++){
//		if (ccIlosc[i] <=0){
//			ccRozklad[i] = -1;
//		} else {
//			ccRozklad[i] /= ccIlosc[i];
//		}	// if else
//	}	// for

	printf("ccRozkladDlaStopniX(): koniec!\n");
}	// ccRozkladDlaStopniX___OLD()


*/ // koniec starej metody

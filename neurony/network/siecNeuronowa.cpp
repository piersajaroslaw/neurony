/*
 * siecNeuronowaDefinicja.cpp
 *
 *  Created on: Jun 30, 2010
 *      Author: piersaj
 *
 * kompilowac z -lpthread
 *
 * 2010-06-30
 * 		przeniesiono z innego fragmentu kodu
 * 2010-07-05
 * 		sortowanie krawedzi po generowaniu w watkach (mam nadzieje)
 * 2010-07-06
 * 		poprawione sortowanie (  Neuron & n;    bylo      Neuron n;)
 * 		zakresy generowania krawedzi
 * 2010-07-07
 * 		poprawione zakresy
 *  2010-07-09
 *  	dodany parametr kontrolujacy ilosc waatkow
 *  2010-08-28
 *  	refactoring klasy
 *  	DegDist / CC / Dynamika / liczenie czesci histogramow zostalo przeniesione do nowych,
 *  	wyspecjalizowanych klas lub prozaicznie usuniete
 * 	2011-04-07
 * 		loool
 * 		dodany konfig w xmlu
 * 	2012-04-11
 * 		Gdzies w miedzyczasie dodane geometrie ER oraz siatki 2d
 * 	2012-07-20
 * 		Fixed minor (??) bug with calculation of the number of neurons
 *
 * TODO: przyspieszyc sortowaania  sortowanie szybkie! i na watkach
 *
 */

#include <pthread.h>
#include "siecNeuronowa.h"
#include <cstdio>
#include <cstdlib>
//#include "parserKonfiguracji.h"



void * wypelnijPolaczenia(void *);
void wypelnijPolaczeniaNaWatkach();


long ladunekSrednia = 2000;
long ladunekWariancja = 100;
int NUM_THREADS = 2;


//nie dotykac, wszystkie watki maja operowac na tej sieci!
SiecNeuronowa * bierzacaSiecDlaWatkowGenerujacych = NULL;


/* Konstruktor
 */
SiecNeuronowa::SiecNeuronowa(SimulationConfig * conf_):
	conf(conf_),
	alpha(conf->getConnectivityAlpha()),
	maxPos(conf->getRadius() /* *2 */),
	iloscKrawedzi(0),
	typ(conf->getGeometry()),
	gestosc(conf->getNeuronsDensity()),
	sumaLadunku(0L),
	energia(0),
	beta(conf->getInverseTemperatureBeta()),
	iloscDrzewek(4),
	wielkoscDrzewka(100)
//	progDlugosciKw(-1)
{

	NUM_THREADS = conf->getThreads_number();

	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	timestamp = new char[80];
	strftime (timestamp,80,"%Y_%m_%d_%H_%M_%S",timeinfo);

	bierzacaSiecDlaWatkowGenerujacych = this;
	pthread_mutex_init(& mutexSN, NULL);
	pthread_mutex_init(& mutex_energia, NULL);


	losujZiarno();

	if (typ==KOSTKA_3D || typ == SFERA_2D || typ == KULA_3D)
		dim = 3;

	if (typ==KOSTKA_2D || typ == SFERA_1D || typ == KOLO_2D || typ == RANDOM_ER || typ == LATTICE_2D)
		dim = 2;

	if (dim == 0){
		typ = KOSTKA_3D;
		dim = 3;
	}	/// if

	if (typ == KOSTKA_2D || typ == KOSTKA_3D || typ == LATTICE_2D){
		lambda = obliczLambdaKostka();
	} else if (typ == SFERA_1D || typ == SFERA_2D){
		lambda = obliczLambdaSfera();
	} else /* typ == Kula_3d || typ == kolo_2d ||typ == RANDOM_ER */ {
		lambda = obliczLambdaKula();
	}	// if

	iloscNeuronow = losujRPoiss(gestosc * lambda);

	logJP = LoggerJP(iloscNeuronow);
	logJP << "Jaroslaw Piersa --- SiecNeuronowa symulacja" << "\n\n";


	/* Losowanie neuronów
	 *
	 */

	ladunekSrednia = conf->getInitialChargeMean();
	ladunekWariancja = conf->getInitialChargeVar();

	float x,y,z;

	for (int i=0;i< iloscNeuronow; i++){

		if (typ == KOSTKA_2D || typ == KOSTKA_3D){
			losujPozycjeSzescian(x, y, z);
		} else if (typ == SFERA_1D || typ == SFERA_2D){
			losujPozycjeSfera(x, y, z);
		}	else /* typ == KULA | KOLO | RandomER*/ {
			losujPozycjeKula(x,y,z);
		}	// if

		long ladunek = (long) fabs(ladunekWariancja*losujRNorm()+ladunekSrednia);

		sumaLadunku += ladunek;

		Neuron n = /* Neuron:: */ Neuron(i, x, y, z, ladunek);
		neurony.push_back(n);

	}	// for i



	logJP.timestamp_nl();
	logJP<< "NUM_THREADS   ==   " << conf->getThreads_number()<< "\n";
	logJP << "generowanie krawedzi:\n";
	wypelnijPolaczeniaNaWatkach();
	logJP.timestamp_nl();
	logJP << "generowanie krawedzi: koniec!\n";

	// raz!
	energia = obliczEnergie();

	/* inicjalizowanie wsparc
	 *
	 */
	maxWsparcie = 1;
	for (int i=0; i< iloscNeuronow; i++){
		if (getNeuron(i).getWsparcie()> maxWsparcie)
			maxWsparcie = getNeuron(i).getWsparcie();
	}	// for
}	// konstruktor()

/* destruktor
 *
 */
SiecNeuronowa::~SiecNeuronowa(){
	delete [] timestamp;
	neurony.clear();
	krawedzie.clear();
	pthread_mutex_destroy(& mutexSN);
	pthread_mutex_destroy(& mutex_energia);
}	// destruktor()


/* opis stanu sieci na stdout
 *
 */
void SiecNeuronowa::opiszStanSieci(){

	logJP<< "\ntyp               = "<< getTypString() << "\n";
	logJP<< "maxpos            = "<< maxPos<<"\n";
	logJP<< "dim               = "<< dim<<"\n";
	logJP<< "lambda            = "<< lambda<<"\n";
	logJP<< "zageszczenie      = "<< gestosc<<"\n";
	logJP<< "ilosc Neuronow    = "<< iloscNeuronow<<"\n";
	logJP<< "temperatura/beta  = "<< beta<<"\n";
	logJP<< "ladunek           =  N( "<< ladunekSrednia << " , "<< ladunekWariancja <<" )\n";
	logJP<< "suma Ladunku      = "<< sumaLadunku <<"\n";
	logJP<< "energia           = "<< energia <<"\n";
	logJP<< "alpha             = "<< alpha<<"\n";
	logJP<< "timestamp         = "<< timestamp <<"\n";

}	// opiszStan


Neuron& SiecNeuronowa::getNeuron(int i){
	if (i<0 || i>iloscNeuronow){
		std::cerr << "getNeuron(): niepoprawny indeks i = " << i <<"\n";
	}	// fi

	return  neurony[i];
}	// getNeuron()

Polaczenie& SiecNeuronowa::getKrawedz(int i){
	if (i<0 || i>= iloscKrawedzi){
			std::cerr << "getKrawedz(): niepoprawny indeks i = " << i <<"\n";
	}	// fi

	return krawedzie[i];
}	// getKrawedz()

/* ij - indeksy neuronow
 * metoda w miare nieglupia bo wyszukujaca binarnie
 *
 * zalozenie jest takie ze lista jest posortowana po indeksach drugiego neuronu... chyba
 *
 * niestety siec byla projektowana tak by nie byly potrzebne takie zapytania wiec teraz wyglada
 * jak wyglada (znaczy sie nieprzemyslane)
 *
 */
bool SiecNeuronowa::czyIstniejeKrawedzMiedzyNeuronami(int i, int j){
	if (i==j)
		return true;
	if (i<0 || j<0 || i>= iloscNeuronow || j>=iloscNeuronow )
		return false;

	int l=0;
	int p= getNeuron(i).getIloscSasiadow()-1;

	while (l<=p) {
		int s = (l+p)/2;

		if (s<0){
			return false;
		}
		if (s >= getNeuron(i).getIloscSasiadow()){
			return false;
		}


		int i1 = getKrawedz( getNeuron(i).getIndeksKrawedzi(s) ).getIndeks1();
		int i2 = getKrawedz( getNeuron(i).getIndeksKrawedzi(s) ).getIndeks2();

		// ten drugi neuron
		int k = (i1 ==i ) ? i2 : i1;
		if (k == j)
			return true;

		if (k < j){
			l = s+1;
		} else {
			p = s-1;
		}	// if

	} 	// while

	return false;
}	// czyIstniejeKrawedzMiedzyNeuronami

/* zwracanie indeksu krawedzi miedzy neuronami ij (tez reprezentowanymi poprzez indeksy)
 *
 * jezeli krawedz nie istnieje zwracane jest -1
 */
int SiecNeuronowa::getIndeksKrawedziMiedzyNeuronami(int i , int j ){
	if (i<0 || j<0 || i>= iloscNeuronow || j>=iloscNeuronow )
		return -1;


	int l=0;
	int p= getNeuron(i).getIloscSasiadow()-1;

	while (l<=p){
		int s = (l+p)/2;

		if (s<0){
			return -1;
		}
		if (s >= getNeuron(i).getIloscSasiadow()){
			return -1;
		}

		int i1 = getKrawedz( getNeuron(i).getIndeksKrawedzi(s)).getIndeks1();
		int i2 = getKrawedz( getNeuron(i).getIndeksKrawedzi(s)).getIndeks2();

		// ten drugi neuron
		int k = (i1 ==i ) ? i2 : i1;
		if (k == j)
			return getNeuron(i).getIndeksKrawedzi(s);

		if (k < j){
			l = s+1;
		} else {
			p = s-1;
		}	// if

	}	// while
	return -1;
} 	// getKrawedzMiedzyNeuronami(int, int)


int SiecNeuronowa::getIndeksItegoSasiada(int i1, int i){
	int ik = getNeuron(i1).getIndeksKrawedzi(i);
	if (getKrawedz(ik).getIndeks1() == i1)
		return getKrawedz(ik).getIndeks2();
	else
		return getKrawedz(ik).getIndeks1();
}	// getIndeksItegoSasiada

Neuron & SiecNeuronowa::getItySasiad(int index, int i){
	return getNeuron( getIndeksItegoSasiada(index, i));
}

SimulationConfig * SiecNeuronowa::getConfig(){
	return conf;
}

/* ****************************************
 *
 * do generowania sieci
 *
 * ***************************************/


void SiecNeuronowa::losujPozycjeSzescian(float &x, float &y, float &z){
	x = losujRUnif(maxPos);
	y = losujRUnif(maxPos);
	z = dim==3 ? losujRUnif(maxPos): 0;
}	// losujPozycjeSzescian

void SiecNeuronowa::losujPozycjeSfera(float &x, float &y, float &z){
	if (dim == 2 ){
		float alpha = losujRUnif(2 * 3.1415);
		float r = maxPos *.5;

		x = r * cos(alpha) + maxPos *.5;
		y = r * sin(alpha) + maxPos *.5;
		z = 0;
	}	else {
		// dim ==3

		float r = maxPos *.5;
		x = losujRNorm();
		y = losujRNorm();
		z = losujRNorm();

		float d = sqrt( x*x + y*y + z*z );
		if (d==0){
			x=1;
			d=1;
		}	// fi

		x = x*r/d + r;
		y = y*r/d + r;
		z = z*r/d + r;
	}	// if
}	// losujPozycjeSfera(x, y, z);

void SiecNeuronowa::losujPozycjeKula(float &x, float &y, float &z){
	z = 0;
	do {
		x = losujRUnif(maxPos);
		y = losujRUnif(maxPos);
		z = dim==3 ? (losujRUnif(maxPos)) : maxPos * .5;
	} while ((x - maxPos * .5)*(x - maxPos * .5) + (y - maxPos * .5)*(y - maxPos * .5)
			+ (z - maxPos * .5)*(z - maxPos * .5) > maxPos * maxPos * .25);
}	// losujPozycjeKula

int SiecNeuronowa::obliczLambdaKostka(){
	return dim==3 ?
			maxPos * maxPos * maxPos : maxPos * maxPos;
}	// oblicz lambda

int SiecNeuronowa::obliczLambdaSfera(){
	// r == maxpos/2
	// 2 pi R
	// 4 pi R^2

	if (dim==2 )
		return (int) ( maxPos * 3.1415);
	else
		return (int) ( maxPos * maxPos *3.1415);
}	// oblicz lambda


int SiecNeuronowa::obliczLambdaKula(){
	// r = maxdim/2
	// pi r^2
	// pi r^3 * 4/3
	if (dim==2 )
		return (int) ( maxPos * maxPos * 3.1415 * .25);
	else
		return (int) ( maxPos * maxPos * maxPos *3.1415 * 4 *.33 * .125);

}

/* ***********************************************************
 *
 *  gettery / settery do pol klasy
 *  tylko te ktore nie sa inline
 *
 *  jak widac wszystkie sa inline :P
 *
 * ***********************************************************/


/* *****************************************************
 * uwatkowienie generowania sieci
 *
 *
 * ****************************************************/





/* wypelnianie polaczen wersja zrownoleglona
 */
void wypelnijPolaczeniaNaWatkach(){
	pthread_t threads[NUM_THREADS];
	int rc;
	long t;
	for(t=0; t<NUM_THREADS; t++){
		rc = pthread_create(&threads[t], NULL, wypelnijPolaczenia, (void *)t);

		if (rc){
			printf("wypelnijPolaczeniaNaWatkach(): ERROR return code from pthread_create() is %d\n", rc);
			exit(-1);
		}	// if
	}	// for

	for(t=0; t<NUM_THREADS; t++){
		pthread_join(threads[t], NULL);
	}	// for

	SiecNeuronowa & n =  *bierzacaSiecDlaWatkowGenerujacych;

	logJP.timestamp_nl();

	for (int i=0; i<n.getIloscNeuronow(); i++){
		n.sortujKrawedzieWNeuronie(i);
	}	// for


	int ilosc2 = 0;
	for (int i=0; i< n.getIloscNeuronow(); i++){
		for (int j=i; j<n.getIloscNeuronow(); j++){
			if (n.czyIstniejeKrawedzMiedzyNeuronami(i, j))
				ilosc2++;
		}	// for j
	}	// for i

	printf("po sortowaniu krawedzi powinno byc %d jest %d \n\n", n.getIloscKrawedzi(), ilosc2);

}	// wypelnijPolaczeniaNaWatkach


/* funkcja watkowa - nie dotykac
 * powinna byc wywolywana z
 * pthread_create(... , wypelnijPolaczenia , ... )
 */
void * wypelnijPolaczenia(void *threadid){
	long tid;
	tid = (long)threadid;

//	int min = bierzacaSiecDlaWatkowGenerujacych->getIloscNeuronow() * tid / conf->getThreads_number();
//	int max = (bierzacaSiecDlaWatkowGenerujacych->getIloscNeuronow() * (tid+1) / conf->getThreads_number()) -1;

	int min = tid == 0 ? 0 :
		bierzacaSiecDlaWatkowGenerujacych->obliczPrzedzialKrawedziDlaWatku(tid-1)+1;
	int max = bierzacaSiecDlaWatkowGenerujacych->obliczPrzedzialKrawedziDlaWatku(tid);





	printf("Thread #%ld started, my range is %d to %d\n", tid, min, max);
	bierzacaSiecDlaWatkowGenerujacych->wypelnijPolaczenia(min, max);
//	printf("Thread #%ld ended\n", tid);

	pthread_exit(NULL);
	return NULL;
}	// threadFunc


/* fragmentaryczne wypelnianie polaczen w sieci
 * funkcja watkowa
 */
void SiecNeuronowa::wypelnijPolaczenia(int odIndeks, int doIndeks){
	float p=0;
	float w = 0;
	float coeff = 1;

	float thresh_er = 0.8 / (getIloscNeuronow()) /* + alpha */;

	// dorzucona krawedz do samego siebie i--i
	for (int i=odIndeks; i<= doIndeks; i++){
		for (int j = i; j< getIloscNeuronow(); j++){

			// regular grid
//			int row = 20;
//			int ri = i % row;
//			int rj = j % row;
//			int ci = i / row;
//			int cj = j / row;


			getNeuron(j).mutexLock();
			if (i!=j)
				getNeuron(i).mutexLock();

			if (typ == SFERA_1D ||  typ == SFERA_2D || typ == KOLO_2D || typ == KULA_3D
					|| typ == KOSTKA_2D || typ == KOSTKA_3D ){
				float odl = getNeuron(i).odleglosc( getNeuron(j) );
	//			if (i!=j)
	//				getNeuron(j).mutexUnLock();
	//			getNeuron(i).mutexUnLock();

				if (odl < 1){
					p = 1;
				} else {
					p = coeff * (float) pow(odl, alpha);
				}	// if
			} else if (typ == RANDOM_ER) {
				if (i!=j)
					p = thresh_er;
				else
					p = 1;
			} else if (typ == LATTICE_2D || typ == LATTICE_3D){
				int d = (int) sqrt(getIloscNeuronow());
				int ri = i % d;
				int rj = j % d;
				int ci = i / d;
				int cj = j / d;

				if (abs(ri - rj) + abs(ci-cj) == 1 ){
					p = 1;
				} else {
					p = 0;
				}	// if
			}	// if else

// NOTE --- used when regular grid is needed
//			if (abs(ri-rj) + abs(ci-cj) == 1 )
//				p = 1;
//			else
//				p = 0;

			if (p>=1 || losujRBool(p)){
				mutexLock();
				iloscKrawedzi++;
				int ik = iloscKrawedzi-1;
				Polaczenie k = /* Polaczenie:: */ Polaczenie(iloscKrawedzi-1, i, j);
				k.setCzyWPuliLosowanychKrawedzi(1);
				krawedzie.push_back(k);
				// dorzucana do losowania w innym SND.this()

				//  energa liczona raz na koniec, strata niewielka,
				// a jest mniej blokad na mutexie

				// jeszcze cos tu mieszalem - nie powinno byc deadlocku, ale uwazac!
				mutexUnLock();

				w = k.getWaga();
				// dodaje indeks krawedzi do obu neuronow
				// dodawana jest kopia lokalna zmiennej, jestesmy poza muteksem!

//				getNeuron(i).mutexLock();
				getNeuron(i).dodajIndeksKrawedzi(ik);
				getNeuron(i).setWsparcie( getNeuron(i).getWsparcie() - w );
//				getNeuron(i).mutexUnLock();

				if (j!=i){
//					getNeuron(j).mutexLock();
					getNeuron(j).dodajIndeksKrawedzi(ik);
					getNeuron(j).setWsparcie( getNeuron(j).getWsparcie() - w );
//					getNeuron(j).mutexUnLock();
				}	// if
			}	// fi

		getNeuron(j).mutexUnLock();
		if (i!=j)
			getNeuron(i).mutexUnLock();


		}	// for j
	}	// for i
}	//wypelnijPolaczenia(int odIndeks, int doIndeks)

void SiecNeuronowa::sortujKrawedzieWNeuronie(int indeks){
	Neuron & n = getNeuron(indeks);

	for (int i=0; i<n.getIloscSasiadow(); i++){
		for (int j=i+1; j<n.getIloscSasiadow(); j++){
			Polaczenie & ki = getKrawedz( n.getIndeksKrawedzi(i) );
			int kluczi = ki.getIndeks1() == indeks ? ki.getIndeks2() : ki.getIndeks1();

			Polaczenie & kj = getKrawedz( n.getIndeksKrawedzi(j));
			int kluczj = kj.getIndeks1() == indeks ? kj.getIndeks2() : kj.getIndeks1();

			if (kluczj < kluczi){
				// swap(i, j);
				n.zamienSasiadow(i, j);
			}	// if
		}	// for j
	}	// for i
}	// sortujKrawedzieWNeuronie(int indeks)


void SiecNeuronowa::testujSortowanieKrawedzi(int indeks){

	long ilosc = 0;
	for (int i=0; i< getIloscNeuronow(); i++){
		for (int j=i; j<getIloscNeuronow(); j++){
			if (czyIstniejeKrawedzMiedzyNeuronami(i, j))
				ilosc++;
		}	// for j
	}	// for i

	printf("krawedzi powinno byc %d jest %ld \n\n", getIloscKrawedzi(), ilosc);

	// takie glupie zabezpieczenie
	if (1+1==2)
		return;


	Neuron & n = getNeuron(indeks);

	printf("adj %d = [", indeks);
	for (int j=0; j<n.getIloscSasiadow()-1; j++){
		Polaczenie & kj = getKrawedz(getNeuron(indeks).getIndeksKrawedzi(j));
		int kluczj = kj.getIndeks1() == indeks ? kj.getIndeks2() : kj.getIndeks1();

		printf("%d ", kluczj);
	}	// forj
	printf("]\n");
	printf("edg %d = [", indeks);
	for (int j=0; j<n.getIloscSasiadow()-1; j++){
		printf("%d ", getNeuron(indeks).getIndeksKrawedzi(j));
	}	// for j

	printf("]\n");



}	// testujSortowanieKrawedzi

/*
 * liczy ostatni neuron jaki nalezy do danego watku
 * niestety w miedzyczasie musi rowniez policzyc nastepne (liczy od konca!)
 *
 * numerWatku liczony jest od zera!
 *
 * hint:
 * wzorek na pole jest mniej wiecej taki:
 * len *(n-p + n-p -len) * 0.5 =  iloscNaWatek
 *
 */
int SiecNeuronowa::obliczPrzedzialKrawedziDlaWatku(int numerWatku){
	double iloscNaWatek = ((double ) getIloscNeuronow() /2.0 / conf->getThreads_number()) * (getIloscNeuronow()-1);

//	printf("il na watek %lf\n", iloscNaWatek);

	if (numerWatku == conf->getThreads_number() -1){
		return getIloscNeuronow() -1;
	} 	// if

	int pierwszy = 0;
	if (numerWatku == 0){
		pierwszy = 0;
	}	else {
		pierwszy = obliczPrzedzialKrawedziDlaWatku(numerWatku-1) +1;
	}	// if

	int n = getIloscNeuronow();
	/*
	 *  len * (n-p + n-p - len) * 0.5 = iloscNaWatek
	 *
	 *  -0.5 len^2 + (n - p) len - iloscNaWatek = 0
	 */

	double delta = ((double)n-pierwszy) * ((double)n-pierwszy) - 2* iloscNaWatek;
//	printf("delta %lf\n", delta);

	if (delta <0){
		// takie zabezpieczenie
		return getIloscNeuronow()-1;
	}	// fi

	/*
	 * len = \frac{ - (n-p)+/- sqrt(letla) }{2 * -0.5}
	 * len = n-p +/- sqrt(delta)
	 *
	 * ret = p + n -p - sqrt(deltaa)
	 */

	int ret = (int) (n - sqrt(delta));
	if (ret <= 0)
		ret = 0;
	if (ret > n-1)
		ret = n-1;

	if (numerWatku == conf->getThreads_number()-1)
		return n-1;
	else
		return ret;
}	// obliczPrzedzialKrawedziDlaWatku


/* "znajdz" == czas liniowy!!
 *
 */
float SiecNeuronowa::znajdzMinimalneWsparcieODodatnimLadunku(){
	float ret = maxWsparcie;
	for (int i=0; i< iloscNeuronow; i++){
		if (getNeuron(i).getLadunek()>0 && getNeuron(i).getWsparcie() < ret){
			ret = getNeuron(i).getWsparcie();
		}	// if
	}	// for
	return ret;
}	// znajdzMinimalneWsparcieODodatnimLadunku

/* zwraca losowa krawedz w grafie
 */
Polaczenie& SiecNeuronowa::wylosujKrawedz(){
	int p =losujRInt(iloscKrawedzi);
	return krawedzie.at(p);
}	// wylosujKrawedz()


/* znajdz = czas liniowy, jak potrzebne wiele razy to
 *  "znajdz raz i zapamietaj!!"
 */
long SiecNeuronowa::znajdzMaksymalnyLadunekNeuronu(){
	long max = 0;

	for (int i=0;i< iloscNeuronow; i++){
		long c = getNeuron(i).getLadunek();
		if (max<c){
			max = c;
		}	// if
	}	// for i

	return max;
}	// znajdzMaksymalnyLadunekNeuronu()

/* znajdz = czas liniowy, jak potrzebne wiele razy to
 *  "znajdz raz i zapamietaj!!"
 */
int SiecNeuronowa::znajdzNeuronOMaksymalnymWsparciu(){
	double max = -1000000;
	double id = 0;


	for (int i=0;i< iloscNeuronow; i++){
		double c = getNeuron(i).getWsparcie();
		if (max<c){
			max = c;
			id = i;
		}	// if
	}	// for i

	return id;
}	// znajdzMaksymalnyLadunekNeuronu()

/* "znajdz" == czas liniowy!
 * jak potrzebne kilka razy to "znajdz raz i zapamietaj!"
 *
 */
long SiecNeuronowa::znajdzMaksymalnyPrzelpywKrawedzi(){
	long max = 0;

	for (int i=0;i< iloscKrawedzi; i++){
		long c = getKrawedz(i).getladunekDo1() + getKrawedz(i).getladunekDo2();
		if (max<c){
			max = c;
		}	// if
	}	// for i

	return max;
}	// znajdzMaksymalnyPrzeplywKrawedzi()

/* liczy energie od zera
 */
float SiecNeuronowa::obliczEnergie(){
	float en = 0;
	for (int i=0; i<iloscKrawedzi; i++){
		Polaczenie & k = getKrawedz(i);
		Neuron & n1 =  getNeuron(k.getIndeks1());
		Neuron & n2 =  getNeuron(k.getIndeks2());
		en += k.getWaga() * fabs(n1.getLadunek() - n2.getLadunek());
	}	// for
	energia = en;
	return en;
}	// obliczEnergie

/* oblicz === czas liniowy
 * jak potrzeba to policzyc raz i zapamietac!
 */
int SiecNeuronowa::obliczIloscNeuronowBezLadunku(){
	int ret =0;

	for (int i=0; i<getIloscNeuronow() ; ++i){
		if (getNeuron(i).getLadunek() == 0)
			ret ++;
	}	// for
	return ret;
}	// obliczIloscNeuronowBezLadunku();

/*
 * siecNeuronowa.cpp
 *
 *  Created on: 08-Feb-2009
 *      Author: piersaj
 *
 *  2009-02-08
 *  	przeniesiony z innego pliku - zostala sie tam czesc getterow
 *  2009-03-21
 *  	korekta do mnk
 *  	dodany parametr zageszczenia neuronow na jednostke objetosci
 *  2009-03-28
 *  	dodano bardzo niebezpieczna zabawke - pula krawedzi dopuszczonych do losowania
 *  	gdy zostanie wylosowana krawedz miedzy neuronami bez ladunku jest usuwana z puli
 * 2009-04-17
 *		kilka korekt do losowania krawedzi: usuwanie z puli krawedzi losowanych
 *		korekty do MNK: ignorowanie wpisow skrajnych
 * 2009-04-19
 * 		korekty do mnk: dodane progi startowy i koncowy, wpisy w teblicy  juz nie sa zerowane
 * 		gdy sa poza progiem
 * 2009-05-04
 * 		szukanie drzewek w grafie
 * 2009-05-21
 * 		MNK i drzewka polecialy do osobnego pliku - zostala sie tylko dynamika
 * 2009-05-30
 * 		metoda testujaca czy neurony n1 n2 sa polaczone krawedzia
 * 		metoda zwracajaca ww krawedz
 * 		do tej pory siec _nie_wymagala_ takich metod i nie byla pod nie optymalizowana
 * 		nie, nie wiem jak to sie zachowuje dla duzych i gestych sieci
 * 		(jest wyszukiwanie binarne wiec moze nie najgorzej)
 * 2009-05-31
 * 		liczenie histogramu przyjmowanych zmian
 * 2009-06-12
 * 		korekty do usuwań krawedzi - usuwanie kilku na raz
 * 		korekty do statystyk symulacji, cos polecialo, cos zostalo zmienione
 * 2009-06-19
 * 		korekta do dynamiki - jezeli krawedz jest dluga to przeplyw przez nia jest zawsze akceptowany
 * 		kilka korekt do formatowania wynikow
 * 2009-06-21
 * 		dodano zapis czasu jaki zajeła dana epoka (== 1/100 całości) symulacji
 * 2009-06-27
 * 		kilka korekt do stylu kodu - rozbito duze metody na kilka krotszych
 * 2009-07-10
 * 		korekty do kodu
 * 2009-11-??
 * 		korekty do wyszukiwania krawedzi itp.
 * 2010-04-??
 * 		usuwanie ladunku i pojawianie sie w innym miescu
 * 2010-05-11
 * 		ww poprawka byla pomylka - wracamy do poprzedniej wersji
 * 2010-06-30
 * 		czesc kodu z definicjami poleciala do osobnego pliczku, zostala tylko dynamika
 * 2010-07-06
 * 		pierwsz wersja dynamiki na watkach - zobaczymy jak pojdzie...
 * 		stara dynamika za sekunde dostanie belki na koniec
 * 		przy okazji stracilismy liczenie histogramu czasu epok
 * 2010-07-07
 * 		dodalismy kilka muteksow na rozne zmienne zeby sie nie blokowaly nawzajem
 * 2010-07-08
 *		kilka korekt do muteksow,
 *		korekty do podsumowania symulacji
 * 2010-08-26
 * 		korekta do usuwania jalowych krawedzi > robione co 2% iteracji
 * 		i tylko przez zerowy watek
 * 2010-08-28
 * 		dodanie progreslogu do symulacji
 * 		korekty do failsafa gdy w sieci nie ma ladunku
 * 2010-09-04
 * 		mala korekta,  mutex na krawedzie losowane zmieniony na rwlock,
 * 		czytac moga wszyscy naraz, pisac tylko jeden i nikt w tym czasie ni moze czytac
 * 2011-04-11
 * 		added reading parameters from unified configuration class
 * 2011-11-14
 * 		Small improvement to the dynamics when charge is depleted, it should work faster now...
 * 2012-07-19
 *		Dramatic improvement int dynamics when removing edges between depleted neurons
 *		should boost it sky-rise
 *
 */
#include <cstdio>
#include <cstdlib>

#include "siecNeuronowaDynamika.h"
#include "simulationProgressLogger.h"

#include "../visualisation/verticesEvolutionSaver.h"
#include "../config/parserKonfiguracji.h"
#include "../statistics/spectrum/spektrumGrafuLapack.h"

SiecNeuronowaDynamika * siecNeuronowaDynamikaGlobalna = NULL;

// do pomiaru czasu w trakcie obliczeń - nie dotykać!!!
long czasomierz1, czasomierz2;

// do pomiaru calkowitego czasu obliczeni - nie dotykac
long czasRozpoczecia, czasZakonczenia;




void * wykonajPetleNaWatku(void * threadid);



/* *************************************************
 *
 * dynamika sieci
 *
 * *************************************************/

SiecNeuronowaDynamika::SiecNeuronowaDynamika(SiecNeuronowa & n_):
	n(n_)
{
	vertSaver = NULL;
	spectSaver = NULL;
	synapseSaver = NULL;
	minWsparcieDodatniLadunek = -100000;
	n.obliczEnergie();
	ilIteracji = n.getConfig()->getIterationsNumber();
	nrIteracji = 0;
	iloscEfektywnychZmian = iloscOdrzuconychZmian = iloscPrzeslanDoSiebie = 0;
	progDlugosciKw = n.getConfig()->getLongRangeSynapseThreshold();
	prawdopodobienstwoUsunieciaLadunku = n.getConfig()->getProbabilityOfChargeRemovalOnLongSynapse();
	flagaLadunekWyczerpany = false;

	if (progDlugosciKw <=0){
		// do nothing
	} else {
		// square it
		progDlugosciKw *= progDlugosciKw;
	}


	NUM_THREADS = n.getConfig()->getThreads_number();

	pthread_mutex_init(&mutex_iloscZmianOdrzuconych, NULL);
	pthread_mutex_init(&mutex_iloscZmianPrzyjetych, NULL);
	pthread_mutex_init(&mutex_nrIteracji, NULL);
	pthread_mutex_init(&mutex_iloscLosowan, NULL);
	pthread_rwlock_init(& rwLockKrawedzieLosowane, NULL);

	// chcarge removal was disabled
	czyUsuwacLadunek = false;
//	czyUsuwacKrawedzie = n.getConfig()->getEdgesRemoval();
//	czyUsuwacLadunek = n.getConfig()->get;
//	czyDodawacLadunek = n.getConfig()->getChargeReappear()();

	pthread_rwlock_wrlock(& rwLockKrawedzieLosowane);
	for (int i=0; i<getIloscKrawedzi(); i++){
		getKrawedz(i).setCzyWPuliLosowanychKrawedzi(true);
		krawedzieLosowane.push_back(i);
	}	// for
	pthread_rwlock_unlock(& rwLockKrawedzieLosowane);

}	// this()

SiecNeuronowaDynamika::~SiecNeuronowaDynamika(){
	pthread_mutex_destroy(& mutex_iloscZmianOdrzuconych);
	pthread_mutex_destroy(& mutex_iloscZmianPrzyjetych);
	pthread_mutex_destroy(& mutex_nrIteracji);
	pthread_mutex_destroy(& mutex_iloscLosowan);
	pthread_rwlock_destroy(& rwLockKrawedzieLosowane);
	if (vertSaver != NULL){
		delete vertSaver;
		vertSaver = NULL;
	}	// fi
	if (spectSaver != NULL){
		delete spectSaver;
		spectSaver = NULL;
	}	// if
	if (synapseSaver!=NULL){
		delete synapseSaver;
		synapseSaver = NULL;
	}	// fi
	krawedzieLosowane.clear();
}	// this()

float SiecNeuronowaDynamika::obliczEnergieWNeuronie( int indeks){
	Neuron n = getNeuron(indeks);
	float ret = 0;

	for (int i=0; i<n.getIloscSasiadow(); i++){
		int ik = n.getIndeksKrawedzi(i);
		Polaczenie k = getKrawedz(ik);
		ret += k.getWaga() * fabs(getNeuron(k.getIndeks1()).getLadunek() -
				getNeuron(k.getIndeks2()).getLadunek());
	}	// for
	return ret;
}	// obliczEnergieWNeuronie

/* przelicza zmiane energii gdy z n1 przeplynie 1 ladunek do wezla n22
 * waga miedzy wezlami wynosi waga
 *
 * zal ze ladunek n1 jest wiekszy rowny 1
 *
 * liczy tylko na podstawie sasiadow n1 i n2 (bo od reszty nie zalezy)
 *
 */
float SiecNeuronowaDynamika::obliczZmianeEnergii(int i1, int i2){
	Neuron &n1 = getNeuron(i1);
	Neuron &n2 = getNeuron(i2);

	float e1 = 0, e2 =0;;
	float ladunek1po = n1.getLadunek()-1;
	float ladunek2po = n2.getLadunek()+1;

	// zmiany energii

	// dla kazdego sasiada n1
	for (int i=0; i<n1.getIloscSasiadow(); i++){
		int ip = n1.getIndeksKrawedzi(i);
		Polaczenie &p = getKrawedz(ip);

		int i3 = p.getIndeks1() != i1 ?  p.getIndeks1() : p.getIndeks2();
		Neuron &n3 = getNeuron(i3);


		// przed zamiana
		e1 += p.getWaga() * fabs(n1.getLadunek() - n3.getLadunek());

		// po zamianie n1.ladunek == 0,
		// oraz n2.ladunek = n2.ladunek + 1
		if (i3 == i2){
			e2 += p.getWaga() * fabs(ladunek1po - ladunek2po);
		} else {
			e2 += p.getWaga() * fabs(ladunek1po - n3.getLadunek());
		}	// else

	}	// for i

	// dla kazdego sasiada n2

	for (int i=0; i<n2.getIloscSasiadow(); i++){
		int ip = n2.getIndeksKrawedzi(i);

		Polaczenie & p = getKrawedz(ip);

		int i3 = p.getIndeks1() != i2 ?  p.getIndeks1() : p.getIndeks2();

		if (i3==i1){
			// krawedz miedzy n1 i n2 dodajemy tylko raz
			continue;
		}

		Neuron& n3 = getNeuron(i3);

		// przed zmiana
		e1 += p.getWaga() * fabs(n2.getLadunek() - n3.getLadunek());
		// po zmianie n2.ladunek = n1.ladunek + n2.ladunek
		e2 += p.getWaga() * fabs(ladunek2po - n3.getLadunek());

	}	// for i

	return e2 - e1;
}	//obliczZmianeEnergii()



/* z polaczenia k wybiera indeksy tak zeby z pierwszego zawsze dalo sie przeniesc ladunek
 *
 * jeden z wezlow musi miec ladunek!!
 * jak oba sa zero to zwraca -1 -1
 *
 */
void SiecNeuronowaDynamika::wybierzZwrotKrawedzi(Polaczenie &k, int &i1_z, int &i2_do){
	// losujemy w ktora strone przeplywa ladunek
	int zwrot = losujRBool(0.5);

	if (zwrot == 1){
		i1_z = k.getIndeks1();
		i2_do = k.getIndeks2();
	}	else {
		i1_z = k.getIndeks2();
		i2_do = k.getIndeks1();
	}	// if


	if (getNeuron(i1_z).getLadunek() == 0){
		// jezeli oba nie maja ladynku to niewiele da sie zrobic
		if (getNeuron(i2_do).getLadunek() == 0){
			i1_z = -1;
			i2_do = -1;
			return;
		}	// if

		// ale jezeli drugi ma energie to wybiezmy drugiego :)
		int t = i1_z;
		i1_z = i2_do;
		i2_do = t;
	}	// if
}	// wybierzZwrotKrawedzi()


inline void SiecNeuronowaDynamika::zwiekszIloscLosowan(){
	iloscWszystkichLosowan ++;
	if (iloscWszystkichLosowan == 1000000000){
		iloscWszystkichLosowan = 0;
		iloscWszystkichLosowanE09 ++;
	}	// fi
}	// zwiekszIloscLosowac

/*
 *  oIle < 1E+9
 */
inline void SiecNeuronowaDynamika::zwiekszIloscLosowan(long oIle){
	while (oIle >= 1000000000){
		oIle -= 1000000000;
		iloscWszystkichLosowanE09 ++;
	}	// fi

	iloscWszystkichLosowan += oIle;

	while (iloscWszystkichLosowan >= 1000000000){
		iloscWszystkichLosowan -= 1000000000;
		iloscWszystkichLosowanE09 ++;
	}	// fi
}	// zwiekszIloscLosowac

/*
 * wykonywac raz na jakis czas i tylko z jednego (zerowego) watku!
 */
void SiecNeuronowaDynamika::usunKrawedzieZListyLosowania(){

	if (n.getConfig()->getEdgesRemoval() == false){
		return;
	}	// fi

	float odsetek = .05;

	if (nrIteracji < ilIteracji * odsetek){
		return;
	}	// fi

	// w ogole to losowanie krawedzi jest rwlocku z tym fragmentem
	// tu jest wrlock
	// w losowaniu (i kazdym innym miejscu) powinien byc rdlock


	pthread_rwlock_wrlock( & rwLockKrawedzieLosowane);
	int k=0;
	int p = krawedzieLosowane.at(0);
	// dobra - jedno powtorzenie....

	minWsparcieDodatniLadunek =  n.znajdzMinimalneWsparcieODodatnimLadunku();

	while (k < (int) krawedzieLosowane.size() ){
		p = (int) krawedzieLosowane.at(k);

		Polaczenie & kr = getKrawedz(p);
		// czytamy krawedz
		int i1 = kr.getIndeks1();
		int i2 = kr.getIndeks2();

		// "lub" czy "i" - oto jest pytanie - raczej jasne ze lub
		// i tak oba maja zero po ladunku!!!
		// ??
		if ( getNeuron(i1).getWsparcie() < minWsparcieDodatniLadunek ||
				getNeuron(i2).getWsparcie() < minWsparcieDodatniLadunek){
			kr.setCzyWPuliLosowanychKrawedzi(false);

			// no erase
			// instead we rewrite these edges
			iloscUsuniecKrawedzi++;
			k++;
		}	else {
			k++;
		}	// fi
	}	// while


	krawedzieLosowane.clear();
	// TODO one more update: instead of for every edge do sth like
	// for every edge present in pred vector
	// if not marked for deletion than copy
	// else skip copying
	// new vector = old vector

	for (int i=0; i< getIloscKrawedzi(); i++){
		if (getKrawedz(i).getCzyWPuliLosowanychKrawedzi()){
			krawedzieLosowane.push_back(i);
		}	// if
	}	// if

	pthread_rwlock_unlock( & rwLockKrawedzieLosowane);


}	// usunKrawedzieZLisyLosowania()

/* to samo ale inteligentniej - dodatkowa struktura danych wieksze prawdopodobienstwo
 * ze w cos trafimy usuwanie nic nie wnoszacch smieci z puli obiektow losowanych etc.
 */
Polaczenie& SiecNeuronowaDynamika::wylosujKrawedzZNiezerowymNeuronem(){
	// najpierw losujemy z listy krawedzi w puli losowania

	pthread_rwlock_rdlock( & rwLockKrawedzieLosowane);
	int k = losujRInt(krawedzieLosowane.size());
	int p = (int) krawedzieLosowane.at(k);
	pthread_rwlock_unlock( & rwLockKrawedzieLosowane);

	// dopiero potem czytamy krawedz
	int i1 = getKrawedz(p).getIndeks1();
	int i2 = getKrawedz(p).getIndeks2();

	pthread_mutex_lock(& mutex_iloscLosowan);
	zwiekszIloscLosowan();
	pthread_mutex_unlock(& mutex_iloscLosowan);

	// usuwanie krawedzi zostalo tegoz miejsca wywalone!!!

	// dopoki trafiamy na krawedz pomiedzy zerem a zerem 0-0
	int ilObrotw = 0;

	while (getNeuron(i1).getLadunek()==0 && getNeuron(i2).getLadunek() == 0){
		k++;
		ilObrotw++;

		// i tu tez lock na krawedzi
		// czy ten mutex jest tu potrzebny???
		// chyba nie

//		pthread_rwlock_t * rwLockKrawedzieLosowane = pthread_rwlock_init( rwLockKrawedzieLosowane
//				);


		// rwlock do czytania powinien starczyc - do pisania jest blokowany przy usuwaniu krawedzi
		// tu byl n.mutexLock();
		// chyba jest ok

		pthread_rwlock_rdlock( & rwLockKrawedzieLosowane);
		if (k >=  (int)krawedzieLosowane.size()){
			k=0;
		}	// fi
		p = (int) krawedzieLosowane.at(k);

		pthread_rwlock_unlock( & rwLockKrawedzieLosowane);

		i1 = getKrawedz(p).getIndeks1();
		i2 = getKrawedz(p).getIndeks2();

		if (ilObrotw > (int)krawedzieLosowane.size() +1){
			// siec oprozniona z ladunku!
			if (czyUsuwacLadunek== true ){
				logJP << "\nSND.losujKrawedzZDL(): wyczerpany ladunek, flagaUsuwaniaLadunku := false\n"
						<< "% iteracji == " << (nrIteracji / 1.0 / ilIteracji * 100) << "\n";
				czyUsuwacLadunek = false;


				// Marker --- dodalismy usuwanie krawedzi po stwierdzeniu ze nie ma juz ladunku
				// sprawdzic czy to niczego nie psuje
				usunKrawedzieZListyLosowania();
			}	// fi


			flagaLadunekWyczerpany = true;

			pthread_mutex_lock(& mutex_iloscZmianOdrzuconych);
			iloscOminietychZmian = ilIteracji - nrIteracji;
			pthread_mutex_unlock(& mutex_iloscZmianOdrzuconych);

			// kolejny myk - dodajemy ladunek do neuronu o maksymalnym wsparciu,
			// a nie losowego
			Neuron & nn = getNeuron( getN().znajdzNeuronOMaksymalnymWsparciu() );
			nn.mutexLock();
			nn.zwiekszLadunek();
			nn.mutexUnLock();

			// i jeszcze jeden- ilosc losowan zwiekszamy jeden raz  wychodzac z petli
			pthread_mutex_lock(& mutex_iloscLosowan);
			zwiekszIloscLosowan(ilObrotw);
			pthread_mutex_unlock(& mutex_iloscLosowan);
			return getKrawedz(p);
		}	// fi
	}	// while

	pthread_mutex_lock(& mutex_iloscLosowan);
	zwiekszIloscLosowan(ilObrotw);
	pthread_mutex_unlock(& mutex_iloscLosowan);

	return getKrawedz(p);
}	// wylosujKrawedz()



/* aktualnie nie uzywane
 * przywracanie krawedzi do puli losowania
 *
 */
void SiecNeuronowaDynamika::przywrocKrawedzie(int i){
	logJP << "ERROR: przywrocKrawedzie(int i = "<< i <<" ):  ktos mnie wywolal!\n";

	Neuron& n = getNeuron(i);

	for (int i=0; i< n.getIloscSasiadow(); i++){
		// indeks krawedzi laczacej n z sasiadem
		int k = n.getIndeksKrawedzi(i);

		Polaczenie& p = getKrawedz(k);

		if (! p.getCzyWPuliLosowanychKrawedzi()){
			iloscPrzywrocenKrawedzi++;
			krawedzieLosowane.push_back(k);
			p.setCzyWPuliLosowanychKrawedzi(1);
		}	// fi
	}	// for i
}	// przywrocKrawedzie

/* statystyki z symulcacji
 */
void SiecNeuronowaDynamika::podsumujSymulacje(){

	logJP << "\n\n";
	logJP.timestamp_nl();


	czasZakonczenia = time(NULL);
	long t = czasZakonczenia - czasRozpoczecia;
	logJP << "czasObliczen                   = " << t/3600 <<"h " << (t%3600)/60 << "m " <<
		(t%60) << "s\n";

	logJP << "iteracji                       = " << ilIteracji << "\n";
	logJP << "iteracji        / sek          = " << ilIteracji / t << "\n";

	logJP << "\n";
	logJP << "iloscZmianPrzyjetych           = " << iloscEfektywnychZmian << " == "
		<<iloscEfektywnychZmian * 1.0 / ilIteracji <<"\n";
	logJP << "iloscZmianPrzyjetych / sek     = " << iloscEfektywnychZmian / t << "\n";

	logJP << "iloscZmianOdrzuconych          = " << iloscOdrzuconychZmian << " == "
			<< iloscOdrzuconychZmian* 1.0 / ilIteracji << "\n";

	logJP << "iloscLosowanOminietych         = " << iloscOminietychZmian<< " == "
				<< iloscOminietychZmian* 1.0 / ilIteracji << "\n";

	logJP << "iloscZmianPodGore              = " << iloscZmianPodGore << " == "
		<< iloscZmianPodGore* 1.0 / iloscEfektywnychZmian << " == "
		<< iloscZmianPodGore* 1.0 / ilIteracji << "\n";

	logJP << "iloscPrzeslanDoSiebie          = " << iloscPrzeslanDoSiebie << " == "
			<< iloscPrzeslanDoSiebie* 1.0 / iloscEfektywnychZmian << " == "
			<< iloscPrzeslanDoSiebie  *1.0 / ilIteracji << "\n";


	logJP << "iloscZmianNaDlugichKrawedziach = " << iloscZmianNaDlugiejKrawedzi << " == "
		<< iloscZmianNaDlugiejKrawedzi* 1.0 / iloscEfektywnychZmian << "\n";

	logJP << "iloscLosowan                   = " << iloscWszystkichLosowanE09 << " G + " << iloscWszystkichLosowan <<"\n";

	double lps = (iloscWszystkichLosowanE09 * 1.0E+09 + iloscWszystkichLosowan) / t;
	logJP << "losowan         / sek      = " <<  lps << "\n";

	logJP << "ostatniaPrzyjetaZmiana     = " << ostatniaZmiana <<"\n";

	logJP << "iloscKrawedziWPuliLosowania= " << krawedzieLosowane.size() << " / " << n.getIloscKrawedzi()
		<< " == " << krawedzieLosowane.size() * 1.0 / n.getIloscKrawedzi() << "\n";
	logJP << "iloscUsuniecKrawedzi       = " << iloscUsuniecKrawedzi <<"\n";
	logJP << "iloscPrzywrocenKrawedzi    = " << iloscPrzywrocenKrawedzi <<"\n";
	logJP << "iloscUsuniecLadunku        = " << iloscUsuniecLadunku<<"\n";

	podsumujSymulacjeIlosciNeuronowZLadunkiem();
	podsumujWynikiNaWsparciach();


	for (int i=0; i<getIloscNeuronow(); i++){
		int ind= n.getIndeksKrawedziMiedzyNeuronami(i,i);
		if  (ind >=0){
			Polaczenie &e = getKrawedz( ind );
			logJP << i << " , " << e.getSumaLadunku() << "\n";
		}	// fi
	}	// for i
}	// podsumujSymulacje

/* podsumowanie wynikow symulacji
 * + kilka obliczen w tle
 *
 */
void SiecNeuronowaDynamika::podsumujSymulacjeIlosciNeuronowZLadunkiem(){
	int suma1=0;
	int sl = 0;
	int suma10 = 0, suma100 = 0, suma1000=0;

	for (int i=0; i<n.getIloscNeuronow(); i++){
		if (getNeuron(i).getLadunek() > 0 ){
			suma1++;
			sl +=getNeuron(i).getLadunek();
			if (getNeuron(i).getLadunek() >10 ){
				suma10++;
				if (getNeuron(i).getLadunek() >100 ){
					suma100++;
					if (getNeuron(i).getLadunek() >1000 ){
						suma1000++;
					}	// if (1000+)
				}	// if (100+)
			}	// if (10+)
		}	// if (1+)
	}	// for

	int iloscNeuronow = n.getIloscNeuronow();

	std::cout.precision(2);
	logJP<< "neurony o >  0 ladunku     = " << suma1 <<" / " << iloscNeuronow
		<< " = " <<  (suma1 * 1.0 / iloscNeuronow)	<< "\n";
	std::cout.precision(2);
	logJP<< "neurony o > 10 ladunku     = " << suma10 <<" / " << iloscNeuronow
		<< " = " <<  (suma10*1.0 / iloscNeuronow) << "\n";
	std::cout.precision(2);
	logJP<< "neurony o >100 ladunku     = " << suma100 <<" / " << iloscNeuronow
		<< " = " <<   (suma100*1.0 / iloscNeuronow) << "\n";
	logJP<< "neurony o >1000 ladunku    = " << suma1000<<" / " << iloscNeuronow
			<< " = " <<   (suma1000*1.0 / iloscNeuronow) << "\n";
	logJP<< "suma ladunku (policzona)   = " << sl<<" + " << iloscUsuniecLadunku << " usuniec\n";
	logJP<< "suma ladunku (zapamietana) = " << n.getSumaLadunku() <<"\n";
	logJP<< "energia (policzona)        = " << n.obliczEnergie() <<"\n";
	logJP<< "energia (zapamietana)      = " << n.getEnergia() <<"\n\n";
}	// sprawdz


/* kilka statystyk apropo wsparc
 */
void SiecNeuronowaDynamika::podsumujWynikiNaWsparciach(){
	float maxWsparcie = 0;
	int najWsparcie = 0;

	float maxLadunek = 0;
	int najLadunek = 0;

	for (int i=0; i< getIloscNeuronow(); i++){
		if (getNeuron(i).getWsparcie()>maxWsparcie){
			maxWsparcie = getNeuron(i).getWsparcie();
			najWsparcie = i;
		}	// if

		if (getNeuron(i).getLadunek()>maxLadunek){
			maxLadunek = getNeuron(i).getLadunek();
			najLadunek= i;
		}	// if
	}	// for

	logJP << "najwieksze wsparcie = "<< maxWsparcie << " ladunek   = "
			<< getNeuron(najWsparcie).getLadunek() << '\n';
	logJP << "najwiekszy ladunek  = "<< maxLadunek << " wsparcie  = "
			<< getNeuron(najLadunek).getWsparcie() << '\n';
}	// analizaWsparc


/* ********************************************************
 *  Dynamika na watkach
 * ********************************************************/


void SiecNeuronowaDynamika::rozpocznijSymulacjeNaWatkach(){
	vertSaver = new VerticesEvolutionSaver(&n);
	if  (n.getConfig()->getCalculateNodesEvolution()){
		vertSaver->generateHeader();
	}	// if

	if (n.getConfig()->getCalculateSpectrumEvolution()){
		spectSaver = new SpectrumEvolutionSaver(&n);
		spectSaver->generateHeader();
	}	// if

	inicjalizujZmiennePrzedSymulacja();
	initProgresLoger(this);

	pthread_t threads[n.getConfig()->getThreads_number()];

	for(long t=0; t<n.getConfig()->getThreads_number(); t++){
		int rc = pthread_create(&threads[t], NULL, wykonajPetleNaWatku, (void *)t);
		if (rc){
			printf("rozpocznijSymulacjeNaWatkach(): ERROR return code from pthread_create() is %d\n", rc);
			exit(-1);
		}	// if
	}	// for

	for(long t=0; t<n.getConfig()->getThreads_number(); t++){
		pthread_join(threads[t], NULL);
	}	// for


	if (n.getConfig()->getCalculateNodesEvolution()){
		vertSaver->generateFooter();
	}	// fi
	delete vertSaver;
	vertSaver = NULL;

	if (n.getConfig()->getCalculateSpectrumEvolution()){
		spectSaver->generateFooter();
		delete spectSaver;
		spectSaver = NULL;
	}	// if


	if (n.getConfig()->getCalculateSynapseEvolution()){
		synapseSaver = new SynapsesEvolutionSaver(& n);
		synapseSaver -> setChargeThreshold( getN().getConfig()->getKohonenChargeThreshold() );
		synapseSaver -> generateSvgSnapshot();
	}	// if

	podsumujSymulacje();
	if (n.getConfig()->getLogProgress()){
		simulationProgresLog->endLog(this);
	}	// if

	// te drzewka tez sa do wywalenia
//	n.kolorujDrzewka();

}	// rozpocznijSymulacjeNaWatkach()



void * wykonajPetleNaWatku(void * threadid){
	long tid;
	tid = (long)threadid;

	long iteracji = siecNeuronowaDynamikaGlobalna->getIloscIteracji() / NUM_THREADS;
	long reszta = siecNeuronowaDynamikaGlobalna->getIloscIteracji() - iteracji*NUM_THREADS;

	if (reszta >= tid+1){
		iteracji++;
	}	// fi

//	printf("Th #%ld started, its = %ld\n", tid, iteracji);
	for (int i=0; i<iteracji; i++){
		siecNeuronowaDynamikaGlobalna->wykonajPojedynczyKrokNaWatku(tid);
	}	// for
//	printf("Th #%ld ended\n", tid);

	pthread_exit(NULL);
	return NULL;
}	// wykonajPetleNaWatku




void SiecNeuronowaDynamika::inicjalizujZmiennePrzedSymulacja(){
	// inicjalizacja statystyk
	nrIteracji = 0L;
	iloscWszystkichLosowan = 0L;
	iloscWszystkichLosowanE09 = 0L;
	iloscZmianNaDlugiejKrawedzi = 0L;
	iloscEfektywnychZmian = 0;
	iloscPrzeslanDoSiebie = 0;
	iloscZmianPodGore = 0;
	iloscOdrzuconychZmian = 0;
	iloscOminietychZmian = 0;
	iloscUsuniecKrawedzi = 0;
	iloscPrzywrocenKrawedzi = 0;
	iloscUsuniecLadunku = 0;

	czasRozpoczecia = time(NULL);
	czasomierz1 = czasRozpoczecia;

	logJP<< "ilosc Krawedzi    = "<< getIloscKrawedzi() <<"\n";
	long x = obliczIloscKrawedziPowyzejProgu();
	logJP<< "progDlugosciKr    = "<< getprogDlugosciKrawedzi() <<" / " << n.getMaxPos()<<  "\n";
	logJP<< "ilKrawedziPowyzejProgu = "<<  x <<" ==  " << (x * 1.0 / getIloscKrawedzi()) << "\n";
	logJP<< "usuwanieKrawedzi       = " << (n.getConfig()->getEdgesRemoval() ? "true" : "false") << "\n";
	logJP<< "usuwanieLadunku        = " << (czyUsuwacLadunek  ? "true" : "false") << "\n";
	logJP<< "P-o usuniecia ladunku  = "<<  prawdopodobienstwoUsunieciaLadunku << "\n";
	logJP<< "dodawanieLadunku       = " << ( n.getConfig()->getChargeReappear()  ? "true" : "false") << "\n";


	if ( x * 1.0 / getIloscKrawedzi() > .05){
		std::cerr<< "\nUWAGA: duza ilosc krawedzi powyzej progu " << (x * 1.0 / getIloscKrawedzi()) << "\n\n";
	}	// if

	logJP<<"\n0  10   20   30   40   50   60   70   80   90  100\n";
}	// SiecNeuronowa::inicjalizujZmiennePrzedSymulacja()

/* wykonanie jednego ktoku przez watek o numerze...
 * watek numer zero czasem robi cos jeszcze ...
 */
void SiecNeuronowaDynamika::wykonajPojedynczyKrokNaWatku(long tid){
	// zwiekszamy numer iteracji, drukujemy # jak potrzeba

	iteracja_watki_zwiekszNumerIteracji(tid);


	if (flagaLadunekWyczerpany == true){
		//  === czy na pewno?
		return;
	}	// fi


	// to moze bolec!
	Polaczenie &k1 = getKrawedz(0);
	int i1=-1, i2 = -1, ik = -1;
	bool ret;

	// losujemy krawedz, jesli ret = 0 to losowanie zakonczone niepowodzeniem
	// moze lepiej niech zwrca indeks krawedz!!

	ret = iteracja_watki_wylosujKrawedz(k1, ik, i1, i2);

	if (ret == false){
		return;
	}	// fis

	Polaczenie & k = getKrawedz(ik);

	// teraz zawsze jest przeplyw z i1/n1 >>>> i2/n2
	Neuron& n1 = getNeuron(i1);
	Neuron& n2 = getNeuron(i2);

	// if przeplyw do samego siebie ...
	// wylosowana krawedz do samego siebie
	if (i1 == i2){
		n1.mutexLock();

		ret = iteracja_watki_obslozKrawedDoSiebie(k, i1, i2);

		// zapominamy o ret, otwieramy muteksy i konczymy!
		n1.mutexUnLock();
		return;
	}	// fi i1==i2

	if (i1 != i2){
		// maly trick, najpierw jest blokowany muteks wyzszego neuronu -
		// zabezpieczenie przed deadlockiem
		if (i1>i2){
			n1.mutexLock(); n2.mutexLock();
		} else {
			n2.mutexLock(); n1.mutexLock();
		}	// if

		iteracja_watki_obslozKrawedzDoInnegoNeuronu(k, i1, i2);

		n2.mutexUnLock();
		n1.mutexUnLock();
		return;
	}	// if i1 !=i2

	return;

}	//  wykonajPojedynczyKrokNaWatku()


/*
 */
void SiecNeuronowaDynamika::iteracja_watki_zwiekszNumerIteracji(long tid){
	pthread_mutex_lock(& mutex_nrIteracji);
	nrIteracji++;

	// progressbar
	unsigned int max = 50;
	unsigned int blok = (ilIteracji / max);


	if (nrIteracji % blok == blok-1){
		// XXX Marker że ogólnie tu jest wykonywany tic co iles iteracji

		logJP<< "#";



		// przeliczenia sa lokalne i bledow nie powinno byc... nie powinno
		// ale liczne poprawki do energii klada koncowa wartosc energii na lopatki
		// dlatego co 2% energia jest przeliczana od zera (w zasadzie potrzebna tylko do wykresu na koniec)
		n.mutexEnergiaLock();
		n.obliczEnergie();
		n.mutexEnergiaUnLock();

		// co 2% usuwamy jalowe krawedzie
		// usuwa watek ktory akurat ma te iteracji - niekoniecznie zerowy!
		if (n.getConfig()->getEdgesRemoval()){
			usunKrawedzieZListyLosowania();
		}	// fi


		// i zrzucamy kilka liczb ze stanu sieci do logu
		if (n.getConfig()->getLogProgress()){
			simulationProgresLog->logProgress(this);
		}	// if

		#ifdef WITH_LAPACK
			int procent = (int) ((nrIteracji+1) * 1.0/ ilIteracji * 100.0);

			// co jakis tick robimy zrzut spektrum do pliku
//			if (procent == 2 || procent == 10 || procent == 26 || procent == 50 || procent == 76
//					|| procent == 100 ){
			if (n.getConfig()->getCalculateSpectrumEvolution()){
				if (procent == 2 || (procent % 10 == 0 && procent >0)){
					spectSaver->generateCalculationTic();
				}	// if
			}	// if
		#endif

	}	// fi

	// co 2%
	// naprawde
	unsigned int maxS = 50;
	unsigned int blokS = (ilIteracji / maxS);


	if (n.getConfig()->getCalculateNodesEvolution() && nrIteracji % blokS == blokS-1){
		vertSaver->generateCalculationTic();
	}	//	// if


	pthread_mutex_unlock(& mutex_nrIteracji);
} 	// iteracja_watki_zwiekszNumerIteracji()

/*
 *
 */
bool SiecNeuronowaDynamika::iteracja_watki_wylosujKrawedz(Polaczenie &k, int &ik, int & i1, int & i2){
	Polaczenie k1  = wylosujKrawedzZNiezerowymNeuronem();

	wybierzZwrotKrawedzi(k1, i1, i2);
	if (i1 == -1){
		pthread_mutex_lock(& mutex_iloscZmianOdrzuconych);
		iloscOdrzuconychZmian++;
		pthread_mutex_unlock(& mutex_iloscZmianOdrzuconych);
		return false;
	}	// if

	ik = k1.getI();
	return true;
}	// iteracja_watki_wylosujKrawedz(Polaczenie &k int & i1, int & i2)

/*
 * wewnatrz sa muteksy na sn,
 * nie ma muteksow na n1 n2!!!
 */
bool SiecNeuronowaDynamika::iteracja_watki_obslozKrawedDoSiebie(Polaczenie &k, int i1, int i2){
	if (i1 != i2){
		printf("iteracja_watki_obslozKrawedzDoSiebie(), i1 != i2  %d %d\n", i1, i2);
		return false;
	}	// fi

	if (k.getIndeks1() != i1 || k.getIndeks2() !=i2){
		printf("iteracja_watki_obslozKrawedzDoSiebie(), k.i1 != k.i2  %d %d\n", k.getIndeks1(), k.getIndeks2());
		return false;
	}	// fi

	Neuron & n1 = getNeuron(i1);

	// przesylamy ladunek
	k.zwiekszLadunekDo2(1);

	// ??
	getN().mutexLock();
	iloscPrzeslanDoSiebie++;
	ostatniaZmiana = nrIteracji;
	getN().mutexUnLock();

	// z prawdopodobienstwem (tu 1.0) usuwamy ladunek z systemu
	if (czyUsuwacLadunek && losujRBool(prawdopodobienstwoUsunieciaLadunku)){

		float ee1 = obliczEnergieWNeuronie( i1);
		n1.zmniejszLadunek();
		n.mutexLock();
		iloscUsuniecLadunku++;
		iloscEfektywnychZmian++;
		n.mutexUnLock();
		float ee2 = obliczEnergieWNeuronie( i1);

		n.mutexEnergiaLock();
		n.setEnergia( n.getEnergia() -ee1 + ee2);
		n.mutexEnergiaUnLock();
	}	else {// fi losujRBool()
		pthread_mutex_lock(&mutex_iloscZmianOdrzuconych);
		iloscOdrzuconychZmian++;
		pthread_mutex_unlock(&mutex_iloscZmianOdrzuconych);
	}
	// konczymy petle
	return true;
}	// iteracja_watki_obslozKrawedDoSiebie

bool SiecNeuronowaDynamika::iteracja_watki_obslozKrawedzDoInnegoNeuronu(Polaczenie &k, int i1, int i2){
	if (i1 == i2){
		printf("iteracja_watki_obslozKrawedzDoInnegoNeuronu(), i1 == i2  %d %d\n", i1, i2);
		return false;
	}	// fi

	if (i1 != k.getIndeks1() &&  i1!= k.getIndeks2()){
		printf("iteracja_watki_obslozKrawedzDoInnegoNeuronu(), k.i1  %d != i1 %d != k.i2 %d\n",
				k.getIndeks1(),  i1, k.getIndeks2());
		return false;
	}	// fi

	if (i2 != k.getIndeks1() &&  i2!= k.getIndeks2()){
		printf("iteracja_watki_obslozKrawedzDoInnegoNeuronu(), k.i1 != i2 != k.i2  %d\n", i2);
		return false;
	}	// fi

	Neuron & n1 = getNeuron(i1);
	Neuron & n2 = getNeuron(i2);

	// liczymy zmiane energii - tak czy inaczej policzycj ja trzeba :P
	// bezz muteksu choc mozna zobaczyc!
	float zmiana = obliczZmianeEnergii(i1, i2);

	// jezeli energia zmalala lub z P=em exp(-beta*zmiana) akceptujemy, lub dluga krawedz
	if ( zmiana < 0 || losujRBool( exp(- n.getBeta() * zmiana) ) ){

		//akceptuj ==  przenies 1 ladunek z n1 do n2,
		// do tego zapisujemy ze nastapil ten przeplyw
		// zakladamy ze muteks jest zalozony!
		n2.zwiekszLadunek( );

		if (k.getIndeks2() == i2){
			k.zwiekszLadunekDo2(1);
		}	else {
			k.zwiekszLadunekDo1(1);
		}	// if .. else

		n1.zmniejszLadunek();


		n.mutexEnergiaLock();
		// JP po co w ogole liczyc te energie?
		n.setEnergia(n.getEnergia()  +zmiana);
		n.mutexEnergiaUnLock();


		pthread_mutex_lock(& mutex_iloscZmianPrzyjetych);
		iloscEfektywnychZmian++;
		if (zmiana>0)
			iloscZmianPodGore++;
		pthread_mutex_unlock(& mutex_iloscZmianPrzyjetych);

		pthread_mutex_lock(& mutex_nrIteracji);
		ostatniaZmiana = nrIteracji;
		pthread_mutex_unlock(& mutex_nrIteracji);

		// uaktualnianie minimalnych ladunkow wywalone w inny fragment kodu!

		// w tej wersji histogram zmian nie jest liczony!!!
		return true;
	}	// if

	// wpw odrzucamy
	pthread_mutex_lock(& mutex_iloscZmianOdrzuconych);
	iloscOdrzuconychZmian++;
	pthread_mutex_unlock(& mutex_iloscZmianOdrzuconych);
	return true;
} 	// iteracja_watki_obslozKrawedzDoInnegoNeuronu


/* "oblicz" === sprawdz kazda z krawedzi, jak bardzo potrzebne to zrobic raz i zapisac wynik!!!!
 *
 */
long SiecNeuronowaDynamika::obliczIloscKrawedziPowyzejProgu(){
	if (progDlugosciKw <=0)
		return 0;

	long ret = 0;

	for (long i =0; i< n.getIloscKrawedzi(); i++){
		Polaczenie p = getKrawedz(i);
		float d = getNeuron(p.getIndeks1()).odlegloscKw( getNeuron(p.getIndeks2()) );

		if (d > progDlugosciKw)
			ret ++;
	}	// for
	return ret;
}	// obliczIloscKrawedziPowyzejProgu

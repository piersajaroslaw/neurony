#ifndef SIECNEURONOWADYNAMIKA_H_
#define SIECNEURONOWADYNAMIKA_H_
/*
 * siecNeuronowaDynamika.h
 *
 *  Created on: Aug 28, 2010
 *      Author: piersaj
 *
 *
 *  2010-08-28
 *  	utworzenie
 *  	klasa przejmuje czesc dawnej funkcjonalnosci SN - to co zwiazane jest z dynamika sieci
 *  	oj bedzie bolalo...
 *  2011-03-23
 *  	w międzyczasie dopisaliśmy kilka bajerków do dynamiki i cały czas coś rozwijamy
 *  	min zapis spektrum, grafu i krawedzi co kilka procent a nie raz na koniec
 */



#include <pthread.h>
#include "../network/siecNeuronowa.h"
#include "../visualisation/verticesEvolutionSaver.h"
#include "../visualisation/synapseEvolutionSaver.h"

#include "../statistics/spectrum/spectrumEvolutionSaver.h"


class SiecNeuronowaDynamika{
private:
	// siec neuronowa
	SiecNeuronowa & n;

	// liczniki zmian smieci
	unsigned int iloscZmianPodGore;
	unsigned int iloscEfektywnychZmian;
	unsigned int iloscPrzeslanDoSiebie;
	unsigned long iloscOdrzuconychZmian;
	unsigned long iloscOminietychZmian;
	unsigned long iloscZmianNaDlugiejKrawedzi;
	unsigned long iloscWszystkichLosowan;
	unsigned long iloscWszystkichLosowanE09;
	unsigned int iloscUsuniecKrawedzi;
	unsigned int iloscPrzywrocenKrawedzi;
	unsigned int iloscUsuniecLadunku;


	unsigned long ilIteracji;
	unsigned long nrIteracji;
	unsigned long ostatniaZmiana;

	VerticesEvolutionSaver * vertSaver;
	SpectrumEvolutionSaver * spectSaver;
	SynapsesEvolutionSaver * synapseSaver;

	// mutex na watki - energia, liczniki, jeszcze troche dodamy
	pthread_mutex_t mutex_nrIteracji, mutex_iloscZmianPrzyjetych, mutex_iloscZmianOdrzuconych,
		mutex_iloscLosowan;

	pthread_rwlock_t rwLockKrawedzieLosowane;


	// prog dlugosci krawedzi powyzej ktorego przeplyw zawsze jest akceptowany
	// -1 znaczy ze nigdy
	float progDlugosciKw ;
	float prawdopodobienstwoUsunieciaLadunku;

//	bool czyUsuwacKrawedzie;
	bool czyUsuwacLadunek;
//	bool czyDodawacLadunek;

	bool flagaLadunekWyczerpany;

	std::vector<int> krawedzieLosowane;
	float minWsparcieDodatniLadunek;

public:

	/*  ***********************************************
	 *
	 *  konsruktory
	 *
	 *  ***********************************************/


	SiecNeuronowaDynamika(SiecNeuronowa &n);
	~SiecNeuronowaDynamika();

	/*  ***********************************************
	 *
	 *  kilka "przeciazonych" metod
	 *
	 *  ***********************************************/


	inline SiecNeuronowa & getN(){
		return n;
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

	/*  ***********************************************
	 *
	 *  Settery i gettery
	 *
	 *  ***********************************************/

	inline unsigned long getIloscWszystkichLosowan(){
		return iloscWszystkichLosowan;
	}

	inline unsigned long getIloscWszystkichLosowan0E9(){
		return iloscWszystkichLosowanE09;
	}

	inline double getIloscWszystkichLosowacDouble(){
		return 1.0E+9* iloscWszystkichLosowanE09 + iloscWszystkichLosowan;
	}


	inline float getprogDlugosciKrawedzi(){
		if (progDlugosciKw <=0)
			return progDlugosciKw;
		else
			return sqrt(progDlugosciKw);
	}

	inline void setProgDlugosciKrawedzi(float val){
		if (val <=0)
			progDlugosciKw = val;
		else
			progDlugosciKw = val * val;
	}

	inline float getPrawdopodobienstwoUsunieciaLadunku(){
		return prawdopodobienstwoUsunieciaLadunku;
	}

	inline void setPrawdopodobienstwoUsunieciaLadunku(float val){
		if (val >=0 && val<=1)
			prawdopodobienstwoUsunieciaLadunku = val;
	}

	inline int getIloscUsuniecLadunku() const{
		return iloscUsuniecLadunku;
	}

	inline long getNumerBiezacejIteracji() const{
		return nrIteracji;
	}

	inline int getIloscEfektywnychZmian() const{
		return iloscEfektywnychZmian;
	}

	inline int getIloscUsuniecKrawedzi() const{
		return iloscUsuniecKrawedzi;
    }

    bool getCzyUsuwacKrawedzie() const{
        return czyUsuwacKrawedzie;
    }

    bool getCzyUsuwacLadunek() const{
        return czyUsuwacLadunek;
    }

    unsigned long getIlIteracji() const{
        return ilIteracji;
    }

    unsigned long getIloscOdrzuconychZmian() const {
        return iloscOdrzuconychZmian;
    }

    unsigned int getIloscPrzeslanDoSiebie() const{
        return iloscPrzeslanDoSiebie;
    }

    unsigned int getIloscPrzywrocenKrawedzi() const{
        return iloscPrzywrocenKrawedzi;
    }

    unsigned long getIloscWszystkichLosowanE09() const{
        return iloscWszystkichLosowanE09;
    }

    unsigned long getIloscZmianNaDlugiejKrawedzi() const{
        return iloscZmianNaDlugiejKrawedzi;
    }

    unsigned int getIloscZmianPodGore() const{
        return iloscZmianPodGore;
    }

    std::vector<int> getKrawedzieLosowane() const{
        return krawedzieLosowane;
    }

    pthread_mutex_t getMutex_nrIteracji() const{
        return mutex_nrIteracji;
    }

	inline long getIloscIteracji(){
		return ilIteracji;
	}

	inline void setIloscIteracji(long il){
		if (il>0)
			ilIteracji = il;
	}

    unsigned long getNrIteracji() const{
        return nrIteracji;
    }

    unsigned long getOstatniaZmiana() const{
        return ostatniaZmiana;
    }

    float getProgDlugosciKw() const{
        return progDlugosciKw;
    }

//    void setCzyUsuwacKrawedzie(bool czyUsuwacKrawedzie){
//        this->czyUsuwacKrawedzie = czyUsuwacKrawedzie;
//    }

    void setCzyUsuwacLadunek(bool czyUsuwacLadunek){
        this->czyUsuwacLadunek = czyUsuwacLadunek;
    }

    void setIlIteracji(unsigned long  ilIteracji){
        this->ilIteracji = ilIteracji;
    }

    void setProgDlugosciKw(float progDlugosciKw){
        this->progDlugosciKw = progDlugosciKw;
	}

    /* **************************************************
     *
     *
     *  D Y N A M I K A
     *
     *
     * **************************************************/

	/* symulacja z tym ze uruchamiana na kilku watkach
	 */
	void rozpocznijSymulacjeNaWatkach();

public:
	/* pojedynczy krok - watkoodporne
	 */
	void wykonajPojedynczyKrokNaWatku(long tid);

private:
	void inicjalizujZmiennePrzedSymulacja();
	void zwiekszIloscLosowan();
	void zwiekszIloscLosowan(long oIle);

	void iteracja_watki_zwiekszNumerIteracji(long tid);

	/* funkcja losuje krawedz k i ustala jej konce tak ze z i1 jest przesylany ladunek do i2
	 *  zwraca jezeli zwraca false to przerywamy iteracje
	 */

	Polaczenie& wylosujKrawedzZNiezerowymNeuronem();
	bool iteracja_watki_wylosujKrawedz(Polaczenie &k, int &ik, int & i1, int & i2);
	/* obsluga przeslania ladunku do samego siebie,
	 * krawedz k jest wylosowana krawedzia do samego siebie,
	 * i1 i i2 sa tym samym indeksem neuronu polaczonego krawedzia k
	 * w przypadku bledow zwraca false!
	 *
	 */

	bool iteracja_watki_obslozKrawedDoSiebie(Polaczenie &k, int i1, int i2);
	/*  obsluga przeslania do innego neuronu przez krawedz
	 * k  - krawedz laczaca neurony,
	 * i1 i2 neurony na krawedzi, przesylamy i1 -> i2
	 *
	 */
	bool iteracja_watki_obslozKrawedzDoInnegoNeuronu(Polaczenie &k, int i1, int i2);

	/* usuwa krawedzie z puli losowanej jezeli sa za slabe
	 */
	void usunKrawedzieZListyLosowania();
	/* metoda ustala indeksy z polaczenia tak zeby z i1_z zawsze dalo sie wyslac ladynek
	 * wymaga krawedzi takiej ze ktorys z neuronow ma ladunek
	 */
	void wybierzZwrotKrawedzi(Polaczenie &k, int &i1_z, int &i2_do);


	void podsumujSymulacje();


	/* smiecenie po strumieniach wymaga kilku przeliczen
	 * ilosci neuronow o ladunki powyzej n
	 *
	 */
	void podsumujSymulacjeIlosciNeuronowZLadunkiem();


	/* i jeszcze jedno podsumowanie
	 * kilka statystyk na wsparciach
	 */
	void podsumujWynikiNaWsparciach();

public:

	/* zwraca (szukajac!) maksymalny przeplyw przez krawedz
	 *
	 */
	long znajdzMaksymalnyPrzelpywKrawedzi();




	/* przelicza zmiane energii gdy z n1 przeplynie 1 ladunek do wezla n22
	 * waga miedzy wezlami wynosi waga
	 *
	 * zal ze ladunek n1 jest wiekszy rowny 1
	 *
	 * liczy tylko na podstawie sasiadow n1 i n2 (bo od reszty nie zalezy)
	 *
	 */
	float obliczZmianeEnergii(int i1, int i2);

	/* oblicza energie na z grawedzi incydentnych zz neuronem indeks
	 */
	float obliczEnergieWNeuronie( int indeks);


	/* zwraca losowa krawedz w grafie
	 * taka ze jeden z wierzcholkow ma dodatni ladunek
	 */
	void przywrocKrawedzie(int i);

	long obliczIloscKrawedziPowyzejProgu();
};


extern SiecNeuronowaDynamika * siecNeuronowaDynamikaGlobalna;
#endif /* SIECNEURONOWADYNAMIKA_H_ */

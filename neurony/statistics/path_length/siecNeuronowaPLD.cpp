/*
 * siecNeuronowaPLD.cpp
 *
 *  Created on: Oct 25, 2010
 *      Author: piersaj
 *
 *  2010-10-25
 *  	utworzenie
 *  2010-12-16
 *  	Coo?? Przez dwa miesiące nic??
 *  	A Floyda-Warshalla to niby kto i kiedy napisał?
 *  	kilka korekt do liczenia średniej
 *  2010-12-17
 *  	dodane liczenie rozklady na podstawie mc
 *  	sprawic jak bedzie dzialalo!
 *  2010-12-18
 *  	dopisany MC - działa... szybciej!
 *  2010-12-20
 *  	kilka korekt do MC
 *  	Michał zasugerował że odpalenie Dijkstry n razy wyjdzie lepiej albo porównywalnie
 *  	(lepiej od FW, a z MC to nie wiadomo)
 *
 */

#include <cstdlib>
#include "siecNeuronowaPLD.h"
#include "../../config/simulationMasterConfig.h"
#include "../../../src/logger.h"



SiecNeuronowaPLD::SiecNeuronowaPLD(SiecNeuronowaDynamika &n_):
	n(n_),
	softThresh(5000),
	hardThresh(20000)
{
	maxDlugoscSciezki = 100;
	iloscKrokowMC = 1000000;
	rozkladDlugosciSciezki = new int[maxDlugoscSciezki];
	apl_val = -1;
	for (int i=0; i<maxDlugoscSciezki; i++){
		rozkladDlugosciSciezki[i]  = 0;
	}	// for
}	// this()


SiecNeuronowaPLD::~SiecNeuronowaPLD(){
	delete[] rozkladDlugosciSciezki;
	rozkladDlugosciSciezki = (int*) NULL;
	maxDlugoscSciezki = 0;
}	// ~this();

void SiecNeuronowaPLD::obliczRozkladDlugosciSciezek(){
	logJP.timestamp() << "PLD: start \n";

	if (n.getIloscNeuronow() > softThresh){
		logJP.timestamp() << "PLD: siec ma "<< n.getIloscNeuronow()
		<< " obliczanie rozkładu może zająć dużo czasu. Zalecane uzycie MC co obliczeń\n";
	}	// fi

	if (n.getIloscNeuronow() > hardThresh){
		logJP.timestamp() << "PLD: wywoluje MC do obliczen (i tak sie tablica w ramie nie zmiesci...)\n";
		obliczRozkladDlugosciSciezekMC();
		logJP.timestamp() << "PLD: koniec\n";
		return;
	}	// fi


	obliczTabliceSciezek();


	for (int i=0; i<n.getIloscNeuronow(); i++){
		for (int j=0; j<i; j++){
			int v = getTabDlugoscSciezki(i, j);
			if (v<maxDlugoscSciezki)
				rozkladDlugosciSciezki[v]++;
			else
				tablicaDlugosciSciezek[maxDlugoscSciezki]++;
		}	// for j
	}	// for i
	zapiszRozkladDlugosciSciezekDoPliku();
	obliczAPL();
	logJP.timestamp() << "PLD: koniec\n";
}	// obliczRozkladDlugosciSciezek()

void SiecNeuronowaPLD::obliczRozkladDlugosciSciezekMC(){
	logJP.timestamp() << "PLD MC: start \n";
	int n = getN().getIloscNeuronow();
	int i1 = -1, i2 = -1, dl = -1;

	// zerowanie tablicy
	for (int i=0; i<maxDlugoscSciezki; i++){
		rozkladDlugosciSciezki[i] = 0;
	}	// for

	// mc
	for (int i=0; i< iloscKrokowMC; i++){
		i1 = rand()% n;
		i2 = i1;
		while (i2==i1){
			i2 = rand()%n;
		}	// while
		dl = obliczDlugoscSciezki(i1, i2);

		if (dl < maxDlugoscSciezki)
			rozkladDlugosciSciezki[dl]++;
		else
			rozkladDlugosciSciezki[maxDlugoscSciezki-1]++;
	}	// for i

	obliczAPL();

	zapiszRozkladDlugosciSciezekDoPliku(true);
	logJP.timestamp() << "PLD MC: koniec\n";
}	// obliczRozkladDlugosciMC()

// aka Floyd Warshall
void SiecNeuronowaPLD::obliczTabliceSciezek(){
	inicjalizujTabliceSciezek();
	int a=0, b=0;
//	int lastK=0;
	for (int k=0; k<n.getIloscNeuronow(); k++){
		for (int i=0; i<n.getIloscNeuronow(); i++){
			for (int j=0; j<i-1; j++){
				a = getTabDlugoscSciezki(i, j);
				b = getTabDlugoscSciezki(i, k) + getTabDlugoscSciezki(k, j);
				if (b<a){
					setTabDlugoscSciezki(i, j, b);
//					lastK = k;
				}	// fi
			}	// for j
		}	// for i
		if (k%1000==999){
			logJP << ".";
//			printf(" %d\n", lastK);
		}	// fi
	}	// for k
	logJP << "\n";
}	// obliczTabliceSciezek

void SiecNeuronowaPLD::inicjalizujTabliceSciezek(){
	tablicaDlugosciSciezek = new int*[n.getIloscNeuronow()];

	// ustawieamy sciezke na "nieskonczonosc"
	for (int i=0; i<n.getIloscNeuronow(); i++){
		tablicaDlugosciSciezek[i] = new int[i+1];
		for (int j=0; j< i+1; j++){
			tablicaDlugosciSciezek[i][j] = maxDlugoscSciezki+1;
		}	// for j
	}	// for i

	// sam do siebie == 0
	for (int i=0; i<n.getIloscNeuronow(); i++){
		setTabDlugoscSciezki(i, i, 0);
	}	// foe i

	int il=0;
	for (int i=0; i<n.getIloscKrawedzi(); i++){
		Polaczenie k = n.getKrawedz(i);
		if (k.getSumaLadunku()>0){
			il++;
			setTabDlugoscSciezki(k.getIndeks1(), k.getIndeks2(), +1);
		}	// fi
	}	// for i
	logJP << "PLD(): il wpisow w tablicy " << il <<"\n";
}	// inicjalizujTanliceSciezek

float SiecNeuronowaPLD::getAPL(){
	if (apl_val <0)
		printf("SNPLD() warn: apl value not ready\n");
	return apl_val;
}

void SiecNeuronowaPLD::obliczAPL(){
	double ret = 0;
	int suma = 0;
	for (int i=0; i<maxDlugoscSciezki; i++){
		suma+=rozkladDlugosciSciezki[i];
	}	// for i


	for (int i=0; i<maxDlugoscSciezki; i++){
		ret += i* (rozkladDlugosciSciezki[i]*1.0/suma);
	}	// for i
	apl_val = ret;
	// APL w modelu Erdosa Reyniego


	logJP << "SN_PLD: APL =" << ret<<"\n";
}	// obliczAPL()

void SiecNeuronowaPLD::wypiszRozkladSciezek(){
	for (int i=0; i< 30; i++){
		printf("% 6d", rozkladDlugosciSciezki[i]);
	}	// for
	printf("\n\n");
}	// wypiszRozkladSciezek()


/*
 * Zapis rozkladu do pliku
 */
void SiecNeuronowaPLD::zapiszRozkladDlugosciSciezekDoPliku(bool usedMC){

	char filename[255];

	if (n.getN().getConfig()->getUseTimestampInFilenames() == true) {
		sprintf(filename, "%s/r_%s__%d_kn_%d_Me_%ldM_it_path_length_distribution.m",
				n.getN().getConfig()->getResultsFolder(),
				n.getN().getTimestamp(),
				n.getN().getIloscNeuronow()/1000,
				n.getN().getIloscKrawedzi()/1000000,
				n.getN().getIloscIteracji() / 1000000
		);
	} else {
		sprintf(filename, "%s/path_length_distribution.m", n.getN().getConfig()->getResultsFolder());
	}	// if .. else


	logJP.timestamp_nl() << "degeneracy_: path length: saving results to file "<< filename << " ... ";


	std::fstream str;
	str.open(filename, std::fstream::out );

	if (str == NULL){
		std::cerr<< "SNPLD.zapiszWyniki() nie mozna utworzyc pliku\n\n";
		return;
	}	// fi

	str << "# timestamp   "<< getN().getN().getConfig()->getTimestamp()<< '\n';
	str << "# Il neuronow "<< getN().getIloscNeuronow() << '\n';
	str << "# Il krawedzi "<< getN().getIloscKrawedzi() << '\n';
	str << "# Il iteracji "<< (int) getN().getIloscIteracji() << '\n';
	if (usedMC)
		str << "# mc ilosc krokow "<< iloscKrokowMC << '\n';

	str << "APL = "<< getAPL() <<"\n";

	str << "Y = [\n";

	for (int i=0; i<maxDlugoscSciezki ; i++){
		str <<  i << ' ' << getRozkladDlugosciSciezki()[i] << ";\n";
	}	// for
	str << "];\n\n";

	str << "plot(Y(:,1), Y(:,2), \"r\");\n";

	str << "# loglog(Y(:,1), Y(:,2), \"b+\");\n"
		<< "grid(\'on\');\n"
		<< "legend(\"Path Length Distribution\");\n"
		<< "S = sprintf(\"neurons = %d\", "<<  getN().getIloscNeuronow() << ");\n"
		<< "text(11, 170, S, \'fontsize\', 14);\n";

	str << "% print( \"/home/piersaj/Desktop/nn/RozkladDlugosciSciezek_"
		<< getN().getN().getConfig()->getTimestamp();

	char buffer[80];
	sprintf(buffer, "_%dk_%dM_%ldM---XXX.svg",
		getN().getIloscNeuronow()/1000,
		getN().getIloscKrawedzi()/1000000,
		getN().getIloscIteracji() / 1000000
	);

	str << buffer << "\" , \"-color\"); \n\n";
	str.close();
	logJP << "SNPLD.zapisWynikow(): ok\n";
}	// zapiszRozkladDlugosciSciezekDoPliku()();




int SiecNeuronowaPLD::obliczDlugoscSciezki(int i1, int i2){
	if (i1==i2)
		return 0;

	// z jakiegos powodu daje o jeden za duzo... albo o jeden za malo
	// juz poprawione - tak sadze
	bool czyOdwiedzony[n.getIloscNeuronow()];
	for (int i=0; i<n.getIloscNeuronow(); i++){
		czyOdwiedzony[i] = false;
	}

	std::queue<int> kolejka  = std::queue<int>();
	kolejka.push(i1);
	int glebokosc = -1;
	int nastepny = i1;
	czyOdwiedzony[i1] = true;
	bool flagaTrzebaUstawicNastepnego = false;
	int wierzcholek = -1;

	int sasiad =-1;

	// BFS
	while (kolejka.empty() == false){
		wierzcholek  = kolejka.front();
		kolejka.pop();

		// w pierwszej iteracji startowy PO tym punkcie powinien miec zero!
		if (wierzcholek == nastepny){
			glebokosc++;
			flagaTrzebaUstawicNastepnego = true;
		}	// fi

		// dodajemy Sasiadow
		Neuron & nw = n.getNeuron(wierzcholek);
		for (int j=0; j< nw.getIloscSasiadow(); j++){
			sasiad = nw.getIndeksKrawedzi(j);
			Polaczenie & k = n.getKrawedz(sasiad);

			if (k.getSumaLadunku() == 0){
				continue;
			}	// fi

			sasiad = k.getIndeks1() == wierzcholek ? k.getIndeks2() : k.getIndeks1();

			// znalezlismy
			if (sasiad == i2 ){
				// kolejka.clear();
				while (kolejka.empty()== false)
					kolejka.pop();
				// na poczatku o jeden raz za duzo zwieksza
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

	// Situation Normal All ... Up
	printf("PLD.BFS(): brak sciezki pomiedzy %d %d <- dziwne...\n", i1, i2);
	return glebokosc+100;
}	// obliczDlugoscSciezki

/*
 * siecNeuronowaCC.cpp
 *
 *  Created on: Aug 30, 2010
 *      Author: piersaj
 *
 *  ????-??-??
 *  	????
 *  2011-07-14
 *  	added gamma value
 *  2011-12-14
 *  	instead of cc hist, now we count ss vals for all of the neurons... plot them as you want :P
 *  2012-07-20
 *  	some crazy bug fixed
 */
#include "siecNeuronowaCC.h"

SiecNeuronowaCC::SiecNeuronowaCC(SiecNeuronowaDynamika &n_):
	n(n_)
{
	ccRozkladLen = n.getIloscNeuronow();
	ccRozklad = new float[ccRozkladLen];

	ccFaktyczny = ccTeoretyczny = -1;
}	// this(SND &)


SiecNeuronowaCC::~SiecNeuronowaCC(){
	if (ccRozkladLen >0){
		delete[] ccRozklad;
		ccRozkladLen = 0;
	}	// fi
}	// ~this()


void SiecNeuronowaCC::obliczRozkladCCDlaGrafu(){

	logJP << "liczenie rozkladu CC... ";

	for (int i=0; i<ccRozkladLen; i++){
		ccRozklad[i] = 0;
//		ccIlosc[i] = 0;
	}	// for

	for (int i=0; i< getN().getIloscNeuronow() ; ++i){
		float cc = obliczCCtylkoKrawedzieWychodzace(i);
		// cc values - one by one
		ccRozklad[i]=cc;

		// CC empirical histogram version
//		int val = round(cc * ccRozkladLen);
//		// taki failsafe - moze trochę kontrowersyjny
//		if (val < 0) {
//			val = 0;
//		} else if (val >= ccRozkladLen){
//			val = ccRozkladLen-1;
//		}	// fi
//
//		ccRozklad[i]=val;
	}	// for i neurony
	logJP<< " ok\n";

	obliczFaktycznyITeoretycznyCCdlaCalejSieci();
}	// obliczRozkladCCdlaGrafu

void SiecNeuronowaCC::obliczFaktycznyITeoretycznyCCdlaCalejSieci(){
	double suma = 0;
	int nnzVals = 0;
	for (int i=0; i<ccRozkladLen; i++){
		//suma += i*1.0 / ccRozkladLen * ccRozklad[i];
		if (ccRozklad[i] >=0){
			suma += ccRozklad[i];
			nnzVals ++;
		}	// fi
	}	// for

	// avg
	ccFaktyczny = suma / nnzVals;

	int iloscKr = 0;
	for (int i=0; i< getIloscKrawedzi(); i++){
		if (getKrawedz(i).getSumaLadunku() >=1){
			iloscKr ++;
		}	// fi
	}	//
	ccTeoretyczny = iloscKr * 2.0  / getIloscNeuronow() /(getIloscNeuronow() -1);

	logJP << "CC Faktyczny                      = " << ccFaktyczny <<"\n"
		 <<  "CC Teoretyczny (odp. siec losowa) = " << ccTeoretyczny<<"\n"
	 	 <<  "CC gamma (CC_Real / CC_Teo)       = " << ccFaktyczny / ccTeoretyczny <<"\n";
}	// obliczFaktycznyITeoretycznyCCdlaCalejSieci()

/*
 * Zapis rozkladu do pliku
 */
void SiecNeuronowaCC::zapiszRozkladCCStopniDoPliku(){

	char filename[255];

	if (n.getN().getConfig()->getUseTimestampInFilenames() == true) {
		sprintf(filename, "%s/r_%s__%d_kn_%d_Me_%ldM_it_rozkladCC.m",
				getN().getN().getConfig()->getResultsFolder(),
				getN().getN().getTimestamp(),
				getN().getIloscNeuronow()/1000,
				getN().getIloscKrawedzi()/1000000,
				getN().getIlIteracji() / 1000000 );
	} else {
		sprintf(filename, "%s/rozkladCC.m", n.getN().getConfig()->getResultsFolder());
	}	// if .. else

	std::fstream str;
	str.open(filename, std::fstream::out );

	if (str == NULL){
		std::cerr<< "SNCC.zapiszWyniki() nie mozna utworzyc pliku\n\n";
		return;
	}	// fi

	str << "# timestamp   "<< getN().getN().getTimestamp() << '\n';
	str << "# Il neuronow "<< getN().getIloscNeuronow() << '\n';
	str << "# Il krawedzi "<< getN().getIloscKrawedzi() << '\n';
	str << "# Il iteracji "<< (int) getN().getIloscIteracji() << '\n';


	str << "Y = [\n";

	for (int i=0; i< ccRozkladLen; i++){
		str << getCCRozlad()[i] << ", \n";
	}	// for
	str << "];\n\n";




	str << "n  = 1000;"
			<< "[a,b] = hist(Y, n);"
			<< "hold off;"
			<< "loglog(b,a, \"b+\");"
			<< "hold on;"
			<< "loglog(b,a, \"b\");"
			<< "grid('on');"
			<< "legend(\"Clustering Coeffitient Distribution " << getIloscNeuronow() / 1000 <<" \"k\");"
			<< "% S = sprintf(\"neurons = %d\", " << getIloscNeuronow() <<");"
			<< "% text(11, 170, S, 'fontsize', 14);";

	str << "% print( \"/home/piersaj/Desktop/nn/RozkladCC_" << getN().getN().getTimestamp();

	char buffer[80];
	sprintf(buffer, "_%dk_%dM_%ldM---XXX.svg",
		getN().getIloscNeuronow()/1000,
		getN().getIloscKrawedzi()/1000000,
		getN().getIloscIteracji() / 1000000
	);

	str << buffer << "\" , \"-color\"); \n\n";
	str.close();
	logJP << "SNCC.zapisWynikow(): ok\n";
}	// zapiszDoPliku();



/* wsp klasteryzacji wezla o indeksie i ale liczony tylko wedlug sasiadow DO ktorych wyplynal
 * ladunek z i (chocby jedna jednostka ladunku!)
 *
 */
float SiecNeuronowaCC::obliczCCtylkoKrawedzieWychodzace(int i){
	Neuron& n1 = getNeuron(i);

	// ilosc sasiadow
	int t = 0;

	for (int l=0; l< n1.getIloscSasiadow(); l++){
		// czy krawedz bedzie liczona do wspolczynnika skupienia

		Polaczenie& k = getKrawedz(n1.getIndeksKrawedzi(l));

		if (k.getSumaLadunku() >= n.getN().getConfig()->getKohonenChargeThreshold()){
			t++;
		}

		// jezeli byl wyslany ladunek z tego nauronu
		// dlaczego, powinno byc symetrycznie!

//		if	(k.getIndeks2() == i && k.getladunekDo1() > 0  ){
//			t++;
//		}	else if (k.getIndeks1() == i && k.getladunekDo2() > 0 ) {
//			t++;
//		}	// fi
	}	// for

	// warunki brzegowe
	// brak sasiadow - nie liczymy
	if (t==0){
		return -1.0;
	}	// if

	// jeden sasiad - zero
	// inne opcje:: -1 - zignoruj neuron // 0 - niesklasteryzowany // +1 całkowicie sklasteryzowany
	if (t==1){
		return -1.0;
	}	// fi

	// tu beda indeksy tych neuronow - sasiadow
	// sasiedzi maja byc wyzej!! tj byl wyslany ladunek do niech
	int indeksySasiadow[t];
	int it = 0;
	int thres = n.getN().getConfig()->getKohonenChargeThreshold();

	for (int l=0; l< n1.getIloscSasiadow(); l++){
		Polaczenie & k = getKrawedz(n1.getIndeksKrawedzi(l));
		if ( k.getSumaLadunku() >= thres){
			indeksySasiadow[it] = k.getIndeks1() == i ? k.getIndeks2() : k.getIndeks1();
			it++;
		}	// if
	}	// for

	int ilosc = 0;
	// liczymy ilosc krawedzi pomiedzy sasiadami
	for (int l=0; l<t; l++){
		for (int k = l+1; k<t; k++){

			if (getN().getN().czyIstniejeKrawedzMiedzyNeuronami(indeksySasiadow[l] , indeksySasiadow[k] )){

				int ik = getN().getN().getIndeksKrawedziMiedzyNeuronami(indeksySasiadow[l] , indeksySasiadow[k] );
				Polaczenie &k = getKrawedz(ik);

				if (k.getSumaLadunku() >= thres ) {
					ilosc ++;
				}	// if
			}	// if

		}	// for k
	}	// for l

	return (ilosc * 2.0) / t / (t-1);
}	// obliczCCtylkoKrawedzieWychodzace(int i)


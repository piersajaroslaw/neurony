/*
 * SiecNeuronowaRozkladStopni.cpp
 *
 *  Created on: Aug 29, 2010
 *      Author: piersaj
 */

#include "time.h"


#include "siecNeuronowaRozkladStopni.h"
#include "../../src/logger.h"

inline double abs(double a){
	return a >=0 ? a : -a;
}	// abs

SiecNeuronowaRozkladStopni::SiecNeuronowaRozkladStopni(SiecNeuronowaDynamika &n_):
	n(n_)
{
	logJP << "SNRS() - rozkład Stopni:\n";
	logJP.timestamp_nl();
	rozkladStopniX = NULL;
	rozkladStopniY = NULL;
	ddLogX = NULL;
	ddLogY = NULL;
	len = 0;
}	// this(snd)


SiecNeuronowaRozkladStopni::~SiecNeuronowaRozkladStopni(){
	if (len>0){
		delete[] rozkladStopniX;
		delete[] rozkladStopniY;
		delete[] ddLogX;
		delete[] ddLogY;

	}	// if
}	// ~this()

void SiecNeuronowaRozkladStopni::obliczTablicePrzplywu(){
	logJP << "SNDD.obliczTablicePrzeplywu(): obliczenia...\n";
	logJP.timestamp_nl();

	len = (int) getN().getN().znajdzMaksymalnyLadunekPrzeplywajacyPrzezNeurony()+1;
	long suma = 0;


	// alokowanie tablic
	rozkladStopniX =new float[len];
	rozkladStopniY =new float[len];
	for (int i=0; i< len; i++){
		rozkladStopniX[i] = i;
		rozkladStopniY[i] = 0;
	}	// for

	// liczenie ladunku
	for (int i=0; i<  getN().getIloscNeuronow(); i++){
		Neuron n = getN().getNeuron(i);
		long l =  n.getLadunekWejsciowy()  /* +  n.getLadunekWyjsciowy() */;
		suma += l;
		rozkladStopniY[l]++;
	}	// for i

	// smieci do loggera
	logJP << "sumaryczny ladunek jaki przeplynal: " << suma << '\n';
	logJP << "maksymalny ladunek jaki przelpynal: " << len-1 <<'\n';

	// wersja kumulatywna
	// TODO dopisac flage ktora przelacza wersje kumulatywna
//	float sumay = 0;
//	for (int i = len-1; i>=0; i--){
		// tu jest dodatkowy haczyk - usuwamy powtarzajace sie zera
		// z powodu zer trzeba liczyc cala sume (ret[i+1] moze byc zerem :P)
//		if (rozkladStopniY[i] != 0){
//		sumay += rozkladStopniY[i];
//		rozkladStopniY[i] = sumay;
//		}	// fi
//	}	// for

	ddLogX = new float[len];
	ddLogY = new float[len];

	// konwersja do log-log
	// zapis do tablicy, zer nie zapisujemy!!!
	// Uwaga: log(1)==0 wiec zera MOGA sie pojawic (zazwyczaj na koncu jedno lub 2) :P
	for (int i=0; i< len; i++){
		ddLogX[i] =      i== 0            ? 0 : (float)log(i);
		ddLogY[i] = rozkladStopniY[i] ==0 ? 0 : (float)log(rozkladStopniY[i]);
	}	// for
}	// oblicz Tablice

void SiecNeuronowaRozkladStopni::obliczNajlepszeDopasowanieMNK(){
	double najlepszeA  = -10000;
	double najMin = 0;
	double najMax = 1;


	for (int i = 0; i <=50; i++){
		for (int j= i+50; j<= 100; j++){
			double min = i / 100.0;
			double max = j / 100.0;
			obliczProstaMNK(min, max);

			// jezeli przyblizenie jest blizsze
			if (abs( mnkA - (-1)) < abs(najlepszeA - (-1)) ){
				najlepszeA = mnkA;
				najMax = max;
				najMin = min;
			}	// fi

		}	// for j
	}	// for i

	logJP << "najlepiej dopasowane nachylenie do rozkladu stopni:\n"
		<< "min = "<< najMin << "  max = " << najMax << "  mnkA = " << mnkA <<"\n";

}	// obliczNajlepszeDopasowanieMNK()

/* przeliczanie wspolczynnikow prostej przyblizajacej wykres przeplywu
 *
 */
void SiecNeuronowaRozkladStopni::obliczProstaMNK(double min, double max){
	if (len<2){
		logJP<< "MNK(): za malo danych w tablicy\n";
		return;
	}	// if

	float s = 0, sx = 0, sy = 0, sxx=0, sxy = 0;

	int start = (int) (min * len);
	int stop = (int) (max * len);


	for (int i=start; i<stop; i++){
		if (ddLogY[i] != 0 &&  ddLogX[i]!= 0 ){
			// mala korekta = to co robimy bedzie uwzglednialo tylko
			// niezerowe wartosci
			s += 1;
			sx += ddLogX[i];
			sy += ddLogY[i];
			sxx += ddLogX[i]*ddLogX[i];
			sxy += ddLogX[i]*ddLogY[i];
		}	// if
	}	// for

	float det = sxx * s - sx * sx;

	if (det == 0){
		logJP<<"MNK(): Error: det==0\n";
		return;
	}	// fi

	mnkA = (sxy * s - sx * sy) / det;
	mnkB = (sxx * sy - sx * sxy) / det;
}	// obliczProstaMNK


void SiecNeuronowaRozkladStopni::zapiszRozkladStopniDoPliku(){

	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime ( &rawtime );

	char timestamp[80];
	strftime (timestamp,80,"%Y_%m_%d_%H_%M_%S",timeinfo);

	char filename[255];

	if (n.getN().getConfig()->getUseTimestampInFilenames()){
		sprintf(filename, "%s/r_%s__%d_kn_%d_Me_%ldM_it_rozkladStopni_it.m",
				n.getN().getConfig()->getResultsFolder(),
				getN().getN().getTimestamp(),
				getN().getIloscNeuronow()/1000,
				getN().getIloscKrawedzi()/1000000,
				getN().getIlIteracji() / 1000000);
	} else {
		sprintf(filename, "%s/rozkladStopni.m",
				n.getN().getConfig()->getResultsFolder());
	}


	std::fstream str;
	str.open(filename, std::fstream::out );

	if (str == NULL){
		std::cerr<< "SNRS.zapiszWyniki() nie mozna utworzyc pliku\n\n";
		return;
	}	// fi

	str << "# timestamp   "<< timestamp << '\n';
	str << "# Il neuronow "<< getN().getIloscNeuronow() << '\n';
	str << "# Il krawedzi "<< getN().getIloscKrawedzi() << '\n';
	str << "# Il iteracji "<< (int) getN().getIloscIteracji() << '\n';


	str << "Y = [\n";

	for (int i=0; i< len; i++){
		str << getRozkladStopniX()[i] << ' ' << getRozkladStopniY()[i] << ";\n";
	}	// for
	str << "];\n\n";

	str << "# plot(Y(:,1), Y(:,2), \"r\");\n";
	str << "loglog(Y(:,1), Y(:,2), \"b+\");\n"
		<< "grid(\'on\');\n"
		<< "legend(\"Cumulative Degree Distribution\");\n"
		<< "S = sprintf(\"neurons = %d \\alpha = %.1f\", "<<  getN().getIloscNeuronow()
		<< ", "<< getN().getN().getAlpha() << ");\n"
		<< "text(11, 170, S, \'fontsize\', 14);\n";

	str << "% print( \"/home/piersaj/Desktop/nn/RozkladStopni_" << timestamp;

	char buffer[80];
	sprintf(buffer, "_%dk_%dM_%ldM---XXX.svg",
		getN().getIloscNeuronow() / 1000,
		getN().getIloscKrawedzi() / 1000000,
		getN().getIloscIteracji() / 1000000
	);

	str << buffer << "\" , \"-color\"); \n\n";
	str.close();
	logJP << "SNRS.zapisWynikow(): ok\n";
}	// zapiszRozkladStopniDoPliku()

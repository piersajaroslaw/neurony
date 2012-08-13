/*
 * synapseEvolutionSaver.cpp
 *
 *  Created on: Mar 16, 2011
 *      Author: piersaj
 *
 *  2011-03-16
 *  	this is supposed to write edges into a file as the simulation continues
 *  	however its quite hazardous as the network can be large - maybe a svg file...
 *  2011-03-22
 *  	few fixes for saving data,
 *  	note even for 3k neurons it weights 600MB and crashes octave :(
 *  	* enforce limit of best edges (done)
 *  	* launch this not so frequently (ewver 2% enstead of .5%)...
 *  	* plots and saving to svg / png file every time block is allocated
 *  	* clear block once used - sorry it's a must :(
 *  	* this type of iterated plots do causes problems even in 2d
 *  2011-03-23
 *  	few more fixes
 *  2011-03-28
 *  	re-normalization of the coordinates
 *  2011-03-29
 *  	picked a random subgraph with limited edges
 *  2011-03-30
 *		Kohonen restricted only to random subgraph
 *		restricted svg plots only to random subgraph
 *		removed restrictions to charge flow threshold
 *	2011-03-31
 *		printing the nodes as well...
 *
 *	TODO: refactorise - remove all unnecessery fields and methods
 */

#include "synapseEvolutionSaver.h"
#include "../utils/funkcjeLosowe.h"

#define LEN 255

SynapsesEvolutionSaver::SynapsesEvolutionSaver(SiecNeuronowa * n_){
	n = n_;
	svgCanvasSizeX = 1000;
	svgCanvasSizeY = 800;
	kohPosX = NULL;
	kohPosY = NULL;
	isNodePicked = NULL;
	fractionThreshold  =.1;
	chargeThreshold = 1;
	edgesLimit = n->getConfig()->getKohonenEdgesLimit();
	maxFlow = 0;

	filename = new char [255];
	currentSnapshot = 0;

	if (n->getConfig()->getUseTimestampInFilenames()) {
		sprintf(filename, "%s/r_%s__%d_kn_%d_Me_%ldM_it__synapsesSnapshot.svg",
			 n->getConfig()->getResultsFolder(),
			 n->getTimestamp(),
			 n->getIloscNeuronow()/1000,
			 n->getIloscKrawedzi()/1000000,
			 n->getIloscIteracji()/1000000
		);
	} else {
		sprintf(filename, "%s/synapsesSnapshot.svg",
				n->getConfig()->getResultsFolder());
	}	// else


	logJP << '\n';
	logJP.timestamp_nl() << "\nsynapses file:    " << filename<< "\n";
	writer = new NNFileWrter(filename);
}	// this(SN*)

SynapsesEvolutionSaver::~SynapsesEvolutionSaver(){
	delete writer;
	delete filename;
	n = NULL;
	if (isNodePicked!=NULL){
		delete [] isNodePicked;
		isNodePicked = NULL;
	}

	if (kohPosX != NULL){
		delete [] kohPosX;
		delete [] kohPosY;
		kohPosX = NULL;
		kohPosY = NULL;
	}

	if (precalcThresholds !=NULL){
		delete [] precalcThresholds;
	}
}	// ~this


void SynapsesEvolutionSaver::generateSvgSnapshot(){
	logJP << "Kohonen Charge Threshold: " << chargeThreshold << "\n";
	logJP<<"\nSaving synapses to svg, edges limit = " << edgesLimit << " ... ";
	generateHeader();
	generateCalculationTic();
	generateFooter();
	logJP<<"done\n";
}	// generateSvgSnapshot

void SynapsesEvolutionSaver::generateHeader(){
	(*writer) << "<?xml version=\"1.0\" encoding=\"utf-8\"  standalone=\"no\"?>\n"
		<< "<\?xml version=\"1.0\" encoding=\"utf-8\"  standalone=\"no\"\?>\n"
		<< "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n"
		<< "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
		<< "<svg width=\""<< svgCanvasSizeX
		<< "\" height=\""<< svgCanvasSizeY
		<< "\" viewBox=\"0 0 "
		<< svgCanvasSizeX << " "
		<< svgCanvasSizeY << "\"\n"
		<< " xmlns=\"http://www.w3.org/2000/svg\"\n"
		<< " xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
		<< ">\n"
		<< "<desc>\n";
	(*writer).timestampNl()<< "Jaroslaw Piersa\nPHD Neurons Simulation\n"
		<< "NEURONS = "<< n->getIloscNeuronow() << "\n"
		<< "EDGES   = "<< n->getIloscKrawedzi() << "\n"
		<< "ITERATIONS   = "<< n->getIloscIteracji() / 1000000 << " M\n"
		<<"</desc>\n";


	(*writer) << "<g style=\"fill:none; color:blue; stroke:currentColor; stroke-width:0.50;"
		<<" stroke-linecap:butt; stroke-linejoin:miter\">\n";

}	// genHeader


/*
 * zapis krawedzi do pliku svg
 * zalozenie jest takie ze wspolrzedne sa znormalizowne na kwadracie
 * [0..maxPosX] \times [0..maxPosY]
 */
void SynapsesEvolutionSaver::generateCalculationTic(){
//	precalculateThresholds();
	selfOrganizeKohonen();
	normalisePositions();
	float scalX = svgCanvasSizeX * 1.0 / maxPosX;
	float scalY = svgCanvasSizeY * 1.0 / maxPosY;

	for (int i=0; i<n->getIloscKrawedzi(); i++){
		Polaczenie & e = n->getKrawedz(i);
		if (isNodePicked[e.getIndeks1()] && isNodePicked[e.getIndeks2()]
					&& e.getSumaLadunku() >= chargeThreshold && e.getIndeks1()!= e.getIndeks2())
		{
			(*writer)
				<< "<path stroke=\'rgb( "
				<< pickEdgeColorR(e.getSumaLadunku())
				<<" , 0, 255)\'"
				<< "  d='M"
				<< kohPosX[e.getIndeks1()] * scalX
				<< ","
				<< kohPosY[e.getIndeks1()] * scalY
				<< " L"
				<< kohPosX[e.getIndeks2()] * scalX
				<< ","
				<< kohPosY[e.getIndeks2()] * scalY
				<< "'></path>\n";
		}	// if
	}	// for i;


	// and now the nodes
	for (int i=0; i<n->getIloscNeuronow(); i++){
		if (isNodePicked[i]){
			(*writer) << "<circle cx=\""
					<< kohPosX[i] * scalX
					<< "\" cy=\""
					<< kohPosY[i] * scalY
					<< "\" r=\"2.5\" fill=\"green\"/>\n";
		}	// if
	}	// for
}	// generateTic


void SynapsesEvolutionSaver::generateFooter(){
	(*writer) << "</g>\n</svg>\n\n";
}

/*
 * @depracated! dont use it
 */
long SynapsesEvolutionSaver::getChargeThreshold() const{
    return chargeThreshold;
}

/*
 * @depracated! dont use it
 */
float SynapsesEvolutionSaver::getFractionThreshold() const{
    return fractionThreshold;
}

int SynapsesEvolutionSaver::getEdgesLimit() const{
    return edgesLimit;
}

/*
 * @depracated! dont use it
 */
void SynapsesEvolutionSaver::setChargeThreshold(long  chargeThreshold){
    this->chargeThreshold = chargeThreshold;
}

/*
 * @depracated! dont use it
 */
void SynapsesEvolutionSaver::setFractionThreshold(float fractionThreshold){
    this->fractionThreshold = fractionThreshold;
}

void SynapsesEvolutionSaver::setEdgesLimit(int el){
    this->edgesLimit = el;
}

#if 0
/* deprecated - no longer used
 */
void SynapsesEvolutionSaver::precalculateThresholds(){
	precalcThresholdsLen = n->znajdzMaksymalnyPrzelpywKrawedzi();

	precalcThresholds = new long[precalcThresholdsLen];
	for (long i=0; i<precalcThresholdsLen; i++){
		precalcThresholds[i] = 0l;
	}	// fi
	for (int i=0; i< n->getIloscKrawedzi(); i++){
		long charge = n->getKrawedz(i).getSumaLadunku();
		if (n->getKrawedz(i).getIndeks1()!=n->getKrawedz(i).getIndeks2()){
			precalcThresholds[charge]++;
		}	// if
	}	// for
	for (int i=precalcThresholdsLen-2; i>=0; i--){
		precalcThresholds[i] = precalcThresholds[i]+precalcThresholds[i+1];
	}	// for

	chargeThreshold = precalcThresholdsLen -2;
	for (int i=precalcThresholdsLen-2; i>=0; i--){
		if (precalcThresholds[i] < edgesLimit)
			chargeThreshold = i;
	}	// for

	//	logJP << "SES.PT(): thresh = "<< chargeThreshold << "\n";
	//	logJP << "SES.PT(): maxCh  = "<< precalcThresholdsLen << "\n";
	//	logJP << "SES.PT(): number of edges = "<< precalcThresholds[chargeThreshold] << "\n";

	delete[] precalcThresholds;
	precalcThresholds  = NULL;


}	// precalculateThresholds()
#endif


void SynapsesEvolutionSaver::initKohonen(){
	maxPosX = n->getMaxPos();
	maxPosY = n->getMaxPos();
	if (kohPosX == NULL)
		kohPosX = new float[n->getIloscNeuronow()];
	if (kohPosY == NULL)
		kohPosY = new float[n->getIloscNeuronow()];

	for (int i=0; i< n->getIloscNeuronow(); i++){

		// TODO afaik there are no negative positions
		// this shifts points from negative quarters

		kohPosX[i] = ( n->getNeuron(i).getX() + maxPosX ) * 0.5;
		kohPosY[i] = ( n->getNeuron(i).getY() + maxPosY ) * 0.5;

		// There might be square -> circle translation problem
	}	// for
	iterations = 4000 * edgesLimit;
	currentIteration = 0L;
}	// initKohonen

void SynapsesEvolutionSaver::deinitKohonen(){
	currentIteration = 0L;
}	// initKohonen


void SynapsesEvolutionSaver::selfOrganizeKohonen(){

	initKohonen();
	pickRandomSubgraph();
	long block_it = iterations / 50, counter = 0;

	// some ugly Kohonen progress trace
	std::cout << "\n";
	for (currentIteration=0; currentIteration<iterations; currentIteration++){
		if (counter++ == block_it){
			std::cout << "#";
			std::cout.flush();
			counter = 0;
		}	// fi
		doSingleStepKohonen();
	}	// for j
	std::cout << "\n";

//	normalisePositions();

}	// selfOrganizeKohonen

long SynapsesEvolutionSaver::findClosestIndexTo(float x, float y){
	int bestI = 0;
	float bestD = 4 * maxPosX * maxPosX + 4 * maxPosY * maxPosY;
	float newD = bestD;
	for (int i=0; i<n->getIloscNeuronow(); i++){
		newD = (kohPosX[i]-x)*(kohPosX[i]-x) + (kohPosY[i]-y)*(kohPosY[i]-y);
		if (isNodePicked[i] && newD <bestD){
			bestD = newD;
			bestI = i;
		}	// if
	}
	return bestI;
}	// findClosest

void SynapsesEvolutionSaver::doSingleStepKohonen(){
	float x=0, y=0;

	pickRandomPointCircle(x, y);


	long i1 = findClosestIndexTo(x, y);
	Neuron &n1 = n->getNeuron(i1);

	float alpha = (currentIteration *1.0 +1000)/ (iterations+1000);

	kohPosX[i1] = alpha * kohPosX[i1] + (1-alpha) * x;
	kohPosY[i1] = alpha * kohPosY[i1] + (1-alpha) * y;
	long ni, ei;
	for (int i=0 ; i < n1.getIloscSasiadow(); i++){
		ei = n1.getIndeksKrawedzi(i);

		ni = n->getIndeksItegoSasiada(i1, i);

		if (isNodePicked[ni] && n->getKrawedz(ei).getSumaLadunku() >= chargeThreshold){
			kohPosX[ni] = alpha * kohPosX[ni] + (1-alpha) * x;
			kohPosY[ni] = alpha * kohPosY[ni] + (1-alpha) * y;
		}	// if
	}	// for

}	// doAStep()

/* picks a random point from square
 */
void SynapsesEvolutionSaver::pickRandomPointSquare(float &x, float &y){
	x = losujRUnif( maxPosX);
	y = losujRUnif( maxPosY);
}	//  pick random quare point

void SynapsesEvolutionSaver::pickRandomPointCircle(float &x, float &y){
	do {
		x = losujRUnif( maxPosX);
		y = losujRUnif( maxPosY);
	} while ( (x-maxPosX/2)*(x-maxPosX/2) + (y-maxPosY/2)*(y-maxPosY/2) >= maxPosX*maxPosY /5);
}	// pickRandomPointCircle()


int SynapsesEvolutionSaver::pickEdgeColorR(long chargeFlow){

	if (maxFlow == 0){
		maxFlow = n->znajdzMaksymalnyPrzelpywKrawedzi();
	}	// fi

	float scal = 10.0;
	int val = (int) ((chargeFlow ) * scal * 255.0 / maxFlow );
	if (val<0)
		val = 0;
	if (val>255)
		val=255;
	return val;
}	// pickEdgeColorR


void SynapsesEvolutionSaver::normalisePositions(){
	float minx=maxPosX;
	float maxx = 0;
	float miny=maxPosY;
	float maxy = 0;
	for (int i=0; i<n->getIloscNeuronow(); i++){
		if (kohPosX[i] < minx)
			minx = kohPosX[i];
		if (kohPosX[i] > maxx)
			maxx = kohPosX[i];
		if (kohPosY[i] < miny)
			miny = kohPosY[i];
		if (kohPosY[i] > maxy)
			maxy = kohPosY[i];
	}	// for


	for (int i=0; i<n->getIloscNeuronow(); i++){
		kohPosX[i] = (kohPosX[i] - minx) / (maxx - minx) * maxPosX;
		kohPosY[i] = (kohPosY[i] - miny) / (maxy - miny) * maxPosY;
	}	// for
}	// renormalise


void SynapsesEvolutionSaver::pickRandomSubgraph(){
	if (isNodePicked == NULL){
		isNodePicked = new bool[n->getIloscNeuronow()];
	}	// fi
	for (int i=0; i<n->getIloscNeuronow(); i++){
		isNodePicked[i] = false;
	}	// for


	int pickedNodes = 0;
	int pickedEdges = 0;

	int neuronsWithNeighbours = 0;

	for (int i=0; i< n->getIloscNeuronow(); i++){
		if (n->getNeuron(i).getIloscSasiadow()>=2)
			neuronsWithNeighbours++;
	}	// for


	int is =  -1;
	int val = -1;
	while (pickedEdges < edgesLimit && pickedNodes <= neuronsWithNeighbours-2){
		val = -1;
		do {
			val = losujRInt(n->getIloscNeuronow());
			// if the nod is already picked
			// or has no edges
			// repeat the sampling
		} while (isNodePicked[val] == true || n->getNeuron(val).getIloscSasiadow() <= 1);


		isNodePicked[val] = true;
		pickedNodes ++;

		for (int i=0; i<n->getNeuron(val).getIloscSasiadow(); i++){

			if (n->getKrawedz(n->getNeuron(val).getIndeksKrawedzi(i)).getSumaLadunku( )>= chargeThreshold){
				is = n->getIndeksItegoSasiada(val, i);
				if (isNodePicked[is] == true && val != is ){
					/* ... i krawedz nie jest do siebie samego */
					pickedEdges++;
				}	// fi
			}	// fi
		}	// for
	}	// while

	logJP << "\nSynapseSaver.RandomSubgraph: picked "
		<< pickedNodes << " nodes  and "
		<< pickedEdges << " edges\n";
}	// pick randomSubgraph



#if 0
/*
 *  WERSJA OCTAVOWA
 *  if 0
 */

void SynapsesEvolutionSaver::generateHeaderM(){
	writer->comment() <<" skrypt w Octave do spisywania polaczen sieci\n\n"
		<< "iloscNeuronow = " << n->getIloscNeuronow()
		<< ";\niloscKrawedzi = " << n->getIloscKrawedzi()
		<< ";\niloscIteracji = " << n->getIloscIteracji() << ";\n\n";

	writer->comment() << "a teraz piszemy nasze neurony\n";



	(*writer) << "NEURONY =  [\n%xx\n";
	for (int i=0; i<n->getIloscNeuronow(); i++){
		(*writer) << (n->getNeuron(i)).getX() << ",  ";
	}	// for i
	(*writer) << ";\n% yy\n";
	for (int i=0; i<n->getIloscNeuronow(); i++){
		(*writer) << (n->getNeuron(i)).getY() << ",  ";
	}	// for i
	(*writer) << ";\n% zz\n";
	for (int i=0; i<n->getIloscNeuronow(); i++){
		(*writer) << (n->getNeuron(i)).getZ() << ",  ";
	}	// for i
	(*writer) << "\n]; % koniec neuronow\n\n";

	(*writer) << "chargeThresh = " << chargeThreshold <<";\n";
	(*writer) << "fractThresh = " << fractionThreshold <<";\n";

	writer->comment() << "a teraz piszemy krawedzie (chwilowo zastanawiam sie jak...)\n";


	currentSnapshot = 0;
}	// genHeader


void SynapsesEvolutionSaver::generateCalculationTicM(){
//	if (currentSnapshot == 0){
//		generateFirstCalculationTic();
//	}	// fi


	(*writer) << "KRAWEDZIE_"<< currentSnapshot  << " = [\n";
	writer->ind1();
	for (int i=0; i<n->getIloscKrawedzi(); i++){
		Polaczenie & e = n->getKrawedz(i);
		if (e.getSumaLadunku() >= chargeThreshold && e.getIndeks1()!= e.getIndeks2()){
			(*writer) << e.getIndeks1() <<  ", ";
		}	// if
	}	// for i;
	(*writer) << ";\n\n";
	writer->ind1();
	for (int i=0; i<n->getIloscKrawedzi(); i++){
		Polaczenie & e = n->getKrawedz(i);
		if (e.getSumaLadunku() >= chargeThreshold && e.getIndeks1()!= e.getIndeks2()){
			(*writer) << e.getIndeks2() << ", ";
		}	// if
	}	// for i;
	(*writer) << ";\n\n";
	writer->ind1();
	for (int i=0; i<n->getIloscKrawedzi(); i++){
		Polaczenie & e = n->getKrawedz(i);
		if (e.getSumaLadunku() >= chargeThreshold && e.getIndeks1()!= e.getIndeks2()){
			(*writer) << e.getSumaLadunku() << ", ";
		}	// if
	}	// for i;
	(*writer) << "\n]; % KRAWEDZIE \n\n";
	currentSnapshot++;
}	// geterate Tic

void SynapsesEvolutionSaver::generateFooterM(){
	(*writer) << "\n\n";
	(*writer) << "hold off;\n";
	(*writer) << "sleeptime=0.1;\n";
	(*writer) << "for i=(1:length(KRAWEDZIE_1(1,:)))\n";
	(*writer).ind1() << "plot3( [NEURONY(1, KRAWEDZIE_1(1, i)+1), NEURONY(1, KRAWEDZIE_1(2, i)+1) ], \n";
	(*writer).ind2() << "[NEURONY(2, KRAWEDZIE_1(1, i)+1), NEURONY(2, KRAWEDZIE_1(2, i)+1) ], \n";
	(*writer).ind2() << "[NEURONY(3, KRAWEDZIE_1(1, i)+1), NEURONY(3, KRAWEDZIE_1(2, i)+1) ], \n";
	(*writer).ind2() << "\"b\" );\n";
	(*writer).ind1() << "sleep(sleeptime);\n";
	(*writer).ind1() << "if i==1\n     hold on;\nend\n";
	(*writer) << "end %for\n";
	(*writer) << "filename = sprintf(\"/home/piersaj/Desktop/delme/shot_%04d0.svg\", 1);\n";
	(*writer) << "hold off;\n";
	(*writer) << "print(filename);\n";
}	// generateFooter




#endif


/*
 * NN_Failure_resiliency_path_len_thread.cpp
 *
 *  Created on: Jul 20, 2012
 *      Author: piersaj
 *
 *
 *  2012-07-20
 *  	written
 *  	or actually copied form another class
 *  2012-08-02
 *  	removed verbatim logging
 *  	a slight fix to auto stepskip
 *
 */



#include "NN_Failure_resiliency_path_len_thread.h"
#include "../../src/logger.h"

#include <cstdlib>

FailureResiliencyPathLength_Thread::FailureResiliencyPathLength_Thread(SiecNeuronowa *n_, NN_FailureResiliancyTester * frt_,
	int tid_, int maxTid_,
	int startingVertex_, int endingVertex_, int stepskip_
):	// end of arguments line
	n(n_),
	frt(frt_),
	tid(tid_),
	t_num(maxTid_),
	startingIndex(startingVertex_),
	endingIndex(endingVertex_),
	stepSkip(stepskip_),
	minStepskip(1),
	maxStepskip(100)
{	// start of constructor body

	pathLengthAtRandomFailure = new double[n->getIloscNeuronow()]; 	// should be endingVertex - startingVertex +1
	pathLengthAtAttack = new double[n->getIloscNeuronow()];


	isNeuronRemoved = new bool[n->getIloscNeuronow()];

	for (int i=0; i<n->getIloscNeuronow(); ++i){
		isNeuronRemoved[i] = false;
		neuronsInConnectedComponent.push_back(i);
		pathLengthAtRandomFailure[i] = -1;
		pathLengthAtAttack[i] = -1;
	}	// for
}	// this

FailureResiliencyPathLength_Thread::~FailureResiliencyPathLength_Thread(){
	delete[] pathLengthAtAttack;
	delete[] pathLengthAtRandomFailure;
	delete[] isNeuronRemoved;
	neuronsInConnectedComponent.clear();
}	// this


void FailureResiliencyPathLength_Thread::run(){
	calculateAPL_vs_Attack_or_Random(true);
	calculateAPL_vs_Attack_or_Random(false);


}	// run


/*
 * this is two-in-one version of the below removal
 * added few "if"s and removal style flag
 */
void FailureResiliencyPathLength_Thread::calculateAPL_vs_Attack_or_Random(bool flagRandom){
	int index = 0;
	int c = 10;
	int iters = c * n->getIloscNeuronow();
	int removedIndex = 0;
	double val = -1;
	char mark = '#';
<<<<<<< HEAD

=======
>>>>>>> 4bc3be897e7c1b721ed6ef1854c93dab1d34c765
//	int d = 500;

	int localStepskip = stepSkip;
//	if (flagRandom == false){
//		localStepskip /= 4;
//	}	// fi

	int thres1 = flagRandom ? n->getIloscNeuronow() / 2 : n->getIloscNeuronow() / 5; // double jumps below this
	int thres2 = flagRandom ? 3 * n->getIloscNeuronow() / 4 : 1 * n->getIloscNeuronow() / 4; // half jumps above this

//	double downratio = flagRandom ? 0.025 : 0.050;
//	double upratio = flagRandom ? 0.001 : 0.002;

<<<<<<< HEAD
=======
	unsigned int thres1 = n->getIloscNeuronow() / 2;
	unsigned int thres2 = n->getIloscNeuronow() / 4;

//	double downratio = flagRandom ? 0.025 : 0.050;
//	double upratio = flagRandom ? 0.001 : 0.002;

>>>>>>> 4bc3be897e7c1b721ed6ef1854c93dab1d34c765

	char buff[250];

	if (flagRandom){
<<<<<<< HEAD
=======

>>>>>>> 4bc3be897e7c1b721ed6ef1854c93dab1d34c765
		sprintf(buff, "  Thread %d: Resiliency test: path length on random failures:\n", tid);
	} else {
		sprintf(buff, "  Thread %d: Resiliency test: path length on attacks:\n", tid);
	}	// if


	if (tid == 0){
		logJP.timestamp() << buff;
	}	// fi

	// reset the removed table
	for (int i=0; i<n->getIloscNeuronow(); ++i){
			isNeuronRemoved[i] = false;
	}	// for


	for (index = 0; index<startingIndex; index++){
		int rem = flagRandom ? frt->getIndexOfRemovedNode_Random()[index] : frt->getIndexOfRemovedNode_Attack()[index];
		isNeuronRemoved[rem] = true;
	}	// for



	// index runs from 0 till end by one
	index = 0;
	// effective index runs only on indices which are to be calculated.
	// if effectiveIndex % thread_num == myTid, tha i have to calculate this crap...
	int effectiveIndex = 0;

	int startingVertex = 0;

	while (index>=0 && index <= n->getIloscNeuronow()){

		effectiveIndex++;
		startingVertex = flagRandom ? frt->getIndexOfLargestComponent_Random()[index] : frt->getIndexOfLargestComponent_Attack()[index];

		if (startingVertex <0){

#if 0
			// removed the debug plot
			std::cout<< "thread "<< tid <<" ending relisiency test with step "<< index <<"\n";
#endif

<<<<<<< HEAD
			break;
=======
			return;
>>>>>>> 4bc3be897e7c1b721ed6ef1854c93dab1d34c765
		}	// if



		// if it is my step to calculate
		if ((effectiveIndex-1) %  t_num == tid){
			findConnectedComponentVector(startingVertex);

			val =  calculateAPL_MC(iters);

			if (flagRandom){
				pathLengthAtRandomFailure[index] = val;
			} else {
				pathLengthAtAttack[index] = val;
			}	// if
<<<<<<< HEAD
=======

>>>>>>> 4bc3be897e7c1b721ed6ef1854c93dab1d34c765
#if 0
			logJP << "t"<<tid  <<": step = "<< index << " component size = " << neuronsInConnectedComponent.size()
					<< " starting index = " << startingIndex <<" " <<"  apl MC = "<< val<<"\n";

#endif
<<<<<<< HEAD
		}	// if


		// index = current step
		if (index < thres1){
			localStepskip = 2*stepSkip;
			mark = '#';
		} else if (index < thres2) {
			localStepskip = stepSkip;
			mark = '@';
		} else {
			localStepskip = stepSkip / 2;
			mark = '^';
		}	// else

		// Progress logging :P
		if (tid == 0){
			logJP << mark;
		}	// fi

=======


			if (neuronsInConnectedComponent.size() > thres1){
				localStepskip = 2*stepSkip;
				mark = '#';
			} else if (neuronsInConnectedComponent.size() > thres2) {
				localStepskip = stepSkip;
				mark = '@';
			} else {
				localStepskip = stepSkip / 2;
				mark = '^';
			}	// else

//			Progress logging :P
			if (tid == 0){
				logJP << mark;
			}	// fi
		}	// if

>>>>>>> 4bc3be897e7c1b721ed6ef1854c93dab1d34c765
		// This is after the first node calculated
		for (int i=0; i<=localStepskip+1; i++){

			removedIndex = flagRandom ? frt->getIndexOfRemovedNode_Random()[index] : frt->getIndexOfRemovedNode_Attack()[index];

			if (removedIndex >=0 && removedIndex < n->getIloscNeuronow()){
				isNeuronRemoved[removedIndex] = true;
			}	// if
			index++;
		}	// for i
	}	// while
	if (tid == 0){
		logJP << "\n";
	} // if

<<<<<<< HEAD
=======
	if (tid == 0)
		logJP << "\n";
	} // if
>>>>>>> 4bc3be897e7c1b721ed6ef1854c93dab1d34c765

}	// calculateAPL_vs_Arrack_or_Random()


double FailureResiliencyPathLength_Thread::getAplAtAttack(int i){
	return pathLengthAtAttack[i];
}

double FailureResiliencyPathLength_Thread::getAplAtRandomDamage(int i){
	return pathLengthAtRandomFailure[i];
}

void FailureResiliencyPathLength_Thread::findConnectedComponentVector(int startingIndex){
	bool * isVisited = new bool[ n->getIloscNeuronow()];
	std::vector<int> stack;

	neuronsInConnectedComponent.clear();
	for (int i=0; i< n->getIloscNeuronow(); ++i){
		isVisited[i] = false;
	}


	// dfs
	isVisited[startingIndex] = true;
	stack.push_back(startingIndex);
	neuronsInConnectedComponent.push_back(startingIndex);


//	int nb_index = -1;

	while (stack.empty() == false){
		int i = stack.back();
		stack.pop_back();

		for (int j=0; j< n->getNeuron(i).getIloscSasiadow(); j++){

			int ni = n->getItySasiad(i, j).getIndeks();
			int ei = n->getIndeksKrawedziMiedzyNeuronami(ni, i);

			// edge not valid
			if (n->getKrawedz(ei).getSumaLadunku() < n->getConfig()->getKohonenChargeThreshold()){
				continue;
			}	// fi

			// neighbour is removed
			if (isNeuronRemoved[ni] == true || isVisited[ni] == true){
				continue;
			}

			isVisited[ni] = true;
			neuronsInConnectedComponent.push_back(ni);
			stack.push_back(ni);

		}	// for each edge

	}	// while

	delete[] isVisited;
}	// findConnectedComponentVector()


double FailureResiliencyPathLength_Thread::calculateAPL_MC(int iters){

	int i1 = -1, i2 = -1, dl = -1;

	int maxPL = n->getIloscNeuronow()+1;

	int * pathLengths = new int[maxPL];
	// zerowanie tablicy
	for (int i=0; i<maxPL; i++){
		pathLengths[i] = 0;
	}	// for


	int effectiveIterations = 0;
	// mc
	for (int i=0; i< iters; i++){

		// pick first neuron
		i1 = rand()% neuronsInConnectedComponent.size();
		i1 = neuronsInConnectedComponent.at(i1);

		// pick second neuron
		i2 = i1;
		while (i2==i1 || isNeuronRemoved[i2] == true){
			i2 = rand()%neuronsInConnectedComponent.size();
			i2 = neuronsInConnectedComponent.at(i2);
		}	// while
		dl = calculatePathLengthBetween(i1, i2);

		if (dl < maxPL){
			pathLengths[dl]++;
			effectiveIterations ++;
		} else {
			// pathLengths[maxPL-1]++;  // dont do that :P
		}	// if
	}	// for i

	double sum = 0;
	for (int i=0; i<maxPL-1; i++){ // note -- -we ommit last element = trash
		sum += pathLengths[i]*i;
	}

	delete[] pathLengths;
	return sum / effectiveIterations;
}	// obliczRozkladDlugosciMC()

int FailureResiliencyPathLength_Thread::calculatePathLengthBetween(int i1, int i2){
	if (i1==i2)
		return 0;

	if (isNeuronRemoved[i1] || isNeuronRemoved[i2]){
		logJP << "Res.PathLength: Error one of the neurons is removed "<< i1 << ", "<< i2 <<"\n";
		return (+10000);
	}

	// z jakiegos powodu daje o jeden za duzo... albo o jeden za malo
	// juz poprawione - tak sadze
	bool czyOdwiedzony[n->getIloscNeuronow()];
	for (int i=0; i<n->getIloscNeuronow(); i++){
		czyOdwiedzony[i] = false;
	}

	std::queue<int> kolejka  = std::queue<int>();
	kolejka.push(i1);
	int depth = -1;
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
		if (wierzcholek == nastepny && isNeuronRemoved[wierzcholek] == false){
			depth++;
			flagaTrzebaUstawicNastepnego = true;
		}	// fi

		// dodajemy Sasiadow
		Neuron & nw = n->getNeuron(wierzcholek);
		for (int j=0; j< nw.getIloscSasiadow(); j++){
			sasiad = nw.getIndeksKrawedzi(j);
			Polaczenie & k = n->getKrawedz(sasiad);

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
				return depth+1;
			}	// fi

			// pierwszy nieodwiedzony wierzcholek na nastepnym poziomie
			if (flagaTrzebaUstawicNastepnego && !czyOdwiedzony[sasiad] && isNeuronRemoved[sasiad] == false){
				flagaTrzebaUstawicNastepnego = false;
				nastepny = sasiad;
			}	// fi

			if (czyOdwiedzony[sasiad] == false && isNeuronRemoved[sasiad] == false){
				czyOdwiedzony[sasiad] = true;
				kolejka.push(sasiad);
			}	// fi
		}	// for

	}	// while

	// Situation Normal All ... Up
	printf("Res_APL.BFS(): no path between %d %d --- fatal!\n", i1, i2);
	return depth+10000;
}	// Calc Path Length

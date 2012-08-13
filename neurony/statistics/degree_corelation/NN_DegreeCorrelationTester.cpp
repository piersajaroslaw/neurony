/*
 * NN_DegreeCorrelationTester.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: piersaj
 *
 *  2012-08-08
 *  	created
 *  	written calculation od Pearson Degree Correlation
 *  2012-08-09
 *  	minor debugging
 *  	saving degrees scatterplot to .m file
 *  	added neuron degree vs avg degree of its neighbours (x,y) and saving to m-file
 *  2012-08-10
 *  	Minor (??) bug fixed
 *  	Ok it was iterations for i=0 to neurons number instead of i=0 to edges number
 *  	it was _not_ minor :P
 *
 */

#include "NN_DegreeCorrelationTester.h"


NN_DegreeCorrelationTester::NN_DegreeCorrelationTester(SiecNeuronowa * n_):
	n(n_)
{

	edgesNum = -1;
	degreeInFunctionalGraph = NULL;
	degreeVsAvgDegree_Avg = NULL;
	degreeVsAvgDegree_Deg = NULL;
	recalculateDegreesInFunctionalGraph();
}	// this()


NN_DegreeCorrelationTester::~NN_DegreeCorrelationTester(){
	if (degreeInFunctionalGraph!= NULL){
		delete[] degreeInFunctionalGraph;
	}	// fi


	if (degreeVsAvgDegree_Avg!= NULL){
		delete[] degreeVsAvgDegree_Avg;
	}	// fi

	if (degreeVsAvgDegree_Deg!= NULL){
		delete[] degreeVsAvgDegree_Deg;
	}	// fi
}	// ~this()

void NN_DegreeCorrelationTester::recalculateDegreesInFunctionalGraph(){
	if (degreeInFunctionalGraph == NULL){
		degreeInFunctionalGraph = new int[n->getIloscNeuronow()];
		for (int i=0; i< n->getIloscNeuronow(); i++){
			degreeInFunctionalGraph[i] = 0;
		}	// for i
	}	// fi

	edgesNum = 0;
	for (int i=0; i<n->getIloscKrawedzi(); i++){
		Polaczenie & e = n->getKrawedz(i);
		// if flow is above threshold value than incident vertices have increased degree
		// note,
		//ATM it is increase by +1, in multigraph case it should be by +|flow|
		if (e.getSumaLadunku()>= n->getConfig()->getKohonenChargeThreshold()){
			if (e.getIndeks1() != e.getIndeks2()){
				degreeInFunctionalGraph[e.getIndeks1()] += 1; //e.getSumaLadunku();
				degreeInFunctionalGraph[e.getIndeks2()] += 1; //e.getSumaLadunku();
				edgesNum++;
			}	// fi
		}	// fi
	}	// for all edges

}	// recalculateDegreesInFunctionalGraph()


double NN_DegreeCorrelationTester::calculatePearsonDegreeCorrelationCoefficient(){
	if (degreeInFunctionalGraph == NULL){
		recalculateDegreesInFunctionalGraph();
	}	// fi


//	double sjk = 0, sj2k2=0, spjk = 0;
	double sj = 0;
	// sum (j+k) ; sum (j^2 + k^2) ; sum j*p respectively




	int ij = -1, ik =-1;

	// index of j and k
	for (int i=0; i< n->getIloscKrawedzi(); i++){
		Polaczenie & e = n->getKrawedz(i);
		if (e.getSumaLadunku()< n->getConfig()->getKohonenChargeThreshold() ){
			continue;
		}	// fi

		if (e.getIndeks1() == e.getIndeks2()){
			continue;
		}	// fi

		ij = e.getIndeks1();
		ik = e.getIndeks2();


		sj += degreeInFunctionalGraph[ij] + degreeInFunctionalGraph[ik];
	}	// for i

	double m1 = 1.0 / edgesNum;
	double EJ = sj * 0.5 *m1;
	double s_x_ex_y_ey = 0;
	double s_x_ex_2 = 0;
//	double s_y_ey_2 = 0;
	int count = 0;

	// index of j and k
	for (int i=0; i< n->getIloscKrawedzi(); i++){
		Polaczenie & e = n->getKrawedz(i);
		if (e.getSumaLadunku() <  n->getConfig()->getKohonenChargeThreshold() ){
			continue;
		}	// fi

		if (e.getIndeks1() == e.getIndeks2()){
			continue;
		}	// fi

		ij = e.getIndeks1();
		ik = e.getIndeks2();


		s_x_ex_y_ey +=  (degreeInFunctionalGraph[ij] - EJ ) * (degreeInFunctionalGraph[ik] - EJ)
				+ (degreeInFunctionalGraph[ik] - EJ ) * (degreeInFunctionalGraph[ij] - EJ);

		s_x_ex_2  += (degreeInFunctionalGraph[ij] - EJ ) * (degreeInFunctionalGraph[ij] - EJ )
				+ (degreeInFunctionalGraph[ik] - EJ ) * (degreeInFunctionalGraph[ik] - EJ );
//		s_y_ey_2  += (degreeInFunctionalGraph[ij] - EJ ) * (degreeInFunctionalGraph[ij] - EJ )
//				+ (degreeInFunctionalGraph[ik] - EJ ) * (degreeInFunctionalGraph[ik] - EJ );

		count +=2;

	}	// for i

	double sc = 1.0 / count;

	double E_x_ex_y_ey =  sc* s_x_ex_y_ey;
	double E_x_ex_2 =  sc *  s_x_ex_2;
//	double E_y_ey_2 = sc *  s_y_ey_2;

	double ret =  E_x_ex_y_ey /E_x_ex_2;

//	logJP << "\ncount = "<< count << "\n"
//			"\nE(x-Ex)(y-Ey)= " << E_x_ex_y_ey <<
//			"\nD^2 X = "<< E_x_ex_2
//			<<"\nD^2 Y = "<< E_y_ey_2 << "\n";
//	logJP << "corr(X,Y) = "<<  E_x_ex_y_ey /E_x_ex_2  << "\n";



//	// index of j and k
//	for (int i=0; i< n->getIloscKrawedzi(); i++){
//		Polaczenie & e = n->getKrawedz(i);
//		if (e.getSumaLadunku()< n->getConfig()->getKohonenChargeThreshold() ){
//			continue;
//		}	// fi
//
//		if (e.getIndeks1() == e.getIndeks2()){
//			continue;
//		}	// fi
//
//		ij = e.getIndeks1();
//		ik = e.getIndeks2();
//
//
//		sjk += degreeInFunctionalGraph[ij] + degreeInFunctionalGraph[ik];
//		sj2k2 += degreeInFunctionalGraph[ij] * degreeInFunctionalGraph[ij]
//		         + degreeInFunctionalGraph[ik] * degreeInFunctionalGraph[ik];
//		spjk += degreeInFunctionalGraph[ij] * degreeInFunctionalGraph[ik];
//	}	// for i


	// after Newman, Assortative Mixing in Networks
	// Physical Review Letters, 2002, Nov
//	double ret = (m1 * 2 * spjk - m1 * m1 * 0.25 * 4 * sjk * sjk )
//			/ (m1 *.5 * 2 *  sj2k2 - m1 * m1 * 0.25 * 4 * sjk * sjk );
//
//	logJP << "\nsjk = " << sjk << "\nsj2k2 = "<< sj2k2 <<"\nspjk = "<< spjk << "\n";

	return ret;
}	// calculatePearsonDegreeCorrelationCoefficient()


double NN_DegreeCorrelationTester::calculateDegVsAverageDegree(){
	degreeVsAvgDegree_Deg = new int[n->getIloscNeuronow()];
	degreeVsAvgDegree_Avg = new double[n->getIloscNeuronow()];

	int sum = 0, count  = 0;
	for (int i=0; i<n->getIloscNeuronow(); i++){
		sum = 0;
		count = 0;
		for (int j=0; j< n->getNeuron(i).getIloscSasiadow(); j++){
			Polaczenie &k = n->getKrawedz(n->getNeuron(i).getIndeksKrawedzi(j));
			if (k.getSumaLadunku() < n->getConfig()->getKohonenChargeThreshold()){
				continue;
			}	// if
			if (k.getIndeks1() == k.getIndeks2()){
				continue;
			}	// if
			sum += degreeInFunctionalGraph[n->getIndeksItegoSasiada(i, j)];
			count ++;

		}	// for neighbours

		if (count == 0){
			sum = -1;
			count  = 1;
		}	// fi

		degreeVsAvgDegree_Deg[i] = degreeInFunctionalGraph[i];
		degreeVsAvgDegree_Avg[i] = 1.0 * sum / count;
	}	// for



	return  -10;
}	// calculateDegVsAverageDegree()

void NN_DegreeCorrelationTester::calculateDegreeCorrelation(){
	logJP.timestamp() << "  Calculating degree correlation = ";
	double corr = calculatePearsonDegreeCorrelationCoefficient();
	logJP<< corr << "\n" ;


	calculateDegVsAverageDegree();




	saveToMFile();
}	// calculateDegreeCorrelation()

void NN_DegreeCorrelationTester::saveToMFile(){

	if (degreeInFunctionalGraph == NULL){
		recalculateDegreesInFunctionalGraph();
	}

	char filename[255];

	if (n->getConfig()->getUseTimestampInFilenames() == true) {
		sprintf(filename, "%s/r_%s__%d_kn_%d_Me_%ldM_it_assortativity.m",
				n->getConfig()->getResultsFolder(),
				n->getTimestamp(),
				n->getIloscNeuronow()/1000,
				n->getIloscKrawedzi()/1000000,
				n->getIloscIteracji() / 1000000
		);
	} else {
		sprintf(filename, "%s/assortativity.m", n->getConfig()->getResultsFolder());
	}	// if .. else


	logJP.timestamp() << ": Assortativity.SaveToMFile() saving results to file "<< filename << " ... ";

	str.open(filename, std::fstream::out );

	if (str == NULL){
		logJP << "NN_DCT.save() str.open Error\n\n";
		return;
	}	// fi



	saveToMFile_generateHeader();
	saveToMFile_generateData();
	saveToMFile_generateFooter();
}	//

void NN_DegreeCorrelationTester::saveToMFile_generateHeader(){
	str << "# timestamp  "<< n->getTimestamp() << '\n';
	str << "# neurons    "<< n->getIloscNeuronow() << '\n';
	str << "# edges      "<< n->getIloscKrawedzi() << '\n';
	str << "# geometry   "<< n->getTypString() << '\n';
	str << "# radius     "<< n->getMaxPos() << '\n';
	str << "# iterations "<< n->getIloscIteracji() << "\n\n";


	str << "%  Assortativity, distribution od degrees";
}	// saveToMFile_generateHeader()


void NN_DegreeCorrelationTester::saveToMFile_generateData(){
	str << "\n\n Degrees = [\n";

	int count = 0;

	for (int i=0; i<n->getIloscKrawedzi(); i++){
		Polaczenie & e = n->getKrawedz(i);
		if (e.getSumaLadunku()>= n->getConfig()->getKohonenChargeThreshold()
				&& e.getIndeks1() != e.getIndeks2()){
			str << "  " << degreeInFunctionalGraph[e.getIndeks1()] << " , "
					<<  degreeInFunctionalGraph[e.getIndeks2()] << "\n";


			str		<< "  " << degreeInFunctionalGraph[e.getIndeks2()] << " , "
					<<  degreeInFunctionalGraph[e.getIndeks1()] << "\n";

			count +=2;
		}	// fi
	}	// for all edges

	str << " ]; % Degrees\n\n";


	str << "\n\n DegVsAvgDeg = [\n";
	for (int i=0; i<n->getIloscNeuronow(); i++){
		str << "  " << degreeVsAvgDegree_Deg[i] << " , " << degreeVsAvgDegree_Avg[i] <<"\n";
	}	// for all edges

	str << " ]; % deg vs avg deg \n\n";

	logJP << "wrote "<< count <<"entries \n";
}	// saveToMFile_generateData()


void NN_DegreeCorrelationTester::saveToMFile_generateFooter(){
	str << "figure(1)\n"
			<<"plot(Degrees(:,1), Degrees(:,2), \"bs\", \"markersize\", 3);\n"
			<< "hold on;\n"
		//	<< "plot(Y_ATTACK(:,1), Y_ATTACK(:,2), \"r*\", \"markersize\", 5);\n"
		//	<< "plot( [1,"<< n->getIloscNeuronow() <<"], ["<< n->getIloscNeuronow() <<",1], \"g\", \"linewidth\", 1.5);\n"
			<< "legend(\""<< "Degrees incident with edges\", \"fontsize\", \"28\");\n"
		//	<< "plot(Y_RAND(:,1), Y_RAND(:,2), \"b\", \"markersize\", 5);\n"
		//	<< "plot(Y_ATTACK(:,1), Y_ATTACK(:,2), \"r\", \"markersize\", 5);\n"
			<< "hold off;\n"
			<< "grid on;\n"
			<< "xlabel(\"Degree n_1\", \"fontsize\", 24);\n"
			<< "ylabel(\"Degree n_2\", \"fontsize\", 24);\n";


		str << "figure(2)\n"
			<< "x = 1:max(Degrees);\n"
			<< "y = 1:max(Degrees);\n"
			<< "z = zeros(max(Degrees) , max(Degrees));\n"
			<< "for k = 1:length(Degrees(:,1))\n\n"
			<< "  if Degrees(k,1) * Degrees(k,2)>= 1\n"
			<< "    z( Degrees(k,1),  Degrees(k,2))++;\n"
			<< "  end\n"
			<< "end;\n\n"
			<< "figure(2);\n"
			<< "surf(x,y,z);\n"
			<< "view(0, 90);\n"
			<< "corr(Degrees)\n\n"

			<< "figure(3);\n"
			<< "plot (DegVsAvgDeg(:,1), DegVsAvgDeg(:,2), \"b+\");\n"
			<< "corr(DegVsAvgDeg)\n";

		str << "% print( \"/home/piersaj/Desktop/nn/degree+Correlation_"<<  n->getTimestamp();

		char buffer[80];
		sprintf(buffer, "_%dk_%dM_%ldM_.svg",
			n->getIloscNeuronow()/1000,
			n->getIloscKrawedzi()/1000000,
			n->getIloscIteracji() / 1000000
		);

		str << buffer << "\" , \"-color\"); \n\n";


		str << "% print( \"/home/piersaj/Desktop/nn/degree+Correlation_"<<  n->getTimestamp();

		sprintf(buffer, "_%dk_%dM_%ldM_.png",
			n->getIloscNeuronow()/1000,
			n->getIloscKrawedzi()/1000000,
			n->getIloscIteracji() / 1000000
		);

		str << buffer << "\" , \"-color\",  \"-dpng\"); \n\n";
		str.close();


}	// saveToMFile_generateFooter()

/*
 * simulationConfigTxtParser.cpp
 *
 *  Created on: Jul 15, 2011
 *      Author: piersaj
 */



#include "simulationConfigTxtParser.h"
#include "../../src/logger.h"
#include <cstdlib>
#include <iostream>



SimulationConfigTxtParser::SimulationConfigTxtParser(char * pathname_):
	DEFAULT_PATHNAME("./neurons.conf"),
	pathname(pathname_)
{
	conf = NULL;
}


SimulationConfigTxtParser::SimulationConfigTxtParser():
	DEFAULT_PATHNAME("./neurons.conf"),
	pathname((char*) DEFAULT_PATHNAME)
{
	conf = NULL;
}

SimulationConfigTxtParser::~SimulationConfigTxtParser(){
	if (conf != NULL){
		delete conf;
		conf = NULL;
	}	// fi
}	// ~this

SimulationConfig *  SimulationConfigTxtParser::getSimulationConfig(){
	parseTxtConfig();
	return conf;
}	// getSimulationConfig

void SimulationConfigTxtParser::parseTxtConfig(){
	if (conf == NULL){
		conf = new SimulationConfig();
	}	// fi

	std::fstream str;
	str.open(pathname, std::fstream::in );

	if (str == NULL){
		std::cerr << "SimConfTxtParser(): error on opening file " << pathname << "\n";
		return;
	}	// fi

	std::string line, param, val;
	size_t eqPos;
	while (str.eof() == false){

		std::getline( str, line);

		size_t ws_pos;
		while ((ws_pos = line.find_first_of(" ")) != line.npos){
			line.erase (ws_pos,1);
		}	// while

		// printf("line **%s**", line.c_str());

		if (line[0] == '#'){
			printf("\n");
			continue;
		}	//	// fi

		eqPos = line.find_first_of("=");

		if (eqPos == line.npos){
			printf("\n");
			// "=" not found
			continue;
		}	// fi

		param = line.substr(0, eqPos);
		val = line.substr(eqPos+1);

		setMasterConfigEntry(param, val);

		// printf(" after spliting ***%s****%s****\n" , param.c_str(), val.c_str());
	}	// while

	str.close();
}	// parseTxtConfig()







/* returns true if var is "true"
 */
bool SimulationConfigTxtParser::atob(char* var){
	if (std::string(var) == "true"){
		return true;
	} else {
		return 0;
	}	// if
}	// atob

/* setting params and values into a SimulationConfig object
 */
void SimulationConfigTxtParser::setMasterConfigEntry(std::string & paramname, std::string & paramval){
	if (conf == NULL){
		conf = new SimulationConfig();
	}	// fi

	char * valStr = (char *) paramval.c_str();

	setMasterConfigEntryNetworkSettings(paramname, valStr);
	setMasterConfigEntryDynamicsSettings(paramname, valStr);
	setMasterConfigEntryResultsSettings(paramname, valStr);
	setMasterConfigEntryLoggingSettings(paramname, valStr);

}	// setMasterConfigEntry

/* setting into config object: settings concerning the network
 */
void SimulationConfigTxtParser::setMasterConfigEntryNetworkSettings(std::string & paramname, char * paramval){
	/*
	 *  network settings
	 */
	if (paramname == "geometry"){
		conf->setGeometry( atoi(paramval));
	}
	if (paramname == "radius"){
		conf->setRadius( atof(paramval));
	}
	if (paramname == "neuronsDensity"){
		conf->setNeuronsDensity( atof(paramval));
	}
	if (paramname == "connectivityAlpha"){
		conf->setConnectivityAlpha( atof(paramval));
	}

	if (paramname == "inverseTemperatureBeta"){
		conf->setInverseTemperatureBeta( atof(paramval));
	}
	if (paramname == "initialChargeMean"){
		conf->setInitialChargeMean( atof(paramval));
	}
	if (paramname == "initialChargeVar"){
		conf->setInitialChargeVar( atof(paramval));
	}
}	// setMasterConfigEntryNetworkSettings()

/* setting into config object: settings concerning the dynamics
 */
void SimulationConfigTxtParser::setMasterConfigEntryDynamicsSettings(std::string & paramname, char * paramval){
	/*
	 * dynamics settings
	 */
	if (paramname == "iterationsNumber"){
		conf->setIterationsNumber( atol(paramval));
	}
	if (paramname == "longRangeSynapseThreshold"){
		conf->setLongRangeSynapseThreshold( atof(paramval));
	}
	if (paramname == "flagChargeRemovalOnLongSynapse"){
		conf->setFlagChargeRemovalOnLongSynapse( atob(paramval));
	}
	if (paramname == "probabilityOfChargeRemovalOnLongSynapse"){
		conf->setProbabilityOfChargeRemovalOnLongSynapse(atof(paramval));
	}
	if (paramname == "chargeReappear"){
		conf->setChargeReappear( atob(paramval));
	}
	if (paramname == "edgesRemoval"){
		conf->setEdgesRemoval( atob(paramval));
	}
	if (paramname == "threads_number"){
		conf->setThreads_number( atoi(paramval));
	}
}	// setMasterConfigEntryDynamicsSettings()

/* setting into config object: settings concerning logging
 */
void SimulationConfigTxtParser::setMasterConfigEntryLoggingSettings(std::string & paramname, char * paramval){
	/*
	 * logging settings
	 */
	if (paramname == "useTimestampInFilenames"){
		conf->setUseTimestampInFilenames( atob(paramval));
	}
	if (paramname == "resultsFolder"){
		conf->setResultsFolder(paramval);
	}
	if (paramname == "logMaster"){
		conf->setLogMaster(atob(paramval));
	}
	if (paramname == "logProgress"){
		conf->setLogProgress( atob(paramval));
	}
	if (paramname == "useGLmode"){
		conf->setUseGLmode( atob(paramval));
	}
}	// setMasterConfigEntryLoggingSettings()

/* setting into config object: settings concerning results aquisition
 */
void SimulationConfigTxtParser::setMasterConfigEntryResultsSettings(std::string & paramname, char * paramval){
	/*
	 * results settings
	 */

	if (paramname == "kohonenEdgesLimit"){
		conf->setKohonenEdgesLimit( atoi(paramval));
	}
	if (paramname == "kohonenChargeThreshold"){
		conf->setKohonenChargeThreshold( atoi(paramval));
	}
	if (paramname == "calculateSpectrum"){
		conf->setCalculateSpectrum( atob(paramval));
	}
	if (paramname == "calculateSpectrumEvolution"){
		conf->setCalculateSpectrumEvolution(atob(paramval));
	}
	if (paramname == "calculateSynapseEvolution"){
		conf->setCalculateSynapseEvolution(atob(paramval));
	}
	if (paramname == "calculateNodesEvolution"){
		conf->setCalculateNodesEvolution(atob(paramval));
	}
	if (paramname == "calculateDegreeDistribution"){
		conf->setCalculateDegreeDistribution(atob(paramval));
	}
	if (paramname == "calculatePathLengthDistribution"){
		conf->setCalculatePathLengthDistribution(atob(paramval));
	}
	if (paramname == "calculatePathLengthDistributionMC"){
		conf->setCalculatePathLengthDistributionMC(atob(paramval));
	}
	if (paramname == "calculateClasteringCoeffitientDistribution"){
		conf->setCalculateClasteringCoeffitientDistribution(atob(paramval));
	}
	if (paramname == "calculateFailureResiliency"){
		conf->setCalculateFailureResiliency(atob(paramval));
	}
	if (paramname == "calculateNetworkDegeneracy"){
		conf->setCalculateNetworkDegeneracy(atob(paramval));
	}
}	// setMasterConfigEntryResultsSettings()

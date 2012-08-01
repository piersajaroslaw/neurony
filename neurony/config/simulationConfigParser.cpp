/*
 * simulationConfigParser.cpp
 *
 *  Created on: Apr 8, 2011
 *      Author: piersaj
 *
 *  2011-04-08
 *  	created
 *  	nicely built, organised and made it work :)
 */

#include "simulationConfigParser.h"
#include <cstdlib>

#ifdef WITH_XML_LIB2

//#include <libxml/xmlreader.h>
//#include <libxml2/libxml/xmlreader.h>

#include <libxml++-2.6/libxml++/libxml++.h>
#include <libxml++-2.6/libxml++/parsers/textreader.h>
#include "../src/logger.h"
#include <glibmm-2.4/glibmm/ustring.h>

SimulationConfigParser::SimulationConfigParser(char * pathname_):
	DEFAULT_PATHNAME("konfig/konfig.xml"),
	pathname(pathname_)
{
	conf = NULL;
}


SimulationConfigParser::SimulationConfigParser():
	DEFAULT_PATHNAME("konfig/konfig.xml"),
	pathname((char*) DEFAULT_PATHNAME)
{
	conf = NULL;
}

SimulationConfig *  SimulationConfigParser::getSimulationConfig(){
	parseXmlConfig();
	return conf;
}	// getSimulationConfig

void SimulationConfigParser::parseXmlConfig(){
	xmlpp::TextReader reader(pathname);
	Glib::ustring paramval, paramname;
	bool res;
	while( (res = reader.read()) )	{
		while (res && reader.get_name() != "paramname"){
			reader.move_to_element();
			res = reader.read();
		}	// while



		while (res && reader.get_node_type()!=reader.Text){
			reader.move_to_element();
			res=reader.read();
		}	// while

		if (res){
//			logJP   << reader.get_value();
			paramname  = reader.get_value();
		}	// fi

		while (res && reader.get_node_type() != reader.Element && reader.get_name() != "paramvalue"){
			reader.move_to_element();
			res=reader.read();
		}	// while


		while (res && reader.get_node_type()!=reader.Text ){
			reader.move_to_element();
			res=reader.read();
		}	// while

		if (res){
//			logJP  << "  " << " " << reader.get_value() << "\n";
			paramval =  reader.get_value();
			setMasterConfigEntry(paramname, paramval);
		}	// fi
	}	// while
}	// parseXmlConfig()


/* returns true if var is "true"
 */
bool atob(char* var){
	if (Glib::ustring(var) == "true"){
		return true;
	} else {
		return 0;
	}
}	// atob

/* setting params and values into a SimulationConfig object
 */
void SimulationConfigParser::setMasterConfigEntry(Glib::ustring & paramname, Glib::ustring & paramval){
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
void SimulationConfigParser::setMasterConfigEntryNetworkSettings(Glib::ustring & paramname, char * paramval){
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
void SimulationConfigParser::setMasterConfigEntryDynamicsSettings(Glib::ustring & paramname, char * paramval){
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
void SimulationConfigParser::setMasterConfigEntryLoggingSettings(Glib::ustring & paramname, char * paramval){
	/*
	 * logging settings
	 */
	if (paramname == "useTimestampInFilenames"){
		conf->setUseTimestampInFilenames( atob(paramval));
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
void SimulationConfigParser::setMasterConfigEntryResultsSettings(Glib::ustring & paramname, char * paramval){
	/*
	 * results settings
	 */

	if (paramname == "kohonenEdgesLimit"){
		conf->setKohonenEdgesLimit( atoi(paramval));
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
}	// setMasterConfigEntryResultsSettings()

#endif // if WITH_XML_LIB2


#ifdef LIBXML_READER_ENABLED
/*
 * this version uses libxml2
 * and its somehow sketchy
 */
void SimulationConfigParser::parseXmlConfig(){

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*
     * ctrl+c-ctrl+v'ed from some tutorial
     */
    xmlTextReaderPtr reader;
    int ret;

    reader = xmlReaderForFile(filename, NULL, 0);
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
            processNode(reader);
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
            jogJP<< "Error: XmlParser failed to parse "<< filename << "\n";
        }
    } else {	// if
    	jogJP<< "Error: XmlParser failed to open "<< filename << "\n";
    }	// fi

    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
//#else
//    logJP << "ERROR SCParser.parseXml(): libxml not available\n";
}

#endif

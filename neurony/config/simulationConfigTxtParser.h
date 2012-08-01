/*
 * simulationConfigTxtParser.h
 *
 *  Created on: Jul 15, 2011
 *      Author: piersaj
 *
 *
 *  Config tool, which does not require third party libs.
 *  Plain txt config support
 *
 *  2011-07-15
 *  	created
 */

#ifndef SIMULATIONCONFIGTXTPARSER_H_
#define SIMULATIONCONFIGTXTPARSER_H_



#include "simulationMasterConfig.h"
#include <string>

/* Reading configuration from txt file
 */
class SimulationConfigTxtParser{
	const char * DEFAULT_PATHNAME;
	char * pathname;
	// note: this should persist through destructor!
	SimulationConfig * conf;

public:
	SimulationConfigTxtParser();
	~SimulationConfigTxtParser();
	SimulationConfigTxtParser(char * pathname);
	SimulationConfig * getSimulationConfig();

private:
	void parseTxtConfig();

	void setMasterConfigEntry(std::string & paramname, std::string & paramval);

	void setMasterConfigEntryNetworkSettings(std::string & paramname, char* paramval);
	void setMasterConfigEntryDynamicsSettings(std::string & paramname, char* paramval);
	void setMasterConfigEntryLoggingSettings(std::string & paramname, char* paramval);
	void setMasterConfigEntryResultsSettings(std::string & paramname, char* paramval);

	bool atob(char* var);


};	// class

#endif /* SIMULATIONCONFIGTXTPARSER_H_ */

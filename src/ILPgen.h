/*
 * ILPgen.h
 *
 *  Created on: Apr 5, 2017
 *      Author: ga63quk
 */

#ifndef ILPGEN_H_
#define ILPGEN_H_

#include <string>
#include "Graph.h"

class ILPgen {

public:
	ILPgen();
	virtual ~ILPgen();

	Graph* g;

	void pathToCoverAllEdges(const char* fileName);
	void path(int pathNumber);
	void addVar(string varName, int varType);

private:
	vector<string> constraints;
	vector<string> variables;
	vector<string> variableTypes;
	vector<string> obj;
};

#endif /* ILPGEN_H_ */

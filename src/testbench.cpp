#include "Graph.h"
#include "ILPgen.h"
#include "ILP.h"


#include <iostream>
using namespace std;

int main() {


	const char* ilpFileName = "pathCoverAllEdges.lp";
	Graph* g = new Graph();
	g->initTest(10,10);

	ILPgen ilpGen;
	ilpGen.g = g;
	ilpGen.pathToCoverAllEdges(ilpFileName,2);
	ILP::ILP_solve(ilpFileName);
	return 0;
}


#include "Graph.h"
#include "ILPgen.h"
#include "ILP.h"


#include <iostream>
using namespace std;

int main() {
	const char* ilpFileName = "pathCoverAllEdges.lp";
	Graph* g = new Graph();
	g->initTest(3,3);

	ILPgen ilpGen;
	ilpGen.g = g;
	ilpGen.pathToCoverAllEdges(ilpFileName);
	ILP::ILP_solve(ilpFileName);
	return 0;
}


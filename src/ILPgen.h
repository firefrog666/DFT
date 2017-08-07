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
#include <map>
#include <vector>
#include "ILP.h";

class ILPgen {

public:
	ILPgen();

	virtual ~ILPgen();

	Graph* g;

	void pathToCoverAllEdges(const char* fileName, int pathNumber);
	void treeToCoverAllEdgesNoLoop(const char* fileName, int pathNumber);
	void pathToCoverAllEdgesNoLoop(const char* fileName, int pathNumber);
	void pathCoverSeverEdges(const char* fileName, int pathNumber,vector<Edge*>& edgesToCover);
	void pathCoverSeverEdgesNoLoop(const char* fileName, int pathNumber,vector<Edge*>& edgesToCover);
	void path(int pathNumber);
	void tree(int treeNumber);
	void pathLoop(int pathNumber);
	void pathNoLoop(int pathNumber);
	void cutSeveralEdgesNoLoop(int pathNumber = 0,const char* fileName = NULL, V<E> edgesToCover = {},V<E>usedEdges = {});
	void cut(int pathNumber);

	void cutGenralGraph(int pathNumber);
	void cutNoFlow(int cutNumber);
	void cutPunishMent(int cutNumber);
	void cutGeneralCoverSeveralEdges(int cutNumber,const char* fileName, V<E> edgeToCover, V<E> usedEdges = V<E>());

	void pathValveSwitchTogether(int pathNumber, V<V<E>> paths, const char* fileName);
	void valveSwitchTogeter(int pathNumber,V<V<E>> paths);

	void noLoop(int pathNumber);
	void coverEdge(vector<Edge*>& edgestoCover, int pathNumber);
	void minimumEdgeUse(int pathNumber);
	void addVar(string varName, int varType);
	void ilpRun(const char* ilpFileName);
	void clearAll();
	void writeInToILPfile(const char* fileName);

	void pathLoopPunishMent(int pathTotalNumber);
	void newEdgeReward(V<E> usedEdges, int pathTotalNumber);

	vector<Edge*> getPath(int pathNumber);

public:

	vector<string> constraints;
	vector<string> variables;
	vector<string> variableTypes;
	vector<string> bounds;
	vector<string> obj;


	map<string,float> varValue;
};

#endif /* ILPGEN_H_ */

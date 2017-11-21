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
	ILP ilp;
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
	void cutUseLeastDuplicateEdges(int cutTotalNumber);
	void oldEdgePunish(V<E> usedEdges, int pathTotalNumber);

	void pathValveSwitchTogether(int pathNumber, V<V<E>> paths, const char* fileName);
	void valveSwitchTogeterGeneralG(int pathNumber);
	void valveSwitchTogeter(int pathNumber,V<V<E>> paths);
	void treeValveSwitchTogether(int pathNumber, V<V<E>> paths, const char* fileName);

	void noLoop(int pathNumber);
	void coverEdge(const vector<Edge*>& edgestoCover, int pathNumber);
	void coverEdgeCut(vector<Edge*>& edgesToCover, int cutIndex,int cutNumber);
	void minimumEdgeUse(int pathNumber);
	void addVar(string varName, int varType);
	void clearAll();
	void writeInToILPfile(const char* fileName);

	void pathLoopPunishMent(int pathTotalNumber);
	void newEdgeReward(V<E> usedEdges, int pathTotalNumber, V<E> edgeToCover);

	vector<Edge*> getPath(int pathNumber);
	vector<Edge*> getPath(int pathNumber, int solutionNumber);

	void pathWishMeLuck(int pathId);
	void pathCoverAllWishLuck(const char* fileName, int pathNumber);
	void pathCoverAsManyEdges(int pathNum,const char * ilpFileName,V<E> edgeToCover,V<E> usedEdges);
	void treeCoverAsManyEdges(int pathNum,const char * ilpFileName,V<E> edgeToCover,V<E> usedEdges);

	void ilpRun(const char* ilpFileName);
	void ilpRun_pool(const char* ilpFileName);
	void setILPTime(float time);
	void setILPGap(float gap);
	void cutKrish(int cutNumber);
	void cutNoFlowKrish(int cutNumber);
	void cutGeneralKrish(int cutNumber,const char* fileName,V<E> edgeToCover, V<E> usedEdges);

	void pathThrougEdge(E edgeToPass, const char* ilpFile);
	void pathLeastResource(int pathTotalNumber,const char* fileName,V<E> edgeToCover, V<E> resourceEdges);
	void leastResource(int pathTotalNumber, V<E> resourceEdges);
	void dfsPaths(int pathTotalNumber,const char* fileName,V<E> edgeToCover, V<E> resourceEdges);
	void dfsCuts(int cutTotalNumber,const char* fileName,V<E> edgeToCover, V<E> resourceEdges);
	void tryUseTheseEdges(const V<E>& edgesToUse,const V<E>& resouceEdges,int pathTotalNumber);
	void dfsPathCutValveSharing(int pathTotalNumber,int cutTotalNumber, const char* fileName,V<E>& edgeToCover, V<E>& resourceEdges);
	/* peak
	 *
	 */
	void pressureSouceNumberConstraint(int pathTotalNumber,  V<E>& testValves);
	void pressureSouceCutShare(int cutTotalNumber, int cutIndexBase, V<E>& testVales);

	void pathVectorNoLeakApp(int pathTotalNumber, V<V<E>> appPath, V<V<E>> mustCloseVales,V<E>& testValves);

	void pathCoverSeverEdgesTryUseTheseEdges(const char* fileName,int pathTotalNumber,const V<E>& edgesToCover,const V<E>& newEdges,const V<E>& resouceEdges);
	void maxNonResource(int pathTotalNumber, V<E> resourceEdges);
	void dfsCutsMostEdgeToCover(const char* fileName,V<E> edgeToCover);
	void coverAsManyEdgeAsPossible(V<E>& edgeToCover);



public:

	vector<string> constraints;
	vector<string> variables;
	vector<string> variableTypes;
	vector<string> bounds;
	vector<string> obj;


	map<string,float> varValue;
	vector<map<string,float>> allVarValues;
};

#endif /* ILPGEN_H_ */

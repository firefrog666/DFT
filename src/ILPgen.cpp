/*
 * ILPgen.cpp
 *
 *  Created on: Apr 5, 2017
 *      Author: ga63quk
 */



#include "ILPgen.h"
#include "toolBox.h"
#include "Graph.h"
#include "writeILP.h"


#define LAGLANGR_RELAXATION 1
#define PUNISHMENT_WEIGHT 1
#define CUTGAP 2
#define CIRCLE 0
__inline__ string getPathName(int pathNumber){
	return  s("p") + s(pathNumber);
}

__inline__ string getCutName(int cutNumber){
	return  s("c") + s(cutNumber);
}


__inline__ string getNodeReach(int cutNumber,Node* s, Node* t){
	return  getPathName(cutNumber) + s->name + S("reach") + t->name;
}
__inline__ string pathUseNode(int pathNumber, Node* node){
	return getPathName(pathNumber) + s("Use") + node->name;
}

__inline__ string getNodeTimestamp(int pathId,Node* n){
	return getPathName(pathId) + n->name + S("time");
}

__inline__ string edgeOutN(int pathId, E e, N n){
	return getPathName(pathId) + e->name + S("Out") + n->name;
}

__inline__ string edgeInN(int pathId, E e, N n){
	return getPathName(pathId) + e->name + S("In") + n->name;
}
__inline__ string pathHeadUseNode(int pathNumber, Node* node){
	return getPathName(pathNumber) + s("HeadUse") + node->name;
}

__inline__ string pathHeadUseCell(int pathNumber, Cell* cell){
	return getPathName(pathNumber) + s("HeadUse") + cell->name;
}


__inline__ string pathTailUseNode(int pathNumber, Node* node){
	return getPathName(pathNumber) + s("TailUse") + node->name;
}

__inline__ string pathUseEdge(int pathNumber, Edge* edge){
	return getPathName(pathNumber) + s("Use") + edge->name;
}

__inline__ string pathNodeFlow(int pathNumber, Node* node){
	return getPathName(pathNumber) + s("Flow") + node->name;
}
__inline__ string pathEdgeFlow(int pathNumber, Edge* edge){
	return getPathName(pathNumber) + s("Flow") + edge->name;
}

__inline__ string pathEdgeFlow(int pathNumber, Edge* edge, Node* source){
	return getPathName(pathNumber) + s("Flow") + edge->name + source->name;
}


__inline__ string pathSuperFlow(int pathNumber){
	return getPathName(pathNumber) + s("SuperFlow");
}

__inline__ string pathSuperFlow(int pathNumber, Node* source){
	return getPathName(pathNumber) + s("SuperFlow") + source->name;
}


__inline__ string conserVNode(int pathNumber, Node* node){
	return getPathName(pathNumber) + node->name + S("Conserve");
}

__inline__ string conserVNode(int pathNumber, Node* node,Node* source){
	return getPathName(pathNumber) + node->name + S("Conserve") + source->name;
}

__inline__ string conserVCell(int pathNumber, Cell* c){
	return getPathName(pathNumber) + c->name + S("Conserve");
}

__inline__ string superFlowCell(int pathNumber, Cell* cell){
	return getPathName(pathNumber) + cell->name + S("SuperFlow");
}

__inline__ string superFlowNode(int pathNumber, Node* node){
	return getPathName(pathNumber) + node->name + S("SuperFlow");
}
__inline__ string superFlowNode(int pathNumber, Node* node, Node* source){
	return getPathName(pathNumber) + node->name + S("SuperFlow") + source->name;
}
__inline__ string potentialNode(int pathNumber, Node* node){
	return getPathName(pathNumber) + node->name + S("potential");
}


__inline__ string cutUseCell(int cutNumber, Cell* cell){
	return getCutName(cutNumber) + s("Use") + cell->name;
}

__inline__ string edgePairSeperate(int pathId, E e0,E e1){
	return getPathName(pathId) + e0->name + e1->name + S("Sep");
}
__inline__ string v0ShareV1PS(E e0, E e1){
	return e0->name + S("SH") + e1->name;
}
ILPgen::ILPgen() {
	// TODO Auto-generated constructor stub

}

ILPgen::~ILPgen() {
	// TODO Auto-generated destructor stub
}


void ILPgen::writeInToILPfile(const char* fileName){
	writeILP ilp;
	ilp.writeOBJ(obj,fileName);
	ilp.writeConstraint(constraints,fileName);
	ilp.writeBounds(bounds,fileName);
	ilp.writeVarNames(this->variables,this->variableTypes,fileName);
	clearAll();
	ilp.closeFile();
}


void ILPgen::coverEdge(const vector<Edge*>& edgesToCover, int pathNumber){
	for(Edge* e:edgesToCover){
		string edgeUsedOnce = "";
		for(int i = 0; i <= pathNumber-1; i ++){
			edgeUsedOnce += S(" + ") + pathUseEdge(i,e);
		}
		constraints.push_back(edgeUsedOnce + S(" >= 1"));
	}
}

void ILPgen::coverEdgeCut(vector<Edge*>& edgesToCover, int cutIndex,int cutNumber){
	for(Edge* e:edgesToCover){
		string edgeUsedOnce = "";
		for(int i = cutIndex; i < cutNumber+cutIndex; i ++){
			edgeUsedOnce += S(" + ") + pathUseEdge(i,e);
		}
		constraints.push_back(edgeUsedOnce + S(" >= 1"));
	}
}

void ILPgen::pathCoverSeverEdges(const char* fileName, int pathNumber,vector<Edge*>& edgesToCover){
	for(int i = 0; i <= pathNumber-1; i++){
			path(i);
	}

	//path cover all the edges
	coverEdge(edgesToCover, pathNumber);
	writeInToILPfile(fileName);

}
void ILPgen::pathCoverSeverEdgesNoLoop(const char* fileName, int pathNumber,vector<Edge*>& edgesToCover){
	for(int i = 0; i <= pathNumber-1; i++){
			pathNoLoop(i);
	}

	//punishment for loops
#if LAGLANGR_RELAXATION
	pathLoopPunishMent(pathNumber);
#endif
	//path cover all the edges
	coverEdge(edgesToCover, pathNumber);

	writeInToILPfile(fileName);
}

void ILPgen::clearAll(){
	obj.clear();
	constraints.clear();
	bounds.clear();
	variables.clear();
	variableTypes.clear();

}

//for a graph, write ILPs
//find paths to cover all the edges

void ILPgen::treeToCoverAllEdgesNoLoop(const char* fileName, int pathNumber){

	for(int i = 0; i <= pathNumber-1; i++){
		tree(i);
		//noLoop(i);
	}

	//punishment for loops
#if LAGLANGR_RELAXATION
	pathLoopPunishMent(pathNumber);
#endif
	//path cover all the edges
	coverEdge(g->edges, pathNumber);
	//minimumEdgeUse(pathNumber);
	//valveSwitchTogeterGeneralG(pathNumber);
	writeInToILPfile(fileName);
}
void ILPgen::pathToCoverAllEdgesNoLoop(const char* fileName, int pathNumber){

	for(int i = 0; i <= pathNumber-1; i++){
		pathNoLoop(i);
	}

	//punishment for loops
#if LAGLANGR_RELAXATION
	pathLoopPunishMent(pathNumber);
#endif
	//path cover all the edges
	coverEdge(g->edges, pathNumber);
	//minimumEdgeUse(pathNumber);
	writeInToILPfile(fileName);
}

//
void ILPgen::tryUseTheseEdges(const V<E>& edgesToUse,const V<E>& resouceEdges,int pathTotalNumber){
	obj.push_back("minimize");
	string cost = "";
	for(int p = 0; p < pathTotalNumber; p++){
		for(E e:resouceEdges){
			if(algo::vecContains(edgesToUse,e)){
				cost+= S(" - ") + pathUseEdge(p,e);
			}
			else{
				cost += S(" + ")+ pathUseEdge(p,e);
			}
			//punish paht total length
			cost += S(" + ") + pathUseEdge(p,e);

		}

	}


	cout<< cost<<endl;
	obj.push_back(cost);

}



void ILPgen::minimumEdgeUse(int pathNumber){
	//min
	string allEdges ="";
	for(Edge* e:g->edges){
		for(int i = 0; i <= pathNumber-1; i ++){
			allEdges += S(" + ")+pathUseEdge(i,e);
		}
	}
	obj.push_back("min");
	obj.push_back(allEdges);
}

void ILPgen::pathCoverSeverEdgesTryUseTheseEdges(const char* fileName,int pathTotalNumber,const V<E>& edgesToCover,const V<E>& newEdges,const V<E>& resouceEdges){
	for(int i = 0; i <= pathTotalNumber-1; i++){
		path(i);
		noLoop(i);
	}

	//path cover all the edges

	coverEdge(edgesToCover,pathTotalNumber);
	tryUseTheseEdges(newEdges,resouceEdges,pathTotalNumber);

	//minimumEdgeUse(pathNumber);
	writeInToILPfile(fileName);

}

void ILPgen::pathToCoverAllEdges(const char* fileName, int pathNumber){

	for(int i = 0; i <= pathNumber-1; i++){
		path(i);
	}

	//path cover all the edges

	coverEdge(g->edges,pathNumber);

	//minimumEdgeUse(pathNumber);
	writeInToILPfile(fileName);
}



//0 for integer, 1 for binary
void ILPgen::addVar(string variableName, int variableType){
	variables.push_back(variableName);
	//integer
	if(variableType == 0){
		variableTypes.push_back("0");
		bounds.push_back(S(-INTEGERBOUND) + S(" <= ") + variableName + S(" <= ") + S(INTEGERBOUND));
	}
	//bi
	else if(variableType == 1)
		variableTypes.push_back("1");
	else if(variableType == 4){
		variableTypes.push_back("4");
		bounds.push_back(S(-INTEGERBOUND) + S(" <= ") + variableName + S(" <= ") + S(INTEGERBOUND));
	}
	else if(variableType == 5){
		variableTypes.push_back("5");
		bounds.push_back(S(-1) + S(" <= ") + variableName + S(" <= ") + S(1));
	}
	else if(variableType == 6){
		variableTypes.push_back("6");
		bounds.push_back(S(-BIG_INTEGER) + S(" <= ") + variableName + S(" <= ") + S(BIG_INTEGER));
	}
}

void ILPgen::cut(int pathNumber){
	for(Node* n:g->nodes){
			string pUseNode = pathUseNode(pathNumber,n);
			addVar(pUseNode,1);
	}

	for(Edge* e:g->edges){
		string pUseEdge = pathUseEdge(pathNumber,e);
		addVar(pUseEdge,1);
	}
	//path starts from entrance
	string pUseOneEntrance = "";
	Node* entrance = g->entrances.at(pathNumber);
	pUseOneEntrance += s(" + ") + pathHeadUseNode(pathNumber,entrance);
	addVar(pathHeadUseNode(pathNumber,entrance),1);

	pUseOneEntrance += s(" = 1");
	constraints.push_back(pUseOneEntrance);
	//path ends with exit
	string pUseOneExit= "";
	Node* exit = g->exits.at(pathNumber);
	pUseOneExit+= s(" + ") + pathTailUseNode(pathNumber,exit);
	addVar(pathTailUseNode(pathNumber,exit),1);

	pUseOneExit+= s(" = 1");
	constraints.push_back(pUseOneExit);

	string onlyOneHead = "";
	//there is only one head
	for(Node* n:g->nodes){
		onlyOneHead += s(" + ") + pathHeadUseNode(pathNumber,n);
	}
	onlyOneHead += s(" = 1");
	constraints.push_back(onlyOneHead);
	//there is only one tail

	string onlyOneTail = "";
	for(Node* n:g->nodes){
		onlyOneTail += s(" + ") + pathTailUseNode(pathNumber,n);
	}
	onlyOneTail+= s(" = 1");
	constraints.push_back(onlyOneTail);



	//for a head or tail, adj edge if not wall then use one;
	for(Node* n:g->nodes){

		string headUseOneEdge = "";
		string tailUseOneEdge = "";
		for(Edge* e:*n->getAdjEdges()){
			if(e->isWall)
				continue;
			headUseOneEdge += S(" + ")+pathUseEdge(pathNumber,e);
			tailUseOneEdge += S(" + ")+pathUseEdge(pathNumber,e);
		}

		//if head use this node, headUseOndge = 1
		constraints.push_back(headUseOneEdge + S(" + ")+ S(M) + S(" ") + pathHeadUseNode(pathNumber,n) + S(" <= ") + S(M+1));
		constraints.push_back(headUseOneEdge + S(" - ")+ S(M) + S(" ") + pathHeadUseNode(pathNumber,n) + S(" >= ") + S(1-M));
		constraints.push_back(tailUseOneEdge + S(" + ")+ S(M) + S(" ") + pathTailUseNode(pathNumber,n) + S(" <= ") + S(M+1));
		constraints.push_back(tailUseOneEdge + S(" - ")+ S(M) + S(" ") + pathTailUseNode(pathNumber,n) + S(" >= ") + S(1-M));
	}

	/*//for 1 edge on the path, one of the neigbour path must be also on the path
	for(Edge* e:g->edges){
		string neibEdges = "";
		for(Edge* neigbEdge:e->adjEdges){
			if(neigbEdge->isWall)
				continue;
			neibEdges += S(" + ") + pathUseEdge(pathNumber,neigbEdge);
		}
		constraints.push_back(neibEdges+ S(" + ") +S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" <= ") + S(1+M));
		constraints.push_back(neibEdges+ S(" - ") +S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" >= ") + S(1-M));
	}*/

	//for one node on path, if not head or tail, 2 neigbour edge on path
	//if head or tail, 1 neigbour edge

	for(Node* n:g->nodes){
		if(!((n->x > entrance->x - CUTGAP && n->x <entrance->x+CUTGAP)||
				(n->y > entrance->y - CUTGAP && n->y <entrance->y+CUTGAP)||
				(n->x > exit->x - CUTGAP && n->x <exit->x+CUTGAP)||
				(n->y > exit->y - CUTGAP && n->y <exit->y+CUTGAP)
				)){
			constraints.push_back( pathUseNode(pathNumber,n) + S(" = ") + S(0));
			continue;
		}
		string neibourEdges = "";
		for(Edge* neibourEdge: *n->getAdjEdges()){
			if(neibourEdge->isWall){
				continue;
			}
			neibourEdges += S(" + ") + pathUseEdge(pathNumber, neibourEdge);
		}
		constraints.push_back(neibourEdges+ S(" + ") +S(M) + S(" ") + pathUseNode(pathNumber,n) +S(" + ") + pathHeadUseNode(pathNumber,n)
				+S(" + ") + pathTailUseNode(pathNumber,n)+ S(" <= ") + S(2+M));
		constraints.push_back(neibourEdges+ S(" - ") +S(M) + S(" ") + pathUseNode(pathNumber,n) +S(" + ") + pathHeadUseNode(pathNumber,n)
						+S(" + ") + pathTailUseNode(pathNumber,n)+ S(" >= ") + S(2-M));
	}

	//if one edge is used on the path, all 2 nodes on it are also on the path;

	for(Edge* e:g->edges){

		if(!((e->x > entrance->x - CUTGAP && e->x <entrance->x+CUTGAP)||
					(e->y > entrance->y - CUTGAP && e->y <entrance->y+CUTGAP)||
					(e->x > exit->x - CUTGAP && e->x <exit->x+CUTGAP)||
					(e->y > exit->y - CUTGAP && e->y <exit->y+CUTGAP)||
					(e->s > entrance->x - CUTGAP && e->s <entrance->x+CUTGAP)||
					(e->t > entrance->y - CUTGAP && e->t <entrance->y+CUTGAP)||
					(e->s > exit->x - CUTGAP && e->s <exit->x+CUTGAP)||
					(e->t > exit->y - CUTGAP && e->t <exit->y+CUTGAP)
					)){
				constraints.push_back( pathUseEdge(pathNumber,e) + S(" = ") + S(0));
				continue;
		}
		Node* n0 = g->getNode(e->x,e->y);
		Node* n1 = g->getNode(e->s,e->t);
		constraints.push_back(pathUseNode(pathNumber,n0) + S(" + ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" <= ") + S(1 + M));
		constraints.push_back(pathUseNode(pathNumber,n0) + S(" - ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" >= ") + S(1 - M));
		constraints.push_back(pathUseNode(pathNumber,n1) + S(" + ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" <= ") + S(1 + M));
		constraints.push_back(pathUseNode(pathNumber,n1) + S(" - ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" >= ") + S(1 - M));
	}



}

void ILPgen::cutUseLeastDuplicateEdges(int cutTotalNumber){
	obj.push_back("min");
	string allEdges = "";
	for(int i = 0; i < cutTotalNumber; i++){
		for(E e:g->edges){
			allEdges += S(" + ") + pathUseEdge(i,e);
		}
	}

	obj.push_back(allEdges);

}


void ILPgen::cutGeneralCoverSeveralEdges(int cutNumber,const char* fileName,V<E> edgeToCover, V<E> usedEdges){
	obj.push_back("max");

	for(int i = 0; i <cutNumber;i++){
		cutGenralGraph(i);

		cutNoFlow(i);
		cutPunishMent(i);
	}


/*	//at least one new edge
	for(int i = 0; i < cutNumber;i++){
		string oneNewEdge = "";
		for(E e:g->edges){
			if(algo::vecContains(usedEdges,e))
				continue;
			oneNewEdge += S(" + ") + pathUseEdge(i,e);
		}
		constraints.push_back(oneNewEdge + S(" >= 1"));
	}*/


	//cutUseLeastDuplicateEdges(cutNumber);
	//oldEdgePunish(usedEdges,cutNumber);

//	obj.push_back("Max");
//	obj.push_back(" ");
	newEdgeReward(usedEdges,cutNumber,edgeToCover);
//	coverEdge(edgeToCover,cutNumber);

	writeInToILPfile(fileName);
}



void ILPgen::cutPunishMent(int cutId){
	//punish ment is minus 0, so it's max

	string totalPunishment = "";
	/*V<Cell*> allcells = g->getCells();
	//is cell is open, drop it
	V<Cell*> cells;
	for(Cell* c:allcells){
		if(c->isOpen())
			continue;
		cells.push_back(c);
	}
*/
	V<Cell*> cells = g->cells;
	//Cell* superCell = g->superCell;
	//cells.push_back(superCell);

	map<Edge*,Cell*> edgeFatherCell;

	for(Cell* c:cells){
		for(E e:*c->adjEdges){
			if (algo::mapContains(edgeFatherCell,e))
				continue;
			else
				edgeFatherCell[e] = c;
		}
	}



	//one head cell
	string onlyOneHead = "";
	for(Cell* c:cells){
		string headCell = pathHeadUseCell(cutId,c);
		addVar(headCell,1);
		onlyOneHead += S(" + ") + headCell;
	}

	// //only one head
	constraints.push_back(onlyOneHead + S(" = ") + S(2*(g->exits.size()-1)));


	for(Cell* c:cells){
	
		for(V<E> edgesOnBrink:g->wToWEdges){
			for(E e:edgesOnBrink){

				if(!algo::vecContains(*c->adjEdges,e))
					continue;

				//if cut use this edge on  waste to waste, pathHead use this edge

				constraints.push_back(pathUseEdge(cutId,e) + S(" - ") + pathHeadUseCell(cutId,c) + S(" = 0"));
					
			}

		}


	}

	//for a not used edge, flow = 0
	for(Edge* e:g->edges){
		addVar(pathEdgeFlow(cutId,e),6); // 4 means continue
		constraints.push_back(pathEdgeFlow(cutId,e) + S(" - ") + S(M) + S(" ") + pathUseEdge(cutId,e) + S(" <= 0"));
		constraints.push_back(pathEdgeFlow(cutId,e) + S(" + ") + S(M) + S(" ") + pathUseEdge(cutId,e) + S(" >= 0"));
	}

	for(E e0:g->sToWEdges[0]){
		for(E e1:g->sToWEdges[1]){
			constraints.push_back(pathEdgeFlow(cutId,e0) + S(" + ") + pathEdgeFlow(cutId,e1) + S( " = 0"));
		}
	}

	for(Cell* c:cells){

		addVar(conserVCell(cutId,c),1); //continues with bound 1
		addVar(superFlowCell(cutId,c),6); // conitnues with bound 10000
		//addVar(potentialNode(pathNumber,n),4);
		string flowOnCell = "";
	/*	if(c == superCell){
			constraints.push_back(superFlowCell(cutId,c)  + S(" = 0"));
			for(Edge* e:*c->adjEdges){

				constraints.push_back(pathEdgeFlow(cutId,e) + S(" = 0"));

			}
			continue;
		}*/

		for(Edge* adjEdge:*c->adjEdges){
			if(adjEdge->isWall)
				continue;
			if(edgeFatherCell[adjEdge] == c){
				flowOnCell+= S(" - ") + pathEdgeFlow(cutId,adjEdge);
			}
			else{
				flowOnCell += S(" + ") + pathEdgeFlow(cutId, adjEdge);
			}
		}

		//if pathHeadUseThis do not use this Node, superFlow = 0
		constraints.push_back(superFlowCell(cutId,c) + S(" + ") +  S(M) + S(" ") + pathHeadUseCell(cutId,c) + S(" >= 0"));
		constraints.push_back(superFlowCell(cutId,c) + S(" - ") +  S(M) + S(" ") + pathHeadUseCell(cutId,c) + S(" <= 0"));


		constraints.push_back(flowOnCell +S(" + ") + superFlowCell(cutId,c) + S(" - ")+conserVCell(cutId,c) + S(" = 0") );
		constraints.push_back(conserVCell(cutId,c) + S(" - ") + cutUseCell(cutId,c) + S(" <= 0"));

		//where a cell on cut but no flow to it
		//totalPunishment+=  S(" - ") +S(M) +S(" ")+ cutUseCell(cutId,c)  + S(" + ")+S(M) +S(" ")+conserVCell(cutId,c);
		//totalPunishment+=   S(" + ")+S(M) +S(" ")+conserVCell(cutId,c) + S(" - ") + S(M) + S(" ")+ cutUseCell(cutId,c);
		totalPunishment+=   S(" + ")+conserVCell(cutId,c) + S(" - ") + cutUseCell(cutId,c);
	}



	obj.push_back(totalPunishment);


}


void ILPgen::cutGeneralKrish(int cutNumber,const char* fileName,V<E> edgeToCover, V<E> usedEdges){
	obj.push_back("max");
	obj.push_back(" ");

	for(int i = 0; i <cutNumber;i++){
		cutKrish(i);

		cutNoFlowKrish(i);
	}


	newEdgeReward(usedEdges,cutNumber,edgeToCover);


	writeInToILPfile(fileName);
}


void ILPgen::cutNoFlowKrish(int cutNumber){
	//for a cut used edge, flow = 0

	Node* source = g->entrances.at(0);
	N exit = g->exits.at(0);

	// source = g->exits.at(0);

	for(Node* n:g->nodes){
		string nodeReach = getNodeReach(cutNumber,source,n);
		addVar(nodeReach,1);
		//pressure source
		if(algo::vecContains(g->entrances,n)){
			constraints.push_back(nodeReach + S(" = 1"));
			continue;
		}

		//pressure sensor
		if(algo::vecContains(g->exits,n)){
			constraints.push_back(nodeReach + S(" = 0"));
			continue;
		}



	}

	//Node reach = or( nodes around it and not by cut)
	for(E e:g->edges){
		N n = g->getNode(e->x,e->y);
		N adj = g->getNode(e->s,e->t);
		//if !cutUseEdge adj = n
		if(e->isWall)
			continue;
		constraints.push_back(getNodeReach(cutNumber,source,n) + S(" - ") +  getNodeReach(cutNumber,source,adj)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(0));
		constraints.push_back(getNodeReach(cutNumber,source,n) + S(" - ") +  getNodeReach(cutNumber,source,adj)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(0));
		constraints.push_back(getNodeReach(cutNumber,source,n) + S(" + ") +  getNodeReach(cutNumber,source,adj)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(1-M));
		constraints.push_back(getNodeReach(cutNumber,source,n) + S(" + ") +  getNodeReach(cutNumber,source,adj)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(1+M));
	}


	//source cannot reach sensor, sensor cannot reach source

	for(Edge* e:g->edges){
		addVar(pathEdgeFlow(cutNumber,e,source),4); // 4 means continue
		constraints.push_back(pathEdgeFlow(cutNumber,e,source) +S(" + ") + S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(M));
		constraints.push_back(pathEdgeFlow(cutNumber,e,source) +S(" - ") + S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(-M));
		addVar(pathEdgeFlow(cutNumber,e,exit),4); // 4 means continue
		constraints.push_back(pathEdgeFlow(cutNumber,e,exit) +S(" + ") + S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(M));
		constraints.push_back(pathEdgeFlow(cutNumber,e,exit) +S(" - ") + S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(-M));

		if(e->isWall){
			constraints.push_back(pathEdgeFlow(cutNumber,e,exit) + S(" = 0"));
		}

	}


	for(Node* n:g->nodes){

		addVar(conserVNode(cutNumber,n,source),5); //continues with bound 1
		addVar(superFlowNode(cutNumber,n,source),6); // conitnues with bound 10000
		//addVar(potentialNode(cutNumber,n),4);
		string flowOnNode = "";
		for(Edge* adjEdge:*n->getAdjEdges()){

			if(adjEdge->edgeOutOfNode(n)){
				flowOnNode += S(" - ") + pathEdgeFlow(cutNumber,adjEdge,source);
			}
			else{
				flowOnNode += S(" + ") + pathEdgeFlow(cutNumber, adjEdge,source);
			}
		}

		if(n != source){
			constraints.push_back(superFlowNode(cutNumber,n,source) + S(" = 0"));
		}


		constraints.push_back(flowOnNode +S(" + ") + superFlowNode(cutNumber,n,source) + S(" - ")+conserVNode(cutNumber,n,source) + S(" = 0") );



		//reach = conserver
		//constraints.push_back(conserVNode(cutNumber,n,source) + S(" - ") + getNodeReach(cutNumber,source,n) + S(" = 0"));

	}


	//cut 2 nodes differ with each other
	for(E e:g->edges){
		N n = g->getNode(e->x,e->y);
		N adj = g->getNode(e->s,e->t);
		//if cutUseEdge adj + n = 1
		constraints.push_back(conserVNode(cutNumber,n,source) + S(" + ") +  conserVNode(cutNumber,adj,source)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(1+M));
		constraints.push_back(conserVNode(cutNumber,n,source) + S(" + ") +  conserVNode(cutNumber,adj,source)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(1-M));
		//if !cutUseEdge adj = n
		constraints.push_back(conserVNode(cutNumber,n,source) + S(" - ") +  conserVNode(cutNumber,adj,source)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(0));
		constraints.push_back(conserVNode(cutNumber,n,source) + S(" - ") +  conserVNode(cutNumber,adj,source)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(0));
	}




	for(Node* n:g->nodes){

		addVar(conserVNode(cutNumber,n,exit),5); //continues with bound 1
		addVar(superFlowNode(cutNumber,n,exit),6); // conitnues with bound 10000
		//addVar(potentialNode(cutNumber,n),4);
		string flowOnNode = "";
		for(Edge* adjEdge:*n->getAdjEdges()){

			if(adjEdge->edgeOutOfNode(n)){
				flowOnNode += S(" - ") + pathEdgeFlow(cutNumber,adjEdge,exit);
			}
			else{
				flowOnNode += S(" + ") + pathEdgeFlow(cutNumber, adjEdge,exit);
			}
		}
		if(!algo::vecContains(g->exits,n)){
			constraints.push_back(superFlowNode(cutNumber,n,exit) + S(" = 0"));
		}


		constraints.push_back(flowOnNode +S(" + ") + superFlowNode(cutNumber,n,exit) + S(" - ")+conserVNode(cutNumber,n,exit) + S(" = 0") );

	}

	//cut 2 nodes differ with each other
	for(E e:g->edges){
		N n = g->getNode(e->x,e->y);
		N adj = g->getNode(e->s,e->t);

		//if cutUseEdge adj + n = 1
		constraints.push_back(conserVNode(cutNumber,n,exit) + S(" + ") +  conserVNode(cutNumber,adj,exit)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(1+M));
		constraints.push_back(conserVNode(cutNumber,n,exit) + S(" + ") +  conserVNode(cutNumber,adj,exit)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(1-M));
		//if !cutUseEdge adj = n
		constraints.push_back(conserVNode(cutNumber,n,exit) + S(" - ") +  conserVNode(cutNumber,adj,exit)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(0));
		constraints.push_back(conserVNode(cutNumber,n,exit) + S(" - ") +  conserVNode(cutNumber,adj,exit)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(0));
	}




}

void ILPgen::cutNoFlow(int cutNumber){
	//for a cut used edge, flow = 0

	Node* source = g->entrances.at(0);
	N exit = g->exits.at(0);

	// source = g->exits.at(0);
	for(Node* n:g->nodes){
		string nodeReach = getNodeReach(cutNumber,source,n);
		addVar(nodeReach,1);
		//pressure source
		if(algo::vecContains(g->entrances,n)){
			constraints.push_back(nodeReach + S(" = 1"));
			continue;
		}

		//pressure sensor
		if(algo::vecContains(g->exits,n)){
			constraints.push_back(nodeReach + S(" = 0"));
			continue;
		}



	}

	//Node reach = or( nodes around it and not by cut)
	for(E e:g->edges){
		N n = g->getNode(e->x,e->y);
		N adj = g->getNode(e->s,e->t);
		//if !cutUseEdge adj = n
		if(e->isWall)
			continue;
		constraints.push_back(getNodeReach(cutNumber,source,n) + S(" - ") +  getNodeReach(cutNumber,source,adj)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(0));
		constraints.push_back(getNodeReach(cutNumber,source,n) + S(" - ") +  getNodeReach(cutNumber,source,adj)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(0));
		constraints.push_back(getNodeReach(cutNumber,source,n) + S(" + ") +  getNodeReach(cutNumber,source,adj)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(1-M));
		constraints.push_back(getNodeReach(cutNumber,source,n) + S(" + ") +  getNodeReach(cutNumber,source,adj)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(1+M));
	}

	//source cannot reach sensor, sensor cannot reach source

	for(Edge* e:g->edges){
		addVar(pathEdgeFlow(cutNumber,e,source),4); // 4 means continue
		constraints.push_back(pathEdgeFlow(cutNumber,e,source) +S(" + ") + S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(M));
		constraints.push_back(pathEdgeFlow(cutNumber,e,source) +S(" - ") + S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(-M));
		addVar(pathEdgeFlow(cutNumber,e,exit),4); // 4 means continue
		constraints.push_back(pathEdgeFlow(cutNumber,e,exit) +S(" + ") + S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(M));
		constraints.push_back(pathEdgeFlow(cutNumber,e,exit) +S(" - ") + S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(-M));

		if(e->isWall){
			constraints.push_back(pathEdgeFlow(cutNumber,e,exit) + S(" = 0"));
		}

	}


	/*for(Node* n:g->nodes){

		addVar(conserVNode(cutNumber,n,source),5); //continues with bound 1
		addVar(superFlowNode(cutNumber,n,source),6); // conitnues with bound 10000
		//addVar(potentialNode(cutNumber,n),4);
		string flowOnNode = "";
		for(Edge* adjEdge:*n->getAdjEdges()){

			if(adjEdge->edgeOutOfNode(n)){
				flowOnNode += S(" - ") + pathEdgeFlow(cutNumber,adjEdge,source);
			}
			else{
				flowOnNode += S(" + ") + pathEdgeFlow(cutNumber, adjEdge,source);
			}
		}

		if(n != source){
			constraints.push_back(superFlowNode(cutNumber,n,source) + S(" = 0"));
		}


		constraints.push_back(flowOnNode +S(" + ") + superFlowNode(cutNumber,n,source) + S(" - ")+conserVNode(cutNumber,n,source) + S(" = 0") );



		//reach = conserver
		//constraints.push_back(conserVNode(cutNumber,n,source) + S(" - ") + getNodeReach(cutNumber,source,n) + S(" = 0"));

	}


	//cut 2 nodes differ with each other
	for(E e:g->edges){
		N n = g->getNode(e->x,e->y);
		N adj = g->getNode(e->s,e->t);
		//if cutUseEdge adj + n = 1
		constraints.push_back(conserVNode(cutNumber,n,source) + S(" + ") +  conserVNode(cutNumber,adj,source)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(1+M));
		constraints.push_back(conserVNode(cutNumber,n,source) + S(" + ") +  conserVNode(cutNumber,adj,source)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(1-M));
		//if !cutUseEdge adj = n
		constraints.push_back(conserVNode(cutNumber,n,source) + S(" - ") +  conserVNode(cutNumber,adj,source)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(0));
		constraints.push_back(conserVNode(cutNumber,n,source) + S(" - ") +  conserVNode(cutNumber,adj,source)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(0));
	}




	for(Node* n:g->nodes){

		addVar(conserVNode(cutNumber,n,exit),5); //continues with bound 1
		addVar(superFlowNode(cutNumber,n,exit),6); // conitnues with bound 10000
		//addVar(potentialNode(cutNumber,n),4);
		string flowOnNode = "";
		for(Edge* adjEdge:*n->getAdjEdges()){

			if(adjEdge->edgeOutOfNode(n)){
				flowOnNode += S(" - ") + pathEdgeFlow(cutNumber,adjEdge,exit);
			}
			else{
				flowOnNode += S(" + ") + pathEdgeFlow(cutNumber, adjEdge,exit);
			}
		}
		if(!algo::vecContains(g->exits,n)){
			constraints.push_back(superFlowNode(cutNumber,n,exit) + S(" = 0"));
		}


		constraints.push_back(flowOnNode +S(" + ") + superFlowNode(cutNumber,n,exit) + S(" - ")+conserVNode(cutNumber,n,exit) + S(" = 0") );

	}

	//cut 2 nodes differ with each other
	for(E e:g->edges){
		N n = g->getNode(e->x,e->y);
		N adj = g->getNode(e->s,e->t);

		//if cutUseEdge adj + n = 1
		constraints.push_back(conserVNode(cutNumber,n,exit) + S(" + ") +  conserVNode(cutNumber,adj,exit)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(1+M));
		constraints.push_back(conserVNode(cutNumber,n,exit) + S(" + ") +  conserVNode(cutNumber,adj,exit)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(1-M));
		//if !cutUseEdge adj = n
		constraints.push_back(conserVNode(cutNumber,n,exit) + S(" - ") +  conserVNode(cutNumber,adj,exit)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(0));
		constraints.push_back(conserVNode(cutNumber,n,exit) + S(" - ") +  conserVNode(cutNumber,adj,exit)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(0));
	}

*/


}

void ILPgen::cutKrish(int cutNumber){

	for(Edge* e:g->edges){
		string pUseEdge = pathUseEdge(cutNumber,e);
		addVar(pUseEdge,1);
		//if edge is hole, then cannot use this path
		if(e->isHole)
			constraints.push_back(pUseEdge + S(" = 0"));
	}


}

void ILPgen::cutGenralGraph(int cutNumber){


	for(Edge* e:g->edges){
		string pUseEdge = pathUseEdge(cutNumber,e);
		addVar(pUseEdge,1);
		//if edge is hole, then cannot use this path
		if(e->isHole)
			constraints.push_back(pUseEdge + S(" = 0"));
	}

	V<Cell*> cells = g->cells;
	//Cell* superCell = g->superCell;
	//cells.push_back(superCell);

	for(Cell* c:cells){
		string pUseCell = cutUseCell(cutNumber,c);
		addVar(pUseCell,1);
	}


	// source to waste, edge use once
	for(V<E> brink:g->sToWEdges){
		string edgeOnBrink = "";
		for(E e:brink){
			edgeOnBrink += S(" + ") + pathUseEdge(cutNumber, e);
		}
		constraints.push_back(edgeOnBrink + S(" = 1"));
	}

	// edge on wast to wast brink can only be used twice or zero
	for(V<E> brink:g->wToWEdges){
		string edgeOnBrink = "";
		for(E e:brink){
			edgeOnBrink += S(" + ") + pathUseEdge(cutNumber, e);
		}
		constraints.push_back(edgeOnBrink + S(" = 2"));
	}

	for(Cell* c:cells){
		string neibourEdges = "";
		for(Edge* neibourEdge: *c->adjEdges){

			neibourEdges += S(" + ") + pathUseEdge(cutNumber, neibourEdge);
		}
		//if one edge is used on the path, all 2 nodes on it are also on the path;



		//if(c == superCell){
			//constraints.push_back(neibourEdges+ S(" - ") +S(M) + S(" ") + cutUseCell(cutNumber,c) + S(" <= ") + S(0));
			//constraints.push_back(neibourEdges+ S(" + ") +S(M) + S(" ") + cutUseCell(cutNumber,c) + S(" >= ") + S(0));
			//must use 4 neibour edge
			//constraints.push_back(neibourEdges + S(" <= ") + S(2));
			//constraints.push_back(neibourEdges+  S(" >= ") + S(2));
		//	continue;
		//}

		//edge on sourc to waste brink can only be used less than 1

		constraints.push_back(neibourEdges+ S(" - ") +S(2) + S(" ") + cutUseCell(cutNumber,c) + S(" = ") + S(0));
	}
}


void ILPgen::path(int pathNumber){

	for(Node* n:g->nodes){
		string pUseNode = pathUseNode(pathNumber,n);
		addVar(pUseNode,1);
	}

	for(Edge* e:g->edges){
		string pUseEdge = pathUseEdge(pathNumber,e);
		addVar(pUseEdge,1);
	}
	//path starts from entrance
	string pUseOneEntrance = "";
	for(Node* entrance:g->entrances){
		pUseOneEntrance += s(" + ") + pathHeadUseNode(pathNumber,entrance);
		addVar(pathHeadUseNode(pathNumber,entrance),1);
	}
	pUseOneEntrance += s(" = 1");
	constraints.push_back(pUseOneEntrance);
	//path ends with exit
	string pUseOneExit= "";
	for(Node* exit:g->exits){
		pUseOneExit+= s(" + ") + pathTailUseNode(pathNumber,exit);
		addVar(pathTailUseNode(pathNumber,exit),1);
	}
	pUseOneExit+= s(" = 1");
	constraints.push_back(pUseOneExit);

	string onlyOneHead = "";
	//there is only one head
	for(Node* n:g->nodes){
		onlyOneHead += s(" + ") + pathHeadUseNode(pathNumber,n);
	}
	onlyOneHead += s(" = 1");
	constraints.push_back(onlyOneHead);
	//there is only one tail

	string onlyOneTail = "";
	for(Node* n:g->nodes){
		onlyOneTail += s(" + ") + pathTailUseNode(pathNumber,n);
	}
	onlyOneTail+= s(" = 1");
	constraints.push_back(onlyOneTail);



	//for a head or tail, adj edge if not wall then use one;
	for(Node* n:g->nodes){
		string headUseOneEdge = "";
		string tailUseOneEdge = "";
		for(Edge* e:*n->getAdjEdges()){
			if(e->isWall)
				continue;
			headUseOneEdge += S(" + ")+pathUseEdge(pathNumber,e);
			tailUseOneEdge += S(" + ")+pathUseEdge(pathNumber,e);
		}

		//if head use this node, headUseOndge = 1
		constraints.push_back(headUseOneEdge + S(" + ")+ S(M) + S(" ") + pathHeadUseNode(pathNumber,n) + S(" <= ") + S(M+1));
		constraints.push_back(headUseOneEdge + S(" - ")+ S(M) + S(" ") + pathHeadUseNode(pathNumber,n) + S(" >= ") + S(1-M));
		constraints.push_back(tailUseOneEdge + S(" + ")+ S(M) + S(" ") + pathTailUseNode(pathNumber,n) + S(" <= ") + S(M+1));
		constraints.push_back(tailUseOneEdge + S(" - ")+ S(M) + S(" ") + pathTailUseNode(pathNumber,n) + S(" >= ") + S(1-M));
	}

	/*//for 1 edge on the path, one of the neigbour path must be also on the path
	for(Edge* e:g->edges){
		string neibEdges = "";
		for(Edge* neigbEdge:e->adjEdges){
			if(neigbEdge->isWall)
				continue;
			neibEdges += S(" + ") + pathUseEdge(pathNumber,neigbEdge);
		}
		constraints.push_back(neibEdges+ S(" + ") +S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" <= ") + S(1+M));
		constraints.push_back(neibEdges+ S(" - ") +S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" >= ") + S(1-M));
	}*/

	//for one node on path, if not head or tail, 2 neigbour edge on path
	//if head or tail, 1 neigbour edge

	for(Node* n:g->nodes){
		string neibourEdges = "";
		for(Edge* neibourEdge: *n->getAdjEdges()){
			if(neibourEdge->isWall){
				continue;
			}
			neibourEdges += S(" + ") + pathUseEdge(pathNumber, neibourEdge);
		}
		constraints.push_back(neibourEdges+ S(" + ") +S(M) + S(" ") + pathUseNode(pathNumber,n) +S(" + ") + pathHeadUseNode(pathNumber,n)
				+S(" + ") + pathTailUseNode(pathNumber,n)+ S(" <= ") + S(2+M));
		constraints.push_back(neibourEdges+ S(" - ") +S(M) + S(" ") + pathUseNode(pathNumber,n) +S(" + ") + pathHeadUseNode(pathNumber,n)
						+S(" + ") + pathTailUseNode(pathNumber,n)+ S(" >= ") + S(2-M));
	}

	//if one edge is used on the path, all 2 nodes on it are also on the path;


	for(Edge* e:g->edges){
		if(e->isWall)
			constraints.push_back(pathUseEdge(pathNumber,e) + S(" = 0"));
		Node* n0 = g->getNode(e->x,e->y);
		Node* n1 = g->getNode(e->s,e->t);
		constraints.push_back(pathUseNode(pathNumber,n0) + S(" + ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" <= ") + S(1 + M));
		constraints.push_back(pathUseNode(pathNumber,n0) + S(" - ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" >= ") + S(1 - M));
		constraints.push_back(pathUseNode(pathNumber,n1) + S(" + ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" <= ") + S(1 + M));
		constraints.push_back(pathUseNode(pathNumber,n1) + S(" - ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" >= ") + S(1 - M));
	}




}


void ILPgen::tree(int treeNumber){


	for(Node* n:g->nodes){
		string pUseNode = pathUseNode(treeNumber,n);
		addVar(pUseNode,1);
	}

	for(Edge* e:g->edges){
		string pUseEdge = pathUseEdge(treeNumber,e);
		addVar(pUseEdge,1);
	}
	//path starts from entrance, not head the pathheaduse this node = 0
	string entranceCandidate = "";
	string noEntranceCandidate = "";
	for(Node* n:g->nodes){
		addVar(pathHeadUseNode(treeNumber,n),1);
		if(algo::vecContains(g->entrances,n)){
			entranceCandidate += s(" + ") + pathHeadUseNode(treeNumber,n);

		}
		else{
			noEntranceCandidate += s(" + ") + pathHeadUseNode(treeNumber,n);
		}
	}
	entranceCandidate += s(" = 1");
	noEntranceCandidate += s(" = 0");
	constraints.push_back(entranceCandidate);
	constraints.push_back(noEntranceCandidate);
	//path ends with exit
	string exitCandidate = "";
	string noExitCandidate = "";
	for(Node* n:g->nodes){
		addVar(pathTailUseNode(treeNumber,n),1);
		if(algo::vecContains(g->exits,n)){
			exitCandidate += s(" + ") + pathTailUseNode(treeNumber,n);

		}
		else{
			noExitCandidate += s(" + ") + pathTailUseNode(treeNumber,n);
		}
	}
	exitCandidate += s(" >= 1");
	noExitCandidate += s(" = 0");
	constraints.push_back(exitCandidate);
	constraints.push_back(noExitCandidate);

	//for a head or tail, adj edge if not wall then use one;
	for(Node* n:g->nodes){
		string headUseOneEdge = "";
		string tailUseOneEdge = "";
		for(Edge* e:*n->getAdjEdges()){
			if(e->isWall)
				continue;
			headUseOneEdge += S(" + ")+pathUseEdge(treeNumber,e);
			tailUseOneEdge += S(" + ")+pathUseEdge(treeNumber,e);
		}

		/*//if head use this node, headUseOndge = 1
		constraints.push_back(headUseOneEdge + S(" + ")+ S(M) + S(" ") + pathHeadUseNode(treeNumber,n) + S(" <= ") + S(M+1));
		constraints.push_back(headUseOneEdge + S(" - ")+ S(M) + S(" ") + pathHeadUseNode(treeNumber,n) + S(" >= ") + S(1-M));
		*///if tail use this node, headUseOndge = 1
		constraints.push_back(tailUseOneEdge + S(" + ")+ S(M) + S(" ") + pathTailUseNode(treeNumber,n) + S(" <= ") + S(M+1));
		constraints.push_back(tailUseOneEdge + S(" - ")+ S(M) + S(" ") + pathTailUseNode(treeNumber,n) + S(" >= ") + S(1-M));
	}

	//if one edge is used on the path, all 2 nodes on it are also on the path;

	for(Edge* e:g->edges){
		Node* n0 = g->getNode(e->x,e->y);
		Node* n1 = g->getNode(e->s,e->t);
		constraints.push_back(pathUseNode(treeNumber,n0) + S(" + ") + S(M) + S(" ") + pathUseEdge(treeNumber,e) + S(" <= ") + S(1 + M));
		constraints.push_back(pathUseNode(treeNumber,n0) + S(" - ") + S(M) + S(" ") + pathUseEdge(treeNumber,e) + S(" >= ") + S(1 - M));
		constraints.push_back(pathUseNode(treeNumber,n1) + S(" + ") + S(M) + S(" ") + pathUseEdge(treeNumber,e) + S(" <= ") + S(1 + M));
		constraints.push_back(pathUseNode(treeNumber,n1) + S(" - ") + S(M) + S(" ") + pathUseEdge(treeNumber,e) + S(" >= ") + S(1 - M));
	}

	//for one node on path, if not head or tail, more than 2 neigbour edge on path
		//if head or tail, 1 neigbour edge

	for(Node* n:g->nodes){
		string neibourEdges = "";
		for(Edge* neibourEdge: *n->getAdjEdges()){
			if(neibourEdge->isWall){
				continue;
			}
			neibourEdges += S(" + ") + pathUseEdge(treeNumber, neibourEdge);
		}

		constraints.push_back(neibourEdges+ S(" - ") +S(M) + S(" ") + pathUseNode(treeNumber,n) +S(" + ") + pathHeadUseNode(treeNumber,n)
						+S(" + ") + pathTailUseNode(treeNumber,n)+ S(" >= ") + S(2-M));
	}

	//tree without loops
	//nodesUsed - edgesUsed = 1;

	string nodesUsed ="";
	for(Node* n:g->nodes){
		nodesUsed +=  S(" + ") +pathUseNode(treeNumber,n);
	}

	string edgeUsed = "";
	for(Edge* e:g->edges){
		edgeUsed += S(" - ") + pathUseEdge(treeNumber,e);
	}

	constraints.push_back(nodesUsed + edgeUsed + S(" = 1"));
}

void ILPgen::pathLoopPunishMent(int pathTotalNumber){
	obj.push_back("max");
	string totalConserve = "";
	for(int pathNumber = 0; pathNumber <pathTotalNumber; pathNumber++){



	//for a not used edge, flow = 0
		for(Edge* e:g->edges){
			addVar(pathEdgeFlow(pathNumber,e),6); // 4 means continue
			constraints.push_back(pathEdgeFlow(pathNumber,e) + S(" - ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" <= 0"));
			constraints.push_back(pathEdgeFlow(pathNumber,e) + S(" + ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" >= 0"));
		}

		for(Node* n:g->nodes){

			addVar(conserVNode(pathNumber,n),5); //continues with bound 1
			addVar(superFlowNode(pathNumber,n),6); // conitnues with bound 10000
			//addVar(potentialNode(pathNumber,n),4);
			string flowOnNode = "";
			for(Edge* adjEdge:*n->getAdjEdges()){
				if(adjEdge->isWall)
					continue;
				if(adjEdge->edgeOutOfNode(n)){
					flowOnNode += S(" - ") + pathEdgeFlow(pathNumber,adjEdge);
				}
				else{
					flowOnNode += S(" + ") + pathEdgeFlow(pathNumber, adjEdge);
				}
			}

			//if pathHeadUseThis do not use this Node, superFlow = 0
			constraints.push_back(superFlowNode(pathNumber,n) + S(" + ") +  S(M) + S(" ") + pathHeadUseNode(pathNumber,n) + S(" >= 0"));
			constraints.push_back(superFlowNode(pathNumber,n) + S(" - ") +  S(M) + S(" ") + pathHeadUseNode(pathNumber,n) + S(" <= 0"));


			constraints.push_back(flowOnNode +S(" + ") + superFlowNode(pathNumber,n) + S(" - ")+conserVNode(pathNumber,n) + S(" = 0") );



			totalConserve+= S(" + ") + S(PUNISHMENT_WEIGHT) + S(" ")+conserVNode(pathNumber,n) + S(" - ")+ S(PUNISHMENT_WEIGHT) + S(" ") + pathUseNode(pathNumber,n);
		}


	}

	obj.push_back(totalConserve);


}


void ILPgen::noLoop(int pathNumber){

	//for a not used edge, flow = 0
		for(Edge* e:g->edges){
			addVar(pathEdgeFlow(pathNumber,e),4); // 4 means continue
			constraints.push_back(pathEdgeFlow(pathNumber,e) + S(" - ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" <= 0"));
			constraints.push_back(pathEdgeFlow(pathNumber,e) + S(" + ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" >= 0"));
		}
	//for a node if node is used and node is not head
	//then then in flow = out flow + 1
	for(Node* n:g->nodes){
		string flowOnNode = "";
		for(Edge* adjEdge:*n->getAdjEdges()){
			if(adjEdge->isWall)
				continue;
			if(adjEdge->edgeOutOfNode(n)){
				flowOnNode += S(" - ") + pathEdgeFlow(pathNumber,adjEdge);
			}
			else{
				flowOnNode += S(" + ") + pathEdgeFlow(pathNumber, adjEdge);
			}
		}
		constraints.push_back(flowOnNode + S(" + ") + S(M) + S(" ") + pathUseNode(pathNumber,n) + S(" - ") +S(M)+S(" ") + pathHeadUseNode(pathNumber,n)
				+ S(" <= ") + S(1 + M));
		constraints.push_back(flowOnNode + S(" - ") + S(M) + S(" ") + pathUseNode(pathNumber,n) + S(" + ") +S(M)+S(" ") + pathHeadUseNode(pathNumber,n)
				 + S(" >= ") + S(1 - M));
	}

}


void ILPgen::pathValveSwitchTogether(int pathNumber, V<V<E>> paths, const char* fileName){
	for(int p = 0; p<pathNumber;p++){
		path(p);
#if !LAGLANGR_RELAXATION
		noLoop(p);
#endif
	}

#if LAGLANGR_RELAXATION
	pathLoopPunishMent(pathNumber);
#endif

	valveSwitchTogeter(pathNumber,paths);
	writeInToILPfile(fileName);

}


void ILPgen::treeValveSwitchTogether(int pathNumber, V<V<E>> paths, const char* fileName){
	for(int p = 0; p<pathNumber;p++){
		tree(p);
#if !LAGLANGR_RELAXATION
		noLoop(p);
#endif
	}

#if LAGLANGR_RELAXATION
	pathLoopPunishMent(pathNumber);
#endif

	valveSwitchTogeter(pathNumber,paths);
	writeInToILPfile(fileName);

}



void ILPgen::valveSwitchTogeterGeneralG(int pathNumber){
	for(int i = 0; i <g->edgePair1.size();i++){
		E e0 = g->edgePair1.at(i);
		E e1 = g->edgePair2.at(i);
		string bothEdgeUsed = "";
		int  howManyEdgeTime = 0;
		for(int p = 0; p < pathNumber;p++){
			bothEdgeUsed +=S(" + ")+ pathUseEdge(p,e0) + S(" + ")+ pathUseEdge(p,e0);
			howManyEdgeTime += 2;
		}

		constraints.push_back(bothEdgeUsed + S(" >= 1"));
		constraints.push_back(bothEdgeUsed + S(" <= ") + S(howManyEdgeTime-1));
	}
}



void ILPgen::valveSwitchTogeter(int pathTotalNum,V<V<E>> paths){
	//more edge pair used the better


/*	for(int i = 0; i <g->edgePair1.size();i++){
		E e0 = g->edgePair1.at(i);
		E e1 = g->edgePair2.at(i);
		string bothEdgeUsed = "";
		int  howManyEdgeTime = 0;
		for(int p = 0; p < pathNumber;p++){
			bothEdgeUsed +=S(" + ")+ pathUseEdge(p,e0) + S(" + ")+ pathUseEdge(p,e0);
			howManyEdgeTime += 2;
		}

		constraints.push_back(bothEdgeUsed + S(" >= 1"));
		constraints.push_back(bothEdgeUsed + S(" <= ") + S(howManyEdgeTime-1));
	}*/

	//edge pair

	// most edgePair target
	for(int pathId = 0; pathId < pathTotalNum; pathId++){

		string moreEdgePair = "";
		for(int i = 0; i <g->edgePair1.size();i++){
			E e0 = g->edgePair1.at(i);
			E e1 = g->edgePair2.at(i);

			string edgePairSep = edgePairSeperate(pathId,e0,e1);
			addVar(edgePairSep,1);
			constraints.push_back(edgePairSep + S(" - ") + pathUseEdge(pathId,e0) + S(" - ") + pathUseEdge(pathId,e1) + S(" <= 0"));
			constraints.push_back(edgePairSep + S(" + ") + pathUseEdge(pathId,e0) + S(" + ") + pathUseEdge(pathId,e1) + S(" <= 2"));
			constraints.push_back(edgePairSep + S(" + ") + pathUseEdge(pathId,e0) + S(" - ") + pathUseEdge(pathId,e1) + S(" >= 0"));
			constraints.push_back(edgePairSep + S(" - ") + pathUseEdge(pathId,e0) + S(" + ") + pathUseEdge(pathId,e1) + S(" >= 0"));
			moreEdgePair += S(" + ") + edgePairSep;
		}
		string localObj = obj.back();
		obj.pop_back();
		localObj += moreEdgePair;
		obj.push_back(localObj);
	}




}


void ILPgen::pathNoLoop(int pathNumber){
	path(pathNumber);
	//path has no circle

	//for a node not head or tail
	//flowIn = flowOut + 1


//no langlare relaxion
#if	!LAGLANGR_RELAXATION
	//for a not used edge, flow = 0
		for(Edge* e:g->edges){
			addVar(pathEdgeFlow(pathNumber,e),4); // 4 means continue
			constraints.push_back(pathEdgeFlow(pathNumber,e) + S(" - ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" <= 0"));
			constraints.push_back(pathEdgeFlow(pathNumber,e) + S(" + ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" >= 0"));
		}
	//for a node if node is used and node is not head
	//then then in flow = out flow + 1
	for(Node* n:g->nodes){
		string flowOnNode = "";
		for(Edge* adjEdge:*n->getAdjEdges()){
			if(adjEdge->isWall)
				continue;
			if(adjEdge->edgeOutOfNode(n)){
				flowOnNode += S(" - ") + pathEdgeFlow(pathNumber,adjEdge);
			}
			else{
				flowOnNode += S(" + ") + pathEdgeFlow(pathNumber, adjEdge);
			}
		}
		constraints.push_back(flowOnNode + S(" + ") + S(M) + S(" ") + pathUseNode(pathNumber,n) + S(" - ") +S(M)+S(" ") + pathHeadUseNode(pathNumber,n)
				+ S(" - ") + S(M) + S(" ") + pathTailUseNode(pathNumber,n) + S(" <= ") + S(1 + M));
		constraints.push_back(flowOnNode + S(" - ") + S(M) + S(" ") + pathUseNode(pathNumber,n) + S(" + ") +S(M)+S(" ") + pathHeadUseNode(pathNumber,n)
				+ S(" + ") + S(M) + S(" ") + pathTailUseNode(pathNumber,n) + S(" >= ") + S(1 - M));
	}
#endif

}

void ILPgen::oldEdgePunish(V<E> usedEdges, int pathTotalNumber){
	cout<< "usedEdges.size is " << usedEdges.size()<<endl;
	string useOneOldEdge = "useOneOldEdge";
	addVar(useOneOldEdge,1);
	string allUsedEdges = "";
	string punish = obj.back();
	obj.pop_back();
	for(int path = 0 ; path < pathTotalNumber; path++){



		for(E e:g->edges){

			if(!algo::vecContains(usedEdges,e))
				continue;
			allUsedEdges += S(" - ") + S(" ")+pathUseEdge(path,e);
			//cout <<"reward is" << reward << endl;
			constraints.push_back(useOneOldEdge + S(" - ") + pathUseEdge(path,e) + S(" >= 0"));
		}
		constraints.push_back(useOneOldEdge + allUsedEdges + S(" <= 0"));
	}
 	punish += S(" + ") + S(M) +S(" ") + useOneOldEdge;
	obj.push_back(punish);
 }

void ILPgen::newEdgeReward(V<E> usedEdges,int pathTotalNumber, V<E> edgeToCover){
	cout<< "usedEdges.size is " << usedEdges.size()<<endl;
		string reward = obj.back();
		obj.pop_back();

		for(E e:edgeToCover){
			if(algo::vecContains(usedEdges,e))
				continue;
			string edgeUsed = e->name + S("used");
			addVar(edgeUsed,1);
			string allPathUseEdge = "";
			for(int path = 0 ; path < pathTotalNumber; path++){
				constraints.push_back(edgeUsed + S(" - ") + pathUseEdge(path,e) + S(" >= 0"));
				allPathUseEdge += S(" - ") + pathUseEdge(path,e);
			}
			constraints.push_back(edgeUsed + allPathUseEdge + S(" <= 0"));
			reward += S(" + ") + edgeUsed;

		}
		obj.push_back(reward);
}

void ILPgen::cutSeveralEdgesNoLoop(int pathNumber,const char* fileName, V<E> edgesToCover,V<E> usedEdges){

	for(int i = 0; i <= pathNumber-1; i++){
		//cut(i);
		path(i);
#if !LAGLANGR_RELAXATION
		noLoop(i);
#endif
	}

	//punishment for loops
#if LAGLANGR_RELAXATION
	pathLoopPunishMent(pathNumber);

	cout << "im here" << endl;
	newEdgeReward(usedEdges,pathNumber,edgesToCover);

#endif
/*	obj.push_back("max");
	obj.push_back(" ");*/
	//path cover all the edges
	//coverEdge(edgesToCover, pathNumber);
	//minimumEdgeUse(pathNumber);
	writeInToILPfile(fileName);
}

void ILPgen::setILPTime(float time){
	ilp.timeMax = time;
}
void ILPgen::setILPGap(float gap){
	ilp.ilpGap = gap;
}


void ILPgen::ilpRun(const char* ilpFileName){
	varValue = ilp.ILP_solve(ilpFileName);
}
void ILPgen::ilpRun_pool(const char* ilpFileName){
	allVarValues = ilp.ILP_solve_pool(ilpFileName);
}
vector<Edge*> ILPgen::getPath(int pathNumber, int solutionNumber){
	vector<Edge*> path;
	for(Edge*e:g->edges){
		string pUe = pathUseEdge(pathNumber,e);
		//+0.5 make it round
		map<string,float> varResult = allVarValues[solutionNumber];
		int ilpValuePue = varResult[pUe] + 0.2;

		if(ilpValuePue == 1)
			path.push_back(e);
	}
/*
#if !LAGLANGR_RELAXATION
	g->sortEdgesInPath(&path);
#endif
*/
	return path;

}

vector<Edge*> ILPgen::getPath(int pathNumber){
	vector<Edge*> path;
	for(Edge*e:g->edges){
		string pUe = pathUseEdge(pathNumber,e);
		//+0.5 make it round
		int ilpValuePue = varValue[pUe] + 0.5;

		if(ilpValuePue == 1)
			path.push_back(e);
	}
/*
#if !LAGLANGR_RELAXATION
	g->sortEdgesInPath(&path);
#endif
*/
	return path;

}




void ILPgen::pathCoverAllWishLuck(const char* fileName, int pathNumber){
	obj.push_back("max");
	for(int i = 0; i <= pathNumber-1; i++){
			pathWishMeLuck(i);
			string useALLedge ="";
				for(E e:g->edges){
					useALLedge += S(" + ") + pathUseEdge(i,e);
				}
				obj.push_back(useALLedge);
	}

	//path cover all the edges
	//coverEdge(g->edges, pathNumber);

	writeInToILPfile(fileName);
}


void ILPgen::pathWishMeLuck(int pathId){
	for(Node* n:g->nodes){
		string pUseNode = pathUseNode(pathId,n);
		addVar(pUseNode,1);
		string nodeTimeStamp = getNodeTimestamp(pathId,n);
		//integer
		addVar(nodeTimeStamp,0);

		for(E adjEdge : *n->getAdjEdges()){

		}
	}

	for(Edge* e:g->edges){
		string pUseEdge = pathUseEdge(pathId,e);
		addVar(pUseEdge,1);

		N n0 = g->getNode(e->x,e->y);
		N n1 = g->getNode(e->s,e->t);


		string edgeOutN0Var = edgeOutN(pathId,e,n0);
		string edgeOutN1Var = edgeOutN(pathId,e,n1);
		string edgeInN0Var = edgeInN(pathId,e,n0);
		string edgeInN1Var = edgeInN(pathId,e,n1);

		addVar(edgeOutN0Var,1);
		addVar(edgeOutN1Var,1);
		addVar(edgeInN0Var,1);
		addVar(edgeInN1Var,1);
	}

	N head = g->entrances.at(0);
	N tail = g->exits.at(0);

	//all node except head or tail must have equal number of in edge and out edge
	for(N n:g->nodes){
			string allOutEdge = "";
			string allInEdge = "";
		if(n == head){

			for(E adjE:*n->getAdjEdges()){
				allOutEdge  += S(" + ") + edgeOutN(pathId,adjE,n);
			}

			constraints.push_back(allOutEdge + S(" = 1"));
			continue;
		}

		if(n == tail){

			for(E adjE:*n->getAdjEdges()){
				allInEdge  += S(" + ") + edgeInN(pathId,adjE,n);
			}

			constraints.push_back(allInEdge + S(" = 1"));
			continue;
		}


		//regular node, in = out

		for(E adjE: *n->getAdjEdges()){
			allOutEdge  += S(" + ") + edgeOutN(pathId,adjE,n);
			allInEdge  += S(" - ") + edgeInN(pathId,adjE,n);
		}

		constraints.push_back(allOutEdge + allInEdge + S(" = 0"));

	}

	for(E e:g->edges){
	
		N n0 = g->getNode(e->x,e->y);
		N n1 = g->getNode(e->s,e->t);
		//in n0 equals to out n1, same otherwise
		constraints.push_back(edgeInN(pathId,e,n0)  +S(" - ")+edgeOutN(pathId,e,n1) + S(" = 0"));	
		constraints.push_back(edgeInN(pathId,e,n1)  +S(" - ")+edgeOutN(pathId,e,n0) + S(" = 0"));	
	//for each node, only in or our or nothing
		constraints.push_back(edgeInN(pathId,e,n0)  +S(" + ")+edgeOutN(pathId,e,n0) + S(" <= 1"));	
		constraints.push_back(edgeInN(pathId,e,n1)  +S(" + ")+edgeOutN(pathId,e,n1) + S(" <= 1"));

		//if edge out n0 to n1, then time n0 > time n1
		//if edge out n1 to n0, then time n1 > time n0

		constraints.push_back(getNodeTimestamp(pathId,n0) + S(" - ") + getNodeTimestamp(pathId,n1) + S(" - ") + S(M) + S(" ") + edgeOutN(pathId,e,n0) + S(" >= ") + S(1-M));
		constraints.push_back(getNodeTimestamp(pathId,n1) + S(" - ") + getNodeTimestamp(pathId,n0) + S(" - ") + S(M) + S(" ") + edgeInN(pathId,e,n0) + S(" >= ") + S(1-M));

		//if edgeOutN or edgeInN 
		constraints.push_back(pathUseEdge(pathId,e) + S(" - ") + edgeInN(pathId,e,n0) + S(" >= 0"));
		constraints.push_back(pathUseEdge(pathId,e) + S(" - ") + edgeOutN(pathId,e,n0) + S(" >= 0"));
		constraints.push_back(pathUseEdge(pathId,e) + S(" - ") + edgeInN(pathId,e,n0) + S(" - ") + edgeOutN(pathId,e,n0) + S(" <= 0"));
	}

}


void ILPgen::pathCoverAsManyEdges(int pathNum,const char * ilpFileName,V<E> edgeToCover,V<E> usedEdges){
	for(int pathId = 0; pathId < pathNum; pathId++){
		path(pathId);

	}
	pathLoopPunishMent(pathNum);
	newEdgeReward(usedEdges,pathNum,edgeToCover);
	writeInToILPfile(ilpFileName);

}

void ILPgen::treeCoverAsManyEdges(int pathNum,const char * ilpFileName,V<E> edgeToCover,V<E> usedEdges){
	for(int pathId = 0; pathId < pathNum; pathId++){
		tree(pathId);

	}
	pathLoopPunishMent(pathNum);
	newEdgeReward(usedEdges,pathNum,edgeToCover);
	writeInToILPfile(ilpFileName);

}



void ILPgen::pathThrougEdge(E edgeToPass, const char* ilpFile){
	path(0);
	noLoop(0);
	V<E> edgesToCover;
	edgesToCover.push_back(edgeToPass);
	coverEdge(edgesToCover,1);
	writeInToILPfile(ilpFile);


}



void ILPgen::pathLeastResource(int pathTotalNumber,const char* fileName,V<E> edgeToCover, V<E> resourceEdges){
	for(int pathId = 0; pathId< pathTotalNumber; pathId++){
		path(pathId);
		noLoop(pathId);
	}

	coverEdge(edgeToCover,pathTotalNumber);

	leastResource(pathTotalNumber,resourceEdges);
	writeInToILPfile(fileName);
}


void ILPgen::dfsPaths(int pathTotalNumber,const char* fileName,V<E> edgeToCover, V<E> resourceEdges){
	g->sortEdgesAndNodes();
	for(int pathId = 0; pathId< pathTotalNumber; pathId++){
		path(pathId);
		noLoop(pathId);
	}



	coverEdge(edgeToCover,pathTotalNumber);
	//pressureSouceNumberConstraint(pathTotalNumber,resourceEdges);
	//leastResource(pathTotalNumber,resourceEdges);
	maxNonResource(pathTotalNumber,resourceEdges);
	writeInToILPfile(fileName);
}

 void ILPgen::coverAsManyEdgeAsPossible(V<E>& edgeToCover){
	 obj.push_back("max");
	 string edgesUsed = "";
	 for(E e:edgeToCover){
		 edgesUsed += S(" + ")+pathUseEdge(0,e);
	 }
	 obj.push_back(edgesUsed);
 }

void ILPgen::dfsCutsMostEdgeToCover(const char* fileName,V<E> edgeToCover){

	cutKrish(0);
	cutNoFlowKrish(0);

	//cover as many edge as possible
	coverAsManyEdgeAsPossible(edgeToCover);
	writeInToILPfile(fileName);

}



void ILPgen::dfsCuts(int cutTotalNumber,const char* fileName,V<E> edgeToCover, V<E> resourceEdges){
	for(int cutId = 0; cutId< cutTotalNumber; cutId++){
		cutKrish(cutId);
		cutNoFlowKrish(cutId);
	}



	coverEdge(edgeToCover,cutTotalNumber);
	//pressureSouceNumberConstraint(pathTotalNumber,resourceEdges);
	//leastResource(pathTotalNumber,resourceEdges);
	writeInToILPfile(fileName);
}




void ILPgen::dfsPathCutValveSharing(int pathTotalNumber,int cutTotalNumber, const char* fileName,V<E>& edgeToCover, V<E>& resourceEdges){
	int pathIndex = 0;
	if(pathTotalNumber > 0){

		for(int cutId = 0; cutId< pathTotalNumber; cutId++){
			cutKrish(cutId);
			cutNoFlowKrish(cutId);
		}
		coverEdge(edgeToCover,pathTotalNumber);
		//pressureSouceNumberConstraint(pathTotalNumber,resourceEdges);
	}



	//leastResource(pathTotalNumber+cutTotalNumber,resourceEdges);
	writeInToILPfile(fileName);
}

void ILPgen::pressureSouceCutShare(int cutTotalNumber, int cutIndexBase, V<E>& testVales){
	for(E e:testVales){
		for(E appE:g->edges){
			if(algo::vecContains(testVales,appE))
				continue;

			string v0SV1PS = v0ShareV1PS(e,appE);
			addVar(v0SV1PS,1);

			for(int cutNum = cutIndexBase; cutNum < cutTotalNumber; cutNum++){
				constraints.push_back(pathUseEdge(cutNum,e) + S(" + ") + pathUseEdge(cutNum,appE) + S(" + ") + S(M) + S(" ") + v0SV1PS + S(" <= ") + S(M + 1));
				constraints.push_back(pathUseEdge(cutNum,e) + S(" + ") + pathUseEdge(cutNum,appE) + S(" - ") + S(M) + S(" ") + v0SV1PS + S(" >= ") + S(-M + 1));
			}
		}

	}



	for(E e:testVales){
		string s = "";
		for(E appE:g->edges){
			if(algo::vecContains(testVales,appE))
				continue;

			string v0SV1PS = v0ShareV1PS(e,appE);

			 s += S(" + ") + v0SV1PS;
		}
		//each testValve must share with at least one
		for(int vectorId = cutIndexBase; vectorId < cutTotalNumber; vectorId++){
			constraints.push_back(s + S(" - ") + pathUseEdge(vectorId,e) + S( " >= 0"));
		}
	}
}


//test vavles must be united with one of the application valves
//app vectors contains which valves are open in a vector
void ILPgen::pressureSouceNumberConstraint(int pathTotalNumber, V<E>& testVales){
	for(E e:testVales){
		for(E appE:g->edges){
			if(algo::vecContains(testVales,appE))
				continue;

			string v0SV1PS = v0ShareV1PS(e,appE);
			addVar(v0SV1PS,1);

			for(int pathNum = 0; pathNum < pathTotalNumber; pathNum++){
				constraints.push_back(pathUseEdge(pathNum,e) + S(" - ") + pathUseEdge(pathNum,appE) + S(" + ") + S(M) + S(" ") + v0SV1PS + S(" <= ") + S(M));
				constraints.push_back(pathUseEdge(pathNum,e) + S(" - ") + pathUseEdge(pathNum,appE) + S(" - ") + S(M) + S(" ") + v0SV1PS + S(" >= ") + S(-M));
			}
		}

	}



	for(E e:testVales){
		string s = "";
		for(E appE:g->edges){
			if(algo::vecContains(testVales,appE))
				continue;

			string v0SV1PS = v0ShareV1PS(e,appE);

			 s += S(" + ") + v0SV1PS;
		}
		//each testValve must share with at least one
		for(int vectorId = 0; vectorId < pathTotalNumber; vectorId++){
			constraints.push_back(s + S(" - ") + pathUseEdge(vectorId,e) + S( " >= 0"));
		}
	}

}

void ILPgen::pathVectorNoLeakApp(int pathTotalNumber, V<V<E>> appPaths,V<V<E>> mustCloseVales, V<E>& testValves){

	for(int i = 0; i < appPaths.size();i++){
		V<E> appPath =  appPaths[i];
		V<E> mustCloseVavle = mustCloseVales[i];
		for(E testVavle:testValves){
			for(E appE:appPath){
				string v0SV1PS = v0ShareV1PS(testVavle,appE);
				if(algo::vecContains(mustCloseVavle,testVavle))
					constraints.push_back(v0SV1PS + S(" = 0"));
			}

		}
	}
}


string useEdgeOnce(E e){
	return e->name + S("used");
}
void ILPgen::leastResource(int pathTotalNumber, V<E> resourceEdges){
	obj.push_back("min");


	string allResourceEdges = "";
	string useResouceEdge = "";
	for(E e:resourceEdges){
		addVar(useEdgeOnce(e),1);
		useResouceEdge += S(" + ") + useEdgeOnce(e);
		for(int pathId = 0; pathId < pathTotalNumber; pathId++){
			constraints.push_back(useEdgeOnce(e) + S(" - ") + pathUseEdge(pathId,e) + S(" >= 0"));
		}
	}

	obj.push_back(useResouceEdge);

}

string notUseEdgeOnce(E e){
	return e->name + S("Nused");
}

void ILPgen::maxNonResource(int pathTotalNumber, V<E> resourceEdges){
	//obj.push_back("max");


	string notUseResouceEdge = "";
	for(E e:resourceEdges){
		addVar(notUseEdgeOnce(e),1);

		notUseResouceEdge += S(" + ") + notUseEdgeOnce(e);
		for(int pathId = 0; pathId < pathTotalNumber; pathId++){
			constraints.push_back(notUseEdgeOnce(e) + S(" + ") + pathUseEdge(pathId,e) + S(" <= 1"));
		}
	}

	//obj.push_back(notUseResouceEdge);
	constraints.push_back(notUseResouceEdge + S(" = ") +  S(resourceEdges.size() - 6));

}


































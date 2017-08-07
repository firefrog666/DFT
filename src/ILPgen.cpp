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
#define CUTGAP 3
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


void ILPgen::coverEdge(vector<Edge*>& edgesToCover, int pathNumber){
	for(Edge* e:edgesToCover){
		string edgeUsedOnce = "";
		for(int i = 0; i <= pathNumber-1; i ++){
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

void ILPgen::pathToCoverAllEdges(const char* fileName, int pathNumber){

	for(int i = 0; i <= pathNumber-1; i++){
		path(i);
	}

	//path cover all the edges

	coverEdge(g->edges,pathNumber);

	minimumEdgeUse(pathNumber);
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


void ILPgen::cutGeneralCoverSeveralEdges(int cutNumber,const char* fileName,V<E> edgeToCover, V<E> usedEdges){
	for(int i = 0; i <cutNumber;i++){
		cutGenralGraph(i);
		cutNoFlow(i);
	}

//	obj.push_back("Max");
//	obj.push_back(" ");
	//newEdgeReward(usedEdges,cutNumber);
	cutPunishMent(cutNumber);
	//coverEdge(edgeToCover,cutNumber);

	writeInToILPfile(fileName);
}

void ILPgen::cutPunishMent(int cutId){
	obj.push_back("min");
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
	Cell* superCell = g->getSuperCell(cells);
	cells.push_back(superCell);

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

	constraints.push_back(onlyOneHead + S(" = 1"));
	//for a not used edge, flow = 0
	for(Edge* e:g->edges){
		addVar(pathEdgeFlow(cutId,e),6); // 4 means continue
		constraints.push_back(pathEdgeFlow(cutId,e) + S(" - ") + S(M) + S(" ") + pathUseEdge(cutId,e) + S(" <= 0"));
		constraints.push_back(pathEdgeFlow(cutId,e) + S(" + ") + S(M) + S(" ") + pathUseEdge(cutId,e) + S(" >= 0"));
	}

	for(Cell* c:cells){

		addVar(conserVCell(cutId,c),5); //continues with bound 1
		addVar(superFlowCell(cutId,c),6); // conitnues with bound 10000
		//addVar(potentialNode(pathNumber,n),4);
		string flowOnCell = "";
		if(c == superCell){
			for(Edge* e:*c->adjEdges){

				constraints.push_back(pathEdgeFlow(cutId,e) + S(" = 0"));

			}
			continue;
		}

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


		//where a cell on cut but no flow to it
		totalPunishment+=  S(" + ") + cutUseCell(cutId,c)  + S(" - ")+ conserVCell(cutId,c);
	}



	obj.push_back(totalPunishment);


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
		constraints.push_back(getNodeReach(cutNumber,source,n) + S(" - ") +  getNodeReach(cutNumber,source,adj)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(0));
		constraints.push_back(getNodeReach(cutNumber,source,n) + S(" - ") +  getNodeReach(cutNumber,source,adj)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(0));
		constraints.push_back(getNodeReach(cutNumber,source,n) + S(" + ") +  getNodeReach(cutNumber,source,adj)+ S(" - ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(1-M));
		constraints.push_back(getNodeReach(cutNumber,source,n) + S(" + ") +  getNodeReach(cutNumber,source,adj)+ S(" + ") +S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(1+M));
	}

/*	for(Edge* e:g->edges){
		addVar(pathEdgeFlow(cutNumber,e,source),4); // 4 means continue
		constraints.push_back(pathEdgeFlow(cutNumber,e,source) +S(" + ") + S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(M));
		constraints.push_back(pathEdgeFlow(cutNumber,e,source) +S(" - ") + S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(-M));
	}
	for(Edge* e:g->edges){
		addVar(pathEdgeFlow(cutNumber,e,exit),4); // 4 means continue
		constraints.push_back(pathEdgeFlow(cutNumber,e,exit) +S(" + ") + S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" <= ") + S(M));
		constraints.push_back(pathEdgeFlow(cutNumber,e,exit) +S(" - ") + S(M) + S(" ") + pathUseEdge(cutNumber,e) + S(" >= ") + S(-M));
	}

	for(Node* n:g->nodes){

		addVar(conserVNode(cutNumber,n,source),5); //continues with bound 1
		addVar(superFlowNode(cutNumber,n,source),6); // conitnues with bound 10000
		//addVar(potentialNode(cutNumber,n),4);
		string flowOnNode = "";
		for(Edge* adjEdge:*n->getAdjEdges()){
			if(adjEdge->isWall)
				continue;
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
			if(adjEdge->isWall)
				continue;
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
	}*/





}

void ILPgen::cutGenralGraph(int cutNumber){



	V<Cell*> cells = g->cells;
	Cell* superCell = g->getSuperCell(cells);
	cells.push_back(superCell);

	for(Cell* c:cells){
		string pUseCell = cutUseCell(cutNumber,c);
		addVar(pUseCell,1);
	}


	for(Edge* e:g->edges){
		string pUseEdge = pathUseEdge(cutNumber,e);
		addVar(pUseEdge,1);
	}



	for(Cell* c:cells){
		string neibourEdges = "";
		for(Edge* neibourEdge: *c->adjEdges){
			if(neibourEdge->isHole){
				continue;
			}
		//	cout<< "e hash value = " << neibourEdge->hashValue<<endl;
			if(neibourEdge->hashValue == 10101){
				neibourEdges += S(" + ") + S("2 ") +pathUseEdge(cutNumber, neibourEdge);
				continue;
			}

			neibourEdges += S(" + ") + pathUseEdge(cutNumber, neibourEdge);
		}
		//if one edge is used on the path, all 2 nodes on it are also on the path;
		/*if(c == superCell)
			continue;*/
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



			totalConserve+= S(" + ") + conserVNode(pathNumber,n);
		}


	}

	obj.push_back(totalConserve);


}


void ILPgen::noLoop(int pathNumber){
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
				+ S(" <= ") + S(1 + M));
		constraints.push_back(flowOnNode + S(" - ") + S(M) + S(" ") + pathUseNode(pathNumber,n) + S(" + ") +S(M)+S(" ") + pathHeadUseNode(pathNumber,n)
				 + S(" >= ") + S(1 - M));
	}
#endif
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



void ILPgen::valveSwitchTogeter(int pathNumber,V<V<E>> paths){
	V<E> edgePair1;
	V<E> edgePair2;

	for(V<E> p:paths){
		for(int i = 0;i<p.size()-1;i++){
			E e0 = p.at(i);
			E e1 = p.at(i+1);

			bool bareBone =  false;
			for(V<E> pOther:paths){
				if(p == pOther)
					continue;

				if(algo::vecContains(pOther,e0) && !algo::vecContains(pOther,e1)){
					bareBone = true;
					break;
				}
				if(!algo::vecContains(pOther,e0) && algo::vecContains(pOther,e1)){
					bareBone = true;
					break;
				}

			}

			if(bareBone)
				continue;


			bool alreadyin = false;
			for(int j = 0;j<edgePair1.size();j++){
				E pE0  = edgePair1.at(j);
				E pE1 = edgePair2.at(j);
				if((Edge::sameEdge(pE0,e0) && Edge::sameEdge(pE1,e1))
					||(Edge::sameEdge(pE1,e0) && Edge::sameEdge(pE0,e1))){
					alreadyin = true;
				}
			}

			if(alreadyin)
				continue;

			edgePair1.push_back(e0);
			edgePair2.push_back(e1);
		}
	}

	for(int i = 0; i <edgePair1.size();i++){
		E e0 = edgePair1.at(i);
		E e1 = edgePair2.at(i);
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

void ILPgen::newEdgeReward(V<E> usedEdges,int pathTotalNumber){
	cout<< "usedEdges.size is " << usedEdges.size()<<endl;
	for(int path = 0 ; path < pathTotalNumber; path++){


		string reward = obj.back();

		obj.pop_back();
		for(E e:g->edges){
			if(algo::vecContains(usedEdges,e))
				continue;
			reward += S(" + ")+pathUseEdge(path,e);
			//cout <<"reward is" << reward << endl;
		}
		obj.push_back(reward);
	}
}

void ILPgen::cutSeveralEdgesNoLoop(int pathNumber,const char* fileName, V<E> edgesToCover,V<E> usedEdges){

	for(int i = 0; i <= pathNumber-1; i++){
		//cut(i);
		path(i);
	}

	//punishment for loops
#if LAGLANGR_RELAXATION
	pathLoopPunishMent(pathNumber);

	newEdgeReward(usedEdges,pathNumber);
	cout << "im here" << endl;
#endif
	//path cover all the edges
	//coverEdge(edgesToCover, pathNumber);
	//minimumEdgeUse(pathNumber);
	writeInToILPfile(fileName);
}


void ILPgen::ilpRun(const char* ilpFileName){
	varValue = ILP::ILP_solve(ilpFileName);
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
#if !LAGLANGR_RELAXATION
	g->sortEdgesInPath(&path);
#endif
	return path;

}













/*	//if head use this node, then pathUsethisNode = 0
	// pathUseNode - M(1-headUse) <= 0
	// pathUseNode +M*headUse <= M
	// pathUseNode +M(1-headUse) >=0
	// pathUseNode - M* headUse >= -M
	for(Node* n:g->nodes){
		constraints.push_back(pathUseNode(pathNumber,n) + s(" + ") + s(M) + s(" ") + pathHeadUseNode(pathNumber,n) + s(" <= ") + s(M));
		constraints.push_back(pathUseNode(pathNumber,n) + s(" - ") + s(M) + s(" ") + pathHeadUseNode(pathNumber,n) + s(" >= ") + s(-M));
	}*/

/*//for a node which is not head or tail, adj edge if not wall then use 2;
for(Node* n:g->nodes){
	string nodeUse2Edge = "";
	for(Edge* e:n->getAdjEdges()){
		nodeUse2Edge += S(" + ")+pathUseEdge(pathNumber,e);
	}

	//if head use this node, headUseOndge = 1
	constraints.push_back(nodeUse2Edge+ S(" + ")+ S(M) + S(" ") + pathUseNode(pathNumber,n) + S(" <= ") + S(M+2));
	constraints.push_back(nodeUse2Edge + S(" - ")+ S(M) + S(" ") + pathUseNode(pathNumber,n) + S(" >= ") + S(2-M));
}*/





//for a node if node is used and node is not head

			//punish flow - out flow - 1

			/*for(Node* n:g->nodes){

				addVar(punishNodeFlow(pathNumber,n),4);
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

				//if head use this node punishment = 0
				//if(algo::vecContains(g->entrances,n)){

				//}

				if(algo::vecContains(g->exits,n)){
					constraints.push_back(punishNodeFlow(pathNumber,n)  +  S(" - ") +S(M)+S(" ") + pathTailUseNode(pathNumber,n)+ S(" >= ") + S(-M));
					constraints.push_back(punishNodeFlow(pathNumber,n)  +  S(" + ") +S(M)+S(" ") + pathTailUseNode(pathNumber,n)+ S(" <= ") + S(M));
				}
				//if path do not use this node punishment = 0
				constraints.push_back(punishNodeFlow(pathNumber,n)  +  S(" - ") +S(M)+S(" ") + pathUseNode(pathNumber,n)+ S(" <= ") + S(0));
				constraints.push_back(punishNodeFlow(pathNumber,n)  +  S(" + ") +S(M)+S(" ") + pathUseNode(pathNumber,n)+ S(" >= ") + S(0));

				//else punishment = flowOnNode - 1 when pathUseNode = 1 when it is not head node nor tailnode
				constraints.push_back(flowOnNode + S(" - ") + punishNodeFlow(pathNumber,n)
						+S(" + ") +S(M)+S(" ") + pathUseNode(pathNumber,n)
						+S(" - ") +S(M)+S(" ") + pathHeadUseNode(pathNumber,n)+S(" - ") +S(M)+S(" ") + pathHeadUseNode(pathNumber,n) + S(" <= ") +S(1+M));
				constraints.push_back(flowOnNode + S(" - ") + punishNodeFlow(pathNumber,n)
						+  S(" - ") +S(M)+S(" ") + pathUseNode(pathNumber,n)
						+  S(" + ") +S(M)+S(" ") + pathHeadUseNode(pathNumber,n)+  S(" + ") +S(M)+S(" ") + pathHeadUseNode(pathNumber,n)+ S(" >= ") +S(1-M));
				//constraints.push_back(flowOnNode + S(" - ") + punishNodeFlow(pathNumber,n)    + S(" = ") +S(1));

				//if pathNode = head punishment = flow on node
				constraints.push_back(flowOnNode + S(" - ")+punishNodeFlow(pathNumber,n)  +  S(" - ") +S(M)+S(" ") + pathHeadUseNode(pathNumber,n)+ S(" >= ") + S(-M));
				constraints.push_back(flowOnNode + S(" - ")+punishNodeFlow(pathNumber,n)  +  S(" + ") +S(M)+S(" ") + pathHeadUseNode(pathNumber,n)+ S(" <= ") + S(M));
				constraints.push_back(flowOnNode + S(" - ")+punishNodeFlow(pathNumber,n)  +  S(" - ") +S(M)+S(" ") + pathTailUseNode(pathNumber,n)+ S(" >= ") + S(-M));
				constraints.push_back(flowOnNode + S(" - ")+punishNodeFlow(pathNumber,n)  +  S(" + ") +S(M)+S(" ") + pathTailUseNode(pathNumber,n)+ S(" <= ") + S(M));



				totalPunishment += S(" + ") + punishNodeFlow(pathNumber,n);
			}*/





//for a node if node is used and node is not head
	//then then in flow = out flow + 1

































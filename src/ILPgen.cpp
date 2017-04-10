/*
 * ILPgen.cpp
 *
 *  Created on: Apr 5, 2017
 *      Author: ga63quk
 */

#include "ILPgen.h"
#include "toolBox.h"
#include "Graph.h"

ILPgen::ILPgen() {
	// TODO Auto-generated constructor stub

}

ILPgen::~ILPgen() {
	// TODO Auto-generated destructor stub
}


//for a graph, write ILPs
//find paths to cover all the edges
void ILPgen::pathToCoverAllEdges(){
	for(int i = 0; i <= 2; i++){
		//find a path
	}

	//path cover all the edges

}

__inline__ string getPathName(int pathNumber){
	return  s("p") + s(pathNumber);
}
__inline__ string pathUseNode(int pathNumber, Node* node){
	return getPathName(pathNumber) + s("Use") + node->name;
}
__inline__ string pathHeadUseNode(int pathNumber, Node* node){
	return getPathName(pathNumber) + s("HeadUse") + node->name;
}
__inline__ string pathTailUseNode(int pathNumber, Node* node){
	return getPathName(pathNumber) + s("TailUse") + node->name;
}

__inline__ string pathUseEdge(int pathNumber, Edge* edge){
	return getPathName(pathNumber) + s("Use") + edge->name;
}

//0 for integer, 1 for binary
void ILPgen::addVar(string variableName, int variableType){
	variables.push_back(variableName);
	if(variableType == 0)
		variableTypes.push_back("0");
	else
		variableTypes.push_back("1");
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
	}
	pUseOneEntrance += s(" = 1");
	constraints.push_back(pUseOneEntrance);
	//path ends with exit
	string pUseOneExit= "";
	for(Node* exit:g->exits){
		pUseOneExit+= s(" + ") + pathTailUseNode(pathNumber,exit);
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
	onlyOneHead += s(" = 1");
	constraints.push_back(onlyOneHead);



	//for a head or tail, adj edge if not wall then use one;
	for(Node* n:g->nodes){
		string headUseOneEdge = "";
		string tailUseOneEdge = "";
		for(Edge* e:n->getAdjEdges()){
			headUseOneEdge += S(" + ")+pathUseEdge(pathNumber,e);
			tailUseOneEdge += S(" + ")+pathUseEdge(pathNumber,e);
		}

		//if head use this node, headUseOndge = 1
		constraints.push_back(headUseOneEdge + S(" + ")+ S(M) + S(" ") + pathHeadUseNode(pathNumber,n) + S(" <= ") + S(M+1));
		constraints.push_back(headUseOneEdge + S(" - ")+ S(M) + S(" ") + pathHeadUseNode(pathNumber,n) + S(" >= ") + S(1-M));
		constraints.push_back(tailUseOneEdge + S(" + ")+ S(M) + S(" ") + pathTailUseNode(pathNumber,n) + S(" <= ") + S(M+1));
		constraints.push_back(tailUseOneEdge + S(" - ")+ S(M) + S(" ") + pathTailUseNode(pathNumber,n) + S(" >= ") + S(1-M));
	}

	//for 1 edge on the path, one of the neigbour path must be also on the path
	for(Edge* e:g->edges){
		string neibEdges = "";
		for(Edge* neigbEdge:e->adjEdges){
			neibEdges += S(" + ") + neigbEdge;
		}
		constraints.push_back(neibEdges+ S(" - ") + pathUseEdge(pathNumber,e) + S(" = 0"));
	}

	//path has no circle





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










































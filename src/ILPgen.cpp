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

__inline__ string pathNodeFlow(int pathNumber, Node* node){
	return getPathName(pathNumber) + s("Flow") + node->name;
}
__inline__ string pathEdgeFlow(int pathNumber, Edge* edge){
	return getPathName(pathNumber) + s("Flow") + edge->name;
}

__inline__ string pathSuperFlow(int pathNumber){
	return getPathName(pathNumber) + s("SuperFlow");
}

ILPgen::ILPgen() {
	// TODO Auto-generated constructor stub

}

ILPgen::~ILPgen() {
	// TODO Auto-generated destructor stub
}


//for a graph, write ILPs
//find paths to cover all the edges
void ILPgen::pathToCoverAllEdges(const char* fileName){
	int pathNumber = 4;
	for(int i = 0; i <= pathNumber-1; i++){
		path(i);
	}

	//path cover all the edges
	for(Edge* e:g->edges){
		string edgeUsedOnce = "";
		for(int i = 0; i <= pathNumber-1; i ++){
			edgeUsedOnce += S(" + ") + pathUseEdge(i,e);
		}
		constraints.push_back(edgeUsedOnce + S(" >= 1"));
	}

	writeILP ilp;
	ilp.writeConstraint(constraints,fileName);
	ilp.writeVarNames(this->variables,this->variableTypes,fileName);
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
	onlyOneTail+= s(" = 1");
	constraints.push_back(onlyOneTail);



	//for a head or tail, adj edge if not wall then use one;
	for(Node* n:g->nodes){
		string headUseOneEdge = "";
		string tailUseOneEdge = "";
		for(Edge* e:n->getAdjEdges()){
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
		for(Edge* neibourEdge: n->getAdjEdges()){
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

	//path has no circle

	//for a node not head or tail
	//flowIn = flowOut + 1

	//for a not used edge, flow = 0
	for(Edge* e:g->edges){
		addVar(pathEdgeFlow(pathNumber,e),0);
		constraints.push_back(pathEdgeFlow(pathNumber,e) + S(" - ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" <= 0"));
		constraints.push_back(pathEdgeFlow(pathNumber,e) + S(" + ") + S(M) + S(" ") + pathUseEdge(pathNumber,e) + S(" >= 0"));
	}

	//for a node if node is used and node is not head
	//then then in flow = out flow + 1
	for(Node* n:g->nodes){
		string flowOnNode = "";
		for(Edge* adjEdge:n->getAdjEdges()){
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










































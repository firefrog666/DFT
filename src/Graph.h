/*
 * Graph.h
 *
 *  Created on: Apr 5, 2017
 *      Author: ga63quk
 */

#ifndef GRAPH_H_
#define GRAPH_H_
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include "toolBox.h"



using namespace std;
class Edge;
class Graph;
class Node;
class Cell;


class Cell{
public:
	float x;
	float y;
	string name;
	vector<Edge*>* adjEdges = new vector<Edge*>();
	vector<Node*>* adjNodes = new vector<Node*>();
	bool isOpen(){
		if(adjEdges->size() < 4)
			return true;
		else
			return false;
	}
};
class Node{
public:
	Node(){
		x = 0;
		y = 0;
		name = "new node";
		hashValue = algo::hash2Int(x,y);
	}
	Node(float a, float b){
		x = a; y = b;

		hashValue = algo::hash2Int(a,b);
		setName();
		//cout << "node " << x << " " << y << " has been created" << endl;
		//cout << "node name is" << name << endl;
	}
	Node(Node* n){
		x = n->x;
		y = n->y;
		hashValue = n->hashValue;
		name = n->name;

	}

public:
	int id;
	float x,y;
	float hashValue;
	string name;
private:
	vector<Node*>* adjNodes = new vector<Node*>();
	vector<Edge*>* adjEdges = new vector<Edge*>();

public:
	void setName(){
		name = s("n") + s("x")+s(x)+s("y")+s(y);
	}
	void setAdjNodes(vector<Node*>& nodes){
		adjNodes = &nodes;
	}
	void addAdjNodes(Node* node){
		if(!algo::vecContains(*adjNodes,node))
			adjNodes->push_back(node);
	}
	void addAdjEdges(Edge* e){
		if(!algo::vecContains(*adjEdges,e))
			adjEdges->push_back(e);
	}

	vector<Node*>*  getAdjNodes(){
		return adjNodes;
	}

	vector<Edge*>* getAdjEdges(){
		return adjEdges;
	}




};

class Edge{
public:

	Edge(Node* a, Node * b);

	Edge(Edge* e){
		x = e->x;
		y = e->y;
		s = e->s;
		t = e->t;
		this->hashValue = e->hashValue;
		name = e->name;
		//cout<<"edge "<< x << " "<< y <<" "<<s<<" "<<t<<" "<< "has been created"<< endl;
		//cout<<"hashValue is "<<hashValue<<endl;
		//cout <<"edge name = "<<name<<endl;
		isWall = e->isWall;
		isHole = e->isHole;
		isOn = e->isOn;
		sa0 = e->sa0;
		sa1 = e->sa1;
	}
	vector<Node*>* adjNodes = new vector<Node*>();
	vector<Edge*>* adjEdges = new vector<Edge*>();
	float x,y,s,t;
	float hashValue;
	string name;
	bool isWall = false;
	bool isHole = false;
	bool isOn = false;
	bool sa0 = false;
	bool sa1 = false;


public:
	void setName(){
		name = S("e") + S("x")+S(x)+S("y")+S(y)+S("s")+S(s)+S("t")+S(t);
	}
	void turnOn();

	void turnOff();

	bool isGood(){ return !sa0 && !sa1;};
	bool intercectEdge(Edge* e){
		if(x > e->x && s > e->x && x >e->s && s > e->s )
			return false;
		if(x < e->x && s < e->x && x <e->s && s < e->s )
			return false;
		if(y > e->y && t > e->y && y >e->t && t > e->t )
			return false;
		if(y < e->y && t < e->y && y <e->t && t < e->t )
			return false;

		return true;


	}


	static bool sameEdge(Edge* e1, Edge* e2){
		if(e1->x == e2->x && e1->y == e2->y && e1->s && e2->s && e1->t == e2->t && e1->isWall == e2->isWall && e1-> isHole == e2->isHole)
			return true;
		else
			return false;
	}
	bool edgeOutOfNode(Node* n){
		if(n->x == this->x && n->y == this->y){
			return true;
		}
		else{
			return false;
		}
	}
	void setAdjNodes(vector<Node*>& nodes){
		adjNodes = &nodes;
	}

	void addAdjNodes(Node* node){
		adjNodes->push_back(node);
	}

	vector<Node*>*  getAdjNodes(){
		return adjNodes;
	}

	/*vector<Edge*>* getAdjEdges(){
		return adjEdges;
	}*/

};

class Graph {
public:
	Graph();
	Graph(const char* csvFile);
	Graph(vector<Node*> nodes,Graph* otherG);
	void creatNodes(const char* cvvFile);
	void creatCells(const char* cvvFile);
	void creatSuperCell(const char* cvvFile);
	virtual ~Graph();
	int width,height;
public:
	map<float,Edge*> hashEdges;
	map<float,Node*> hashNodes;
	vector<Edge*> edges;
	vector<Node*> nodes;
	vector<Node*> entrances;
	vector<Node*> exits;
	vector<Cell*> cells;
	Cell* superCell;

	map<Node*,vector<Node*>> superNodeToVents;

	V<E> insideEdges;
	V<E> brinkEdges;

private:
	void getHashEdges();
	vector<Node*> outSideWalk(Node* initNode);
	bool nodeOnTheBrink(Node* node);
	bool edgeOnBrink(Edge* e,vector<Cell*> cells);
	vector<Node*> outSideWalk(Node* initNode, int steps);
	void breadSearchPath(vector<Node*>& path, vector<vector<Node*>> alltheGraphs);
	Edge* pathShareSameEdge(vector<Edge*>* p1, vector<Edge*>* p2);
	vector<Node*> getHoleNodes(Node* seed);
	Node* getLeftCorner();

public:
	Edge* getEdge(Node* start, Node* end);
	Edge* getEdge(float x,float y,float s,float t);
	Node* getNode(float i, float j);
	bool nodeOnEdge(Node* n, Edge* e);
	bool edgeOnEdge(Edge* e0, Edge* e1);
	static float hash2Nodes(Node* a, Node* b);
	void initTest(int w, int h);
	void hashAllEdges();
	void hashALLNodes();
	void removeEdge(Edge* e);
	void removeNode(Node* n);
	vector<Graph*> splitGraph(int graphNum, unsigned int nodesSize);
	void cutOffSubGraph(Graph* subGraph);
	Graph* splitGraphByBoundingBox(float ld_x, float ld_y, float ru_x,float ru_y );
	V<Graph*> splitGraphByAxis(vector<float> xAxis,vector<float> yAxis);
	vector<Edge*> connectPaths(vector<Edge*> path1, vector<Edge*> path2);
	vector<Edge*> getWallsBetweenSubGrapsh(vector<Graph*>* subGraphs);
	vector<vector<Edge*>*> connectPaths(vector<vector<Edge*>*>& p1Set, vector<vector<Edge*>*>& p2Set);
	vector<Cell*> getCells();
	Cell* getSuperCell(V<Cell*> cells);
	vector<Node*> outSideNodes();
	Graph* fromGraphToChip();
	bool  dfs(Node* s, Node* t);
	void sortEdgesInPath(vector<Edge*>* pathToSort);
	PATHN edgesToNodes(const vector<Edge*>* sortedPath);
	PATHE pathNodesToEdges(vector<Node*> sortedNodes);
	V<PATHE> getPathsFromEdges(vector<Edge*> edgeSet);
	V<PATHE> getRingsFromEdges(vector<Edge*> edgeSet);
	V<PATHE> getGroupsFromEdges(vector<Edge*> edgeSet);
	bool nodeInsideRing(Node* n,vector<Edge*> ring);
	PATHN  getNodesOnRing(vector<Edge*> ring);
	vector<Node*>  getNodesInRing(vector<Edge*> ring);
	vector<Node*>  getNodesOutRing(vector<Edge*> ring);
	V<V<PATHE>> connectRings(PATHE ringS,PATHE ringT,vector<vector<Edge*>> rings);


	map<Node*,int> ringToOtherRings(vector<Edge*> ring,vector<vector<Edge*>> allRings);

	vector<Node*> getNodesInOrOnAllRings(vector<vector<Edge*>> allRings);
	vector<Node*> getNodesOutAllRings(vector<vector<Edge*>> allRings);

	V<PATHE> fromStepToPath(map<Node*,int>step, Node* start, V<N> disabledNodes);
	V<PATHE> fromStepToShortedPath(map<N,int>steps, V<N> starts,V<N> disabledNodes);
	V<PATHE> ringToRingPaths(PATHE ring0, PATHE ring1, V<PATHE> allRings); //ring to ring shortestpats
	map<pair<PATHE,PATHE>,V<PATHE>> allRingShortPaths(V<PATHE> rings);
	V<PATHE> selectshortPaths(PATHE ring0, PATHE ring1, map<pair<PATHE,PATHE>,V<PATHE>> ringToRingShortedPaths);
	bool pathIntercectPaths(PATHE p, V<PATHE> paths);
	bool pathIntercectPath(PATHE p0, PATHE p1);
	void moveGraph(float x, float y);

	void connectLoopToPath(V<E>& loop, V<E>& path);
	V<N> nodesOnPath(const V<E>& path);
	V<E> shortestPathToNodes(N start, V<N> destinations, V<N> forbiddenNodes);
	V<N> traverseSortedPath(N start, N end, const V<E>& sortedPath);



};



#endif /* GRAPH_H_ */

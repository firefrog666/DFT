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
	bool isBusy = false; //ocupied by some task
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
	bool sameNode(Node* otherN){
		if(otherN->hashValue == hashValue)
			return true;
		else
			return false;
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
	Edge(int ex,int ey, int es, int et){
		x = ex;
		y = ey;
		s = es;
		t = et;
		hashValue = algo::hash4Int(x,y,s,t);
		setName();

		//cout<<"edge "<< x << " "<< y <<" "<<s<<" "<<t<<" "<< "has been created"<< endl;
		//cout<<"hashValue is "<<hashValue<<endl;
		//cout <<"edge name = "<<name<<endl;
		isWall = false;
		isHole = false;
		isStore = false;
		isOn = true;
		sa0 = false;
		sa1 = false;
	}
	//vector<Node*>* adjNodes = new vector<Node*>();
	//vector<Edge*>* adjEdges = new vector<Edge*>();
	float x,y,s,t;
	float hashValue;
	string name;
	bool isWall = false;
	bool isHole = false;
	bool isStore = true;
	bool isOn = false;
	bool sa0 = false;
	bool sa1 = false;
	int id;
	bool isBusy = false;

public:
	void setName(){
		name = S("e") + S("x")+S(x)+S("y")+S(y)+S("s")+S(s)+S("t")+S(t);
	}
	void turnOn();

	void turnOff();
	void cpySpecFrom(E e){
		isWall = e->isWall;
		isHole = e->isHole;
		isOn = e->isOn;
		sa0 = e->sa0;
		sa1 = e->sa1;
		id = e->id;
	}

	bool isGood(){ return !sa0 && !sa1;};
	bool intercectEdge(Edge* e){


/*
		if(x > e->x && s > e->x && x >e->s && s > e->s )
			return false;
		if(x < e->x && s < e->x && x <e->s && s < e->s )
			return false;
		if(y > e->y && t > e->y && y >e->t && t > e->t )
			return false;
		if(y < e->y && t < e->y && y <e->t && t < e->t )
			return false;*/

		P p0(x,y);
		P p1(s,t);
		P p2(e->x,e->y);
		P p3(e->s,e->t);

		return algo::seg(p0,p1,p2,p3);


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






	/*vector<Edge*>* getAdjEdges(){
		return adjEdges;
	}*/

};

class Graph {
public:
	Graph();
	Graph(const char* csvFile);
	Graph(vector<Node*> nodes,Graph* otherG);
	void creatNodes(const char* xml);
	void creatEdges(const char* );
	void creatEdgePair(const char* xml);
	void creatCells(const char* cvvFile);
	void creatSuperCell(const char* cvvFile);
	V<V<E>> locateWalls();
	V<V<E>> locateHoles();
	void deleteHoles(V<V<E>> setOfWalls);
	void deleteWalls(V<V<E>> setOfWalls);
	void sortEdgesAndNodes();
	virtual ~Graph();
	int width,height;
public:
	map<float,Edge*> hashEdges;
	map<float,Node*> hashNodes;
	vector<Edge*> edges;
	vector<Node*> nodes;

	vector<Edge*> oldEdges;
	vector<Node*> oldNodes;
	map<E,E> newEdgeToOldEde;
	vector<Node*> entrances;
	vector<Node*> exits;
	vector<Cell*> cells;
	Cell* superCell;

	map<Node*,vector<Node*>> superNodeToVents;
	V<E> edgePair1; //edgePair cannot be together
	V<E> edgePair2; //edgePair cannot be together
	V<E> insideEdges;
	V<E> brinkEdges;

	V<V<E>> sToWEdges;
	V<V<E>> wToWEdges;

public:
	void getHashEdges();
	Node* getLeftCorner();
	Edge* getEdge(Node* start, Node* end);
	Edge* getEdge(float x,float y,float s,float t);
	Node* getNode(float i, float j);
	Node* getNodeById(int id);
	V<E> getAdjEdges(E e);
	E getEdgeById(int id);
	bool nodeOnEdge(Node* n, Edge* e);
	bool edgeOnEdge(Edge* e0, Edge* e1);
	static float hash2Nodes(Node* a, Node* b);
	void initTest(int w, int h);
	void hashAllEdges();
	void hashALLNodes();

	bool  dfs(Node* s, Node* t);
	void sortEdgesInPath(vector<Edge*>* pathToSort);
	PATHN edgesToNodes(const vector<Edge*>* sortedPath);
	PATHE pathNodesToEdges(vector<Node*> sortedNodes);

	bool pathIntercectPaths(PATHE p, V<PATHE> paths);
	bool pathIntercectPath(PATHE p0, PATHE p1);
	void moveGraph(float x, float y);
	V<E> findPath(N start, N end, V<N> forbideNodes, V<E> forbidEdges);
	bool dfs(Node* s, Node* t, V<E>& path, V<E>& fobidEdges, V<N>& fobidNodes);
	bool dfsValveSharing(Node* s, Node* t, V<E>& path, V<E>& forbidEdges,V<N>& forbidNodes,map<E,E> shareValveMap);




	void removeNode(Node * n);
	void removeEdge(Edge* e);
	void replaceNode(N target,N newNode);
	bool addEdge(E e);
	bool addNode(N n);
	bool addEdges(V<E> edgesToAdd);

	bool edgesConnected(E e0, E e1);
	Graph* fitGraphIntoGrid();
	void findBoundingBox(V<float>& boundingbox);
	void initByBoundingBox(V<float>& boundingBox);

	void howShareVavles(V<V<E>>& paths, V<V<E>>& cuts, V<E>& newEdges, V<E>& sharePlans);
	bool validPathWithShareValves(const V<E>& path,const V<E>& newEdges, V<E>& sharePlan);
	bool validCutWithShareValves(const V<E>& path, const V<E>& newEdges, V<E>& sharePlan);
    bool howShareValvesMostLikeOTherPlan(const V<V<E>>& paths,const V<V<E>>& cuts,const V<E>& newEdges,V<E>& sharePlan, const V<E>& otherNewEdges,const V<E>& otherSharePlan);

};



#endif /* GRAPH_H_ */

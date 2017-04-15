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


class Node{
public:
	Node();
	Node(int a, int b){
		x = a; y = b;
		name = s("n") + s("x")+s(x)+s("y")+s(y);
		hashValue = algo::hash2Int(a,b);
		cout << "node " << a << " " << b << " has been created" << endl;
		cout << "node name is" << name << endl;
	}

public:
	int x,y;
	int hashValue;
	string name;
private:
	vector<Node*> adjNodes;
	vector<Edge*> adjEdges;

public:
	void setAdjNodes(const vector<Node*>& nodes){
		adjNodes = nodes;
	}
	void addAdjNodes(Node* node){
		adjNodes.push_back(node);
	}

	vector<Node*>  getAdjNodes(){
		return adjNodes;
	}

	vector<Edge*> getAdjEdges(){
		return adjEdges;
	}




};

class Edge{
public:
	Edge();
	Edge(Node* a, Node * b);
	vector<Node*> adjNodes;
	vector<Edge*> adjEdges;
	int x,y,s,t;
	int hashValue;
	string name;
	bool isWall;
	bool isHole;

public:
	bool edgeOutOfNode(Node* n){
		if(n->x == this->x && n->y == this->y){
			return true;
		}
		else{
			return false;
		}
	}
	void setAdjNodes(const vector<Node*>& nodes){
		adjNodes = nodes;
	}

	void addAdjNodes(Node* node){
		adjNodes.push_back(node);
	}

	vector<Node*>  getAdjNodes(){
		return adjNodes;
	}

	vector<Edge*> getAdjEdges(){
		return adjEdges;
	}

};

class Graph {
public:
	Graph();
	virtual ~Graph();
	int width,height;
public:
	map<int,Edge*> hashEdges;
	map<int,Node*> hashNodes;
	vector<Edge*> edges;
	vector<Node*> nodes;
	vector<Node*> entrances;
	vector<Node*> exits;

	Edge* getEdge(Node* start, Node* end);
	Node* getNode(int i, int j);
private:
	void getHashEdges();
public:
	static int hash2Nodes(Node* a, Node* b);
	void initTest(int w, int h);
	void hashAllEdges();
	void hashALLNodes();
};



#endif /* GRAPH_H_ */

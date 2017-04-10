/*
 * Graph.cpp
 *
 *  Created on: Apr 5, 2017
 *      Author: ga63quk
 */

#include "Graph.h"
#include "toolBox.h"
#include <iostream>
using namespace std;
Graph::Graph() {
	// TODO Auto-generated constructor stub
	cout << "i'm contrucing a graph"<< endl;

}

Graph::~Graph() {
	// TODO Auto-generated destructor stub
}

Node* Graph::getNode(int i, int j){
	auto it = hashNodes.find(algo::hash2Int(i,j));
	if(it != hashNodes.end())
		return it->second;
	else
	{
		cout << "can find the node " <<endl;
		return NULL;
	}
}

Edge* Graph::getEdge(Node* start, Node* end){
	int index = hash2Nodes(start,end);
	cout<< "finding " <<index <<endl;
	auto it = hashEdges.find(index);
		if(it != hashEdges.end())
			return it->second;
		else
		{
			cout << "can't find the Edge" <<endl;
			return NULL;
		}
}

 int Graph::hash2Nodes(Node* a, Node* b){
		Node* temp;
		if(a->y > b->y ||a->x > b->x){
			temp = a;
			a = b;
			b = temp;
		}


		return algo::hash4Int(a->x,a->y,b->x,b->y);


	}

Edge::Edge(Node* a, Node* b){
	assert(!((a->x == b->x) && (a->y == b->y)));
	int hash= Graph::hash2Nodes(a,b);
	this->hashValue = hash;

	if(a->x > b->x || a->y > b->y){
		Node* temp = a;
		a = b;
		b = temp;
	}

	x = a->x;
	y = a->y;
	s = b->x;
	t = b->y;
	name = S("e") + S("x")+S(x)+S("y")+S(x)+S("s")+S(s)+S("t")+S(t);

	cout<<"edge "<< x << " "<< y <<" "<<s<<" "<<t<<" "<< "has been created"<< endl;
	cout<<"hashValue is "<<hashValue<<endl;
	cout <<"edge name = "<<name<<endl;



}

void Graph::hashAllEdges(){
	for(Node* node:nodes){
		auto adjNodes = node->getAdjNodes();
		for(Node* adjNode: adjNodes){
			if(!getEdge(adjNode,node)){
				Edge* edge = new Edge(node,adjNode);
				edges.push_back(edge);
				hashEdges[edge->hashValue] = edge;

				//hashTarEdges.put(edge.hashValue(), edge);
			}
		}
	}
}

void Graph::initTest(int w, int h){

	width = w;
	height = h;
	//give number to each Node; Hash nodes
		int id = 0;

		for(int i = 0; i< height; i++){
			for(int j =0; j < width ; j ++){
				Node* node = new Node(i,j);
				nodes.push_back(node);
				int index = algo::hash2Int(i,j);
				hashNodes[index] =  node;
				id ++;

			}
		}



		//assign joint nodes to each node
		for(int i = 0; i< height; i++){
			for(int j =0; j < width; j ++){
				if(i>=1)
					getNode(i,j)->addAdjNodes(getNode(i-1,j));
				if(j>=1)
					getNode(i,j)->addAdjNodes(getNode(i,j-1));
				if(j<width-1)
					getNode(i,j)->addAdjNodes(getNode(i,j+1));
				if(i<height-1)
					getNode(i,j)->addAdjNodes(getNode(i+1,j));
			}
		}

		hashAllEdges();


}


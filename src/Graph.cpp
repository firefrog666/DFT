/*
 * Graph.cpp
 *
 *  Created on: Apr 5, 2017
 *      Author: ga63quk
 */

#include "Graph.h"
#include <map>
#include "toolBox.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "../rapidxml-1.13/rapidxml.hpp"
#include <cstring>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>
#include "draw.h"
using namespace std;


int stringToInt(string s){
	int r;
	istringstream abuffer(s);
	abuffer>>r;
	return r;
}

void Edge::turnOn(){
	if(isWall){
		if(sa0){
			cout<<name<<"is wall sa0, cannot turn on"<<endl;
			isOn = false;
		}
		else if(sa1){
			cout<<name<<"is wall sa1, can turn on"<<endl;
			isOn = true;

		}
		else{
			cout<<name<<"is wall and good, cannot turn on"<<endl;
			isOn = false;
		}
	}
	else if (isHole){
		if(sa0){
			cout<<name<<"is hole sa0, cannot turn on"<<endl;
			isOn = false;
		}
		else if(sa1){
			cout<<name<<"is hole  sa1, can turn on"<<endl;
			isOn = true;

		}
		else{
			cout<<name<<"is hole and good, can turn on"<<endl;
			isOn = true;
		}

	}
	else{
		if(sa0){
			cout<<name<<"is regular sa0, cannot turn on"<<endl;
			isOn = false;
		}
		else if(sa1){
			cout<<name<<"is regular sa1, can turn on"<<endl;
			isOn = true;

		}
		else{
			cout<<name<<"is regular and good, can turn on"<<endl;
			isOn = true;
		}
	}

}

void Edge::turnOff(){
	if(isWall){
		if(sa0){
			cout<<name<<"is wall sa0, can turn off"<<endl;
			isOn = false;
		}
		else if(sa1){
			cout<<name<<"is wall sa1, cannot off"<<endl;
			isOn = true;

		}
		else{
			cout<<name<<"is wall and good, can turn off"<<endl;
			isOn = false;
		}
	}
	else if (isHole){
		if(sa0){
			cout<<name<<"is hole sa0, can turn off"<<endl;
			isOn = false;
		}
		else if(sa1){
			cout<<name<<"is hole sa1, cannot off"<<endl;
			isOn = true;

		}
		else{
			cout<<name<<"is hole and good, cannot turn off"<<endl;
			isOn = false;
		}

	}
	else{
		if(sa0){
			cout<<name<<"is regular sa0, can turn off"<<endl;
			isOn = false;
		}
		else if(sa1){
			cout<<name<<"is regular sa1, cannot off"<<endl;
			isOn = true;

		}
		else{
			cout<<name<<"is regular and good, can turn off"<<endl;
			isOn = false;
		}
	}
}



Graph::~Graph() {
	// TODO Auto-generated destructor stub
}

//get a sub graph from nodes from other graph
Graph::Graph(vector<Node*> nodesFromOtherGraph,Graph* otherG){
	//put all nodes in
	for(Node* n:nodesFromOtherGraph){
		Node* newNode = new Node(n);

		if(!algo::vecContains(nodes,newNode)){
			nodes.push_back(newNode);
			hashNodes[algo::hash2Int(newNode->x,newNode->y)] = newNode;
		}
	}

	//add nodes connected to thisgraph

	V<N> outSiderNodes;
	//set neighbor to newNodes
	for(Node* n:nodesFromOtherGraph){
		for(Node* nbr:*n->getAdjNodes()){

			//neighbor node
			Node* newNode = getNode(n->x,n->y);
			Node* newNbr = getNode(nbr->x,nbr->y);
			if(newNbr == NULL){
				newNbr = new Node(nbr);
			}

			newNode->addAdjNodes(newNbr);
			newNbr->addAdjNodes(newNode);

			if(!algo::vecContains(nodes,newNbr)){

				nodes.push_back(newNbr);
				hashNodes[algo::hash2Int(newNbr->x,newNbr->y)] = newNbr;
			}

			//neighbor edge
			Edge* otherE = otherG->getEdge(n,nbr);
			Edge* newE = new Edge(otherE);

			if(hashEdges.find(algo::hash4Int(newE->x,newE->y,newE->s,newE->t))==hashEdges.end()){
				edges.push_back(newE);
				hashEdges[algo::hash4Int(newE->x,newE->y,newE->s,newE->t)] = newE;
				newNode->addAdjEdges(newE);
				newNbr->addAdjEdges(newE);
			}

		}
	}
}


Graph::Graph(){

}

Graph::Graph(const char* xmlFile){
	creatNodes(xmlFile);
	creatEdges(xmlFile);
	//creatCells(xmlFile);
	//creatEdgePair(xmlFile);
}







void Graph::creatNodes(const char* nodes_xml){
	cout<<"creating nodes from xml"<<endl;
	//getNodes

	rapidxml::xml_document<> doc;    // character type defaults to char
	rapidxml::xml_node<> * root;
	ifstream theFile (nodes_xml);
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc
	doc.parse<0>(&buffer[0]);
	root = doc.first_node("root");



	map<int,Node*> IdTonode;
	//initialize all nodes
	for (rapidxml::xml_node<> * node = root->first_node("node"); node; node = node->next_sibling())
	{
		if (strcmp(node->name(), "node") != 0)
					continue;
		int x = stringToInt(node->first_attribute("x")->value());
		int y = stringToInt(node->first_attribute("y")->value());
		Node* n = new Node(x,y);
		n->id = stringToInt(node->first_attribute("id")->value());
		hashNodes[n->hashValue] = n;
		nodes.push_back(n);
		IdTonode[n->id] = n;
	}


	for (rapidxml::xml_node<> * entrance = doc.first_node("entrance"); entrance; entrance = entrance->next_sibling())
	{
		if (strcmp(entrance->name(), "entrance") != 0)
			continue;
		int nodeid = stringToInt(entrance->first_attribute("nodeId")->value());
		Node* thisNode = IdTonode[nodeid];
		entrances.push_back(thisNode);
	}


	for (rapidxml::xml_node<> * exit = doc.first_node("exit"); exit; exit = exit->next_sibling())
	{
		if (strcmp(exit->name(), "exit") != 0)
			continue;
		int nodeid = stringToInt(exit->first_attribute("nodeId")->value());
		Node* thisNode = IdTonode[nodeid];
		exits.push_back(thisNode);
	}




}

//edge pair must be in 2 different
void Graph::creatEdgePair(const char* xml){
	cout<< "creating Edges pair"<<endl;
	//getNodes
	rapidxml::xml_document<> doc;    // character type defaults to char

	ifstream theFile (xml);
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc
	doc.parse<0>(&buffer[0]);




	map<int,Edge*> IdToEdge;
	//initialize all nodes
	for (rapidxml::xml_node<> * edgePair = doc.first_node("edgepair"); edgePair; edgePair = edgePair->next_sibling())
	{

		if (strcmp(edgePair->name(), "edgepair") != 0)
					continue;

		int id0 = stringToInt(edgePair->first_attribute("id1")->value());
		int id1 = stringToInt(edgePair->first_attribute("id2")->value());
		E e0 = NULL;
		E e1 = NULL;
		for(E e:edges){
			if(e->id == id0)
				e0 = e;
			if(e->id == id1)
				e1 = e;
		}
		assert(e0 != NULL);
		assert(e1 != NULL);

		edgePair1.push_back(e0);
		edgePair2.push_back(e1);

	}


}


void Graph::creatEdges(const char* xml){
	cout<< "creating Edges"<<endl;
	//getNodes
	rapidxml::xml_document<> doc;    // character type defaults to char
	ifstream theFile (xml);
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc
	doc.parse<0>(&buffer[0]);



	map<int,Edge*> IdToEdge;
	//initialize all nodes
	for (rapidxml::xml_node<> * edge = doc.first_node("edge"); edge; edge = edge->next_sibling())
	{

		if (strcmp(edge->name(), "edge") != 0)
					continue;

		int nodeId0 = stringToInt(edge->first_attribute("nodeId0")->value());
		int nodeId1 = stringToInt(edge->first_attribute("nodeId1")->value());

		int isWall = stringToInt(edge->first_attribute("isWall")->value());
		int isHole = stringToInt(edge->first_attribute("isHole")->value());
		int isStore = stringToInt(edge->first_attribute("isStore")->value());
		//set adjEdges to Node
		Node* n0 = getNodeById(nodeId0);
		Node* n1 = getNodeById(nodeId1);

		Edge* e = new Edge(n0,n1);
		e->id = stringToInt(edge->first_attribute("id")->value());

		hashEdges[e->hashValue] = e;
		edges.push_back(e);
		IdToEdge[e->id] = e;
		e->isHole = isHole;
		e->isWall = isWall;
		e->isStore = isStore;



		assert(n0 != NULL);
		assert(n1 != NULL);
		algo::distincAddVec(*n0->getAdjEdges(),e);
		algo::distincAddVec(*n1->getAdjEdges(),e);
		algo::distincAddVec(*n0->getAdjNodes(),n1);
		algo::distincAddVec(*n1->getAdjNodes(),n0);

	}


}

bool Graph::edgesConnected(E e0, E e1){
	N e0n0 = getNode(e0->x,e0->y);
	N e0n1 = getNode(e0->s,e0->t);
	N e1n0 = getNode(e1->x,e1->y);
	N e1n1 = getNode(e1->s,e1->t);

	if(e0n0 == e1n0)
		return true;
	if(e0n0 == e1n1)
		return true;
	if(e0n1 == e1n0)
		return true;
	if(e0n1 == e1n1)
		return true;

	return false;
}

void Graph::sortEdgesInPath(vector<Edge*>* pathToSort){
	N head = entrances.at(0);
	E headEdge = NULL;
	for(E adjE: *head->getAdjEdges() ){
		if(algo::vecContains(*pathToSort,adjE))
			headEdge = adjE;
	}
	assert(headEdge != NULL);

	V<E> stack;
	vector<Edge*> sortedEdges;
	stack.push_back(headEdge);
	while(stack.size()>0){
		E edge = stack.back();
		stack.pop_back();
		sortedEdges.push_back(edge);

		//find edges connected to edge
		for(E eToSort:*pathToSort){
			if(eToSort == edge)
				continue;
			if(algo::vecContains(sortedEdges,eToSort))
				continue;
			if(!edgesConnected(eToSort,edge))
				continue;

			stack.push_back(eToSort);

		}
	}
	pathToSort->clear();

	for(Edge* e:sortedEdges){
		pathToSort->push_back(e);
	}

}


vector<Edge*> Graph::pathNodesToEdges(vector<Node*> sortedNodes){
	vector<Edge*> r;
	for(auto it = sortedNodes.begin();it != sortedNodes.end()-1; it++){
		Node* fNode = *it;
		Node* lNode = *(it+1);

		Edge* e = getEdge(fNode,lNode);
		//assert(e != NULL);
		if(e != NULL)
			r.push_back(e);
	}
	return r;
}

vector<Node*> Graph::edgesToNodes(const vector<Edge*>* sortedPath){
	N head = entrances.at(0);

	V<N> stack;
	stack.push_back(head);
	V<N> sortedNodes;
	while(stack.size()>0){
		N n = stack.back();
		//cout<< "visiting "<< n->name<<endl;
		stack.pop_back();
		sortedNodes.push_back(n);
		for(N adjNode : *n->getAdjNodes()){
			if(algo::vecContains(sortedNodes,adjNode))
				continue;
			E e = getEdge(n,adjNode);

			if(e  == NULL)
				continue;

			if(!algo::vecContains(*sortedPath,e))
				continue;

			stack.push_back(adjNode);
		}

	}
	return sortedNodes;

}

Node* Graph::getLeftCorner(){
	Node* result;

	int minX = 100000;
	int minY = 100000;
	for(Node* n:nodes){
		if(n->x < minX && n->y < minY){
			minX = n->x;
			minY = n->y;
			result = n;
		}
	}

	return result;
}

Node* Graph::getNodeById(int id){
	N result = NULL;
	for(Node* n:nodes){
		if(n->id == id)
			result = n;
	}

	return result;
}

E Graph::getEdgeById(int id){
	for(E e:edges){
		if(e->id == id)
			return e;
	}
	return NULL;
}

Node* Graph::getNode(float i, float j){
	auto it = hashNodes.find(algo::hash2Int(i,j));
	Node* n = it->second;

	if(it != hashNodes.end()){
		//cout<<"find out n "<< n->name<<endl;
		return n;
	}
	else
	{
		//cout << "can find the node " <<endl;
		return NULL;
	}
}

Edge* Graph::getEdge(Node* start, Node* end){
	//cout << "find Edge"<<" " << start->x<<" " << start->y<<" "<< end->x<<" "<<end->y<<endl;

	float index = hash2Nodes(start,end);
	//cout<< "finding " <<index <<endl;
	auto it = hashEdges.find(index);
		if(it != hashEdges.end())
			return it->second;
		else
		{
			//cout << "can't find the Edge" <<endl;
			return NULL;
		}
}
Edge* Graph::getEdge(float x,float y,float s,float t){
	float index = algo::hash4Int(x,y,s,t);
	auto it = hashEdges.find(index);
		if(it != hashEdges.end())
			return it->second;
		else
		{
			//cout << "can't find the Edge" <<endl;
			return NULL;
		}
}

 float Graph::hash2Nodes(Node* a, Node* b){
	 	 float x = a->x;
	 	 float y = a->y;
	 	 float s = b->x;
	 	 float t = b->y;

	 	 float c = s-x;
	 	 float d = t-y;

	 	 if(d <= 0){
	 		 if(!(c > 0 && d == 0)){
	 			Node* temp;
				temp = a;
				a = b;
				b = temp;

	 		 }
	 	 }




		return algo::hash4Int(a->x,a->y,b->x,b->y);


	}

Edge::Edge(Node* a, Node* b){
	assert(!((a->x == b->x) && (a->y == b->y)));
	float hash= Graph::hash2Nodes(a,b);
	this->hashValue = hash;


	 float c = b->x-a->x;
	 float d = b->y-a->y;

	 if(d <= 0){
		 if(!(c > 0 && d == 0)){
			Node* temp;
			temp = a;
			a = b;
			b = temp;
		 }
	 }

	isWall = 0;
	isHole = 0;
	isOn = false;
	sa0 = false;
	sa1 = false;

	x = a->x;
	y = a->y;
	s = b->x;
	t = b->y;
	setName();
}




void Graph::hashAllEdges(){
		int edgeCount = 0;
	for(Node* node:nodes){
		auto adjNodes = node->getAdjNodes();
		for(Node* adjNode: *adjNodes){

			if(!getEdge(adjNode,node)){
				Edge* edge = new Edge(node,adjNode);
				edge->id = edgeCount;
				edgeCount++;
				edges.push_back(edge);
				hashEdges[edge->hashValue] = edge;
			/*	cout << S("<edge") + S(" id = \"") +S(edge->id) +S("\" ") + S(" x = \"") + S(edge->x) + S("\" ")
				+ S(" y = \"") + S(edge->y) + S("\" ")
				+ S(" s = \"") + S(edge->s) + S("\" ")
				+ S(" t = \"") + S(edge->t) + S("\" ")
				+ S(" isHole = \"") + S(edge->isHole) + S("\" ")
				+ S(" isWall = \"") + S(edge->isWall) + S("\" ") + S("></edge>")<<endl;*/


				//cout<<"creating edge id " << edge->id<< " name "<<edge->name <<endl;
				//hashTarEdges.put(edge.hashValue(), edge);
			}
		}
	}

	//set adjEdge to every node
	for(Node* n:nodes){
		for(Node* neigbour:*n->getAdjNodes()){
			//if edge exit
			Edge* e = getEdge(n,neigbour);
			if(e != NULL){
				n->addAdjEdges(e);
			}

		}
	}

	//set adjEdge to every edge
	for(Edge* e:edges){
		Node* n0 = getNode(e->x,e->y);
		Node* n1 = getNode(e->s,e->t);

		for(Edge* adjEdge:*n0->getAdjEdges()){
			if(adjEdge != e){
				//e->adjEdges->push_back(adjEdge);
			}
		}

		for(Edge* adjEdge:*n1->getAdjEdges()){
			if(adjEdge != e){
				//e->adjEdges->push_back(adjEdge);
			}
		}
 	}
}



//delete walls for cut

V<E> Graph::getAdjEdges(E e){
	V<E> r;
	N n0 = getNode(e->x,e->y);
	N n1 = getNode(e->s,e->t);

	for(E adjEdge : *n0->getAdjEdges()){
		if(adjEdge == e)
			continue;
		//cout<< e->name<<" 's adj edge" << adjEdge->name<<endl;
		r.push_back(adjEdge);
	}
	for(E adjEdge : *n1->getAdjEdges()){
		if(adjEdge == e)
			continue;
		//cout<< e->name<<" 's adj edge" << adjEdge->name<<endl;
		r.push_back(adjEdge);
	}
	return r;
}

void Graph::removeNode(Node * n){
	if(!algo::vecContains(nodes,n)){
		cout<< " i have no n, so be it"<<endl;
		return;
	}
	if(n->getAdjEdges()->size()>0){
		cout<< " cannot remove this node, have adj edges"<<endl;
		exit(-1);
	}

	algo::remove(nodes,n);
	hashNodes.erase(n->hashValue);


}
bool Graph::addNode(N n){
	if(algo::mapContains(hashNodes,n->hashValue)){
		cout << n->name <<" has existed, return false"<<endl;
		return false;
	}

	nodes.push_back(n);
	hashNodes[n->hashValue] = n;

}

//if e aleady exits, return false, or return true;
bool Graph::addEdges(V<E> edgesToAdd){
	for (E e:edgesToAdd){
		addEdge(e);
	}
}
bool Graph::addEdge(E e){
	if(algo::mapContains(hashEdges,e->hashValue)){
		cout << e->name <<" has existed, return false"<<endl;
		return false;
	}
	cout<<"adding edge "<<e->name<<endl;

	N n0 = getNode(e->x,e->y);
	N n1 = getNode(e->s,e->t);



	if(n0 == NULL){
		N newN0 = new Node(e->x,e->y);
		n0 = newN0;
		addNode(newN0);
	}
	if(n1 == NULL){
		N newN1 = new Node(e->s,e->t);
		n1 = newN1;
		addNode(newN1);
	}




	n0->addAdjNodes(n1);
	n1->addAdjNodes(n0);
	edges.push_back(e);
	hashEdges[e->hashValue] = e;
	n0->addAdjEdges(e);
	n1->addAdjEdges(e);
	return true;
}


void Graph::replaceNode(N target,N newNode){
	cout<<"replace node "<<target->name<<" to "<<newNode->name<<endl;

	while(target->getAdjEdges()->size() >0){
		E adjE = target->getAdjEdges()->front();
		N otherNode = NULL;
		N n0 = getNode(adjE->x,adjE->y);
		N n1 = getNode(adjE->s,adjE->t);
		if(n0 == target)
			otherNode = n1;
		else
			otherNode = n0;

		//remove this edge
		removeEdge(adjE);

		//creat new Edge

		E newE = new Edge(otherNode,newNode);

		newE->cpySpecFrom(adjE);


		//if adjE is already new Edge, find its father
		if(algo::mapContains(newEdgeToOldEde,adjE))
			newEdgeToOldEde[newE] = newEdgeToOldEde[adjE];
		else
			newEdgeToOldEde[newE] = adjE;

		cout<<"add new edge "<<newE->name<<endl;

		//connect old node to new node
		addEdge(newE);


	}
	//remove adjEdges
	assert(target->getAdjEdges()->size() == 0);
	assert(target->getAdjNodes()->size() == 0);
	if(algo::vecContains(entrances,target)){
		algo::remove(entrances,target);
		entrances.push_back(newNode);
	}

	if(algo::vecContains(exits,target)){
		algo::remove(exits,target);
		exits.push_back(newNode);
	}




	removeNode(target);
}

void Graph::removeEdge(E e){
	if(!algo::vecContains(edges,e)){
		cout<<"no edge "<<e->name<<endl;
		return;
	}
	cout<< "removing edge "<<e->name<<endl;
	N n0 = getNode(e->x,e->y);
	N n1 = getNode(e->s,e->t);
	assert(n0 != NULL);
	assert(n1 != NULL);
	algo::remove(*n0->getAdjNodes(),n1);

	algo::remove(*n1->getAdjNodes(),n0);



	algo::remove(*n0->getAdjEdges(),e);

	algo::remove(*n1->getAdjEdges(),e);

	if(n0->getAdjEdges()->size() == 0)
		removeNode(n0);
	if(n1->getAdjEdges()->size() == 0)
		removeNode(n1);
	algo::remove(edges,e);
	hashEdges.erase(e->hashValue);



}






void Graph::moveGraph(float x, float y){
	hashNodes.clear();
	hashEdges.clear();
	for(N n:nodes){
		n->x = n->x+x;
		n->y = n->y + y;
		n->hashValue = algo::hash2Int(n->x,n->y);
		hashNodes[n->hashValue] = n;
		n->setName();

	}

	for(E e:edges){
		e->x = e->x+x;
		e->y = e->y + y;
		e->s = e->s+x;
		e->t = e->t + y;

		e->hashValue = algo::hash4Int(e->x,e->y,e->s,e->t);
		hashEdges[e->hashValue] = e;
		e->setName();
	}
}




bool Graph::nodeOnEdge(Node* n, Edge* e){
	Node* n0 = getNode(e->x,e->y);
	Node* n1 = getNode(e->s,e->t);
	assert(n0!=NULL);
	assert(n1!=NULL);
	if(n == n0)
		return true;
	if(n == n1)
		return true;

	return false;
}
void Graph::initTest(int w, int h){

	//give number to each Node; Hash nodes
	int id = 0;
	width = w;
	height = h;
	for(int i = 0; i< w; i++){
		for(int j =0; j < h; j ++){

			Node* node = new Node(i,j);
			nodes.push_back(node);
			float index = algo::hash2Int(i,j);
			//cout<<"index is "<<index<<endl;
			hashNodes[index] =  node;
			id ++;

		}
	}

	entrances.push_back(getNode(0,0));
	//entrances.push_back(getNode(0,h-1));

	//exits.push_back(getNode(w-1,0));
	exits.push_back(getNode(w-1,h-1));

	//assign joint nodes to each node
	for(int i = 0; i< w; i++){
		for(int j =0; j < h; j ++){
			//cout << "im node" << getNode(i,j)->name << endl;
			if(i>=1){
				getNode(i,j)->addAdjNodes(getNode(i-1,j));
				//cout << "adj Node:" << getNode(i-1,j)->name<<endl;
			}
			if(j>=1){
				getNode(i,j)->addAdjNodes(getNode(i,j-1));
				//cout << "adj Node:" << getNode(i,j-1)->name<<endl;
			}
			if(i<w-1){
				Node* n1 = getNode(i,j);
				Node* n2 = getNode(i+1,j);
				n1->addAdjNodes(n2);
				//cout << "adj Node:" << getNode(i,j+1)->name<<endl;
			}
			if(j<h-1){
				getNode(i,j)->addAdjNodes(getNode(i,j+1));
				//cout << "adj Node:" << getNode(i+1,j)->name<<endl;
			}
		}
	}

	hashAllEdges();


}




bool Graph::pathIntercectPaths(PATHE p, V<PATHE> paths){
	for(PATHE p0:paths){
		if(pathIntercectPath(p0,p))
			return true;
	}

	return false;
}
bool Graph::pathIntercectPath(PATHE p0, PATHE p1){
	for(E e0:p0){
		for(E e1:p1){
			if(edgeOnEdge(e0,e1))
				return true;
		}
	}

	return false;
}


bool Graph::edgeOnEdge(Edge* e0, Edge* e1){
	//same edge return false
	if(e0->x == e1->x && e0->y == e1->y && e0->s == e1->s && e0->t == e1->t)
		return false;
	if(e0->x == e1->s && e0->y == e1->t && e0->s == e1->x && e0->t == e1->y)
		return false;


	//share same node
	if(e0->x == e1->x && e0->y == e1->y)
		return true;
	if(e0->s == e1->x && e0->t == e1->y)
		return true;
	if(e0->x == e1->s && e0->y == e1->t)
		return true;
	if(e0->s == e1->s && e0->t == e1->t)
		return true;

	return false;



}


//genearting random path from s to t
bool Graph::dfsValveSharing(Node* s, Node* t, V<E>& path, V<E>& forbidEdges,V<N>& forbidNodes,map<E,E> shareValveMap){

	int dfsTryCount = 0;
	//srand(time(NULL));
	do{
		vector<Node*> searchPath;
		vector<Node*> stack;
		V<N> joints;
		stack.push_back(s);
		joints.push_back(s);

		bool findT = false;
		while(stack.size()>0){
			Node* seed = stack.back();
			N joint = joints.back();

			stack.pop_back();
			joints.pop_back();

			algo::pop_To_Element(searchPath,joint);
			searchPath.push_back(seed);

			if(seed == t){
				findT = true;
				break;
			}


			//find a random adjNode
			V<int> visitedAdjId;
			while(visitedAdjId.size()<seed->getAdjEdges()->size()){
				int randAdjNodeId = rand()%seed->getAdjEdges()->size();
				if(algo::vecContains(visitedAdjId,randAdjNodeId))
					continue;
				else
					visitedAdjId.push_back(randAdjNodeId);

				Node* adjNode = (*seed->getAdjNodes()).at(randAdjNodeId);

				if(adjNode->isBusy && adjNode != t)
					continue;

				if(algo::vecContains(forbidNodes,adjNode))
					continue;

				if(algo::vecContains(searchPath,adjNode))
					continue;

				Edge* e = getEdge(seed,adjNode);
				if(e->isBusy)
					continue;

				stack.push_back(adjNode);
				joints.push_back(seed);
			}
		}

		if(!findT){
			return false;

		}

		//findT, see if this match valve share protocal
		V<E> searchPathE = pathNodesToEdges(searchPath);
		V<E> shareEdges;
		for(E e:searchPathE){
			if(algo::mapContains(shareValveMap,e)){
				E shareEdge = shareValveMap[e];
				shareEdges.push_back(shareEdge);
			}
		}
		bool illegalValveShare = false;
		for(E e:shareEdges){
			//cout<<"shareEdges are "<<e->name <<endl;
			N n0 = getNode(e->x,e->y);
			N n1 = getNode(e->s,e->t);
			if(algo::vecContains(searchPath,n0))
				illegalValveShare = true;

			if(algo::vecContains(searchPath,n1))
				illegalValveShare = true;

			if(e->isBusy)
				illegalValveShare = true;
		}

		if(!illegalValveShare){
			path.clear();
			algo::join2Vec(path,searchPathE);
			algo::join2Vec(path,shareEdges);
			return true;
		}

		//cout<<"illegal valver sharing"<<endl;
		dfsTryCount++;

	}while(dfsTryCount < 100);


	//cout<<" no possible valve sharing path right now"<<endl;
	return false;

}

//genearting random path from s to t
bool Graph::dfs(Node* s, Node* t, V<E>& path, V<E>& forbidEdges,V<N>& forbidNodes){
	//srand(time(NULL));

	vector<Node*> searchPath;
	vector<Node*> stack;
	V<N> joints;
	stack.push_back(s);
	joints.push_back(s);

	bool findT = false;
	while(stack.size()>0){
		Node* seed = stack.back();
		N joint = joints.back();

		stack.pop_back();
		joints.pop_back();

		algo::pop_To_Element(searchPath,joint);
		searchPath.push_back(seed);

		if(seed == t){
			findT = true;
			break;
		}


		//find a random adjNode
		V<int> visitedAdjId;
		while(visitedAdjId.size()<seed->getAdjEdges()->size()){
			int randAdjNodeId = rand()%seed->getAdjEdges()->size();
			if(algo::vecContains(visitedAdjId,randAdjNodeId))
				continue;
			else
				visitedAdjId.push_back(randAdjNodeId);

			Node* adjNode = (*seed->getAdjNodes()).at(randAdjNodeId);

			if(adjNode->isBusy && adjNode != t)
				continue;

			if(algo::vecContains(searchPath,adjNode))
				continue;

			Edge* e = getEdge(seed,adjNode);
			if(e->isBusy)
				continue;

			stack.push_back(adjNode);
			joints.push_back(seed);
		}
	}

	if(findT){
		V<E> searchPathE = pathNodesToEdges(searchPath);
		path.clear();
		algo::join2Vec(path,searchPathE);
		return true;
	}
	else{
		return false;
	}

}

bool Graph::dfs(Node* s, Node* t){
	vector<Node*> searchPath;
	vector<Node*> stack;

	stack.push_back(s);

	while(stack.size()>0){
		Node* seed = stack.back();
		//cout<<"dfs seed "<< seed->name<<endl;

		searchPath.push_back(seed);
		stack.pop_back();
		for(Node* adjNode:*seed->getAdjNodes()){

			if(adjNode == t)
				return true;

			if(algo::vecContains(searchPath,adjNode))
				continue;

			Edge* e = getEdge(seed,adjNode);
			//cout<<"adj edge is "<<e->name<<endl;
			if(e->isOn){
				stack.push_back(adjNode);
			}
		}
	}

	return false;
}


Graph* Graph::fitGraphIntoGrid(){
	//locate bounding box  of otherG
	V<float> boundingbox;
	findBoundingBox(boundingbox);

	Graph* g = new Graph();
	g->initByBoundingBox(boundingbox);
	//remove edge intercect with egdgs in graph
	V<E> edgesToRemove;
	for(E newE:g->edges){
		if(newE->x == 3 && newE->y == 6 && newE->s == 3 && newE->t ==  7)
			cout <<"stop"<<endl;
		for(E e:edges){
			if(!e->intercectEdge(newE))
				continue;
			//if they share one node continue
			N n0 = g->getNode(newE->x,newE->y);
			N n1 = g->getNode(newE->s,newE->t);
			N n2 = getNode(e->x,e->y);
			N n3 = getNode(e->s,e->t);

			if(n0->sameNode(n2) || n0->sameNode(n3))
				continue;
			if(n1->sameNode(n2) || n1->sameNode(n3))
				continue;

			algo::distincAddVec(edgesToRemove,newE);
			cout << "im about remove "<< newE->name<<endl;

		}
	}
		//system("read");
	for(E eToRemove:edgesToRemove){
		N n0 = g->getNode(eToRemove->x,eToRemove->y);
		N n1 = g->getNode(eToRemove->s,eToRemove->t);
		if(getEdge(n0,n1)!=NULL)
			continue;
		g->removeEdge(eToRemove);
	}

	//add edge in orighin graph
	for(N n:nodes){
		for(N adjNode:*n->getAdjNodes()){
			if(g->getEdge(n,adjNode)!=NULL)
				continue;

			E newE = new Edge(n,adjNode);
			g->addEdge(newE);
		}
	}

	for(N head:entrances){
		N newHead = g->getNode(head->x,head->y);
		g->entrances.push_back(newHead);
	}
	for(N head:exits){
		N newExits = g->getNode(head->x,head->y);
		g->exits.push_back(newExits);
	}


	return g;

}


void Graph::findBoundingBox(V<float>& boundingbox){
	boundingbox.clear();
	float ldx,ldy,urx,ury;
	ldx = M;
	ldy = M;
	urx = -M;
	urx = -M;
	for(N n:nodes){
		if(n->x < ldx)
			ldx = n->x;
		if(n->y < ldy)
			ldy = n->y;

		if(n->x > urx)
			urx = n->x;
		if(n->y > ury)
			ury = n->y;
	}

	boundingbox.push_back(ldx);
	boundingbox.push_back(ldy);
	boundingbox.push_back(urx);
	boundingbox.push_back(ury);
}

void Graph::initByBoundingBox(V<float>& boundingBox){
	//give number to each Node; Hash nodes
	int id = 0;
	float ldx = boundingBox[0];
	float ldy = boundingBox[1];
	float rux = boundingBox[2];
	float ruy = boundingBox[3];
	for(int i = ldx; i<= rux; i++){
		for(int j = ldy; j <= ruy; j ++){

			Node* node = new Node(i,j);
			nodes.push_back(node);
			float index = algo::hash2Int(i,j);

			hashNodes[index] =  node;
			id ++;

		}
	}



	//assign joint nodes to each node
	for(int i = ldx; i<= rux; i++){
		for(int j = ldy; j <= ruy; j ++){
			//cout << "im node" << getNode(i,j)->name << endl;
			if(i>=ldx+1){
				getNode(i,j)->addAdjNodes(getNode(i-1,j));

			}
			if(j>=ldy+1){
				getNode(i,j)->addAdjNodes(getNode(i,j-1));

			}
			if(i<=rux-1){
				Node* n1 = getNode(i,j);
				Node* n2 = getNode(i+1,j);
				n1->addAdjNodes(n2);

			}
			if(j<=ruy-1){
				getNode(i,j)->addAdjNodes(getNode(i,j+1));

			}
		}
	}

	hashAllEdges();


}


bool Graph::howShareValvesMostLikeOTherPlan(const V<V<E>>& paths,const V<V<E>>& cuts,const V<E>& newEdges,V<E>& sharePlan, const V<E>& otherNewEdges,const V<E>& otherSharePlan){
	sortEdgesAndNodes();
	sharePlan.clear();
	map<E,E> otherSharePlanMap; //

	for(int i = 0; i < otherNewEdges.size();i++){
		E e = otherNewEdges[i];
		E shareE = otherSharePlan[i];
		otherSharePlanMap[e] = shareE;
	}

	//srand(time(NULL));
	map<E,int> thissharePlan;
	int count = 0;
	int maxCount = 10;
	while(true){
		if(count >= maxCount)
			return false;
		for(E e:newEdges){
			if(algo::mapContains(otherSharePlanMap,e)){
				thissharePlan[e] = otherSharePlanMap[e]->id;
			}
			else{
				thissharePlan[e] = rand()%edges.size() + 1;//cause id 0 resvered for no edge
			}

		}
		V<E> tmpSharePlan;
		for(E e:newEdges){
			int eid = thissharePlan[e];
			E shareE = getEdgeById(eid);
			tmpSharePlan.push_back(shareE);

		}
		cout<< "try share plan: "<<endl;
		for(E e:newEdges){
			cout<<e->name <<" ";

		}
		cout<<endl;
		for(E e:tmpSharePlan){

			cout<<e->name<<" ";
		}
		cout<<endl;
		bool allPass = true;
		for(V<E> path:paths){
			if(!validPathWithShareValves(path,newEdges,tmpSharePlan)){
				allPass = false;
				break;
			}
		}
		if(allPass){
			for(V<E> cut:cuts){
				if(!validCutWithShareValves(cut,newEdges,tmpSharePlan)){
					allPass = false;
					break;
				}
			}
		}

		if(allPass){
			cout<<"find share plan"<<endl;
			for(E e:newEdges){
				cout<<e->name <<" ";

			}
			cout << endl;
			for(E e:tmpSharePlan){
				sharePlan.push_back(e);
				cout<<e->name<<" ";
			}
			cout<<endl;

			return true;
		}
		count++;
	}



	return false;

}

void Graph::howShareVavles(V<V<E>>& paths, V<V<E>>& cuts, V<E>& newEdges,V<E>& sharePlan){
	sortEdgesAndNodes();


	//generating all possible share plans
	V<E> oldEdges;
	for(E e:edges){
		if(!algo::vecContains(newEdges,e))
			oldEdges.push_back(e);
	}

	int count = 0;

	Draw d;
	//d.drawPath(oldEdges,this,"oldEdges");

	//d.drawPath(newEdges,this,"newEdges");
	//srand(time(NULL));
	while(true){
		sharePlan.clear();
		int digit = count;
		for(int i = 0; i < newEdges.size();i++){
			//int j = digit % oldEdges.size();
			int j = rand()%oldEdges.size();

			E e = oldEdges[j];
			sharePlan.push_back(e);
			digit = digit/oldEdges.size();
			cout << e->name << " ";
		}

		cout<<endl;
		count++;
		if(digit > 0){
			cout<< count;
			break;
		}

		bool allPass = true;
		for(V<E> path:paths){
			if(!validPathWithShareValves(path,newEdges,sharePlan)){
				allPass = false;
				break;
			}
		}
		if(allPass){
			for(V<E> cut:cuts){
				if(!validCutWithShareValves(cut,newEdges,sharePlan)){
					allPass = false;
					break;
				}
			}
		}

		if(allPass){
			cout<<"find share plan"<<endl;
			for(E e:newEdges){
				cout<<e->name <<" ";

			}
			cout << endl;
			for(E e:sharePlan){
				cout<<e->name<<" ";
			}

			break;
		}
	}
	system("read");
}



bool Graph::validPathWithShareValves(const V<E>& path, const V<E>& newEdges, V<E>& sharePlan){
	//open all valves which are shared
	for(E e:edges){
		e->isOn = false;
	}

	for(E e:path)
		e->isOn = true;

	for(int i = 0; i<newEdges.size();i++){
		E newEdge = newEdges[i];
		E shareEdge = sharePlan[i];

		if(newEdge->isOn)
			shareEdge->isOn = true;

		if(shareEdge->isOn)
			newEdge->isOn = true;
	}


	//close one valve on the path at a time see if dfs can find target
	bool allPass = true;
	for(E e:path){
		e->isOn = false;

		if(dfs(entrances[0],exits[0])){
			allPass = false;
			break;
		}
	}

	return allPass;
}

bool Graph::validCutWithShareValves(const V<E>& cut,const V<E>& newEdges, V<E>& sharePlan){
	//open all valves which are shared
	for(E e:edges){
		e->isOn = true;
	}

	for(E e:cut)
		e->isOn = false;

	for(int i = 0; i<newEdges.size();i++){
		E newEdge = newEdges[i];
		E shareEdge = sharePlan[i];

		if(!newEdge->isOn)
			shareEdge->isOn = false;

		if(!shareEdge->isOn)
			newEdge->isOn = false;
	}


	//close one valve on the path at a time see if dfs can find target
	bool allPass = true;
	for(E e:cut){
		e->isOn = true;

		if(!dfs(entrances[0],exits[0])){
			allPass = false;
			break;
		}
	}

	return allPass;
}

void Graph::sortEdgesAndNodes(){
	for(N n:nodes){
		n->adjEdges->clear();
		n->adjNodes->clear();
	}
	for(E e:edges){
		N n0 = getNode(e->x,e->y);
		N n1 =getNode(e->s,e->t);
		assert(n0!=NULL);
		assert(n1!=NULL);
		algo::distincAddVec(*n0->adjEdges,e);
		algo::distincAddVec(*n1->adjEdges,e);
		algo::distincAddVec(*n0->adjNodes,n1);
		algo::distincAddVec(*n1->adjNodes,n0);
	}
	cout<<"finish sorting edges and nodes in graph"<<endl;
}









































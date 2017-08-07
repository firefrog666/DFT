/*
 * Graph.cpp
 *
 *  Created on: Apr 5, 2017
 *      Author: ga63quk
 */

#include "Graph.h"
#include "toolBox.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "../rapidxml-1.13/rapidxml.hpp"
#include <cstring>
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
	hashAllEdges();
	creatCells(xmlFile);

}

V<Graph*> Graph::splitGraphByAxis(vector<float> xAxis,vector<float> yAxis){

	V<Graph*> r;
	Graph* upperGraph = new Graph();
	r.push_back(upperGraph);
	map<Node*,Graph*> upperNodeToSubGraph;
	map<Graph*,Node*> subGraphToUpperNodes;
	for(int i = 0; i < xAxis.size()-1;i++){
		float lx = xAxis.at(i);
		float rx = xAxis.at(i+1);
		for(int j = 0; j < yAxis.size()-1;j++){
			float dy = yAxis.at(j);
			float uy = yAxis.at(j+1);
			Node* upperNode = new Node(i,j);
			upperGraph->nodes.push_back(upperNode);
			upperGraph->hashNodes[upperNode->hashValue] = upperNode;

			Graph* g = splitGraphByBoundingBox(lx,dy,rx,uy);
			r.push_back(g);

			upperNodeToSubGraph[upperNode] = g;
			subGraphToUpperNodes[g] =upperNode;
		}
	}

	for(Graph* subG0:r){
		N upperNode0 = subGraphToUpperNodes[subG0];
		for(Graph* subG1:r){
			if(subG0 == subG1){
				continue;
			}
			V<N> ventFromG0ToG1;
			V<N> ventFromG1ToG0;
			N upperNode1 = subGraphToUpperNodes[subG1];

			for(Node* nSub0: subG0->nodes){
				for(Node* nSub1:subG1->nodes){
					if(nSub0->x == nSub1->x && nSub0->y == nSub1->y){
						if(nSub0->getAdjNodes()->size() == 1){
							ventFromG0ToG1.push_back(nSub0);
						}
						if(nSub1->getAdjNodes()->size() == 1){
							ventFromG1ToG0.push_back(nSub1);
						}
					}
				}
			}

			subG0->superNodeToVents[upperNode1] = ventFromG0ToG1;
			subG1->superNodeToVents[upperNode0] = ventFromG1ToG0;

			if(ventFromG0ToG1.size()>0){
				upperNode0->addAdjNodes(upperNode1);
				upperNode1->addAdjNodes(upperNode0);

				Edge* upperE = new Edge(upperNode0,upperNode1);

				upperGraph->edges.push_back(upperE);
				upperGraph->hashEdges[upperE->hashValue] = upperE;
			}

		}
	}

	return r;

}



void Graph::creatCells(const char* xml){
	//getNodes
	rapidxml::xml_document<> doc;    // character type defaults to char
	ifstream theFile (xml);
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc
	doc.parse<0>(&buffer[0]);




	//initialize all edges
	int cellcount = 1;
	for (rapidxml::xml_node<> * cell = doc.first_node("cell"); cell; cell= cell->next_sibling())
	{
		if (strcmp(cell->name(), "cell") != 0)
			continue;
		Cell* c = new Cell();
		c->name = s("c") + s(cellcount);
		for (rapidxml::xml_node<> * edge = cell->first_node("edge"); edge; edge= edge->next_sibling())
		{

			int x = stringToInt(edge->first_attribute("x")->value());
			int y = stringToInt(edge->first_attribute("y")->value());
			int s = stringToInt(edge->first_attribute("s")->value());
			int t = stringToInt(edge->first_attribute("t")->value());


			Node* n0 = getNode(x,y);
			Node* n1 = getNode(s,t);

			Edge* e = getEdge(n0,n1);
			assert(e != NULL);

			c->adjEdges->push_back(e);

		}
		for(Edge* e:*c->adjEdges){
			cout<< "adding edge " << e->name << " to " <<c->name<<endl;
		}
		cells.push_back(c);
		cellcount++;

	}
	//super cell
	for (rapidxml::xml_node<> * superCellXML = doc.first_node("superCell"); superCellXML; superCellXML= superCellXML->next_sibling())
	{
		if (strcmp(superCellXML->name(), "superCell") != 0)
			continue;
		superCell = new Cell();
		superCell->name = s("c") + s(0);
		for (rapidxml::xml_node<> * edge = superCellXML->first_node("edge"); edge; edge= edge->next_sibling())
		{

			int x = stringToInt(edge->first_attribute("x")->value());
			int y = stringToInt(edge->first_attribute("y")->value());
			int s = stringToInt(edge->first_attribute("s")->value());
			int t = stringToInt(edge->first_attribute("t")->value());


			Node* n0 = getNode(x,y);
			Node* n1 = getNode(s,t);

			Edge* e = getEdge(n0,n1);
			assert(e != NULL);

			superCell->adjEdges->push_back(e);

		}
		for(Edge* e:*superCell->adjEdges){
			cout<< "adding edge " << e->name << " to " <<superCell->name<<endl;
		}
		for(Edge* e:*superCell->adjEdges){
			cout<< "adding super edge " << e->name << " to " <<superCell->name<<endl;
		}
	}



}
void Graph::creatNodes(const char* nodes_xml){
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

		int x = stringToInt(node->first_attribute("x")->value());
		int y = stringToInt(node->first_attribute("y")->value());
		Node* n = new Node(x,y);
		n->id = stringToInt(node->first_attribute("id")->value());
		hashNodes[n->hashValue] = n;
		nodes.push_back(n);
		IdTonode[n->id] = n;
	}

	for (rapidxml::xml_node<> * node = root->first_node("node"); node; node = node->next_sibling())
	{

		int id = stringToInt(node->first_attribute("id")->value());
		Node* thisNode = IdTonode[id];
		int adjNode0 = stringToInt(node->first_attribute("adjNode0")->value());
		int adjNode1 = stringToInt(node->first_attribute("adjNode1")->value());
		int adjNode2 = stringToInt(node->first_attribute("adjNode2")->value());
		int adjNode3 = stringToInt(node->first_attribute("adjNode3")->value());
		if(adjNode0 >= 0){
			thisNode->addAdjNodes(IdTonode[adjNode0]);
		}
		if(adjNode1 >= 0){
			thisNode->addAdjNodes(IdTonode[adjNode1]);
		}
		if(adjNode2 >= 0){
			thisNode->addAdjNodes(IdTonode[adjNode2]);
		}
		if(adjNode3 >= 0){
			thisNode->addAdjNodes(IdTonode[adjNode3]);
		}
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


void Graph::sortEdgesInPath(vector<Edge*>* pathToSort){

	vector<Edge*> visitedEdge;
	vector<Edge*> sortedEdges;
	Edge* e = pathToSort->back();
	pathToSort->pop_back();
	sortedEdges.push_back(e);
	Node* firstNode = getNode(e->x,e->y);
	Node* lastNode = getNode(e->s,e->t);
	while(pathToSort->size()!=0){
		Edge* targetEdge = NULL;
		for(Edge* e:*pathToSort){

			if(algo::vecContains(visitedEdge,e))
				continue;

			Node* ln = getNode(e->x,e->y);
			Node* rn = getNode(e->s,e->t);

			if(ln == firstNode){
				visitedEdge.push_back(e);
				sortedEdges.insert(sortedEdges.begin(),e);
				firstNode = rn;
				targetEdge = e;
				break;
			}
			if(ln == lastNode){
				visitedEdge.push_back(e);
				sortedEdges.push_back(e);
				targetEdge = e;
				lastNode = rn ;
				break;
			}
			if(rn == firstNode){
				visitedEdge.push_back(e);
				sortedEdges.insert(sortedEdges.begin(),e);
				targetEdge = e;
				firstNode = ln;
				break;
			}
			if(rn == lastNode){
				visitedEdge.push_back(e);
				sortedEdges.push_back(e);
				targetEdge = e;
				lastNode = ln;
				break;
			}
		}

		//must find a edge
		assert(targetEdge != NULL);
		algo::remove(*pathToSort,targetEdge);
	}

	//
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
		assert(e != NULL);

		r.push_back(e);
	}
	return r;
}

vector<Node*> Graph::edgesToNodes(const vector<Edge*>* sortedPath){
	vector<Node*> result;
	//no edge
	assert(sortedPath->size()>0);
	//1 edge
	if(sortedPath->size() == 1){
		Edge* e = sortedPath->front();
		result.push_back(getNode(e->x,e->y));
		result.push_back(getNode(e->s,e->t));
		return result;
	}

	//2 or more edges
	Edge* firstEdge = sortedPath->front();
	for(Edge* secondEdge: *sortedPath){
		if(firstEdge == secondEdge)
			continue;
		Node* fa = getNode(firstEdge->x,firstEdge->y);
		Node* fb = getNode(firstEdge->s,firstEdge->t);
		Node* sa = getNode(secondEdge->x,secondEdge->y);
		Node* sb = getNode(secondEdge->s,secondEdge->t);

		if(fa == sa){
			algo::distincAddVec(result,fb);
			algo::distincAddVec(result,sa);
			algo::distincAddVec(result,sb);

		}
		else if(fa == sb){
			algo::distincAddVec(result,fb);
			algo::distincAddVec(result,sb);
			algo::distincAddVec(result,sa);
		}
		else if(fb == sa){
			algo::distincAddVec(result,fa);
			algo::distincAddVec(result,sa);
			algo::distincAddVec(result,sb);
		}
		else if(fb == sb){
			algo::distincAddVec(result,fa);
			algo::distincAddVec(result,sb);
			algo::distincAddVec(result,sa);
		}
		else{
			assert(!"two edges are not adjacent");
		}

		firstEdge = secondEdge;
	}

	return result;



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

	//cout<<"edge "<< x << " "<< y <<" "<<s<<" "<<t<<" "<< "has been created"<< endl;
	//cout<<"hashValue is "<<hashValue<<endl;
	//cout <<"edge name = "<<name<<endl;



}

bool Graph::edgeOnBrink(Edge* e,vector<Cell*> cells){
	int cellContainEdsgeCount = 0;
	for(Cell* c:cells){
		if(!algo::vecContains(*c->adjEdges,e))
			continue;
		cellContainEdsgeCount++;
		if(c->isOpen())
			return true;
	}

	//if only one cell contain this edge, its also an open edge
	assert(cellContainEdsgeCount > 0);
	if(cellContainEdsgeCount == 1)
		return true;


	return false;
}

bool Graph::nodeOnTheBrink(Node* node){
	int nbrCount = 0;
	nbrCount = node->getAdjNodes()->size();


	int nbrHas2Nbr = 0;

	if(nbrCount == 4){
		for(Node* nbr:*node->getAdjNodes()){
			int nbrNbrCount = 0;
			nbrNbrCount = nbr->getAdjNodes()->size();

			if(nbrNbrCount<=3)
				nbrHas2Nbr++;
		}
		if(nbrHas2Nbr >= 2){
			return true;
		}
		else
			return false;
	}
	else
		return true;
}

vector<Node*> Graph::outSideNodes(){
	vector<Node*> results;
	vector<Cell*> cells = getCells();
	for(Edge* e:edges){
		if(edgeOnBrink(e,cells)){
			Node* a = getNode(e->x,e->y);
			Node* b = getNode(e->s,e->t);
			assert(a != NULL);
			assert(b != NULL);
			algo::distincAddVec(results,a);
			algo::distincAddVec(results,b);
		}
	}
	return results;
}
vector<Node*> Graph::outSideWalk(Node* initNode){
	vector<Cell*> cells =getCells();
	vector<Node*> nodesOnBrink;

	Node* lastNode = NULL;
	Node* thisNode = initNode;

	do{
		algo::distincAddVec(nodesOnBrink,thisNode);
		for(Node* nbr : *thisNode->getAdjNodes()){
			if(nbr == lastNode)
				continue;
		/*	if(algo::vecContains(nodesOnBrink,nbr))
				continue;*/

			Edge* nbrEdge = getEdge(thisNode,nbr);
			assert(nbrEdge != NULL);

			if(edgeOnBrink(nbrEdge,cells)){
				lastNode = thisNode;
				thisNode = nbr;
				break;
			}
		}
	}while((thisNode != initNode));

	return nodesOnBrink;

}

Cell* Graph::getSuperCell(V<Cell*> cells){
	V<E> lonelyEdges;

	for(E e:edges){
		int parent = 0;
		for(Cell* c:cells){
			if(algo::vecContains(*c->adjEdges,e)){
				parent++;
			}
		}

		if(parent<=1){
			lonelyEdges.push_back(e);
		}
	}

	Cell* r = new Cell();
	r->name = s("c") + S(0);
	if(lonelyEdges.size()>0){
		r->adjEdges->insert(r->adjEdges->end(),lonelyEdges.begin(),lonelyEdges.end());
	}

	return r;
}

vector<Cell*> Graph::getCells(){
	vector<Cell*> cells;
	int counter = 1;
	for(Node* n:nodes){
		Cell* c = new Cell();
		c->name = s("c") + S(counter);
		c->x = n->x;
		c->y = n->y;
		//adjNodes
		for(int x = 0; x < 2; x++){
			for(int y = 0; y < 2; y++){
				Node* adjNode = getNode(c->x + x, c->y +y);
				if(adjNode != NULL){
					algo::distincAddVec(*c->adjNodes,adjNode);
				}
			}
		}

		//adjEdges;
		for(Node* n0:*c->adjNodes){
			for(Node* n1:*c->adjNodes){
				if(n0 == n1)
					continue;
				Edge* adjEdge = getEdge(n0->x,n0->y,n1->x,n1->y);
				if(adjEdge != NULL)
					algo::distincAddVec(*c->adjEdges,adjEdge);
			}
		}
		algo::distincAddVec(cells,c);
		counter++;
	}



	return cells;
}
vector<Node*> Graph::outSideWalk(Node* initNode, int steps){
	vector<Node*> nodesOnBrink;

	Node* lastNode = NULL;
	Node* thisNode = initNode;
	int stepCover = 0;
	while(stepCover < steps){
		stepCover++;
		algo::distincAddVec(nodesOnBrink,thisNode);
		for(Node* nbr : *thisNode->getAdjNodes()){
			if(nbr == lastNode)
				continue;

			if(algo::vecContains(nodesOnBrink,nbr))
				continue;

			Edge* nbrEdge = getEdge(thisNode,nbr);
			assert(nbrEdge != NULL);
			vector<Cell*> cells =getCells();
			if(edgeOnBrink(nbrEdge,cells)){
				lastNode = thisNode;
				thisNode = nbr;
				break;
			}
		}
	}


	return nodesOnBrink;
}


Graph* Graph::splitGraphByBoundingBox(float ld_x, float ld_y, float ru_x, float ru_y){
	Graph* result;
	vector<Node*> nodesInBox;
	//add nodes in this bounding box
	for(Node* n:nodes){
		if(n->x >= ld_x && n->y >= ld_y && n->x <= ru_x && n->y <= ru_y)
			nodesInBox.push_back(n);
	}

	result = new Graph(nodesInBox,this);

	return result;
}



vector<vector<Edge*>*> Graph::connectPaths(vector<vector<Edge*>*>& p1Set, vector<vector<Edge*>*>& p2Set){

	vector<vector<Edge*>*> result;

	vector<int> usedPathInP1;
	vector<int> usedPathInP2;
	map<int,int> p1containsEdge;
	map<int,int> p2containsEdge;


	//for each path in p1, find a path in p2, connect them
	while(p1Set.size() > usedPathInP1.size()){
		int itP1 = 0;
		while(algo::vecContains(usedPathInP1,itP1)){
			itP1++;
		}

		Edge* sharedEdge;
		vector<Edge*>* p1 = p1Set.at(itP1);
		usedPathInP1.push_back(itP1);

		bool findP1InP2Set = false;
		for(int itP2 = 0; itP2<p2Set.size();itP2++){
			if(algo::vecContains(usedPathInP2,itP2))
				continue;

			vector<Edge*>* p2 = p2Set.at(itP2);

			if(pathShareSameEdge(p1,p2)!=NULL){
				sharedEdge = pathShareSameEdge(p1,p2);
				vector<Edge*>* p = new vector<Edge*>();

				*p = connectPaths(*p1,*p2);
				result.push_back(p);

				p2containsEdge[sharedEdge->hashValue] = itP2;
				findP1InP2Set = true;
				usedPathInP2.push_back(itP2);
				break;
			}
		}
		if(!findP1InP2Set){
			int itP2 = p2containsEdge[sharedEdge->hashValue];
			vector<Edge*>* p2 = p2Set.at(itP2);
			vector<Edge*>* p = new vector<Edge*>();
			*p = connectPaths(*p1,*p2);
			result.push_back(p);
		}
	}


	//for each path in p1, find a path in p2, connect them
	while(p2Set.size() > usedPathInP2.size()){
		int itP2 = 0;
		while(algo::vecContains(usedPathInP2,itP2)){
			itP2++;
		}

		Edge* sharedEdge;
		vector<Edge*>* p2 = p2Set.at(itP2);
		usedPathInP1.push_back(itP2);

		bool findP2InP1Set = false;
		for(int itP1 = 0; itP1<p1Set.size();itP1++){
			if(algo::vecContains(usedPathInP1,itP1))
				continue;

			vector<Edge*>* p1 = p1Set.at(itP1);

			if(pathShareSameEdge(p2,p1)!=NULL){
				sharedEdge = pathShareSameEdge(p2,p1);
				vector<Edge*>* p = new vector<Edge*>();

				*p = connectPaths(*p2,*p1);
				result.push_back(p);

				p2containsEdge[sharedEdge->hashValue] = itP2;
				findP2InP1Set = true;
				usedPathInP1.push_back(itP1);
				break;
			}
		}
		if(!findP2InP1Set){
			int itP1 = p1containsEdge[sharedEdge->hashValue];
			vector<Edge*>* p1 = p1Set.at(itP1);
			vector<Edge*>* p = new vector<Edge*>();
			*p = connectPaths(*p1,*p2);
			result.push_back(p);
		}
	}

	return result;
}

Edge* Graph::pathShareSameEdge(vector<Edge*>* p1, vector<Edge*>* p2){
	if(p1->front() == p2->front())
		return p1->front();
	else if(p1->back() == p2->front())
		return p1->back();
	else if(p1->front() == p2->back())
		return p1->front();
	else if(p1->back() == p2->back())
		return p1->back();
	else
		return NULL;
}

vector<Edge*> Graph::connectPaths(vector<Edge*> path1, vector<Edge*> path2){

	Edge* p1Head = path1.front();
	Edge* p1Tail = path1.back();
	Edge* p2Head = path2.front();
	Edge* p2Tail = path2.back();

	if(Edge::sameEdge(p1Head,p2Head)){
		reverse(path1.begin(),path1.end());
		path1.pop_back();
	}
	else if(Edge::sameEdge(p1Tail,p2Head)){
		path1.pop_back();
	}
	else if(Edge::sameEdge(p1Head,p2Tail)){
		reverse(path1.begin(),path1.end());
		path1.pop_back();
		reverse(path2.begin(),path2.end());
	}
	else if(Edge::sameEdge(p1Tail,p2Tail)){
		path1.pop_back();
		reverse(path2.begin(),path2.end());
	}

	for(Edge* e:path2){
		path1.push_back(e);
	}

	return path1;
}



vector<Node*> Graph::getHoleNodes(Node* seed){
	vector<Node*> holeNodes;
	vector<Node*> newHoleNodes;

	vector<Node*> queue	;
	queue.push_back(seed);


	while(queue.size()>0){

		Node* n = queue.front();
		queue.erase(queue.begin());
		for(Node* nbr :* n->getAdjNodes()){
			if(algo::vecContains(holeNodes,nbr))
				continue;
			Edge* nbrEdge = getEdge(n,nbr);
			assert(nbrEdge!=NULL);

			if(nbrEdge->isHole){
				holeNodes.push_back(nbr);
				queue.push_back(nbr);
			}
		}

	}

	return holeNodes;
}



//find a set of walls to set apart one graph into several graphs
vector<Graph*> Graph::splitGraph(int graphNum, unsigned int eachGraphValveSize){

	vector<Graph*> result;



	//Node* initNode = getNode(1,0);
	Node* initNode = getLeftCorner();
	//set vent for each graph


	vector<vector<Node*>*> initNodesofGraph;
	vector<int> ventGaps;
	//calculate perimeter of the graph
	vector<Node*> brinkNodes = outSideWalk(initNode);
	int primeter = brinkNodes.size();
	int firstGap = primeter/(graphNum);
	int lastGap = primeter - firstGap * (graphNum-1);
	for(int i = 0; i<graphNum; i++){
		vector<Node*>* temp = new vector<Node*>();
		initNodesofGraph.push_back(temp);
		if(i<graphNum-1){
			ventGaps.push_back(firstGap);
			for(int i = 0; i < firstGap;i++){
				temp->push_back(brinkNodes.front());
				brinkNodes.erase(brinkNodes.begin());
			}
		}
		else{
			for(int i = 0; i < lastGap;i++){
				ventGaps.push_back(lastGap);
				temp->push_back(brinkNodes.front());
				brinkNodes.erase(brinkNodes.begin());
			}
		}
	}


	//set entrances and exits for each subGraph
	vector<Node*> entrancesSubGraphs;
	vector<Node*> exitsSubGraphs;
	for(vector<Node*>* initNodes:initNodesofGraph){
		Node* entrance = initNodes->front();
		Node* ha = initNodes->back();

		entrancesSubGraphs.push_back(entrance);
		exitsSubGraphs.push_back(ha);
	}



	//bread first search for each path
	vector<bool> graphExtendFlags;
	for(int i = 0; i < graphNum; i++){
		graphExtendFlags.push_back(true);
	}
	bool needExpand = true;
	do{
		for(int i = 0; i < graphNum; i++){
			if(graphExtendFlags.at(i) == false)
				continue;

			vector<Node*>* initNodes = initNodesofGraph.at(i);
			//expand it!
			vector<Node*> nodesToAdd;
			nodesToAdd.clear();
			for(Node* n:*initNodes){
				vector<Node*> holeNodes; //contain all nodes in a hole, add them all!


				//node is not on the brink of a hole
				for(Node* nbr:* n->getAdjNodes()){

					if(find(initNodes->begin(), initNodes->end(), nbr) != initNodes->end()){
						continue;
					}

					bool nbrIsInOtherGraph = false;
					for(int j = 0; j < graphNum; j++){
						if(j == i )
							continue;
						vector<Node*>* nodesInOtherGraph = initNodesofGraph.at(j);
						if(find(nodesInOtherGraph->begin(), nodesInOtherGraph->end(), nbr) != nodesInOtherGraph->end()){
							nbrIsInOtherGraph = true;
							break;
						}
					}

					if(nbrIsInOtherGraph)
						continue;




					//when you want to push in a neighbor
					if(find(nodesToAdd.begin(), nodesToAdd.end(), nbr) == nodesToAdd.end()){
						nodesToAdd.push_back(nbr);

						//check if its neighbor is a whole
						//if is hole
						for(Node* nbrnbr:*nbr->getAdjNodes()){
							Edge* nbrnbrEdge = getEdge(nbrnbr,nbr);
							if(nbrnbrEdge->isHole){
								holeNodes = getHoleNodes(nbr);
								for(Node* holeNode:holeNodes){
									//in its own nodes, dont add
									if(algo::vecContains(*initNodes,holeNode)){
										continue;
									}

									//in other sub Graphs, dont add
									bool holeNodeIsInOtherGraph = false;
									for(int j = 0; j < graphNum; j++){
										if(j == i )
											continue;
										vector<Node*>* nodesInOtherGraph = initNodesofGraph.at(j);
										if(algo::vecContains(*nodesInOtherGraph,holeNode)){
											holeNodeIsInOtherGraph = true;
											break;
										}
									}

									if(holeNodeIsInOtherGraph)
										continue;
									//add
									if(!algo::vecContains(nodesToAdd,holeNode)){
										nodesToAdd.push_back(holeNode);
									}
								}
								continue;
							}
						}

					}
				}
			}


			//no nodes could be added
			if(nodesToAdd.size() == 0){
				graphExtendFlags.at(i) = false;
				continue;
			}

			for(Node* n:nodesToAdd){
				initNodes->push_back(n);
			}

			//calculate the size of the subGraph
			int subGraphValveSize = 0;
			for(Node* n:*initNodes){
				for(Node* nbr:*n->getAdjNodes()){
					if(algo::vecContains(*initNodes,nbr)){
						Edge* e = getEdge(n,nbr);
						if(!e->isHole)
							subGraphValveSize++;
					}
				}
			}
			subGraphValveSize = subGraphValveSize/2;

			if(subGraphValveSize >= eachGraphValveSize)
				graphExtendFlags.at(i) = false;
		}

		needExpand = false;
		for(bool extend:graphExtendFlags){
			if(extend == true){
				needExpand = true;
				break;
			}
		}

	}while(needExpand);


	//creat each subGraph
	for(vector<Node*>* initNodes:initNodesofGraph){
		Graph* subGraph = new Graph(*initNodes,this);


		//heads and tails
		Node* etranceInMainGraph = entrancesSubGraphs.front();
		entrancesSubGraphs.erase(entrancesSubGraphs.begin());
		Node* exitInMainGraph = exitsSubGraphs.front();
		exitsSubGraphs.erase(exitsSubGraphs.begin());

		Node* entranceInSubGraph = subGraph->getNode(etranceInMainGraph->x,etranceInMainGraph->y);
		Node* exitInSubGraph = subGraph->getNode(exitInMainGraph->x,exitInMainGraph->y);
		assert(entranceInSubGraph != NULL);
		assert(exitInSubGraph != NULL);

		subGraph->entrances.push_back(entranceInSubGraph);
		subGraph->exits.push_back(exitInSubGraph);

		result.push_back(subGraph);
	}
	return result;


}

void Graph::hashAllEdges(){
	for(Node* node:nodes){
		auto adjNodes = node->getAdjNodes();
		for(Node* adjNode: *adjNodes){

			if(!getEdge(adjNode,node)){
				Edge* edge = new Edge(node,adjNode);

				edges.push_back(edge);
				hashEdges[edge->hashValue] = edge;

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
				e->adjEdges->push_back(adjEdge);
			}
		}

		for(Edge* adjEdge:*n1->getAdjEdges()){
			if(adjEdge != e){
				e->adjEdges->push_back(adjEdge);
			}
		}
 	}
}


void Graph::removeEdge(Edge* e){

	if(e == NULL)
		return;

	Node* a = getNode(e->x,e->y);
	Node* b = getNode(e->s,e->t);

	algo::remove(*a->getAdjEdges(),e);
	algo::remove(*b->getAdjEdges(),e);
	algo::remove(*a->getAdjNodes(),b);
	algo::remove(*b->getAdjNodes(),a);


	auto it = find(edges.begin(),edges.end(),e);
	edges.erase(it);

	auto hashIt = hashEdges.find(algo::hash4Int(e->x,e->y,e->s,e->t));
	hashEdges.erase(hashIt);

}

void Graph::removeNode(Node* n){
	//remove all the edges attached to it
	while(n->getAdjNodes()->size()>0){
		Node* nbr = n->getAdjNodes()->back();
		n->getAdjNodes()->pop_back();

		Edge* e = getEdge(n,nbr);
		if(e != NULL)
			removeEdge(e);
	}
	if(n != NULL)
		algo::remove(nodes,n);

	auto hashIt = hashNodes.find(algo::hash2Int(n->x,n->y));
	hashNodes.erase(hashIt);
}

void Graph::cutOffSubGraph(Graph* subGraph){
	//cut off nodes
	for(Node* subNode:subGraph->nodes){
		Node* mainGraphNode = getNode(subNode->x,subNode->y);
		removeNode(mainGraphNode);
	}
}

vector<Edge*> Graph::getWallsBetweenSubGrapsh(vector<Graph*>* subGraphs){


	vector<Edge*> walls;
	//outside walk for each Graph to get all the brink
	for(Graph* subGraph: *subGraphs){
		vector<Node*> brinkNode = subGraph->outSideNodes();
		//get walls in the main graph
		for(Node* n:brinkNode){
			Node* nodeInMainGraph = this->getNode(n->x,n->y);

			assert(nodeInMainGraph!=NULL);

			for(Edge* edgeInMainGraph:*nodeInMainGraph->getAdjEdges()){
				float x = edgeInMainGraph->x;
				float y = edgeInMainGraph->y;
				float s = edgeInMainGraph->s;
				float t = edgeInMainGraph->t;
				Edge* edgeInSubGraph = subGraph->getEdge(x,y,s,t);
				if(edgeInSubGraph == NULL){
					algo::distincAddVec(walls,edgeInMainGraph);
				}
			}

		}

	}



	return walls;
}

Graph* Graph::fromGraphToChip(){
	Graph* chip = new Graph();

	//create joints
	for(Node* n:nodes){
		float l = n->x - 0.5;
		float d = n->y - 0.5;
		float r = n->x + 0.5;
		float u = n->y + 0.5;

		float ld = algo::hash2Int(l,d);
		float lu = algo::hash2Int(l,u);
		float rd = algo::hash2Int(r,d);
		float ru = algo::hash2Int(r,u);

		if(!algo::mapContains(chip->hashNodes,ld)){
			Node* ldJoint = new Node(l,d);
			chip->hashNodes[ld] = ldJoint;
			chip->nodes.push_back(ldJoint);
		}
		if(!algo::mapContains(chip->hashNodes,lu)){
			Node* luJoint = new Node(l,u);
			chip->hashNodes[lu] = luJoint;
			chip->nodes.push_back(luJoint);
		}
		if(!algo::mapContains(chip->hashNodes,rd)){
			Node* rdJoint = new Node(r,d);
			chip->hashNodes[rd] = rdJoint;
			chip->nodes.push_back(rdJoint);
		}
		if(!algo::mapContains(chip->hashNodes,ru)){
			Node* ruJoint = new Node(r,u);
			chip->hashNodes[ru] = ruJoint;
			chip->nodes.push_back(ruJoint);
		}
	}

	//find neigbour nodes
	for(Node* n:chip->nodes){
		float x = n->x;
		float y = n->y;

		float d = y - 1;
		float u = y + 1;
		float l = x - 1;
		float r = x + 1;

		Node* ln = chip->getNode(l,y);
		Node* rn = chip->getNode(r,y);
		Node* un = chip->getNode(x,u);
		Node* dn = chip->getNode(x,d);

		if(ln != NULL)
			n->getAdjNodes()->push_back(ln);
		if(rn != NULL)
			n->getAdjNodes()->push_back(rn);
		if(un != NULL)
			n->getAdjNodes()->push_back(un);
		if(dn != NULL)
			n->getAdjNodes()->push_back(dn);

	}

	//create valves
	for(Node* n0:chip->nodes){
		for(Node* n1:*n0->getAdjNodes()){
			Edge* valve = new Edge(n0,n1);
			if(algo::mapContains(chip->hashEdges,valve->hashValue)){
				delete valve;
				continue;
			}

			n0->getAdjEdges()->push_back(valve);
			n1->getAdjEdges()->push_back(valve);


			Edge* intercectedEdge = NULL;
			for(Edge* e:edges){
				if(valve->intercectEdge(e))
					intercectedEdge = e;
			}

			if(intercectedEdge == NULL){
				chip->hashEdges[valve->hashValue] = valve;
				chip->edges.push_back(valve);

				brinkEdges.push_back(valve);
				valve->isWall = true;
				valve->isHole = false;
				valve->isOn = false;
				valve->sa0 = false;
				valve->sa1 = false;


			}
			else{
				if(intercectedEdge->isHole)
					cout<< "i find a hole!"<<endl;
				insideEdges.push_back(valve);
				chip->hashEdges[valve->hashValue] = valve;
				chip->edges.push_back(valve);
				valve->isWall  = intercectedEdge->isWall;
				valve->isHole = intercectedEdge->isHole;
				valve->isOn = intercectedEdge->isOn;
				valve->sa0 = intercectedEdge->sa0;
				valve->sa1 = intercectedEdge->sa1;
			}

		}
	}

	//add head and tails for cuts
	//assume only one head and one tail
	N head = entrances.at(0);
	N tail = exits.at(0);
	V<N> nodesOnBrink =outSideWalk(head);
	V<N> nodesOnLeftBrink;
	V<N> nodesOnRightBrink;
	int tailPos = 0;
	for(int i = 0;;i++){
		N n = nodesOnBrink.at(i);
		if(n == tail){
			nodesOnLeftBrink.push_back(tail);
			break;
		}

		nodesOnLeftBrink.push_back(n);
		tailPos++;
	}

	for(int i = tailPos; i<nodesOnBrink.size();i++){
		N n = nodesOnBrink.at(i);
		nodesOnRightBrink.push_back(n);
	}
	nodesOnRightBrink.push_back(head);

	V<E> edgeOnLeftBrink  = pathNodesToEdges(nodesOnLeftBrink);
	V<E> edgeOnRightBrink  = pathNodesToEdges(nodesOnRightBrink);

	V<N> outSideJoint = chip->outSideNodes();

	for(E e_graph:edgeOnLeftBrink){
		for(E valve:chip->edges){
			if(e_graph->intercectEdge(valve)){
				N n0 = chip->getNode(valve->x,valve->y);
				N n1 = chip->getNode(valve->s,valve->t);

				if(algo::vecContains(outSideJoint,n0)){
					chip->entrances.push_back(n0);
				}

				if(algo::vecContains(outSideJoint,n1)){
					chip->entrances.push_back(n1);
				}
			}
		}
	}

	for(E e_graph:edgeOnRightBrink){
		for(E valve:chip->edges){
			if(e_graph->intercectEdge(valve)){
				N n0 = chip->getNode(valve->x,valve->y);
				N n1 = chip->getNode(valve->s,valve->t);

				if(algo::vecContains(outSideJoint,n0)){
					chip->exits.push_back(n0);
				}

				if(algo::vecContains(outSideJoint,n1)){
					chip->exits.push_back(n1);
				}
			}
		}
	}


	return chip;

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

	for(int i = 0; i< w; i++){
		for(int j =0; j < h; j ++){
			if(i == 2 && j ==9)
				cout<<"stop"<<endl;
			Node* node = new Node(i,j);
			nodes.push_back(node);
			float index = algo::hash2Int(i,j);
			cout<<"index is "<<index<<endl;
			hashNodes[index] =  node;
			id ++;

		}
	}

	entrances.push_back(getNode(0,0));
	//entrances.push_back(getNode(0,h-1));

	exits.push_back(getNode(w-1,0));
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

		V<Cell*> allcells = getCells();
		//is cell is open, drop it
		for(Cell* c:allcells){
			if(c->isOpen())
				continue;
			cells.push_back(c);
		}


}

vector<vector<Edge*>> Graph::getPathsFromEdges(vector<Edge*> edgeSet){
	vector<vector<Edge*>> groups = getGroupsFromEdges(edgeSet);


	vector<vector<Edge*>> paths;
	for(Node* head:entrances){
		for(vector<Edge*> group:groups){
			for(Edge* groupE:group){
				if(nodeOnEdge(head,groupE)){
					paths.push_back(group);
					break;
				}
			}
		}
	}

	return paths;
}

vector<Node*> Graph::getNodesInOrOnAllRings(vector<vector<Edge*>> allRings){
	vector<Node*> nodesInsideRings;
	for(vector<Edge*> ring:allRings){
		vector<Node*> nodesInsideRing = getNodesInRing(ring);
		vector<Node*> nodesOnRing = getNodesOnRing(ring);
		nodesInsideRings.insert(nodesInsideRings.end(),nodesInsideRing.begin(),nodesInsideRing.end());
		nodesInsideRings.insert(nodesInsideRings.end(),nodesOnRing.begin(),nodesOnRing.end());
	}
	return nodesInsideRings;
}

vector<Node*> Graph::getNodesOutAllRings(vector<vector<Edge*>> allRings){
	vector<Node*> nodesOutRings;
	vector<Node*> nodesInsideRings = getNodesInOrOnAllRings(allRings);

	for(Node* n:nodes){
		if(algo::vecContains(nodesInsideRings,n))
			continue;

		algo::distincAddVec(nodesOutRings,n);
	}


	return nodesOutRings;
}

V<PATHE> Graph::fromStepToShortedPath(map<N,int>steps, V<N> starts,V<N> disabledNodes){
	V<PATHE> shortedPaths;
	V<N> neareastNodes;

	int near = 10000;
	for(N n:starts){
		if (steps[n] < near){
			near = steps[n];
		}
	}

	for(N n: starts){
		if (steps[n] == near){
			neareastNodes.push_back(n);
			cout<<"nearest nodes are " <<n->name<<endl;
		}

	}

	for(N n:neareastNodes){
		V<PATHE> paths = fromStepToPath(steps,n,disabledNodes);
		 shortedPaths.insert(shortedPaths.end(),paths.begin(),paths.end());
	}

	return shortedPaths;
}

vector<vector<Edge*>> Graph::fromStepToPath(map<Node*,int>steps, Node* start,V<N> disabledNodes){
	vector<vector<Edge*>> r;
	Node* sential = new Node();
	vector<Node*> stack;
	vector<Node*> divergeNodes;
	vector<Node*> path;

	stack.push_back(start);
	divergeNodes.push_back(sential);

	while(stack.size()>0){
		Node* divergeNode = divergeNodes.back(); divergeNodes.pop_back();
		Node* n = stack.back();
		stack.pop_back();

		algo::pop_To_Element(path,divergeNode);
		path.push_back(n);

		//finding end
		if(steps[n] == 0){
			vector<Edge*> pathEdges = pathNodesToEdges(path);
			r.push_back(pathEdges);
			continue;
		}

		for(Node* adjNode:*n->getAdjNodes()){
			Edge* e = getEdge(n,adjNode);
			if(e == NULL){
				continue;
			}

			if(algo::vecContains(disabledNodes,adjNode))
				continue;
			if(algo::vecContains(path,adjNode))
				continue;

			if(steps[adjNode] != steps[n] - 1)
				continue;

			stack.push_back(adjNode);
			divergeNodes.push_back(n);
		}

	}

	delete sential;
	return r;
}


//ring to ring shorted paths
V<PATHE> Graph::ringToRingPaths(PATHE ring0, PATHE ring1, V<PATHE> allRings){
	map<N,int> steps = ringToOtherRings(ring0,allRings);
	V<N> nodesOnRing0 = getNodesOnRing(ring0);
	V<N> nodesOnRing1 = getNodesOnRing(ring1);
	V<N> nodesOnAllRings;
	V<N> nodesOnOherRings;
	for(PATHE ring:allRings){
		V<N> nodesOnRing = getNodesOnRing(ring);
		nodesOnAllRings.insert(nodesOnAllRings.end(),nodesOnRing.begin(),nodesOnRing.end());
	}

	for(N n:nodesOnAllRings){
		if(algo::vecContains(nodesOnRing0,n))
			continue;
		if(algo::vecContains(nodesOnRing1,n))
			continue;

		nodesOnOherRings.push_back(n);
	}

	V<PATHE> shortedPath = fromStepToShortedPath(steps,nodesOnRing1,nodesOnOherRings);
	return shortedPath;

}
map<Node*,int> Graph::ringToOtherRings(vector<Edge*> ring,vector<vector<Edge*>> allRings){
	vector<Node*> possibleNodes;

	possibleNodes = getNodesOutAllRings(allRings);
	vector<Node*> nodesOnOtherRings;
	for(vector<Edge*> otherRing:allRings){
		if(otherRing == ring )
			continue;
		vector<Node*> nodesOnOtherRing = getNodesOnRing(otherRing);
		nodesOnOtherRings.insert(nodesOnOtherRings.end(),nodesOnOtherRing.begin(),nodesOnOtherRing.end());
	}
	//bread first search, mark every node move
	vector<Node*> nodesOnRing = getNodesOnRing(ring);


	map<Node*,int> steps;
	for(Node* n:nodes){
		steps[n] = 10000;
	}



	vector<Node*> nodesToTouch;
	for(Node* n:nodesOnRing){
		steps[n] = 0;
		nodesToTouch.push_back(n);
	}

	while(nodesToTouch.size() > 0){
		Node* n = nodesToTouch.front();
		algo::pop_front(nodesToTouch);

		for(Node* adjNode:*n->getAdjNodes()){
			//if closed edge
			Edge* e = getEdge(n,adjNode);
			if(e == NULL)
				continue;

			if(e->isWall)
				continue;

			if(algo::vecContains(nodesOnOtherRings,adjNode)){
				if(steps[adjNode] > steps[n]){
					steps[adjNode] = steps[n] +1;
				}
			}

			if(!algo::vecContains(possibleNodes,adjNode))
				continue;

			if(steps[adjNode] <= steps[n])
				continue;
			//
			steps[adjNode] = steps[n]+1;
			nodesToTouch.push_back(adjNode);

		}

	}


	return steps;

}

/**/



map<pair<PATHE,PATHE>,V<PATHE>> Graph::allRingShortPaths(V<PATHE> rings){
	map<pair<PATHE,PATHE>,V<PATHE>> ringToRingShortedPaths;
	for(auto it0 = rings.begin();it0 != rings.end()-1;it0++){
		PATHE ring0 = *it0;
		for(auto it1 = it0+1; it1 != rings.end(); it1++){
			PATHE ring1 = * it1;
			V<PATHE> paths =ringToRingPaths(ring0,ring1,rings);

			int i = 0;
			for(PATHE p:paths){

				for(E e:p){
					cout<<"path"<< i <<":"<< e->name << endl;
				}
				i++;
			}
			ringToRingShortedPaths[make_pair(ring0,ring1)] = paths;
		}
	}
	return ringToRingShortedPaths;
}

V<PATHE> Graph::selectshortPaths(PATHE ring0, PATHE ring1,map<pair<PATHE,PATHE>,V<PATHE>> ringToRingShortedPaths){
	pair<PATHE,PATHE> p = make_pair(ring0,ring1);
  	return algo::map_select_pair(ringToRingShortedPaths,p);
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

V<V<PATHE>> Graph::connectRings(PATHE ringS,PATHE ringT,V<PATHE> rings){
	map<pair<PATHE,PATHE>,V<PATHE>>ringToRingShortedPaths = allRingShortPaths(rings);

	V<V<PATHE>> netWork;
	V<PATHE> coveredRings;
	//rings is head, ringt is tail, dfs to get all paths
	V<PATHE> stack_path;
	V<PATHE> stack_ring;
	PATHE noPath = * new PATHE();
	V<PATHE> divergePoint_path;
	V<PATHE> divergePoint_ring;
	V<PATHE> path_paths;
	V<PATHE> path_rings;


	path_rings.push_back(ringS);
	for(PATHE ring:rings){
		if(ring == ringS)
			continue;

		V<PATHE> paths_between = selectshortPaths(ringS,ring,ringToRingShortedPaths);
		for(PATHE p:paths_between){
			stack_path.push_back(p);
			stack_ring.push_back(ring);
			divergePoint_path.push_back(noPath);
			divergePoint_ring.push_back(ringS);
		}
	}

	while(stack_path.size()>0){
		PATHE thisPath = stack_path.back();
		stack_path.pop_back();
		PATHE thisRing = stack_ring.back();
		stack_ring.pop_back();
		PATHE diverge_path = divergePoint_path.back();
		divergePoint_path.pop_back();
		PATHE diverge_ring = divergePoint_ring.back();
		divergePoint_ring.pop_back();

		if(diverge_path == noPath){
			path_paths.clear();
		}
		algo::pop_To_Element(path_paths,diverge_path);

		algo::pop_To_Element(path_rings,diverge_ring);
		path_paths.push_back(thisPath);
		path_rings.push_back(thisRing);

		//find a path
		if(thisRing == ringT){
			netWork.push_back(path_paths);
			for(PATHE ring:path_rings){
				algo::distincAddVec(coveredRings,ring);
			}
			//if all rings are covered, return
			if(coveredRings.size() == rings.size())
				return netWork;

			continue;

		}
		//add adj rings and paths
		for(PATHE ring:rings){
			if(algo::vecContains(path_rings,ring))
				continue;

			V<PATHE> paths_between = selectshortPaths(thisRing,ring,ringToRingShortedPaths);
			for(PATHE p:paths_between){
				//if paths intercect with path
				if(pathIntercectPaths(p,path_paths))
					continue;

				stack_path.push_back(p);
				stack_ring.push_back(ring);
				divergePoint_path.push_back(thisPath);
				divergePoint_ring.push_back(thisRing);
			}

		}

	}


	return netWork;

}




vector<Node*> Graph::getNodesOnRing(vector<Edge*> ring){
	vector<Node*> r;
	for(Edge* e:ring){
		Node* n0 = getNode(e->x,e->y);
		Node* n1 = getNode(e->s,e->t);
		algo::distincAddVec(r,n0);
		algo::distincAddVec(r,n1);
	}

	return r;
}

vector<Node*>  Graph::getNodesOutRing(vector<Edge*> ring){
	vector<Node*> r;
	vector<Node*> nodesOnRing = getNodesOnRing(ring);
	vector<Node*> nodesInRing = getNodesInRing(ring);
	for(Node* n:nodes){

		if(algo::vecContains(nodesOnRing,n))
			continue;

		if(algo::vecContains(nodesInRing,n))
			continue;

		algo::distincAddVec(r,n);
	}

	return r;
}

vector<Node*>  Graph::getNodesInRing(vector<Edge*> ring){
	vector<Node*> r;

	for(Node* n:nodes){
		if(nodeInsideRing(n,ring)){
			algo::distincAddVec(r,n);
		}
	}

	return r;
}

//if head can reach node, then it is outside the ring,
bool Graph::nodeInsideRing(Node* n,vector<Edge*> ring){
	Node* head = entrances.at(0);
	//dfs

	vector<Node*> nodeOnRing = getNodesOnRing(ring);
	bool findNode = true;
	vector<Node*> searchPath;
	vector<Node*> stack;

	stack.push_back(head);

	while(stack.size()>0){
		Node* seed = stack.back();
		searchPath.push_back(seed);
		stack.pop_back();
		for(Node* adjNode:*seed->getAdjNodes()){



			if(algo::vecContains(searchPath,adjNode))
				continue;

			//brink of ring
			if(algo::vecContains(nodeOnRing,adjNode))
				continue;

			if(adjNode == n){
				findNode = true;
				break;
			}

			Edge* e = getEdge(seed,adjNode);

			if(e->isOn){
				stack.push_back(adjNode);
			}
		}

		if(findNode){
			return false;
		}
	}

	return true;
}
vector<vector<Edge*>> Graph::getRingsFromEdges(vector<Edge*> edgeSet){
	vector<vector<Edge*>> groups = getGroupsFromEdges(edgeSet);


	vector<vector<Edge*>> rings;
	for(Node* head:entrances){
		for(vector<Edge*> group:groups){

			bool isAPath = false;
			for(Edge* groupE:group){
				if(nodeOnEdge(head,groupE)){
					isAPath = true;
					break;
				}
			}

			if(!isAPath){
				rings.push_back(group);
			}
		}
	}

	return rings;
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
vector<vector<Edge*>> Graph::getGroupsFromEdges(vector<Edge*> edgeSet){
	vector<vector<Edge*>> groups;

	while(edgeSet.size()>0){
		vector<Edge*> group;
		Edge* firste = edgeSet.back();
		group.push_back(firste);
		edgeSet.pop_back();

		bool findNewEdge = true;
		while(findNewEdge){
			findNewEdge = false;
			for(Edge* e:edgeSet){

				for(Edge* groupE : group){
					if(edgeOnEdge(e,groupE)){
						algo::remove(edgeSet,e);
						group.push_back(e);
						findNewEdge = true;
						break;
					}
				}

				if(findNewEdge)
					break;
			}
		}

		groups.push_back(group);
	}

	return groups;

}

bool Graph::dfs(Node* s, Node* t){
	vector<Node*> searchPath;
	vector<Node*> stack;

	stack.push_back(s);

	while(stack.size()>0){
		Node* seed = stack.back();
		searchPath.push_back(seed);
		stack.erase(stack.end());
		for(Node* adjNode:*seed->getAdjNodes()){

			if(adjNode == t)
				return true;

			if(algo::vecContains(searchPath,adjNode))
				continue;

			Edge* e = getEdge(seed,adjNode);
			if(e->isOn){
				stack.push_back(adjNode);
			}
		}
	}

	return false;
}

V<N> Graph::nodesOnPath(const V<E>& path){
	V<N> r;
	for(E e:path){
		Node* n0 = getNode(e->x,e->y);
		Node* n1 = getNode(e->s,e->t);
		algo::distincAddVec(r,n0);
		algo::distincAddVec(r,n1);
	}

	return r;
}

V<E> Graph::shortestPathToNodes(N start, V<N> destinations, V<N> forbiddenNodes){
	V<N> queue;

	map<N,int> stepToStart;
	for(Node* n:nodes){
		stepToStart[n] = BIG_INTEGER;
	}
	stepToStart[start] = 0;
	bool findDestinations = false;
	queue.push_back(start);
	while(queue.size()>0){
		N nextNode = queue.front();
		algo::pop_front(queue);

		for(N adjNode:*nextNode->getAdjNodes()){

			if(algo::vecContains(forbiddenNodes,adjNode))
				continue;

			if(stepToStart[adjNode] > stepToStart[nextNode]+1){

				stepToStart[adjNode] = stepToStart[nextNode] + 1;
			}

			if(algo::vecContains(destinations,adjNode)){
				findDestinations = true;
			}
			else{
				queue.push_back(adjNode);
			}


		}


		if(findDestinations)
			break;


	}

	V<N> disableNodes;

	V<V<E>> shortestPaths = fromStepToShortedPath(stepToStart,destinations,disableNodes);


	return shortestPaths.at(0);



}

//return all nodes between start and end
V<N> Graph::traverseSortedPath(N start, N end,const V<E>& sortedPath){
	V<N> sortedPathNode =edgesToNodes(&sortedPath);
	assert(algo::vecContains(sortedPathNode,start));
	assert(algo::vecContains(sortedPathNode,end));

	N head = sortedPathNode.front();
	N tail = sortedPathNode.back();
	V<N> queue;
	V<N> stack;

	stack.push_back(start);
	while(stack.size()>0){
		N n = stack.back();
		stack.pop_back();
		queue.push_back(n);
		for(N adjNode:*n->getAdjNodes()){
			E e = getEdge(n,adjNode);

			if(!algo::vecContains(sortedPath,e)){
				continue;
			}
			if(adjNode == head || adjNode == tail){
				algo::pop_To_Element(queue,start);
				continue;
			}

			if(algo::vecContains(queue,adjNode))
				continue;

			if(adjNode == end){
				queue.push_back(adjNode);
				return queue;
			}
			stack.push_back(adjNode);
		}
	}
	return queue;

}
//connect loop to a path and leave out edges cannot fit in
void Graph::connectLoopToPath(V<E>& loop, V<E>& path){
	V<N> nLoop = nodesOnPath(loop);
	V<N> nPath = nodesOnPath(path);

	//0 from node on loop by shortest path to path
	//1 from node on loop by shortest path to path
	E eWithSmallestPrice = loop.at(0);
	int smallEestPrice = BIG_INTEGER;
	for(E e:loop){
		break;
		N n0 = getNode(e->x,e->y);
		N n1 = getNode(e->s,e->t);
		//find shortest path from n0 to path
		V<N> forbiddenNodes;
		forbiddenNodes.clear();
		forbiddenNodes.insert(forbiddenNodes.end(),nLoop.begin(),nLoop.end());
		V<E> n0ShortestPath = shortestPathToNodes(n0,nPath,forbiddenNodes);
		V<N> n0ShortestPathInNodes = nodesOnPath(n0ShortestPath);

		forbiddenNodes.insert(forbiddenNodes.end(), n0ShortestPathInNodes.begin(),n0ShortestPathInNodes.end());
		V<E> n1ShortestPath = shortestPathToNodes(n1,nPath,forbiddenNodes);
		V<N> n1ShortestPathInNodes = nodesOnPath(n1ShortestPath);
		sortEdgesInPath(&path);
		V<N> pathSortedN = edgesToNodes(&path);
		N nStart = NULL;
		N nEnd = NULL;
		for(N n:nPath){
			for(N nElse:n0ShortestPathInNodes){
				if(n == nElse)
					nStart = n;
			}
			for(N nElse:n1ShortestPathInNodes){
				if(n == nElse)
				nEnd= n;
			}
		}
		V<N> pathBetween = traverseSortedPath(nStart,nEnd,path);
		V<E> pathBetweenEdges = pathNodesToEdges(pathBetween);
		cout << " im " << e ->name<<endl;
		cout << " my price is " <<pathBetweenEdges.size() << endl;
		if(pathBetweenEdges.size() < smallEestPrice){
			eWithSmallestPrice = e;
			smallEestPrice = pathBetweenEdges.size();
		}
	}
	eWithSmallestPrice = loop.at(1);
	E e = eWithSmallestPrice;
	N n0 = getNode(e->x,e->y);
	N n1 = getNode(e->s,e->t);
	//find shortest path from n0 to path
	V<N> forbiddenNodes;
	forbiddenNodes.insert(forbiddenNodes.end(),nLoop.begin(),nLoop.end());
	V<E> n0ShortestPath = shortestPathToNodes(n0,nPath,forbiddenNodes);
	V<N> n0ShortestPathInNodes = nodesOnPath(n0ShortestPath);

	forbiddenNodes.insert(forbiddenNodes.end(), n0ShortestPathInNodes.begin(),n0ShortestPathInNodes.end());
	V<E> n1ShortestPath = shortestPathToNodes(n1,nPath,forbiddenNodes);
	V<N> n1ShortestPathInNodes = nodesOnPath(n1ShortestPath);
	sortEdgesInPath(&path);
	V<N> pathSortedN = edgesToNodes(&path);
	N nStart = NULL;
	N nEnd = NULL;
	for(N n:nPath){
		for(N nElse:n0ShortestPathInNodes){
			if(n == nElse)
				nStart = n;
		}
		for(N nElse:n1ShortestPathInNodes){
			if(n == nElse)
			nEnd= n;
		}
	}
	V<N> pathBetween = traverseSortedPath(nStart,nEnd,path);
	V<E> pathBetweenEdges = pathNodesToEdges(pathBetween);
	if(pathBetweenEdges.size() < smallEestPrice){
		eWithSmallestPrice = e;
		smallEestPrice = pathBetweenEdges.size();
	}
	//remove e from loop
	algo::remove(loop,e);
	//remove pathBetween from path
	algo::remove(path,pathBetweenEdges);

	//join path with shotest path 1 and shortest path2 and loop
	algo::join2Vec(path,n0ShortestPath);
	algo::join2Vec(path,n1ShortestPath);
	algo::join2Vec(path,loop);
	//clear loop
	loop.clear();
	//join loop with e and pathBetween
	loop.push_back(e);
	algo::join2Vec(loop,pathBetweenEdges);

}


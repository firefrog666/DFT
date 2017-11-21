#include "Graph.h"
#include "ILPgen.h"
#include "ILP.h"
#include <vector>
#include "stdlib.h"
#include <iostream>
#include <ctime>
#include "toolBox.h"
#include "draw.h"
#include "sequenceGraph.h"
#include "device.h"
#include "ListAlgorithm.h"
#include "gurobi_c++.h"
#include <sstream>
#include <iomanip>
#include "pso.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
using namespace std;


void drawNewPath(V<E>& newEdgePath, Graph* g, string fileName){
	cout<< " drawing: "<<fileName<<endl;
	Draw* d = new Draw();

	d->drawEdges(newEdgePath,"edges.txt");
	d->drawNodes(g->nodes,"nodes.txt");
	string command = S("./drawGraph.sh ")+ fileName;
	system(command.c_str());
	//cout<<"press enter to continue;"<<endl;
	//system("read");
}

void drawPath(V<E>& newEdgePath, Graph* g, string fileName){
	cout<< " drawing: "<<fileName<<endl;
	Draw* d = new Draw();
	V<N> ports;
	algo::join2Vec(ports,g->entrances);
	algo::join2Vec(ports,g->exits);
	d->drawEdges(newEdgePath,"edges.txt");

	d->drawNodes(ports,"nodes.txt");
	string command = S("./drawGraph.sh ")+ fileName;
	system(command.c_str());
	//cout<<"press enter to continue;"<<endl;
	//system("read");
}




void testEdgeNoPath(){
	const char* ilpFileName = "pathCoverAllEdges.lp";


	//const char* graphxml = "graph_cells.xml";
	//const char* graphxml = "graph_test.xml";
	//const char* graphxml = "graph_test_small.xml";
	const char* graphxml = "delayed_chip.xml";
		Graph* g = new Graph(graphxml);


		Draw* d = new Draw();
		for(Node* head: g->entrances){
			cout<< "node "<<head->id<<" is head"<<endl;
		}
		for(Node* exit: g->exits){
			cout<< "node "<<exit->id<<" is tail"<<endl;
		}

		//keep only one exits;

		N exit = g->exits.back();
		g->exits.clear();
		g->exits.push_back(exit);

	d->drawEdges(g->edges,"edges.txt");
	d->drawNodes(g->exits,"nodes.txt");
	string command =S("./drawGraph.sh")+ S(" chip");
	system(command.c_str());


	ILPgen ilpGen;
	ilpGen.g = g;
	V<E> noPathEdges;
	for(E e:g->edges){
		ilpGen.pathThrougEdge(e,ilpFileName);
		ilpGen.ilpRun(ilpFileName);
		if(ilpGen.varValue.size() == 0){
			cout<< e->name << " has no path to run through"<<endl;
			noPathEdges.push_back(e);
		}
		else{
			cout<< e->name << " have edge to run through"<<endl;
		}

	}

	drawNewPath(noPathEdges,g,"noPathEdges");




}


void testEachEdgeCoverByPath(Graph* g, V<E> edgeToCover, V<E> resourceEdge){
	ILPgen ilpGen;
	ilpGen.setILPTime(1000);
	ilpGen.setILPGap(0.01);
	//ilpGen.pathCoverAsManyEdges(pathEachRun,ilpFileName,edgeToCover,usedEdges);
	//ilpGen.pathLeastResource(pathEachRun,ilpFileName,edgeToCover,resourceEdge);
	const char* ilpFileName = "pathCoverAllEdges.lp";
	int pathEachRun = 2;
	ilpGen.dfsPaths(pathEachRun,ilpFileName,edgeToCover,resourceEdge);
	ilpGen.ilpRun(ilpFileName);

	V<E> usedEdges;
	int oldUsedSize = usedEdges.size();
	vector<Edge*> path;
	V<E> newChip;
	newChip = g->edges;
	for(int i = 0; i<pathEachRun;i++){

		path = ilpGen.getPath(i);


		for(E e:path){


			if(!algo::vecContains(resourceEdge,e))
				continue;

			algo::distincAddVec(usedEdges,e);
			algo::distincAddVec(newChip,e);
			e->isStore = true;
		}

	}

}

void genGrid(Graph* graph, V<E>& edgeToCover,V<E>& resourceEdge){
	const char* ilpFileName = "pathCoverAllEdges.lp";
	//const char* graphxml = "graph_cells.xml";
	//const char* graphxml = "graph_test.xml";
	//const char* graphxml = "graph_test_small.xml";
	//const char* graphxml = "delayed_chip.xml";
	//Graph* graph = new Graph(graphxml);

	N exit = graph->exits.back();
	graph->exits.clear();
	graph->exits.push_back(exit);
	Graph* grid = graph->fitGraphIntoGrid();

	ILPgen ilpGen;
	ilpGen.g = graph;
	V<E> noPathEdges;
	for(E e:graph->edges){
		ilpGen.pathThrougEdge(e,ilpFileName);
		ilpGen.ilpRun(ilpFileName);
		if(ilpGen.varValue.size() == 0){
			cout<< e->name << " has no path to run through"<<endl;
			noPathEdges.push_back(e);
		}
		else{
			cout<< e->name << " have edge to run through"<<endl;
		}

	}
	//drawPath(graph->edges,graph,"graph");

	//drawPath(grid->edges,grid,"grid");

	for(E e:noPathEdges){
		N n0 = graph->getNode(e->x,e->y);
		N n1 = graph->getNode(e->s,e->t);
		E edgeInGrid = grid->getEdge(n0,n1);
		assert(edgeInGrid != NULL);
		edgeToCover.push_back(edgeInGrid);
	}
	for(E e:grid->edges){
		N n0 = grid->getNode(e->x,e->y);
		N n1 = grid->getNode(e->s,e->t);
		E edgeInGraph = graph->getEdge(n0,n1);
		if(edgeInGraph == NULL){
			resourceEdge.push_back(e);
			graph->addEdge(e);
		}
	}

	//drawNewPath(resourceEdge,grid,"resouceGraph");

	V<E> usedEdges = V<E>();


}

void testFitGraphIntoGrid(Graph* graph){
	const char* ilpFileName = "pathCoverAllEdges.lp";
	//const char* graphxml = "graph_cells.xml";
	//const char* graphxml = "graph_test.xml";
	//const char* graphxml = "graph_test_small.xml";
	//const char* graphxml = "delayed_chip.xml";
	//Graph* graph = new Graph(graphxml);

	N exit = graph->exits.back();
	graph->exits.clear();
	graph->exits.push_back(exit);
	Graph* grid = graph->fitGraphIntoGrid();

	ILPgen ilpGen;
	ilpGen.g = graph;
	V<E> noPathEdges;
	for(E e:graph->edges){
		ilpGen.pathThrougEdge(e,ilpFileName);
		ilpGen.ilpRun(ilpFileName);
		if(ilpGen.varValue.size() == 0){
			cout<< e->name << " has no path to run through"<<endl;
			noPathEdges.push_back(e);
		}
		else{
			cout<< e->name << " have edge to run through"<<endl;
		}

	}
	//drawPath(graph->edges,graph,"graph");

	//drawPath(grid->edges,grid,"grid");
	V<E> edgeToCover;
	V<E> resourceEdge;
	for(E e:noPathEdges){
		N n0 = graph->getNode(e->x,e->y);
		N n1 = graph->getNode(e->s,e->t);
		E edgeInGrid = grid->getEdge(n0,n1);
		assert(edgeInGrid != NULL);
		edgeToCover.push_back(edgeInGrid);
	}
	for(E e:grid->edges){
		N n0 = grid->getNode(e->x,e->y);
		N n1 = grid->getNode(e->s,e->t);
		E edgeInGraph = graph->getEdge(n0,n1);
		if(edgeInGraph == NULL)
			resourceEdge.push_back(e);
	}

	//drawNewPath(resourceEdge,grid,"resouceGraph");

	V<E> usedEdges = V<E>();

	drawPath(grid->edges,grid,"grid");
	drawPath(resourceEdge,grid,"resouce");
	drawPath(edgeToCover,grid,"edgeToCover");

	int count = 0;


	ilpGen.g = grid;
	V<V<E>> paths;
	cout<< "each run num:"<<endl;

	int pathEachRun = 2;
	//cin>> pathEachRun;
	while(true){
		ilpGen.setILPTime(1000);
		ilpGen.setILPGap(0.01);
		//ilpGen.pathCoverAsManyEdges(pathEachRun,ilpFileName,edgeToCover,usedEdges);
		//ilpGen.pathLeastResource(pathEachRun,ilpFileName,edgeToCover,resourceEdge);
		ilpGen.dfsPaths(pathEachRun,ilpFileName,edgeToCover,resourceEdge);
		ilpGen.ilpRun(ilpFileName);
		int oldUsedSize = usedEdges.size();
		vector<Edge*> path;
		V<E> newChip;
		newChip = graph->edges;
		for(int i = 0; i<pathEachRun;i++){

			path = ilpGen.getPath(i);


			for(E e:path){


				if(!algo::vecContains(resourceEdge,e))
					continue;

				algo::distincAddVec(usedEdges,e);
				algo::distincAddVec(newChip,e);
				graph->addEdge(e);
				e->isStore = true;
			}

			count++;
			paths.push_back(path);
		}

		int newUsedSize = usedEdges.size();



		cout << "path "<<count<<endl;

		//drawPath(usedEdges,grid,"usedEdges");
		drawPath(graph->edges,graph,"newChip");
		if(edgeToCover.size() <= usedEdges.size())
			break;

		if(newUsedSize == oldUsedSize)
			break;


		cout << "continue?"<<endl;
		bool goOn;
		cin >> goOn;
		if(goOn)
			continue;
		else
			break;


	}

	cout<<"done valve sharing"<<endl;
	system("read");
}


void readSequenceGraph(){
	ListAlgorithm L;
	const char* fileName= "pcr.xml";
	L.readFromXml(fileName);
	for(Op* op:L.ops){
		op->printSelf();
	}

	const char* graphxml = "chip.xml";
	Graph* chip = new Graph(graphxml);
	L.chip = chip;

	drawNewPath(chip->edges,chip,"chip");
}
class Animal{
public:
	string name;
	int heigt;
public:
	void eat(){
		cout<< "animal eat something" <<endl;
	}
	void selfIntro(){
		cout<< " my name is "<<name<<endl;
	}
};


vector<Animal*> getAnimals(){
	V<Animal*> r;
	Animal a;
	a.name = "lily";
	a.heigt = 10;
	Animal b;
	b.name = "bolai";
	b.heigt = 100;
	r.push_back(&a);
	r.push_back(&b);
	for(Animal* a:r){
		a->selfIntro();
		a->name = "changedName";
	}
	return r;
}

void test(){
	ListAlgorithm L;
	PSO p;
	//const char* assayName= "delayed.xml";
	const char* assayName = "delayed_ver2.xml";
	const char* chipFile = "delayed_chip.xml";

	L.readChipFromXml(chipFile);
	L.readFromXml(assayName);
	L.listILP();
	//V<E> resouceEdges;
	//p.particalSwarmOpt(L.chip,resouceEdges);
	//V<V<E>> allNewEdges = p.fitGraphIntoGrid(L.chip);

	/*for(V<E> newEdgesUsed:allNewEdges){
		L.readChipFromXml(chipFile);
		for(E e:newEdgesUsed){
			L.chip->addEdge(e);
		}
		//drawPath(L.chip->edges,L.chip,"psoInit");
	}
*/


}

void testList(){



	ListAlgorithm L;
	ListAlgorithm L2;


	const char* assayName= "delayed.xml";
	const char* chipFile = "delayed_chip.xml";

	L.readChipFromXml(chipFile);
	L.readFromXml(assayName);
	testFitGraphIntoGrid(L.chip);
	//drawPath(L.chip->edges,L.chip,"newChip");

	for(Op* op:L.ops){
		op->printSelf();
	}
	for(Device* dev:L.devs){
		dev->printSelf();
	}
	L.listinit();
	L.list();

	cout<<"press enter"<<endl;
	system("read");
	L2.readChipFromXml(chipFile);
	L2.readFromXml(assayName);
	testFitGraphIntoGrid(L2.chip);
	drawNewPath(L2.chip->edges,L2.chip,"chip");
	L2.list();
	return;
}

void testCutGeneral(){
	Graph* g = new Graph();
	g->initTest(4,4);
	// drawPath(g->edges,g,"a_graph");
	ILPgen ilpGen;
	const char* ilpFileName = "cut.lp";
	ilpGen.g = g;
	ilpGen.cutKrish(1);
	ilpGen.cutNoFlowKrish(1);
	//ilpGen.writeInToILPfile(ilpFileName);
	ilpGen.ilpRun(ilpFileName);
	V<E> cut = ilpGen.getPath(1);
	drawPath(cut,g,"cut");

}

V<E> whichEdgesToUseDfs(Graph* g,const V<float>& edgePlan){
	V<E> r;
	return r;

}

V<E> whichEdgesToShare(Graph* g, const V<float>& edgePlan){
	V<E> r;
	return r;
}






void pso(){
	PSO pso;
	V<V<float>> newEdgeAndSharePlan;
	int solutionNumber = 3;
	pso.pso();
}

void testTestVector(){
	ListAlgorithm L;
	ILPgen ilpgen;

	//const char* assayName = "delayed_ver2.xml";
	//const char* assayName = "delayed.xml";
	//const char* assayName = "pcr.xml";
	//const char* assayName = "ivd.xml";
	const char* assayName = "ipd.xml";
	//const char* assayName = "cpa.xml";
	//const char* chipFile = "delayed_chip.xml";
	//const char* chipFile = "ra30.xml";
	//const char* chipFile = "ivd_chip.xml";

	//const char* chipFile = "cpa_chip.xml";
	const char* chipFile = "mrnaChip.xml";
	//const char* chipFile = "ivdChipForPcR.xml";
	L.readFromXml(assayName);
	L.readChipFromXml(chipFile);
	ilpgen.g = L.chip;
	const char* f = "testvectors.lp";
	ilpgen.treeToCoverAllEdgesNoLoop(f,2);
	ilpgen.ilpRun(f);

	V<E> usedEdges;
	ilpgen.dfsCuts(8,f,L.chip->edges,usedEdges);
	ilpgen.ilpRun(f);


}
int main(){
	pso();
	return 0;

	testTestVector();
	return 0;

	srand(time(NULL));


	testCutGeneral();
	return 0;
	test();
	return 0;
	//poolSearchTest();
	//return 0;
	testList();
	return 0;
	//testFitGraphIntoGrid();

	return 0;

	test();
	return 0;
	readSequenceGraph();
	return 0;


	testEdgeNoPath();
	return 0;
}


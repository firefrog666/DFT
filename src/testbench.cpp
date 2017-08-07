#include "Graph.h"
#include "ILPgen.h"
#include "ILP.h"
#include <vector>
#include "stdlib.h"
#include <iostream>
#include "toolBox.h"
#include "draw.h"
using namespace std;

void testEliminateLoop(){
	const char* ilpFileName = "pathCoverAllEdges.lp";
		Graph* g = new Graph();
		int x,y;

		cout<< "graph x size:"<<endl;
		cin>>x;
		cout<< "graph y size:"<<endl;
		cin>>y;

		g->initTest(x,y);
		Draw* d = new Draw();


		ILPgen ilpGen;
		ilpGen.g = g;
		int pathNum = 2;

		cout<< "pathNum :"<<endl;
		cin>>pathNum;
		ilpGen.pathToCoverAllEdgesNoLoop(ilpFileName,pathNum);
		ilpGen.ilpRun(ilpFileName);

		for(int i = 0; i<pathNum;i++){
			vector<Edge*> pathWithLoop = ilpGen.getPath(i);
			d->drawEdges(pathWithLoop,"edges.txt");
			d->drawNodes(g->nodes,"nodes.txt");
			string command =S("./drawGraph.sh")+ S(" withLoopP")+S(i);
			system(command.c_str());

			vector<vector<Edge*>> paths = g->getPathsFromEdges(pathWithLoop);
			vector<Edge*> path = paths.at(0);

			d->drawEdges(path,"edges.txt");
			d->drawNodes(g->nodes,"nodes.txt");
			command =S("./drawGraph.sh")+ S(" path")+S(i);
			system(command.c_str());
			//drawRings

			vector<vector<Edge*>> rings = g->getRingsFromEdges(pathWithLoop);

			if(rings.size()>0){
				for(V<E> ring:rings){
					g->connectLoopToPath(ring, path);
					d->drawEdges(path,"edges.txt");
					vector<Node*> nodesOnRing= g->getNodesOutRing(ring);
					d->drawNodes(nodesOnRing,"nodes.txt");
					command = S("./drawGraph.sh")+ S(" newP")+S(i);
					system(command.c_str());

					d->drawEdges(ring,"edges.txt");
					d->drawNodes(nodesOnRing,"nodes.txt");
					command = S("./drawGraph.sh")+ S(" newLoop")+S(i);
					system(command.c_str());
				}
			}

		}

	return;
}

void testCutGeneralGraph(){
	const char* ilpFileName = "pathCoverAllEdges.lp";
	Graph* g = new Graph();
	int x,y;

	cout<< "graph x size:"<<endl;
	x = 2;
	cin>>x;
	cout<< "graph y size:"<<endl;
	y = 2;
	cin>>y;

	g->initTest(x,y);


/*	const char* graphxml = "graph_cells.xml";
		Graph* g = new Graph(graphxml);*/

		Draw* d = new Draw();
		for(Node* head: g->entrances){
			cout<< "node "<<head->id<<" is head"<<endl;
		}
		for(Node* exit: g->exits){
			cout<< "node "<<exit->id<<" is tail"<<endl;
		}

	/*d->drawEdges(g->edges,"edges.txt");
	d->drawNodes(g->nodes,"nodes.txt");
	string command =S("./drawGraph.sh")+ S(" chip");
	system(command.c_str());*/

	ILPgen ilpGen;
	ilpGen.g = g;
	int pathNum = 2;

	cout<< "CutNum EachRoll:"<<endl;
	cin>>pathNum;

	V<E> usedEdges = V<E>();
	//ilpGen.pathCoverSeverEdgesNoLoop(ilpFileName,pathNum,g->insideEdges);
	int count = 0;
	while(true){

		ilpGen.cutGeneralCoverSeveralEdges(pathNum,ilpFileName,g->edges,usedEdges);
		ilpGen.ilpRun(ilpFileName);
		for(int i = 0; i<pathNum;i++){
			vector<Edge*> pathWithLoop = ilpGen.getPath(i);
			algo::join2Vec(usedEdges,pathWithLoop);
			d->drawEdges(pathWithLoop,"edges.txt");
			d->drawNodes(g->nodes,"nodes.txt");
			string command =S("./drawGraph.sh")+ S(" cutGeneral")+S(count);
			system(command.c_str());
		}

		cout << "break?" << endl;
		bool ifBreak = false;
		cin>> ifBreak;
		if(ifBreak){
			break;
		}
		count++;
	}


}


void testSameTime(){
	const char* ilpFileName = "pathCoverAllEdges.lp";
	Graph* g = new Graph();
	int x,y;

	cout<< "graph x size:"<<endl;
	x = 2;
	cin>>x;
	cout<< "graph y size:"<<endl;
	y = 2;
	cin>>y;

	g->initTest(x,y);


	Draw* d = new Draw();
	d->drawEdges(g->edges,"edges.txt");
	d->drawNodes(g->nodes,"nodes.txt");
	string command =S("./drawGraph.sh")+ S(" chip");
	system(command.c_str());

	ILPgen ilpGen;
	ilpGen.g = g;
	int pathNum = 2;

	cout<< "pathNum :"<<endl;
	cin>>pathNum;
	ilpGen.pathCoverSeverEdgesNoLoop(ilpFileName,pathNum,g->edges);
	ilpGen.ilpRun(ilpFileName);

	V<V<E>> paths;
	for(int i = 0; i<pathNum;i++){
		vector<Edge*> pathWithLoop = ilpGen.getPath(i);
		d->drawEdges(pathWithLoop,"edges.txt");
		d->drawNodes(g->nodes,"nodes.txt");
		string command =S("./drawGraph.sh")+ S(" path")+S(i);
		system(command.c_str());
		paths.push_back(pathWithLoop);
	}

	cout<< "pathNum cover switch same time: "<<endl;
	cin>>pathNum;

	ilpGen.pathValveSwitchTogether(pathNum,paths,ilpFileName);
	ilpGen.ilpRun(ilpFileName);
	for(int i = 0; i<pathNum;i++){
		vector<Edge*> path = ilpGen.getPath(i);
		d->drawEdges(path,"edges.txt");
		d->drawNodes(g->nodes,"nodes.txt");
		string command =S("./drawGraph.sh")+ S(" path")+S(i);
		system(command.c_str());

	}
}

void testSplitGraph(){
	const char* ilpFileName = "pathCoverAllEdges.lp";
	Graph* g = new Graph();
	int x,y;

	cout<< "graph x size:"<<endl;
	x = 4;
	//cin>>x;
	cout<< "graph y size:"<<endl;
	y = 4;
	//cin>>y;

	g->initTest(x,y);
	Draw* d = new Draw();
	d->drawEdges(g->edges,"edges.txt");
	d->drawNodes(g->nodes,"nodes.txt");
	string command =S("./drawGraph.sh")+ S(" chip");
	system(command.c_str());



	vector<float> xAxis = {-0.5,1.5,3.5};
	vector<float> yAxis = {-0.5,1.5,3.5};

	V<Graph*> subGs = g->splitGraphByAxis(xAxis,yAxis);
	int count = 0;
	for(Graph* subG:subGs){
		d->drawEdges(subG->edges,"edges.txt");
		d->drawNodes(subG->nodes,"nodes.txt");
		string command =S("./drawGraph.sh")+ S(" subG") + S(count);
		system(command.c_str());
		count++;
	}


}
void testCut(){
	const char* ilpFileName = "pathCoverAllEdges.lp";
	Graph* g = new Graph();
	int x,y;

	cout<< "graph x size:"<<endl;
	x = 2;
	cin>>x;
	cout<< "graph y size:"<<endl;
	y = 2;
	cin>>y;

	g->initTest(x,y);
	Graph* chip = g->fromGraphToChip();
	chip->moveGraph(0.5,0.5);
	for(N n:chip->entrances){
		cout<< "head is "<<n->name<<endl;
	}

	for(N n:chip->exits){
		cout<< "tail is "<<n->name<<endl;
	}

	Draw* d = new Draw();
	d->drawEdges(chip->edges,"edges.txt");
	d->drawNodes(chip->nodes,"nodes.txt");
	string command =S("./drawGraph.sh")+ S(" chip");
	system(command.c_str());

	ILPgen ilpGen;
	ilpGen.g = chip;
	int pathNum = 2;

	//cout<< "pathNum :"<<endl;
	//cin>>pathNum;
	//ilpGen.pathCoverSeverEdgesNoLoop(ilpFileName,pathNum,g->insideEdges);
	V<E> usedEdges = V<E>();
	int counter = 0;
	while(true){

		ilpGen.cutSeveralEdgesNoLoop(1,ilpFileName,g->insideEdges,usedEdges);
		ilpGen.ilpRun(ilpFileName);
		vector<Edge*> pathWithLoop = ilpGen.getPath(0);
		for(E e:pathWithLoop){
			algo::distincAddVec(usedEdges,e);
		}
		d->drawEdges(pathWithLoop,"edges.txt");
		d->drawNodes(g->nodes,"nodes.txt");

		if(usedEdges.size() == g->insideEdges.size()){
			cout<<"need " << counter+1 <<"cuts"<<endl;
			string command =S("./drawGraph.sh")+ S(" withLoopP")+S(counter);
			system(command.c_str());
			break;
		}
		counter++;

		//system("read");

	}


/*	for(int i = 0; i<pathNum;i++){
		vector<Edge*> pathWithLoop = ilpGen.getPath(i);
		d->drawEdges(pathWithLoop,"edges.txt");
		d->drawNodes(g->nodes,"nodes.txt");
		string command =S("./drawGraph.sh")+ S(" withLoopP")+S(i);
		system(command.c_str());
	}*/
}

vector<int>* test2(){
	vector<int>* z = new vector<int>();
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;
	z->push_back(a);
	z->push_back(b);
	z->push_back(c);
	z->push_back(d);
	vector<int> y;
	y.push_back(a);
	y.push_back(b);
	algo::remove(*z,a);
	algo::remove(y,a);
	//cout<< z->size()<<endl;
	for(int i: *z){
		cout << i <<endl;
	}
	for(int i:y)
		cout<<i<<endl;

	return z;
}
void test(){
/*	vector<int>* a = new vector<int>();
		a->push_back(1);
		a->push_back(2);
		a->push_back(3);
		vector<int>* b;
		b = a;
		for(int i:*b){
			cout<< i << endl;
		}

		a->pop_back();
		a->pop_back();
		for(int i:*a){
			cout<<"a " << i << endl;
		}

		for(int i:*b){
			cout<<"b "<< i << endl;
		}

		return;*/
	vector<int>* a = test2();
	cout << "sth" <<endl;
	for(int i:*a){
				cout<<"a " << i << endl;
	}

}


void testLR(){
	const char* ilpFileName = "pathCoverAllEdges.lp";
	Graph* g = new Graph();
	int x,y;

	cout<< "graph x size:"<<endl;
	x = 2;
	cin>>x;
	cout<< "graph y size:"<<endl;
	y = 2;
	cin>>y;

	g->initTest(x,y);
	Draw* d = new Draw();
	/*d->drawEdges(g->edges,"edges.txt");
	d->drawNodes(g->nodes,"nodes.txt");
	string command =S("./drawGraph.sh")+ S(" withLoopGraph");
	system(command.c_str());*/

	ILPgen ilpGen;
	ilpGen.g = g;
	int pathNum = 2;

	cout<< "pathNum :"<<endl;
	cin>>pathNum;
	ilpGen.pathToCoverAllEdgesNoLoop(ilpFileName,pathNum);
	ilpGen.ilpRun(ilpFileName);


	for(int i = 0; i<pathNum;i++){
		vector<Edge*> pathWithLoop = ilpGen.getPath(i);
		d->drawEdges(pathWithLoop,"edges.txt");
		d->drawNodes(g->nodes,"nodes.txt");
		string command =S("./drawGraph.sh")+ S(" withLoopLR")+S(i);
		system(command.c_str());
	}
}

void testTree(){
	const char* ilpFileName = "pathCoverAllEdges.lp";
	Graph* g = new Graph();
	int x,y;

	cout<< "graph x size:"<<endl;
	x = 2;
	cin>>x;
	cout<< "graph y size:"<<endl;
	y = 2;
	cin>>y;
	Draw* d = new Draw();
	g->initTest(x,y);
	ILPgen ilpGen;
	ilpGen.g = g;
	int TreeNum = 2;

	cout<< "treeNum :"<<endl;
	cin>>TreeNum;
	ilpGen.treeToCoverAllEdgesNoLoop(ilpFileName,TreeNum);
	ilpGen.ilpRun(ilpFileName);
	for(int i = 0; i<TreeNum;i++){
		vector<Edge*> pathWithLoop = ilpGen.getPath(i);
		d->drawEdges(pathWithLoop,"edges.txt");
		d->drawNodes(g->nodes,"nodes.txt");
		string command =S("./drawGraph.sh")+ S(" withLoopP")+S(i);
		system(command.c_str());
	}
	return;
}

void testReadXml(){
	const char* graphxml = "graph.xml";
	Draw* d = new Draw();
	Graph* g = new Graph(graphxml);

	for(Node* head: g->entrances){
		cout<< "node "<<head->id<<" is head"<<endl;
	}
	for(Node* exit: g->exits){
		cout<< "node "<<exit->id<<" is tail"<<endl;
	}
	d->drawEdges(g->edges,"edges.txt");
	d->drawNodes(g->nodes,"nodes.txt");
	string command =S("./drawGraph.sh")+ S(" xml");
	system(command.c_str());

	const char* ilpFileName = "pathCoverAllEdges.lp";
	ILPgen ilpGen;
	ilpGen.g = g;
	int pathNum = 2;

	cout<< "pathNum :"<<endl;
	cin>>pathNum;
	ilpGen.treeToCoverAllEdgesNoLoop(ilpFileName,pathNum);
	ilpGen.ilpRun(ilpFileName);
	for(int i = 0; i<pathNum;i++){
		vector<Edge*> pathWithLoop = ilpGen.getPath(i);
		d->drawEdges(pathWithLoop,"edges.txt");
		d->drawNodes(g->nodes,"nodes.txt");
		string command =S("./drawGraph.sh")+ S(" withLoopP")+S(i);
		system(command.c_str());
	}
	return;
}

void test3(){
	Draw* d = new Draw();

		const char* ilpFileName = "pathCoverAllEdges.lp";
		Graph* g = new Graph();
		int x,y;

		cout<< "graph x size:"<<endl;
		cin>>x;

		cout<< "graph y size:"<<endl;
		cin>>y;

		g->initTest(x,y);




	/*	for(int i = 2; i <4 ; i++){
			for(int j = 2; j <4 ; j++ ){
				Node* n = g->getNode(i,j);
				for(Node* nbr:*n->getAdjNodes()){
					Edge* e = g->getEdge(n,nbr);
					e->isHole = true;
				}
			}
		}

		for(int i = 5; i <8 ; i++){
			for(int j = 5; j <8 ; j++ ){
				Node* n = g->getNode(i,j);
				for(Node* nbr:*n->getAdjNodes()){
					Edge* e = g->getEdge(n,nbr);
					e->isWall = true;
				}
			}
		}*/

		Graph* chip = g->fromGraphToChip();

		//d->drawEdges(g->edges,"edges.txt");
		//d->drawNodes(g->nodes,"nodes.txt");
		d->drawEdges(chip->edges,"edges.txt");
		d->drawNodes(chip->nodes,"nodes.txt");
		string command =S("./drawGraph.sh")+ S(" chip");
		system(command.c_str());

		d->drawEdges(g->edges,"edges.txt");
		d->drawNodes(g->nodes,"nodes.txt");
		command =S("./drawGraph.sh")+ S(" graph");
		system(command.c_str());

		ILPgen ilpGen;
		ilpGen.g = g;
		cout<<"input path number:"<<endl;
		int pathNumber;
		cin>>pathNumber;

		ilpGen.pathToCoverAllEdges(ilpFileName,pathNumber);
		ilpGen.ilpRun(ilpFileName);
		cout<<"which path to print"<<endl;
		int pathToPrint;
		cin>> pathToPrint;
		vector<Edge*> path = ilpGen.getPath(pathToPrint);
		d->drawEdges(chip->edges,"edges.txt");
		d->drawNodes(chip->nodes,"nodes.txt");
		d->drawEdges(path,"edges.txt");
		command =S("./drawGraph.sh")+ S(" path") + S(pathToPrint);
		system(command.c_str());

		return ;
}

void testILPwithoutLoop(){
	const char* ilpFileName = "pathCoverAllEdges.lp";
	Graph* g = new Graph();
	int x,y;

	cout<< "graph x size:"<<endl;
	cin>>x;
	cout<< "graph y size:"<<endl;
	cin>>y;

	g->initTest(x,y);
	Draw* d = new Draw();
	/*d->drawEdges(g->edges,"edges.txt");
	d->drawNodes(g->nodes,"nodes.txt");
	string command =S("./drawGraph.sh")+ S(" withLoopGraph");
	system(command.c_str());*/

	ILPgen ilpGen;
	ilpGen.g = g;
	int pathNum = 2;

	cout<< "pathNum :"<<endl;
	cin>>pathNum;
	ilpGen.pathToCoverAllEdges(ilpFileName,pathNum);
	ilpGen.ilpRun(ilpFileName);





	for(int i = 0; i<pathNum;i++){
		vector<Edge*> pathWithLoop = ilpGen.getPath(i);
		d->drawEdges(pathWithLoop,"edges.txt");
		d->drawNodes(g->nodes,"nodes.txt");
		string command =S("./drawGraph.sh")+ S(" withLoopP")+S(i);
		system(command.c_str());

		vector<vector<Edge*>> paths = g->getPathsFromEdges(pathWithLoop);
		vector<Edge*> path = paths.at(0);

		d->drawEdges(path,"edges.txt");
		d->drawNodes(g->nodes,"nodes.txt");
		command =S("./drawGraph.sh")+ S(" path")+S(i);
		system(command.c_str());
		//drawRings

		vector<vector<Edge*>> rings = g->getRingsFromEdges(pathWithLoop);

		vector<Edge*> allRingsEdges;
		for(vector<Edge*> ring:rings){
			d->drawEdges(ring,"edges.txt");
			//d->drawNodes(g->nodes,"nodes.txt");

			//draw nodes in the ring
			/*int ha;
			cout<< "say ha"<<endl;
			cin>>ha;*/
			vector<Node*> nodesOnRing= g->getNodesOutRing(ring);
			d->drawNodes(nodesOnRing,"nodes.txt");

			//command =S("./drawGraph.sh")+ S(" rings")+S(i);
			//system(command.c_str());
			//return;
			allRingsEdges.insert(allRingsEdges.end(),ring.begin(),ring.end());

		}

		/*vector<Edge*> ring0 = rings.at(0);

		map<Node*,int> steps = g->ringToOtherRings(ring0,rings);
		for(auto const it:steps){
			Node* first = it.first;
			int second = it.second;
			cout<<"name is "<< first->name<<endl;
			cout<<"step is "<<second<<endl;

		}
		N n0  = g->getNode(0,9);
		V<N> otherRingsNodes;
		for(auto it = rings.begin()+1;it!=rings.end();it++){
			V<E> ringEdge = *it;
			V<N> otherRingNodes = g->getNodesOnRing(ringEdge);
			otherRingsNodes.insert(otherRingsNodes.end(),otherRingNodes.begin(),otherRingNodes.end());
		}

		V<PATHE> shortedPaths = g->fromStepToPath(steps,n0,otherRingsNodes);
		for(PATHE p: shortedPaths){
			d->drawEdges(p,"edges.txt");
		}*/

		vector<Edge*> ringS;
		ringS.push_back(g->getEdge(0,0,1,0));
		vector<Edge*> ringT;
		ringT.push_back(g->getEdge(8,9,9,9));

		rings.push_back(ringS);rings.push_back(ringT);

		V<V<PATHE>> network =  g->connectRings(ringS,ringT,rings);

		for(V<PATHE> paths:network){
			for(PATHE p:paths){
				d->drawEdges(p,"edges.txt");
			}
		}

		command =S("./drawGraph.sh")+ S(" rings")+S(i);
		system(command.c_str());
		return;

		//path cover ring
		/*cout<< "pathNum cover ring:"<<endl;
		cin>>pathNum;
		ilpGen.pathCoverSeverEdges(ilpFileName,pathNum,allRingsEdges);
		ilpGen.ilpRun(ilpFileName);

		vector<Edge*> pathCoverRing = ilpGen.getPath(0);
		d->drawEdges(pathCoverRing,"edges.txt");
		d->drawNodes(g->nodes,"nodes.txt");
		command =S("./drawGraph.sh")+ S(" coverrings")+S(i);
		system(command.c_str());*/

	}
	return;

}
int main() {
	testCutGeneralGraph();
	return 0;
	testTree();
	return 0;

	testCut();
	return 0;
	testEliminateLoop();
	return 0;


	testLR();
	return 0;

	testSameTime();
	return 0;

	testSplitGraph();
	return 0;



	testReadXml();

	return 0;




	testILPwithoutLoop();
	return 0;

	Draw* d = new Draw();



	const char* ilpFileName = "pathCoverAllEdges.lp";
	Graph* g = new Graph();
	int x,y;

	cout<< "graph x size:"<<endl;
	cin>>x;

	cout<< "graph y size:"<<endl;
	cin>>y;

	g->initTest(x,y);


	ILPgen ilpGen;
	ilpGen.g = g;
	ilpGen.pathToCoverAllEdges(ilpFileName,2);
	ILP::ILP_solve(ilpFileName);
	return 0;

/*	for(int i = 2; i <4 ; i++){
		for(int j = 2; j <4 ; j++ ){
			Node* n = g->getNode(i,j);
			for(Node* nbr:*n->getAdjNodes()){
				Edge* e = g->getEdge(n,nbr);
				e->isHole = true;
			}
		}
	}

	for(int i = 5; i <8 ; i++){
		for(int j = 5; j <8 ; j++ ){
			Node* n = g->getNode(i,j);
			for(Node* nbr:*n->getAdjNodes()){
				Edge* e = g->getEdge(n,nbr);
				e->isWall = true;
			}
		}
	}*/

	Graph* chip = g->fromGraphToChip();

	//d->drawEdges(g->edges,"edges.txt");
	//d->drawNodes(g->nodes,"nodes.txt");
	d->drawEdges(chip->edges,"edges.txt");
	d->drawNodes(chip->nodes,"nodes.txt");
	string command =S("./drawGraph.sh")+ S(" chip");
	system(command.c_str());

	d->drawEdges(g->edges,"edges.txt");
	d->drawNodes(g->nodes,"nodes.txt");
	command =S("./drawGraph.sh")+ S(" graph");
	system(command.c_str());





	/*Graph* subG = g->splitGraphByBoundingBox(0,0,5,5);
	d->drawEdges(subG->edges,"edges.txt");
	d->drawNodes(subG->nodes,"nodes.txt");
	string command1 =S("./drawGraph.sh") + S(" subG");
	system(command1.c_str());

	return 0;
	//return 0;*/
	int step = 0;

	while(true){
		int graphNum = 5;
		int valveSize = 50;
		vector<Graph*> subGraphs = g->splitGraph(graphNum, valveSize);
		int counter = 0;
		for(Graph* subg:subGraphs){
			Graph* subChip = subg->fromGraphToChip();
			d->drawEdges(subChip->edges,"edges.txt");
			d->drawNodes(subChip->nodes,"nodes.txt");
			command =S("./drawGraph.sh") + S(" ") + S("subChip") + S(counter);
			//system(command.c_str());

			g->cutOffSubGraph(subg);

			ILPgen ilpGen;
			ilpGen.g = subg;
			cout<<"input path number:"<<endl;
			int pathNumber;
			cin>>pathNumber;
			ilpGen.pathToCoverAllEdges(ilpFileName,pathNumber);
			ilpGen.ilpRun(ilpFileName);
			vector<Edge*> path = ilpGen.getPath(0);
			d->drawEdges(path,"edges.txt");
			//d->drawEdges(subChip->edges,"edges.txt");
			//d->drawNodes(subChip->nodes,"nodes.txt");
			command =S("./drawGraph.sh") + S(" ") + S("path") + S(counter);
			//system(command.c_str());
			counter++;

		}





		/*command =S("./drawGraph.sh") + S(" ") + S("whole") + S(step);
		system(command.c_str());
		//draw cut off graph
		command = S("./drawGraph.sh") + S(" ") + S("cutOff") + S(step);
		d->drawEdges(g->edges,"edges.txt");
		d->drawNodes(g->nodes,"nodes.txt");
		system(command.c_str());

		vector<Edge*> walls = g->getWallsBetweenSubGrapsh(&subGraphs);



		//draw walls
		d->drawEdges(walls,"edges.txt");
		d->drawNodes(g->nodes,"nodes.txt");
		command =S("./drawGraph.sh");
		command += S(" walls");
		cout<<"drawing walls"<<endl;
		system(command.c_str());

		ILPgen ilpGen;
		ilpGen.g = g;
		cout<<"input path number:"<<endl;
		int pathNumber;
		cin>>pathNumber;

		ilpGen.pathCoverSeverEdges(ilpFileName,pathNumber,walls);
		ILP::ILP_solve(ilpFileName);
		return 0;*/




		cout<<"0 to exit, 1 to continue"<<endl;
		int c;
		cin>>c;
		getchar();
		if(c == 0)
			break;

	}
	command =S("./drawGraph.sh") + S(" ") + S("all");
	system(command.c_str());
	cout<<"bye bye"<<endl;
	return 0;

	int graphNum = 5;
	int nodesSize = 20;
	vector<Graph*> subGraphs = g->splitGraph(graphNum, nodesSize);
	int counter = 0;
	for(Graph* subg:subGraphs){
		d->drawEdges(subg->edges,"edges.txt");
		d->drawNodes(subg->nodes,"nodes.txt");
		counter++;
		g->cutOffSubGraph(subg);

	}
	command =S("./drawGraph.sh") + S(" ") + S("whole");
	system(command.c_str());

	command = S("./drawGraph.sh") + S(" ") + S("cutOff");
	d->drawEdges(g->edges,"edges.txt");
	d->drawNodes(g->nodes,"nodes.txt");
	system(command.c_str());
	return 0;
	//return 0;

	vector<Edge*> walls = g->getWallsBetweenSubGrapsh(&subGraphs);
/*	for(Graph* subg:subGraphs){
		d->drawEdges(subg->edges,"edges.txt");
		d->drawNodes(subg->nodes,"nodes.txt");
		counter++;
	}*/
	d->drawEdges(walls,"edges.txt");
	d->drawNodes(g->nodes,"nodes.txt");
	command =S("./drawGraph.sh");
	command += S(" walls");
	cout<<"drawing walls"<<endl;
	system(command.c_str());




	return 0;

}




/*
 * ILP.cpp
 *
 *  Created on: Apr 6, 2017
 *      Author: ga63quk
 */

#include "ILP.h"

ILP::ILP() {
	// TODO Auto-generated constructor stub

}

ILP::~ILP() {
	// TODO Auto-generated destructor stub
}

#ifndef ILP_CPP
#define ILP_CPP
#include <hash_map>
#include <map>
#include <iostream>
#include "gurobi_c++.h"
#include <string.h>
#include <functional>
#include <fstream>
using namespace std;


/*struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};

template <class Key, class Value>
unsigned long mapSize(const std::map<Key,Value> &map){
    unsigned long size = sizeof(map);
    for(typename std::map<Key,Value>::const_iterator it = map.begin(); it != map.end(); ++it){
        size += it->first.size();
        size += sizeof(Value);
    }
    return size;
}
void toFile(map<string,int>& varMap, size_t size){
	std::ofstream fs("example.bin", std::ios::out | std::ios::binary | std::ios::app);
	fs.write((const char*)&varMap, size);
	fs.close();
}*/


std::vector<std::map<std::string,float>> ILP::ILP_solve_pool(const char* argv){
  vector<map<string,float>> results;
  GRBVar* vars = 0;



  try {
  //double timeMax = 600;
  //double ilpGap = 0.10; //for generate graph 20*20 holes
  //double ilpGap = 0.05;
    //GRBEnv env = GRBEnv();
	 GRBEnv* env = new GRBEnv("poolsearch_c++.log");
    GRBModel model = GRBModel(*env, argv);
    //model.getEnv().set(GRB_DoubleParam_IntFeasTol,1e-6);
    model.getEnv().set(GRB_DoubleParam_TimeLimit,timeMax);
    model.getEnv().set(GRB_DoubleParam_MIPGap, ilpGap);
    vars = model.getVars();
    // set global sense for ALL objectives
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
    // Limit how many solutions to collect
    model.set(GRB_IntParam_PoolSolutions, 1024);

    // Limit the search space by setting a gap for the worst possible solution that will be accepted
    model.set(GRB_DoubleParam_PoolGap, 0.10);

    // do a systematic search for the k-best solutions
    model.set(GRB_IntParam_PoolSearchMode, 2);

    // save problem
    model.write("poolsearch_c++.lp");

    // Optimize
    model.optimize();

    int optimstatus = model.get(GRB_IntAttr_Status);

    if (optimstatus == GRB_INF_OR_UNBD) {
      model.getEnv().set(GRB_IntParam_Presolve, 0);
      model.optimize();
      optimstatus = model.get(GRB_IntAttr_Status);
    }
    //ofstream varNames;
    ofstream varResults;
   // varNames.open("varName.txt");
    varResults.open("varResults.txt");

    if (optimstatus == GRB_OPTIMAL) {
      int nSolution  = model.get(GRB_IntAttr_SolCount);
     // cout << "Number of solutions found: " << nSolution << endl;
/*      if(nSolution > 10)
          nSolution = 10;*/
      for(int solutionNumber = 0 ; solutionNumber < nSolution;solutionNumber++){
        map<std::string,float> result;
        model.set(GRB_IntParam_SolutionNumber, solutionNumber);

        double objval = model.get(GRB_DoubleAttr_ObjVal);
        GRBVar* evars = model.getVars();
       //cout << "Optimal objective: " << objval << endl;
        //cout << "printing " << solutionNumber << "st solution"<<endl;
        varResults<< solutionNumber <<"st answer"<<endl;
        for(int i =0; i<model.get(GRB_IntAttr_NumVars);i++){
            string varName = evars[i].get(GRB_StringAttr_VarName);
            result[varName] = evars[i].get(GRB_DoubleAttr_Xn);
            varResults << varName << "     "<< result[varName] << "\n";
        }
         
        results.push_back(result); 

      }      //important! when deal with integer, +0.5 may make it round

    }
    else if (optimstatus == GRB_INFEASIBLE) {
      cout << "Model is infeasible" << endl;

      // compute and write out IIS

      //model.computeIIS();
      //model.write("model.ilp");
    }
    else if (optimstatus == GRB_UNBOUNDED) {
      cout << "Model is unbounded" << endl;
    }
    else {
      cout << "Optimization was stopped with status = "
           << optimstatus << endl;
      int nSolution  = model.get(GRB_IntAttr_SolCount);
      //cout << "Number of solutions found: " << nSolution << endl;
/*      if(nSolution > 10)
          nSolution = 10;*/
      for(int solutionNumber = 0 ; solutionNumber < nSolution;solutionNumber++){
        map<std::string,float> result;
        model.set(GRB_IntParam_SolutionNumber, solutionNumber);
        double objval = model.get(GRB_DoubleAttr_ObjVal);
        //cout << "Optimal objective: " << objval << endl;
        //cout << "printing " << solutionNumber << "st solution"<<endl;
        GRBVar* evars = model.getVars();
        for(int i =0; i<model.get(GRB_IntAttr_NumVars);i++){
            string varName = evars[i].get(GRB_StringAttr_VarName);
            result[varName] = evars[i].get(GRB_DoubleAttr_Xn);
            varResults << varName << "     "<< result[varName] << "\n";
        }
         
      	  results.push_back(result);
      }
    }

  } catch(GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch (...) {
    cout << "Error during optimization" << endl;
  }

  return results;

}

std::map<std::string,float>	ILP::ILP_solve(const char* argv)
{

	map<string,float> results;
	GRBVar* vars = 0;

//	if (argc < 2) {
//		cout << "Usage: lp_c++ filename" << endl;
//		return 1;
//	}

  try {
	//double timeMax = 600;
	//double ilpGap = 0.10; //for generate graph 20*20 holes
	//double ilpGap = 0.05;
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env, argv);
    //model.getEnv().set(GRB_DoubleParam_IntFeasTol,1e-6);
    model.getEnv().set(GRB_DoubleParam_TimeLimit,timeMax);
    model.getEnv().set(GRB_DoubleParam_MIPGap, ilpGap);
    vars = model.getVars();
    model.optimize();

    int optimstatus = model.get(GRB_IntAttr_Status);

    if (optimstatus == GRB_INF_OR_UNBD) {
      model.getEnv().set(GRB_IntParam_Presolve, 0);
      model.optimize();
      optimstatus = model.get(GRB_IntAttr_Status);
    }
    //ofstream varNames;
    ofstream varResults;
   // varNames.open("varName.txt");
    varResults.open("varResults.txt");

    if (optimstatus == GRB_OPTIMAL) {
      double objval = model.get(GRB_DoubleAttr_ObjVal);
      cout << "Optimal objective: " << objval << endl;
      for(int i =0; i<model.get(GRB_IntAttr_NumVars);i++){
    	  	string varName = vars[i].get(GRB_StringAttr_VarName);
    	  	results[varName] = vars[i].get(GRB_DoubleAttr_X);
    		varResults << varName << "     "<< results[varName] << "\n";
    	  	//cout << vars[i].get(GRB_StringAttr_VarName) << " " << vars[i].get(GRB_DoubleAttr_X) << endl;

      }

      //important! when deal with integer, +0.5 may make it round


      cout << "size of results is " << sizeof(results)*results.size()<< endl;
    /*  cout << "using mapSize function, size is " << mapSize(results)<<endl;
      toFile(results,mapSize(results));*/


    } else if (optimstatus == GRB_INFEASIBLE) {
      cout << "Model is infeasible" << endl;

      // compute and write out IIS

      //model.computeIIS();
      //model.write("model.ilp");
    } else if (optimstatus == GRB_UNBOUNDED) {
      cout << "Model is unbounded" << endl;
    } else {
      cout << "Optimization was stopped with status = "
           << optimstatus << endl;

      double objval = model.get(GRB_DoubleAttr_ObjVal);
           cout << "Optimal objective: " << objval << endl;
           for(int i =0; i<model.get(GRB_IntAttr_NumVars);i++){
         	  	string varName = vars[i].get(GRB_StringAttr_VarName);
         	  	results[varName] = vars[i].get(GRB_DoubleAttr_X);

         	  	//cout << vars[i].get(GRB_StringAttr_VarName) << " " << vars[i].get(GRB_DoubleAttr_X) << endl;
         	  	varResults << varName << "     "<< results[varName] << "\n";
           }


    }

  } catch(GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch (...) {
    cout << "Error during optimization" << endl;
  }

  return results;

  //return 0;
}
#endif

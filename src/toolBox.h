#ifndef TOOLBOX_H_
#define TOOLBOX_H_
#include "assert.h"
using namespace std;

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>


#define M 10000
#define BIG_INTEGER 10000
#define INTEGERBOUND 100
#define R 0.1
#define RED 0
#define BLUE 1
#define WHITE 2

#define PATHN vector<Node*>
#define PATHE vector<Edge*>
#define V vector
#define N Node*
#define E Edge*

//const char* drawGraphSh = "drawGraph.sh";
string s(int i);

string s(const char* s);
string s(string s);
string S(int i);

string S(const char* s);
string S(string s);

class  algo final{

public:
	template<typename T>
	static bool vecContains(const  std::vector<T>& vec, T& element)
	{
		if(find(vec.begin(),vec.end(),element) == vec.end())
			return false;
		else
			return true;
	}

	template<typename key, typename T>
	static bool mapContains(std::map<key,T>& m, key& k)
	{
		if(m.find(k)==m.end())
			return false;
		else
			return true;
	}
	template<typename T>
	static void remove(std::vector<T>& vec, T& element)
	{
		auto it = find(vec.begin(),vec.end(),element);

		if(it == vec.end())
			return;

		vec.erase(it);

	}
	template<typename T>
	static void remove(std::vector<T>& vec, std::vector<T>& elements)
	{
		for(T e:elements){
			remove(vec,e);
		}
	}

	template<typename T>
	static void join2Vec(std::vector<T>& vec0, std::vector<T>& vec1)
	{
		vec0.insert(vec0.end(),vec1.begin(),vec1.end());
	}


	template<typename pair_first,typename pair_second,typename T>
	static T map_select_pair(map<pair<pair_first,pair_second>,T>& amap, pair<pair_first,pair_second>& apair)
	{
		if(mapContains(amap,apair))
			return amap[apair];

		pair<pair_first,pair_second> reverser_pair = make_pair(apair.second,apair.first);

		if(mapContains(amap,reverser_pair))
			return amap[reverser_pair];

		T* r = NULL;

		return *r;

	}


	template<typename T>
	static void distincAddVec(std::vector<T>& vec,  T& element){
		if(!vecContains(vec,element))
			vec.push_back(element);
	}


	static int countDigital(int a){
		//assert(a > 0);
		int dig = 1;
		while( a > 10){
			a += 1;
			a = a / 10;
		}
		return dig;
	}

	template<typename T>
	static void pop_front(std::vector<T>& vec)
	{
	    assert(!vec.empty());
	    vec.erase(vec.begin());
	}

	template<typename T>
	static void pop_To_Element(std::vector<T>& vec, T& element)
	{
		if(!vecContains(vec,element))
			return;
		while(element != vec.back()){
			vec.pop_back();
		}
	}

	static float hash2Int(float a, float b){

		float r = a*100+b;

		return r;

	}
	static float hash4Int(float a,float b, float c, float d){


		float dig_1 = d;
		float  dig_10 =  c * 100;
		float  dig_100 = b * 10000;
		float dig_1000 = a * 1000000;
		return dig_1 + dig_10 + dig_100 + dig_1000;

	}

};
#endif

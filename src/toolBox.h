#ifndef TOOLBOX_H_
#define TOOLBOX_H_
#include "assert.h"
using namespace std;

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>


#define M 10000
#define BIG_INTEGER	 1000
#define INTEGERBOUND 1000
#define R 0.1
#define RED 0
#define BLUE 1
#define WHITE 2

#define PATHN vector<Node*>
#define PATHE vector<Edge*>
#define V vector
#define N Node*
#define E Edge*


#define cs const
#define cp const P&
#define opr operator
const  double eps = 1e-8;
inline int sig(double x) {return (x>eps)-(x<-eps);}

struct P{
    double x, y;
    void in() { scanf("%lf%lf", &x, &y); }
    P(double x=0.0, double y=0.0) : x(x), y(y) {}

    P opr-(cp a)cs { return P(x-a.x, y-a.y); }
    double opr^(cp a)cs { return x*a.y - y*a.x; }    //叉积
    double opr*(cp a)cs {return x*a.x + y*a.y;}

    double cross(P a, P b) { return (a-*this) ^ (b-*this); }
    double dot(P a, P b)  { return (a-(*this)) * (b-(*this)); }
    bool on_seg(P a, P b) { return !sig(cross(a, b)) && sig(dot(a, b)) <= 0; }//判断是否在点上
};



//const char* drawGraphSh = "drawGraph.sh";
string s(int i);

string s(const char* s);
string s(string s);
string S(int i);

string S(const char* s);
string S(string s);


class  algo final{

public:
	bool static seg(P a, P b, P c, P d) { //判断相交(a - b)线段 、(c - d)线段
	    if(a.on_seg(c, d) || b.on_seg(c, d) || c.on_seg(a, b) || d.on_seg(a, b))
	        return true;
	    return sig(a.cross(b, c)*a.cross(b, d)) < 0 && sig(c.cross(d, a)*c.cross(d, b)) < 0;
	}

	template<typename T>
	static T stringToNumber(string& s){
		T r;
		istringstream abuffer(s);
		abuffer>>r;
		return r;
	}

	template<typename T>
	static bool vecContains(const  std::vector<T>& vec, T& element)
	{
		if(find(vec.begin(),vec.end(),element) == vec.end())
			return false;
		else
			return true;
	}

	template<typename T>
	static bool vecsContains(const  std::vector<std::vector<T>>& vecs, T& element)
	{
		for(V<T> vec:vecs){
			if(vecContains(vec,element))
				return true;
		}

		return false;
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

	template<typename T>
	static void writeNumbersInFile(const char* fileName, const V<T>& numbers){
		ofstream file;
 		//ofstream file;
 		if(!file.is_open())
 			file.open(fileName);


 		for(int i = 0; i<= numbers.size()-1; i++)
 		{
 			file<<numbers.at(i)<<endl;
 		}

 		file.close();

	}
	template<typename T>
	static void readNumbersInFile(const char* fileName,  V<T>& numbers){
		  string line;
		  ifstream file (fileName);
		  if (file.is_open())
		  {
		    while ( getline (file,line) )
		    {
		      T number;
				istringstream abuffer(line);
				abuffer>>number;
		    	numbers.push_back(number);
		    }
		    file.close();
		  }


		  return;

	}

};
#endif

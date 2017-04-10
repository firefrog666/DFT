#include "assert.h"
#ifndef TOOLBOX_H_
#define TOOLBOX_H_
using namespace std;

#include <string>
#include <sstream>

#define M 10000
string s(int i);

string s(const char* s);
string s(string s);
string S(int i);

string S(const char* s);
string S(string s);

class  algo final{

public:
	static int countDigital(int a){
		//assert(a > 0);
		int dig = 1;
		while( a > 10){
			a += 1;
			a = a / 10;
		}
		return dig;
	}



	static int hash2Int(int a, int b){

		int dig_b = countDigital(b);
		return a*(10^dig_b) + b;

	}

	static int hash4Int(int a,int b, int c, int d){
		int dig_b = countDigital(b);
		int dig_c = countDigital(c);
		int dig_d = countDigital(d);

		int dig_1 = d;
		int dig_10 =  c * 1000;
		int dig_100 = b * 1000000;
		int dig_1000 = a * 1000000000 ;
		return dig_1 + dig_10 + dig_100 + dig_1000;

	}

};
#endif

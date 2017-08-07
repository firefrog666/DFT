#include "toolBox.h"

using namespace std;

#include <string>
#include <sstream>
string s(int i){
	stringstream ss;
	ss<<i;
	return ss.str();
}

string s(float i){
	stringstream ss;
	ss<<i;
	return ss.str();
}
string s(const char* s){
	stringstream ss;
	ss<<s;
	return ss.str();
}
string s(string s){
	stringstream ss;
	ss<<s;
	return ss.str();
}

string S(int i){
	stringstream ss;
	ss<<i;
	return ss.str();
}
string S(float i){
	stringstream ss;
	ss<<i;
	return ss.str();
}
string S(const char* s){
	stringstream ss;
	ss<<s;
	return ss.str();
}
string S(string s){
	stringstream ss;
	ss<<s;
	return ss.str();
}




/*
 * ILP.h
 *
 *  Created on: Apr 6, 2017
 *      Author: ga63quk
 */

#ifndef ILP_H_
#define ILP_H_
#include <map>
#include <list>
#include <string>
#include <vector>
class ILP {
public:
	ILP();
	virtual ~ILP();
	 float ilpGap = 0.01;
	float timeMax = 600;

public:
	std::map<std::string,float>	ILP_solve(const char* argv);
	std::vector<std::map<std::string,float>> ILP_solve_pool(const char* argv);
void setGap(double i){
	ilpGap = i;
}
void setTime(double i){
	timeMax = i;
}


};


#endif /* ILP_H_ */

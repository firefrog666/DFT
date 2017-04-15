/*
 * ILP.h
 *
 *  Created on: Apr 6, 2017
 *      Author: ga63quk
 */

#ifndef ILP_H_
#define ILP_H_
#include <map>
#include <string>
class ILP final{
public:
	ILP();
	virtual ~ILP();

public:
static	std::map<std::string,int>	ILP_solve(const char* argv);
};

#endif /* ILP_H_ */

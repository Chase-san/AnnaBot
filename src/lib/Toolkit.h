/*
 * Toolkit.h
 *
 *  Created on: May 21, 2010
 *      Author: Chase
 */

#ifndef TOOLKIT_H_
#define TOOLKIT_H_

#include <string>

std::string trim(std::string line);
void trimi(std::string &line);
void toloweri(std::string &str);
char * load_file(const char* filename);

#endif /* TOOLKIT_H_ */

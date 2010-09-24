/*
 * Toolkit.cpp
 *
 *  Created on: May 21, 2010
 *      Author: Chase
 */

#include "Toolkit.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

string trim(string str) {
	trimi(str);
	return str;
}

void trimi(string &str) {
	//Trim Both leading and trailing spaces
	size_t startpos = str.find_first_not_of(" \t\n\r");
	size_t endpos = str.find_last_not_of(" \t\n\r");

	//if all spaces or empty return an empty string
	if(string::npos == startpos || string::npos == endpos)
		str = "";
	else
		str = str.substr(startpos, endpos - startpos + 1);

	//return str;
}

void toloweri(string &str) {
	const int length = str.length();
	for(int i = 0; i < length; ++i) {
		str[i] = tolower(str[i]);
	}
}

char * load_file(const char* filename) {
	size_t file_size = 0;
	{
		struct stat results;
		stat(filename, &results);// == 0
		file_size = results.st_size;

		if(file_size == 0) {
			return NULL;
		}
	}
	FILE * fp = fopen(filename, "rb");
	char *data = (char*) malloc(file_size * sizeof(char));

	//read data to memory
	fread(data, 1, file_size, fp);
	fclose(fp);

	return data;
}

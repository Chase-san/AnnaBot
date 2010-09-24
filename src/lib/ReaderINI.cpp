/*
 * ReaderINI.cpp
 *
 *  Created on: May 21, 2010
 *      Author: Chase
 */

#include "ReaderINI.h"
#include "Toolkit.h"
#include <stdlib.h>
#include <string.h>

ReaderINI::ReaderINI(const string& filename) {
	//load the ini into a map
	valmap = new map<string, string> ();
	string file = trim(filename);
	char *data = load_file(file.c_str());
	if(data == NULL)
		return;

	char * tok = strtok(data, "\n\r");
	while(tok != NULL) {
		string tmp(tok);
		tmp = trim(tmp);
		size_t pos = tmp.find_first_not_of("\t ");
		if(pos != string::npos && tmp.length() > 2) {
			if(tmp[pos] != '#') {
				pos = tmp.find('=');
				string key = trim(tmp.substr(0, pos));
				string val = trim(tmp.substr(pos + 1));

				//printf("%s -> %s\n",key.c_str(),val.c_str());

				valmap->insert(pair<string, string> (key, val));
			}
		}
		tok = strtok(NULL, "\n\r");
	}
	free(data);
}

ReaderINI::~ReaderINI() {
	delete valmap;
}

int ReaderINI::read_int(const string& key) const {
	return read_int(key, 0);
}
float ReaderINI::read_float(const string& key) const {
	return read_float(key, 0);
}
bool ReaderINI::read_bool(const string& key) const {
	return read_bool(key, false);
}
string ReaderINI::read_string(const string& key) const {
	return read_string(key, "");
}

int ReaderINI::read_int(const string& key, int defaultValue) const {
	map<string, string>::iterator it;
	it = valmap->find(key);
	if(it != valmap->end()) {
		return atoi(it->second.c_str());
	}
	return defaultValue;
}
float ReaderINI::read_float(const string& key, float defaultValue) const {
	map<string, string>::iterator it;
	it = valmap->find(key);
	if(it != valmap->end()) {
		return atof(it->second.c_str());
	}
	return defaultValue;
}
bool ReaderINI::read_bool(const string& key, bool defaultValue) const {
	map<string, string>::iterator it;
	it = valmap->find(key);
	if(it != valmap->end()) {
		string tmp = it->second;
		toloweri(tmp);
		if(tmp.compare("true") == 0 || tmp.compare("on") == 0 || tmp.compare("1") == 0) {
			return true;
		} else {
			return false;
		}
	}
	return defaultValue;
}
string ReaderINI::read_string(const string& key, const string& defaultValue) const {
	map<string, string>::iterator it;
	it = valmap->find(key);
	if(it != valmap->end()) {
		return it->second;
	}
	return defaultValue;
}

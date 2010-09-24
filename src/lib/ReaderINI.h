/*
 * ReaderINI.h
 *
 *  Created on: May 21, 2010
 *      Author: Chase
 */

#ifndef READERINI_H_
#define READERINI_H_

#include <string>
#include <map>

using namespace std;

class ReaderINI {
public:
	ReaderINI(const string &filename);
	~ReaderINI();

	int read_int(const string& key) const;
	float read_float(const string& key) const;
	bool read_bool(const string& key) const;
	string read_string(const string& key) const;

	int read_int(const string& key, int defaultValue) const;
	float read_float(const string& key, float defaultValue) const;
	bool read_bool(const string& key, bool defaultValue) const;
	string read_string(const string& key, const string& defaultValue) const;

	bool isLoaded() const {
		return (valmap != NULL) && (valmap->size() > 0);
	}
	int size() const {
		return valmap->size();
	}
private:
	map<string, string> * valmap;
};

#endif /* READERINI_H_ */

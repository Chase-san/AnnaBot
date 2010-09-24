//============================================================================
// Name        : NeneCPP.cpp
// Author      : Chase
//============================================================================

#include "lib/ReaderINI.h"
#include "IRC.h"
#include <cstdio>
#include <unistd.h>

int main(int argc, char *argv[]) {
	string host, nick, alt;
	int port;

	ReaderINI *ini = NULL;
	if(argc < 2) {
		printf("Attempting to load default configuration. ");
		ini = new ReaderINI("config.ini");
	} else {
		printf("Attempting to load configuration \"%s\". ", argv[1]);
		string file(argv[1]);
		ini = new ReaderINI(file);
	}

	if(ini == NULL || !ini->isLoaded()) {
		printf("Failed!\nExiting Program.\n");
		exit(1);
	}
	printf("Success!\n");

	nick = ini->read_string("nick", "Nene");
	alt = ini->read_string("alt_nick", "Yumi");
	host = ini->read_string("host", "localhost");
	port = ini->read_int("port", 6667);

	delete ini;

	//Do the actual connections and all that

	IRC irc;
	irc.setLogin(nick);
	irc.setNick(nick);
	irc.setAltNick(alt);

	irc.connect(host, port);

	//Get text from console for manual handling
	while(true) {
		char input[255];
		//This blocks for input
		if(fgets(input, 255, stdin) != NULL) {
			string raw(input);
			if(raw.length() > 2) {
				irc.sendRaw(raw);
			}
		}
		usleep(10000);
	}

	return 0;
}


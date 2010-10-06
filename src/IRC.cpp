/*
 * IRC.cpp
 *
 *  Created on: May 21, 2010
 *      Author: Chase
 */

#include "IRC.h"
#include "lib/Toolkit.h"
#include <string>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <time.h>
#ifndef WIN32
#include <unistd.h>
#endif

#define SLEEP_TIME	1
#define VERSION		"0.1.23"

const int RCVBUFSIZE = 32; // Size of receive buffer

void* irc_thread_fun(void* ptr) {
	static_cast<IRC*> (ptr)->run();
	return 0;
}

void* input_thread_fun(void* ptr) {
	static_cast<IRC*> (ptr)->input();
	return 0;
}

void* output_thread_fun(void* ptr) {
	static_cast<IRC*> (ptr)->output();
	return 0;
}

void mpSleep(int time) {
#ifdef WIN32
	Sleep(time);
#else
	usleep(time);
#endif
}

IRC::IRC() {
	_nick = "Nene";
	_altnick = "Nene_";
	_login = "Nene";
	_client = "NeneBOT "VERSION" Anna";

	_topics = new map<string,string>();

	connected = false;
}

IRC::~IRC() {
	if(connected)
		delete socket;
	delete _topics;
}

void IRC::connect(const string& host, const uint32_t port) {
	_server = host;
	_port = port;
	socket = new Socket(host,port);

	irc_thread::Handle irch;
	input_thread::Handle inh;
	output_thread::Handle outh;

	//OLD pthread stuff
	//pthread_create(&input_thread, NULL, input_thread_fun, (void*) this);
	//pthread_create(&output_thread, NULL, output_thread_fun, (void*) this);

	input_thread::Create((input_thread::Handler)input_thread_fun,(void*)this,&inh);
	output_thread::Create((output_thread::Handler)output_thread_fun,(void*)this,&outh);

	write("USER " + _login + " host server : " + _client);
	write("NICK " + _nick);

	connected = true;

	while(true) {
		string line = read();
		if(line.length() > 0) {
			handle(line);
			//get first and second space, to find the code
			size_t s1 = line.find_first_of(" ");
			size_t s2 = line.find_first_of(" ", s1 + 1);
			if(s2 != string::npos) {
				if(line.compare(s1 + 1, 3, "004") == 0) {
					//connected!
					break;
				} else if(line.compare(s1 + 1, 3, "433") == 0) {
					//nickname taken!
					write("NICK " + _altnick);
				}
			}
		}
	}

	//pthread_create(&irc_thread, NULL, irc_thread_fun, (void*) this);
	irc_thread::Create((irc_thread::Handler)irc_thread_fun,(void*)this,&irch);

	onConnect();
}

void IRC::reconnect() {
	disconnect();
	connect(_server,_port);
}

void IRC::disconnect() {
	if(connected) {
		fast_write("QUIT");
		delete socket; //same effect
		connected = false;
	}
}

void IRC::setNick(const string& nick) {
	if(connected) {
		write("NICK " + nick);
	} else {
		_nick = nick;
	}
}

void IRC::setAltNick(const string& alt) {
	_altnick = alt;
}

void IRC::setClient(const string& client) {
	_client = client;
}

void IRC::setLogin(const string& login) {
	_login = login;
}

void IRC::setFinger(const string& finger) {
	_finger = finger;
}

void IRC::sendRaw(const string& raw) {
	write(raw);
}

void IRC::sendMessage(const string& target, const string& message) {
	write("PRIVMSG " + target + " :" + message);
}

void IRC::sendNotice(const string& target, const string& message) {
	write("NOTICE " + target + " :" + message);
}

void IRC::sendAction(const string& target, const string& action) {
	sendCTCP(target, "ACTION " + action);
}

void IRC::sendCTCP(const string& target, const string& message) {
	write("PRIVMSG " + target + " :\x01" + message + "\x01");
}

void IRC::sendCTCPReply(const string& target, const string& message) {
	write("NOTICE " + target + " :\x01" + message + "\x01");
}

void IRC::quit() {
	write("QUIT");
}

void IRC::quit(const string& reason) {
	write("QUIT :" + reason);
}

void IRC::join(const string& channel) {
	write("JOIN " + channel);
}

void IRC::join(const string& channel, const string& key) {
	write("JOIN " + channel + " " + key);
}

void IRC::part(const string& channel) {
	write("PART " + channel);
}

void IRC::part(const string& channel, const string& reason) {
	write("PART " + channel + " :" + reason);
}

void IRC::kick(const string& channel, const string& nick) {
	write("KICK " + channel);
}

void IRC::kick(const string& channel, const string& nick, const string& reason) {
	write("KICK " + channel + " " + nick + " :" + reason);
}

void IRC::sendInvite(const string& nick, const string& channel) {
	write("INVITE " + nick + " :" + channel);
}

string IRC::getNick(void) const {
	return _nick;
}

string IRC::getAltNick() const {
	return _altnick;
}
string IRC::getClient() const {
	return _client;
}
string IRC::getLogin() const {
	return _login;
}
string IRC::getFinger() const {
	return _finger;
}

void IRC::setMode(const string& target, const string& modes) {
	write("MODE " + target + " " + modes);
}

void IRC::setTopic(const string& channel, const string& topic) {
	write("TOPIC " + channel + " :" + topic);
}

void IRC::ban(const string& channel, const string& hostmask) {
	write("MODE " + channel + " +b " + hostmask);
}

void IRC::unBan(const string& channel, const string& hostmask) {
	write("MODE " + channel + " -b " + hostmask);
}

void IRC::op(const string& channel, const string& nick) {
	write("MODE " + channel + " +o " + nick);
}

void IRC::deOp(const string& channel, const string& nick) {
	write("MODE " + channel + " -o " + nick);
}

void IRC::hop(const string& channel, const string& nick) {
	write("MODE " + channel + " +h " + nick);
}

void IRC::deHop(const string& channel, const string& nick) {
	write("MODE " + channel + " -h " + nick);
}

void IRC::voice(const string& channel, const string& nick) {
	write("MODE " + channel + " +v " + nick);
}

void IRC::deVoice(const string& channel, const string& nick) {
	write("MODE " + channel + " -v " + nick);
}

void IRC::identify(const string& password) {
	/* This function currently only identifies to NickServ on
	 * services like Anope. We may need to extend this with a flag
	 * to mark to use which identification method for e.g. UnderNet
	 * or QuakeNet. Maybe leave it alone and have the sender just
	 * do a regular PRIVMSG?
	 *
	 * TODO: Fix this?
	 */
	sendMessage("NickServ", "IDENTIFY " + password);
}

string IRC::getTopic(const string& channel) const {
	/*
	//Alternate way to do this
	map<string,string>::iterator it = _topics->find(channel);
	if(it != _topics->end())
		return it->second;
	return "";
	 */

	return (*_topics)[channel];
}

void IRC::onTime(string nick, string login, string host, string target) {
	time_t timex = time(NULL);
	string timestr = ctime(&timex);
	sendCTCPReply(nick, "TIME " + trim(timestr));
}

void IRC::onVersion(string nick, string login, string host, string target) {
	sendCTCPReply(nick, "VERSION " + _client);
}

void IRC::onPing(string nick, string login, string host, string target, string value) {
	sendCTCPReply(nick, "PING " + value);
}

void IRC::onFinger(string nick, string login, string host, string target) {
	sendCTCPReply(nick, "FINGER " + _finger);
}

void IRC::onServerPing(string line) {
	write("PONG " + line);
}

void IRC::handle(const string& oline) {
	string line = trim(oline);
	printf("%s\n", line.c_str());

	//We are pinged, respond!
	if(line.compare(0, 4, "PING") == 0) {
		onServerPing(line.substr(5));
		return;
	}

	char* cstr = new char[line.size() + 1];
	strcpy(cstr, line.c_str());

	string sender, nick, login, host;

	bool nickOnly = false;
	size_t tokn = 0;
	char *tok = strtok(cstr, " ");
	while(tok != NULL) {
		//For each token, check
		string tmp = tok;
		if(tokn == 0) {
			sender = tmp;
			if(sender[0] == ':')
				sender = sender.substr(1);

			size_t exp = sender.find("!");
			size_t at = sender.find("@");

			//TODO: Add checks for 'exp' and 'at' being string::npos
			//  to avoid calling these when there are (no ill effects
			//  have been seen from not doing so yet, however for sanity sake)
			if(exp == string::npos || at == string::npos) {
				nick = trim(sender);
				login = "";
				host = "";
				nickOnly = true;
			} else {
				nick = trim(sender.substr(0, exp));
				login = trim(sender.substr(exp + 1, at));
				host = trim(sender.substr(at + 1));
			}
		}

		if(tokn == 1) {
			tok = strtok(NULL, " ");
			string target(tok); //AKA channel

			uint32_t code = 0;
			if(tmp.length() == 3 && (code = (uint32_t) atoi(tmp.c_str())) != 0 && code < 1000) {
				string response = line.substr(sender.length()+5+target.length()+1);
				handle_numeric(code,target,response);
				onNumeric(sender,code,target,response);
			} else if(tmp.compare("PRIVMSG") == 0) {
				handle_msg(target, nick, login, host, line.substr(line.find(" :") + 2));
			} else if(tmp.compare("NOTICE") == 0) {
				onNotice(target, nick, login, host, line.substr(line.find(" :") + 2));
			} else if(tmp.compare("JOIN") == 0) {
				//TODO: Add this user to the channel.
				onJoin(target, nick, login, host);
			} else if(tmp.compare("PART") == 0) {
				//TODO: Remove this user from the channel.
				onPart(target, nick, login, host);
			} else if(tmp.compare("TOPIC") == 0) {
				onTopic(target, line.substr(line.find(" :") + 2), nick, time(NULL), true);
			} else if(tmp.compare("KICK") == 0) {
				tok = strtok(NULL, " ");
				string recipient(tok);
				onKick(target, nick, login, host, recipient, line.substr(line.find(" :") + 2));
			} else if(tmp.compare("QUIT") == 0) {
				onQuit(nick, login, host, line.substr(line.find(" :") + 2));
			} else if(tmp.compare("MODE") == 0) {
				tok = strtok(NULL, " ");
				string mode = tok;
				if(mode[0] == ':')
					mode = mode.substr(1);
				handle_mode(nick, login, host, target, mode);
			}
			//TODO: Add mode handler here
			else {
				onUnknown(line);
			}

			break;
		}

		tok = strtok(NULL, " ");
		++tokn;
	}

	delete[] cstr;
}

void IRC::handle_msg(const string& chan, const string& nick, const string& login, const string& host, const string& omsg) {
	string msg = omsg;
	if(chan.find_first_of("#&+!") == 0) {
		//Channel Message
		onMessage(chan, nick, login, host, msg);
	} else {
		//Private Messages
		if(msg.find("\x001") == 0 && msg.rfind("\x001") == msg.length() - 1) {
			//CTCP
			msg = msg.substr(1, msg.length() - 2);
			if(msg.compare("TIME") == 0) {
				onTime(nick,login,host,chan);
			} else if(msg.compare("VERSION") == 0) {
				onVersion(nick,login,host,chan);
			} else if(msg.substr(0, 4).compare("PING") == 0) {
				onPing(nick,login,host,chan,msg.substr(5));
			} else if(msg.compare("FINGER") == 0) {
				onFinger(nick,login,host,chan);
            } else if(msg.substr(0, 6).compare("ACTION") == 0) {
            	onAction(nick,login,host,chan,msg.substr(7));
            }

			return;
		}
		onPrivateMessage(nick, login, host, msg);
	}
}

void IRC::handle_numeric(int code, const string& target, const string& response) {
	if(code == RPL_ENDOFMOTD) {
		onMOTD();
	} else if(code == RPL_TOPIC) {
		string channel = response.substr(0,response.find(" :"));
		string topic = response.substr(response.find(" :") + 2);

		//Have I ever mentioned I really love that you can overload operators in C++?
		(*_topics)[channel] = topic;
	} else if(code == RPL_TOPICINFO) {
		//Get channel topic, and tell it back via onTopic
		char cstr[response.length()+1];
		strcpy(cstr, response.c_str());

		string channel(strtok(cstr, " "));
		string setBy(strtok(NULL," "));
		long time = atol(strtok(NULL," "));

		onTopic(channel, getTopic(channel), setBy, time, false);
	}
}

void IRC::handle_mode(const string& nick, const string& login, const string& host,
		const string& target, const string& mode) {
	//TODO: We can either have +a+b+c OR +abc

	//Possible modes are channel and user modes
	if(target.find_first_of("#&+!") == 0) {
		//Channel mode
		//TODO: Mode may contain a user list! Check for it.
	} else {
		//User mode

	}
}

void IRC::remove_channel(const string& channel) {
	//TODO: Determined what happens if you try to erase something from a map that isn't in it.
	_topics->erase(channel);
}

string IRC::read() {
	string out = "";
	if(input_buffer.size() > 0) {
		out = input_buffer.front();
		input_buffer.pop();
	}
	return out;
}

void IRC::write(const string& line) {
	printf(">>> %s\n", line.c_str());
	output_buffer.push(line + "\n");
}

void IRC::fast_write(const string& line) {
	string out = trim(line) + "\n";
	socket->write(out.c_str(), out.length());
}

void IRC::input() {
	string line_buffer = "";
	char buffer[RCVBUFSIZE + 1];
	while(true) {
		int bytesReceived = 0;

		//this thread is used for Socket IO
		if((bytesReceived = (socket->read(buffer, RCVBUFSIZE))) <= 0) {
			//We have been disconnected
			break;
		}

		buffer[bytesReceived] = '\0';
		line_buffer += buffer;

		//if the line buffer has a new line, flush it to read
		size_t line = line_buffer.find("\n");
		if(line != string::npos) {
			input_buffer.push(line_buffer.substr(0, line));
			line_buffer = line_buffer.substr(line + 1);
		}

		mpSleep(SLEEP_TIME);
	}

	onDisconnect();
}

void IRC::output() {
	while(true) {
		if(output_buffer.size() > 0) {
			string out = output_buffer.front();
			output_buffer.pop();
			socket->write(out.c_str(), out.length());
		}
		mpSleep(SLEEP_TIME);
	}
}

void IRC::run() {
	while(true) {
		//This thread handles the bots subtasks, event calls, etc etc
		string line = read();
		if(line.length() > 0) {
			handle(line);
		}
		mpSleep(SLEEP_TIME);
	}
}


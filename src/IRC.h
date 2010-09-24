/*
 * IRC.h
 *
 *  Created on: May 21, 2010
 *      Author: Chase
 */

#ifndef __ANNA_IRC_H__
#define __ANNA_IRC_H__

#include "lib/SocketLite.h"
#include "Numeric.h"
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <queue>
#include <map>

using namespace std;

class IRC {
	friend void* irc_thread_fun(void*);
	friend void* input_thread_fun(void*);
	friend void* output_thread_fun(void*);

public:
	IRC();
	virtual ~IRC();
	void connect(const string& host, const uint32_t port);
	void reconnect();
	void disconnect();

	void setNick(const string& nick);
	void setAltNick(const string& alt);
	void setClient(const string& client);
	void setLogin(const string& login);
	void setFinger(const string& finger);

	string getNick() const;
	string getAltNick() const;
	string getClient() const;
	string getLogin() const;
	string getFinger() const;

	void sendRaw(const string& raw);
	void sendMessage(const string& target, const string& message);
	void sendNotice(const string& target, const string& message);
	void sendAction(const string& target, const string& action);
	void sendCTCP(const string& target, const string& message);
	void sendCTCPReply(const string& target, const string& message);

	void quit();
	void quit(const string& reason);
	void join(const string& channel);
	void join(const string& channel, const string& key);
	void part(const string& channel);
	void part(const string& channel, const string& reason);
	void kick(const string& channel, const string& nick);
	void kick(const string& channel, const string& nick, const string& reason);
	void identify(const string& password); // Identifying to services
	void sendInvite(const string& nick, const string& channel);
	void setMode(const string& target, const string& modes);
	void setTopic(const string& channel, const string& topic);

	//These are basic set/unset mode commands
	void ban(const string& channel, const string& hostmask);
	void unBan(const string& channel, const string& hostmask);
	void op(const string& channel, const string& nick);
	void deOp(const string& channel, const string& nick);
	void hop(const string& channel, const string& nick);
	void deHop(const string& channel, const string& nick);
	void voice(const string& channel, const string& nick);
	void deVoice(const string& channel, const string& nick);
	string getTopic(const string& channel) const;

protected:
	/**
	 *	This is called after code 004, after initial login
	 */
	virtual void onConnect() {

	}

	/**
	 *	This is called when the bot gets disconnected
	 */
	virtual void onDisconnect() {

	}

	/**
	 *	This is called when the bot receives a server ping. It replied correctly
	 *	automatically.
	 *	@param line - any text following the PING command.
	 */
	virtual void onServerPing(string line);

	/**
	 *	This is called when the bot receives a message on a channel
	 *	@param channel - Channel message recieved on
	 *	@param nick - Nickname of user
	 *	@param login - Login of user
	 *	@param host - hostmask of user
	 *	@param message - message received
	 */
	virtual void onMessage(string channel, string nick, string login, string host, string message) {

	}

	/**
	 *	This is called when the bot receives a private message
	 *	@param nick - Nickname of user
	 *	@param login - Login of user
	 *	@param host - hostmask of user
	 *	@param message - message received
	 */
	virtual void onPrivateMessage(string nick, string login, string host, string message) {

	}

	/**
	 *	This is called when the bot recieves a notice
	 *	@param target - the target of the notice (nick or channel)
	 *	@param nick - Nickname of user
	 *	@param login - Login of user
	 *	@param host - hostmask of user
	 *	@param notice - the notice text
	 */
	virtual void onNotice(string target, string nick, string login, string host, string notice) {

	}

	/**
	 *  This is called after the MOTD (code 376)
	 */
	virtual void onMOTD() {

	}

	/**
	 *	Called when a users joins a room. Could be us.
	 *	@param channel - Channel joined
	 *	@param nick - Nickname of user
	 *	@param login - Login of user
	 *	@param host - hostmask of user
	 */
	virtual void onJoin(string channel, string nick, string login, string host) {

	}

	/**
	 *	Called when a users leaves (parts) a room. Could be us.
	 *	@param channel - Channel left
	 *	@param nick - Nickname of user
	 *	@param login - Login of user
	 *	@param host - hostmask of user
	 */
	virtual void onPart(string channel, string nick, string login, string host) {

	}

	/**
	 *	Called when a users quits. Could be us.
	 *	@param nick - Nickname of user
	 *	@param login - Login of user
	 *	@param host - hostmask of user
	 *	@param reason - reason for quit
	 */
	virtual void onQuit(string nick, string login, string host, string reason) {

	}

	/**
	 *	This is called when a user kicks another user from a channel. Could be us.
	 *	@param channel - The channel the kick occured on
	 *	@param nick - Nickname of user who is kicking
	 *	@param login - Login of user who is kicking
	 *	@param host - hostmask of user who is kicking
	 *	@param recipient - nick of the person who got kicked
	 *	@param reason - reason for quit
	 */
	virtual void onKick(string channel, string nick, string login, string host, string recipient, string reason) {

	}

	/**
	 *	This is called when a numeric response is received from the server.
	 *	@param sender - The server that sent this message.
	 *	@param code - The numeric code of this numeric
	 *	@param target - The target of this numeric (usually always us)
	 *	@param message - The message of the numeric
	 */
	virtual void onNumeric(string sender, uint32_t code, string target, string message) {

	}

	/**
	 *	Called whenever a user sets the topic, or when we join a channel and discover the topic.
	 *	@param channel - The channel of the topic change
	 *	@param topic - The new topic.
	 *	@param setBy - The nick who set the topic.
	 *	@param date - Timestamp of the topic change.
	 */
	virtual void onTopic(string channel, string topic, string setBy, long date, bool changed) {

	}

	/**
	 *  Called whenever we get a TIME Request. This replies correctly by default.
	 *
	 *	@param nick - Nickname of user
	 *	@param login - Login of user
	 *	@param host - hostmask of user
	 *	@param target - target of the TIME request, be it our nick or a channel name.
	 */
	virtual void onTime(string nick, string login, string host, string target);

	/**
	 *  Called whenever we get a FINGER Request. This returns the _finger by default.
	 *	@param nick - Nickname of user
	 *	@param login - Login of user
	 *	@param host - hostmask of user
	 *	@param target - target of the FINGER request, be it our nick or a channel name.
	 */
	virtual void onFinger(string nick, string login, string host, string target);

	/**
	 *  Called whenever we get a PING Request from another user. This replies correctly
	 *  by default.
	 *	@param nick - Nickname of user
	 *	@param login - Login of user
	 *	@param host - hostmask of user
	 *	@param target - target of the PING request, be it our nick or a channel name.
	 *	@param value - value that was supplied as an argument to the PING command.
	 */
	virtual void onPing(string nick, string login, string host, string target, string value);

	/**
	 *  Called whenever an ACTION is sent from a user.
	 *	@param nick - Nickname of user
	 *	@param login - Login of user
	 *	@param host - hostmask of user
	 *	@param target - The target of the action, be it a channel or our nick.
	 *	@param action - The action carried out by the user.
	 */
	virtual void onAction(string nick, string login, string host, string target, string action) {

	}

	/**
	 *  Called whenever we receive a VERSION request. This responds with the built in client
	 *  version by default.
	 *
	 *	@param nick - Nickname of user
	 *	@param login - Login of user
	 *	@param host - hostmask of user
	 *	@param target - target of the VERSION request, be it our nick or a channel name.
	 */
	virtual void onVersion(string nick, string login, string host, string target);

	/**
	 *	This is called when we cannot figure out what the line is.
	 */
	virtual void onUnknown(string line) {

	}

private:
	string read();
	void write(const string& out);
	void fast_write(const string& out);
	void handle(const string& line);
	void handle_msg(const string& chan, const string& nick, const string& login, const string& host, const string& data);
	void handle_numeric(int code, const string& target, const string& response);
	void handle_mode(const string& nick, const string& login, const string& host, const string& target, const string& mode);
	void remove_channel(const string& channel);

	queue<string> input_buffer;
	queue<string> output_buffer;

	Socket *socket;

	void run(); /* IRC Thread */
	void input(); /* Socket Input Thread */
	void output(); /* Socket Output Thread */
	pthread_t irc_thread;
	pthread_t input_thread;
	pthread_t output_thread;

	bool connected;

	map<string,string> *_topics;

	string _client;
	string _finger;
	string _nick;
	string _altnick;
	string _login;

	string _server;
	string _password;
	uint32_t _port;
};

#endif /* __ANNA_IRC_H__ */

/*
 * SocketLite.h
 *
 * Author: Chase
 *
 * Version:
 *  1.0.2 - Sept 4, 2010
 *  	Added ServerSocket class.
 *  1.0.1 - Sept 3, 2010
 *  	Removed C ERRNO as error reporting system.
 *  	Added Exceptions as optional error reporting.
 *  1.0.0 - May 24, 2010
 *  	Initial release
 */

#ifndef __SOCKET_LITE_SOCKETLITE_H__
#define __SOCKET_LITE_SOCKETLITE_H__

#include <string>
using namespace std;

/*
 * If -fno-exceptions is used when compiling the file, then this macro is not defined.
 * Therefore exceptions would then not be used in compiling this.
 */
#ifdef __EXCEPTIONS
class SocketException: public exception {
	virtual const char* what() const throw() {
		return "Socket Error.";
	}
};
#endif

inline short port2net(const short port);
inline long ip2net(const unsigned long ip);

enum {
	SOCKS_NONE,
	SOCKS_4,
	SOCKS_4a
};

class SOCKS {
public:
	SOCKS() {
		_protocol = SOCKS_NONE;
	}
	SOCKS(int protocol) {
		_protocol = protocol;
	}
	void setServer(const string &address) {
		_address = address;
	}
	void setPort(unsigned short port) {
		_port = port;
	}
	void setUserID(const string &id) {
		_id = id;
	}

private:
	friend class Socket;
	int _protocol;
	unsigned short _port;
	string _id;
	string _address;
};

/**
 * Client TCP socket. Supports SOCKS.
 */
class Socket {
public:
	/**
	 * Initializes this socket.
	 */
	Socket()
#ifdef __EXCEPTIONS
		throw(SocketException)
#endif
	;

	/**
	 * Initializes this socket. Then proceeds to connect.
	 */
	Socket(const string &address, unsigned short port)
#ifdef __EXCEPTIONS
		throw(SocketException)
#endif
	;

	/**
	 * Closes and destroys this socket.
	 */
	~Socket();

	/**
	 * Sets up the given socks configuration. Should be set before
	 * trying to connect.
	 */
	void setSOCKS(const SOCKS *socks);

	/**
	 * Connects this socket to the given address and port
	 */
	void connect(const string &address, unsigned short port)
#ifdef __EXCEPTIONS
		throw(SocketException)
#endif
	;

	/**
	 * Closes this socket.
	 */
	void disconnect();

	/**
	 * Writes to the socket.
	 * Upon successful completion, returns the number of bytes sent.
	 * Be sure to call connect first.
	 */
	int write(const void *buffer, unsigned int length)
#ifdef __EXCEPTIONS
		throw(SocketException)
#endif
	;

	/**
	 * Reads from the socket.
	 * Returns 0 if the socket is blocking and the connection to the remote node failed.
	 * Be sure to call connect first. Returns -1 on error/disconnect.
	 */
	int read(void *buffer, unsigned int length)
#ifdef __EXCEPTIONS
		throw(SocketException)
#endif
	;


	/**
	 *   Get the foreign address.
	 */
	string getAddress()
#ifdef __EXCEPTIONS
			throw(SocketException)
#endif
	;

	/**
	 *   Get the foreign port.
	 */
	unsigned short getPort()
#ifdef __EXCEPTIONS
			throw(SocketException)
#endif
	;

private:
	friend class ServerSocket;
	friend class SOCKS;

	/**
	 * General initialization function.
	 */
	void init()
#ifdef __EXCEPTIONS
		throw(SocketException)
#endif
	;

	/**
	 * Initializes this from a descriptor
	 */
	Socket(int desc);

	int descriptor;
	bool nosocks;
	const SOCKS *_socks;
};

/**
 * Server TCP socket. Does not support SOCKS.
 */
class ServerSocket: public Socket {
public:

	/**
	 * Creates a new ServerSocket for accepting connections
	 * on the given port. The queue is the number of outstanding
	 * connections to accept.
	 */
	ServerSocket(unsigned short port, int queue = 10)
#ifdef __EXCEPTIONS
		throw(SocketException)
#endif
		;

	/**
	 * Blocks until there is a client to connect.
	 */
	Socket *accept()
#ifdef __EXCEPTIONS
		throw(SocketException)
#endif
	;

private:
	void setListen(int queue)
#ifdef __EXCEPTIONS
		throw(SocketException)
#endif
	;
};

#endif /* __SOCKET_LITE_SOCKETLITE_H__ */

/*
 * Exceptions.h
 *
 *  Created on: Sep 3, 2010
 *      Author: Chase
 */

#ifndef __SOCKET_LITE_EXCEPTIONS_H__
#define __SOCKET_LITE_EXCEPTIONS_H__

#ifdef __EXCEPTIONS
#ifdef WIN32
/* Only on Windows */
class BadWinsockLoad: public SocketException {
	virtual const char* what() const throw() {
		return "Unable to load winsock.";
	}
} ex_winsock;
#endif

class BadSocketCreate: public SocketException {
	virtual const char* what() const throw() {
		return "Failed to create socket.";
	}
} ex_create;
class BadSocketConnect: public SocketException {
	virtual const char* what() const throw() {
		return "Failed to connect socket.";
	}
} ex_connect;
class BadSocketResolve: public SocketException {
	virtual const char* what() const throw() {
		return "Failed to resolve address.";
	}
} ex_resolve;
class BadSocketWrite: public SocketException {
	virtual const char* what() const throw() {
		return "Failed to write to socket.";
	}
} ex_write;
class BadSocketRead: public SocketException {
	virtual const char* what() const throw() {
		return "Failed to read from socket.";
	}
} ex_read;
class BadSocketFetch: public SocketException {
	virtual const char* what() const throw() {
		return "Failed to fetch socket information.";
	}
} ex_fetch;
class BadSocketListen: public SocketException {
	virtual const char* what() const throw() {
		return "Failed to listen on server socket port.";
	}
} ex_listen;
#endif

#endif /* __SOCKET_LITE_EXCEPTIONS_H__ */

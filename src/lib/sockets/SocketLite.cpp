/*
 * SocketLite.cpp
 *
 *  Created on: May 24, 2010
 *      Author: Chase
 */

#include "SocketLite.h"
#include "Exceptions.h"

#ifdef _WIN32
#ifndef WIN32
#define WIN32
#endif
#endif


#include <stdint.h>
#ifdef WIN32
#include <winsock.h>
typedef int socklen_t;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#endif

using namespace std;

/**********************************************************************************
 **********************************************************************************
 *                             Client Sockets                                     *
 **********************************************************************************
 **********************************************************************************/

#define ERESOLVEHOST 2201

//This is surprisingly supported by GCC
#pragma pack(0)
typedef union {
	struct {
		union {
			struct {
				uint8_t r8a;
				uint8_t r8b;
			};
			uint16_t r16a;
		};
		union {
			struct {
				uint8_t r8c;
				uint8_t r8d;
			};
			uint16_t r16b;
		};
	};
	uint32_t r32a;
} split;
#pragma pack()

#ifdef WIN32
bool initialized = false;
#endif

short port2net(const short port) {
	return htons(port);
}
long ip2net(const unsigned long ip) {
	return htonl(ip);
}

static void fillAddr(const string &address, unsigned short port, sockaddr_in &addr)
#ifdef __EXCEPTIONS
	throw(BadSocketResolve)
#endif
{
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;

	hostent *host;
	if((host = ::gethostbyname(address.c_str())) == NULL) {
			errno = ERESOLVEHOST;
#ifdef __EXCEPTIONS
			throw ex_resolve;
#endif
			/* In case we are not debugging! */
			return;
	}
	addr.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);

	/* Assign port in network byte order */
	addr.sin_port = ::htons(port);
}

Socket::Socket()
#ifdef __EXCEPTIONS
	throw(SocketException)
#endif
{
	nosocks = true;
	init();
}

Socket::Socket(int desc) {
	nosocks = true;
	descriptor = desc;
	//init();
}

Socket::Socket(const string &address, unsigned short port)
#ifdef __EXCEPTIONS
	throw(SocketException)
#endif
{
	nosocks = true;
	init();
	connect(address, port);
}

Socket::~Socket() {
#ifdef WIN32
	::closesocket(descriptor);
#else
	::close(descriptor);
#endif
	descriptor = -1;
}

void Socket::setSOCKS(const SOCKS *socks) {
	nosocks = false;
	if(socks == NULL)
		nosocks = true;
	_socks = socks;
}

void Socket::init()
#ifdef __EXCEPTIONS
	throw(SocketException)
#endif
{
#ifdef WIN32
	if(!initialized) {
		WORD wVersionRequested;
		WSADATA wsaData;

		wVersionRequested = MAKEWORD(2, 0);

		if(WSAStartup(wVersionRequested, &wsaData) != 0) {
#ifdef WIN32SOCKEXCEPTION
			throw ex_winsock;
#endif
			return;
		}
		initialized = true;
	}
#endif
	descriptor = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
#ifdef __EXCEPTIONS
	if(descriptor < 0) {
			throw ex_create;
	}
#endif
}

void Socket::connect(const string &address, unsigned short port)
#ifdef __EXCEPTIONS
	throw(SocketException)
#endif
{
	if(nosocks) {
		sockaddr_in destAddr;
		fillAddr(address, port, destAddr);
		/* We fall back on standard C errno if we are not using exceptions. */
		if(errno == ERESOLVEHOST)
			return;
#ifdef __EXCEPTIONS
		int conn =
#endif
			::connect(descriptor, (sockaddr *) &destAddr, sizeof(destAddr));
#ifdef __EXCEPTIONS
		if(conn < 0)
			throw ex_connect;
#endif
	} else {
		//if(socks)
		if(_socks->_protocol == SOCKS_4
		|| _socks->_protocol == SOCKS_4a) {
			sockaddr_in socksAddr;
			fillAddr(_socks->_address, _socks->_port, socksAddr);

			if(errno == ERESOLVEHOST)
				return;

#ifdef __EXCEPTIONS
			int conn =
#endif
					::connect(descriptor, (sockaddr *) &socksAddr, sizeof(socksAddr));
#ifdef __EXCEPTIONS
			if(conn < 0)
				throw ex_connect;
#endif

			uint8_t out[8];
			out[0] = 0x04;
			out[1] = 0x01;

			split spl;
			spl.r16a = ::htons(port);
			out[2] = spl.r8a;
			out[3] = spl.r8b;

			if(_socks->_protocol != SOCKS_4a) {
				sockaddr_in destAddr;
				fillAddr(address, port, destAddr);
				if(errno == ERESOLVEHOST)
					return;

				spl.r32a = destAddr.sin_addr.s_addr;
				out[4] = spl.r8a;
				out[5] = spl.r8b;
				out[6] = spl.r8c;
				out[7] = spl.r8d;
			}
			write((void*)_socks->_id.c_str(),_socks->_id.length()+1);

			if(_socks->_protocol == SOCKS_4a) {
				write((void*)address.c_str(),address.length()+1);
			}
			//read until we get all the bytes we need back
		} else {
			nosocks = true;
			_socks = NULL;
		}
	}
}

void Socket::disconnect() {
#ifdef WIN32
	::closesocket(descriptor);
#else
	::close(descriptor);
#endif
	descriptor = -1;
}

int Socket::write(const void *buffer, unsigned int length)
#ifdef __EXCEPTIONS
	throw(SocketException)
#endif
{
	int send = ::send(descriptor, (char *) buffer, length, 0);
#ifdef __EXCEPTIONS
	if(send < 0) {
		throw ex_write;
	}
#endif
	return send;
}

int Socket::read(void *buffer, unsigned int length)
#ifdef __EXCEPTIONS
	throw(SocketException)
#endif
{
	int output = ::recv(descriptor, (char *) buffer, length, 0);
	if(output < 0) {
#ifdef __EXCEPTIONS
		throw ex_read;
#endif
		return -1;
	}
	return output;
}


string Socket::getAddress()
#ifdef __EXCEPTIONS
		throw(SocketException)
#endif
{
	sockaddr_in addr;
	unsigned int addr_len = sizeof(addr);

	if (::getpeername(descriptor, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0) {
#ifdef __EXCEPTIONS
		throw ex_fetch;
#endif
		return "ERR";
	}

	return ::inet_ntoa(addr.sin_addr);
}

unsigned short Socket::getPort()
#ifdef __EXCEPTIONS
		throw(SocketException)
#endif
{
	sockaddr_in addr;
	unsigned int addr_len = sizeof(addr);

	if (::getpeername(descriptor, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
#ifdef __EXCEPTIONS
		throw ex_fetch;
#endif
		return 0;
	}
	return ::ntohs(addr.sin_port);
}

/**********************************************************************************
 **********************************************************************************
 *                             Server Sockets                                     *
 **********************************************************************************
 **********************************************************************************/

ServerSocket::ServerSocket(unsigned short port, int queue)
#ifdef __EXCEPTIONS
	throw(SocketException)
#endif
{
	sockaddr_in localAddr;
	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(port);

	if (::bind(descriptor, (sockaddr *)&localAddr, sizeof(sockaddr_in)) < 0) {
#ifdef __EXCEPTIONS
		throw ex_create;
#endif
	}

	setListen(queue);
}

Socket *ServerSocket::accept()
#ifdef __EXCEPTIONS
		throw(SocketException)
#endif
{
	int newSocketDesc = ::accept(descriptor, NULL, 0);
	if (newSocketDesc < 0) {
#ifdef __EXCEPTIONS
		throw ex_connect;
#endif
		return NULL;
	}

	return new Socket(newSocketDesc);
}

void ServerSocket::setListen(int queue)
#ifdef __EXCEPTIONS
		throw(SocketException)
#endif
{
	if (::listen(descriptor, queue) < 0) {
#ifdef __EXCEPTIONS
		throw ex_listen;
#endif
	}
}

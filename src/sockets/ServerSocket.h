/*
 * ServerSocket.h
 *
 *  Created on: 12/10/2011
 *      Author: jose
 */

#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include "Socket.h"

class ServerSocket: private Socket {
public:
	ServerSocket(int port);
	ServerSocket();
	virtual ~ServerSocket();

	const ServerSocket &operator << (const std::string &) const;
	const ServerSocket &operator >> (std::string &) const;

	void accept(ServerSocket &);
};

#endif /* SERVERSOCKET_H_ */

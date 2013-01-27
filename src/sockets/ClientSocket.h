/*
 * ClientSocket.h
 *
 *  Created on: 12/10/2011
 *      Author: jose
 */

#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

#include "Socket.h"

class ClientSocket: private Socket {
public:
	ClientSocket(std::string host, int port);
	virtual ~ClientSocket();

	const ClientSocket &operator << (const std::string &) const;
	const ClientSocket &operator >> (std::string &) const;
};

#endif /* CLIENTSOCKET_H_ */

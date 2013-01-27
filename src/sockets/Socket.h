/*
 * Socket.h
 *
 *  Created on: 12/10/2011
 *      Author: jose
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;
const int MSG_NOSIGNAL_SOCKET = 0; // defined by dgame

class Socket {
public:
	Socket();
	virtual ~Socket();

	// Server initialization
	bool create();
	bool bind(const int port);
	bool listen() const;
	bool accept(Socket &) const;

	// Client initialization
	bool connect(const std::string host, const int port);

	// Data Transimission
	bool send(const std::string) const;
	int recv(std::string &) const;


	void set_non_blocking(const bool);

	bool is_valid() const { return m_sock != -1; }

private:

	int m_sock;
	sockaddr_in m_addr;
};

#endif /* SOCKET_H_ */

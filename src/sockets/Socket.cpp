/*
 * Socket.cpp
 *
 *  Created on: 12/10/2011
 *      Author: jose
 */

#include "Socket.h"
#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>

Socket::Socket() : m_sock(-1) {
	memset(&m_addr,
			0,
			sizeof(m_addr));
}

Socket::~Socket() {
	if (is_valid()) {
		::close (m_sock);
	}
}

bool Socket::create() {

	bool resultado = false;

	m_sock = socket(AF_INET,
					SOCK_STREAM,
					0);

	if (is_valid()) {
		// TIME_WAIT - argh
		int on = 1;
		if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on)) != -1) {
			resultado = true;
		}
	}

	return resultado;
}

bool Socket::bind(const int port) {

	bool resultado = false;

	if (is_valid()) {
		m_addr.sin_family = AF_INET;
		m_addr.sin_addr.s_addr = INADDR_ANY;
		m_addr.sin_port = htons ( port );

		int bind_return = ::bind(m_sock,
				(struct sockaddr *) &m_addr,
				sizeof(m_addr));

		if (bind_return != -1) {
			resultado = true;
		}
	}
	return resultado;
}

bool Socket::listen() const {

	bool resultado = false;

	if (is_valid()) {
		int listen_return = ::listen(m_sock, MAXCONNECTIONS);

		if (listen_return != -1) {
			resultado = true;
		}
	}

	return resultado;
}

bool Socket::accept(Socket &new_socket) const {
	bool resultado = false;
	int addr_length = sizeof(m_addr);
	new_socket.m_sock = ::accept(m_sock, (sockaddr *) &m_addr, (socklen_t *) &addr_length);

	if (new_socket.m_sock > 0) {
		resultado = true;
	}
	return resultado;
}

bool Socket::send(const std::string s) const {
	bool resultado = false;
	int status = ::send(m_sock, s.c_str(), s.size(), MSG_NOSIGNAL_SOCKET);
	if (status != -1) {
		resultado = true;
	}
	return resultado;
}

int Socket::recv(std::string &s) const {
	char buf[MAXRECV + 1];
	s = "";

	memset ( buf, 0, MAXRECV + 1 );

	int status = ::recv ( m_sock, buf, MAXRECV, 0 );

	if (status == -1) {
		std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
		status = 0;
	} else {
		s = buf;
	}
	return status;
}

bool Socket::connect(const std::string host, const int port) {
	bool resultado = false;
	if (is_valid()) {

		m_addr.sin_family = AF_INET;
		m_addr.sin_port = htons(port);

		int status = inet_pton(AF_INET, host.c_str(), &m_addr.sin_addr);

		if (errno != EAFNOSUPPORT) {
			status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );

			if (status == 0) {
				resultado = true;
			}
		}
	}
	return resultado;
}

void Socket::set_non_blocking(const bool b) {

	int opts;

	opts = fcntl(m_sock, F_GETFL);

	if ( opts >= 0 ) {
		if (b) {
			opts = (opts | O_NONBLOCK);
		} else {
			opts = (opts & ~O_NONBLOCK);
		}

		fcntl(m_sock, F_SETFL, opts);

	}
}

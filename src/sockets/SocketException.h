/*
 * SocketException.h
 *
 *  Created on: 12/10/2011
 *      Author: jose
 */

#ifndef SOCKETEXCEPTION_H_
#define SOCKETEXCEPTION_H_

#include <string>

class SocketException {
public:
	SocketException(std::string s);
	virtual ~SocketException();

	std::string description();

private:
	std::string m_s;
};

#endif /* SOCKETEXCEPTION_H_ */

/*
 * SocketException.cpp
 *
 *  Created on: 12/10/2011
 *      Author: jose
 */

#include "SocketException.h"

SocketException::SocketException(std::string s) : m_s(s) {

}

SocketException::~SocketException() {

}

std::string SocketException::description() {
	return m_s;
}

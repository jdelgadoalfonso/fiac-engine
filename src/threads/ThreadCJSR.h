/*
 * ThreadCJSR.h
 *
 *  Created on: 21/06/2012
 *      Author: jose
 */

#ifndef THREADCJSR_H_
#define THREADCJSR_H_

#include "Thread.h"
#include "Mutex.h"
#include "../v8/JavascriptC.h"
#include "../sockets/ServerSocket.h"
#include "../sockets/SocketException.h"
#include "../OGRE/CreadorEscena.h"

class ThreadCJSR: public Thread, public Mutex {
public:
	ThreadCJSR(const CreadorEscena *);
	virtual ~ThreadCJSR();

protected:
	void run();

private:
	JavascriptC *js;
	const CreadorEscena *mEscenaOGRE;
};

#endif /* THREADCJSR_H_ */

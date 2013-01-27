/*
 * Thread.h
 *
 *  Created on: 13/11/2011
 *      Author: jose
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>

class Thread {
public:
	Thread();
	virtual ~Thread();

	pthread_t getLaunchThread();

	int crearThread();
	int esperarThread(void **);
	void terminarThread(void *);

protected:
	virtual void run();

private:
	static void *threadMaker(void *);
	pthread_t launchThread;
};

#endif /* THREAD_H_ */

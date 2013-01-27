/*
 * Thread.cpp
 *
 *  Created on: 13/11/2011
 *      Author: jose
 */

#include "Thread.h"

Thread::Thread() {
	// TODO Auto-generated constructor stub
}

Thread::~Thread() {
	// TODO Auto-generated destructor stub
}

void *Thread::threadMaker(void *t) {
	Thread* thread = static_cast<Thread*> (t);
	void (Thread::*f)() = &Thread::run;
	(thread->*f)();
	return NULL;
}

pthread_t Thread::getLaunchThread() {
	return this->launchThread;
}

int Thread::crearThread() {
	int iret = pthread_create(
			&this->launchThread,
			NULL,
			&Thread::threadMaker,
			static_cast<void*> (this));
	return iret;
}

int Thread::esperarThread(void **retVal) {
	int iret = pthread_join(this->launchThread , retVal);
	return iret;
}

void Thread::terminarThread(void *retVal) {
	pthread_exit(retVal);
}

void Thread::run() {

}

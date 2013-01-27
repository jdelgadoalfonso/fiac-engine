/*
 * Mutex.h
 *
 *  Created on: 19/11/2011
 *      Author: jose
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#include <pthread.h>
#include <bits/pthreadtypes.h>

class Mutex {
public:
	Mutex();
	virtual ~Mutex();
protected:
	int bloquearMutex();
	int intentarBloquearMutex();
	int desbloquearMutex();
private:
	pthread_mutex_t mutex;
};

#endif /* MUTEX_H_ */

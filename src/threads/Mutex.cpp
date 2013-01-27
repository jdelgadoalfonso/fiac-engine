/*
 * Mutex.cpp
 *
 *  Created on: 19/11/2011
 *      Author: jose
 */

#include "Mutex.h"

Mutex::Mutex() {
	pthread_mutex_init(&this->mutex, NULL);
}

Mutex::~Mutex() {
	pthread_mutex_destroy(&this->mutex);
}

int Mutex::bloquearMutex() {
	return pthread_mutex_lock(&this->mutex);
}

int Mutex::intentarBloquearMutex() {
	return pthread_mutex_trylock(&this->mutex);
}

int Mutex::desbloquearMutex() {
	return pthread_mutex_unlock(&this->mutex);
}

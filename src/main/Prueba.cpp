//============================================================================
// Name        : Prueba.cpp
// Author      : Jose Antonio Delgado Alfonso
// Version     : 1.0
// Copyleft    : 
// Description : main C++, Ansi-style
//============================================================================

#include "Prueba.h"

int main (int argc, char **argv) {

	std::cout << "running....\n";

	CreadorEscena *app = new CreadorEscena();
	ThreadCJSR *thread = new ThreadCJSR(app);

	thread->crearThread();

	try {
		app->createScene();
	} catch( Ogre::Exception& e ) {
		std::cerr << "An exception has occured: " <<
				e.getFullDescription().c_str() << std::endl;
	}
	delete app;

	delete thread;

	return 0;
}

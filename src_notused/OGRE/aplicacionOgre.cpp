/**
 * This file is part of IberOgre examples.
 *
 * IberOgre examples are free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License ad
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) ant later version.
 *
 * IberOgre examples are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with IberOgre examples.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @author David Saltares Márquez
 * @file aplicacionOgre.cpp
 * @date 26-09-2010
 */

#include "aplicacionOgre.h"

using std::cout;
using std::endl;
using std::string;
using std::ostringstream;

AplicacionOgre::AplicacionOgre(
		const Ogre::String& ventana): nombreVentana(ventana) {

	// Si iniciar Ogre u OIS falla, debemos salir
	salir = (!iniciarOgre() || !iniciarOIS());
}

bool AplicacionOgre::iniciarOgre() {
	// Elegimos el fichero plugins (según Debug o Release)
#ifdef _DEBUG
	pluginsCfg = "conf/pluginsDebug.cfg";
	cout << "Debug" << endl;
#else
	pluginsCfg = "conf/plugins.cfg";
	cout << "Release" << endl;
#endif

	// Fichero de configuración y de logging
	ogreCfg = "ogre.cfg";
	ogreLog = "ogre.log";

	// Creamos el objeto principal de Ogre, la raíz
	raiz = new Ogre::Root(pluginsCfg, ogreCfg, ogreLog);

	// Tomamos el log que creamos en la inicialización de Ogre
	log = Ogre::LogManager::getSingleton().getLog(ogreLog);

	// Diálogo para configurar Ogre
	if (!raiz->restoreConfig() && !raiz->showConfigDialog()) {
		log->logMessage("AplicacionOgre::iniciarOgre(): " \
				"El usuario ha cancelado el diálogo de configuración");
		return false;
	}

	// Inicializamos Ogre y creamos una ventana
	if (raiz->initialise(true, nombreVentana)) {
		// Tomamos la ventana creada
		ventana = raiz->getAutoCreatedWindow();

		// Comenzamos a esperar eventos de ventana
		Ogre::WindowEventUtilities::addWindowEventListener(ventana, this);
	} else {
		log->logMessage("AplicacionOgre::iniciarOgre(): " \
				"No se pudo inicializar Ogre");
		return false;
	}

	return true;
}

bool AplicacionOgre::iniciarOIS() {
	// Configuramos el listener de teclado OIS
	OIS::ParamList parametrosOIS;
	size_t ventanaHandle = 0;
	ostringstream ventanaHandleStr;

	ventana->getCustomAttribute("WINDOW", &ventanaHandle);
	ventanaHandleStr << ventanaHandle;
	parametrosOIS.insert(std::make_pair(string("WINDOW"),
			     ventanaHandleStr.str())
	);

#if defined OIS_WIN32_PLATFORM
	parametrosOIS.insert(std::make_pair(string("w32_mouse"),
			     string("DISCL_FOREGROUND"))
	);
	parametrosOIS.insert(std::make_pair(string("w32_mouse"),
			     string("DISCL_NONEXCLUSIVE"))
	);
	parametrosOIS.insert(std::make_pair(string("w32_keyboard"),
			     string("DISCL_FOREGROUND"))
	);
	parametrosOIS.insert(std::make_pair(string("w32_keyboard"),
			     string("DISCL_NONEXCLUSIVE"))
	);
#elif defined OIS_LINUX_PLATFORM
	parametrosOIS.insert(std::make_pair(string("x11_mouse_grab"),
			     string("false"))
	);
	parametrosOIS.insert(std::make_pair(string("x11_mouse_hide"),
			     string("false"))
	);
	parametrosOIS.insert(std::make_pair(string("x11_keyboard_grab"),
			     string("false"))
	);
	parametrosOIS.insert(std::make_pair(string("XAutoRepeatOn"),
			     string("false"))
	);
#endif

	// Creamos el gestor de entrada (con teclado y ratón)
	gestorEntrada = OIS::InputManager::createInputSystem(parametrosOIS);
	teclado = static_cast<OIS::Keyboard*>(gestorEntrada->createInputObject(
		OIS::OISKeyboard,
		true)
	);
	raton = static_cast<OIS::Mouse*>(gestorEntrada->createInputObject(
		OIS::OISMouse,
		true)
	);

	// Pedimos que nos envíen eventos de ratón teclado
	raton->setEventCallback(this);
	teclado->setEventCallback(this);

	return true;
}

AplicacionOgre::~AplicacionOgre() {
	// Destruimos OIS
	if (gestorEntrada) {
		gestorEntrada->destroyInputObject(teclado);
		gestorEntrada->destroyInputObject(raton);
		OIS::InputManager::destroyInputSystem(gestorEntrada);
		teclado = 0;
		raton = 0;
		gestorEntrada = 0;
	}

	// Al destruir la raíz, cerramos Ogre de manera limpia
	delete raiz;
}

void AplicacionOgre::buclePrincipal() {
	// Mientras no se cierre la ventana
	while (!salir) {
		// Actualizamos la entrada
		teclado->capture();
		raton->capture();

		// Sacamos todos los eventos de ventana de la pila
		Ogre::WindowEventUtilities::messagePump();

		// No podemos renderizar porque no hemos creado un SceneManager
	}
}

void AplicacionOgre::windowClosed(Ogre::RenderWindow* ventana) {
	cout << "Ventana cerrada" << endl;
}

bool AplicacionOgre::windowClosing(Ogre::RenderWindow* ventana) {
	salir = true;
	cout << "Cerrando ventana" << endl;
	return true;
}

void AplicacionOgre::windowResized(Ogre::RenderWindow* ventana) {
	cout << "Ventana redimensionada" << endl;
}

bool AplicacionOgre::keyPressed(const OIS::KeyEvent &arg) {
	if (arg.key == OIS::KC_ESCAPE) {
		salir = true;
	}

	return true;
}

bool AplicacionOgre::keyReleased(const OIS::KeyEvent &arg) {
	return true;
}

bool AplicacionOgre::mouseMoved(const OIS::MouseEvent &arg) {
	return true;
}

bool AplicacionOgre::mousePressed(const OIS::MouseEvent &arg,
		OIS::MouseButtonID id) {
	return true;
}

bool AplicacionOgre::mouseReleased(const OIS::MouseEvent &arg,
		OIS::MouseButtonID id) {
	return true;
}

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

#include "escenaSimple.h"

/**
 * @author David Saltares Márquez
 * @file escenaSimple.cpp
 * @date 17-11-2010
 */

using std::cout;
using std::endl;

EscenaSimple::EscenaSimple(): AplicacionOgre(
		"OGRE - Luces, sombras y entorno") {
	configurarSceneManager();
    crearCamara();
    prepararRecursos();
    crearEscena();
    iniciarTemporizador();
    raiz->addFrameListener(this);
    //-------------------------------------------------------------------------
    raiz->startRendering();
}

EscenaSimple::~EscenaSimple() {
    delete temporizador;
}

void EscenaSimple::prepararRecursos() {
    Ogre::String nombreGrupo, nombreTipo, nombreArchivo;
    Ogre::ConfigFile ficheroConfig;
 
    // Abrimos el fichero de recursos
    ficheroConfig.load("conf/resources2.cfg");
 
    // Iterador para recorrer grupos
    Ogre::ConfigFile::SectionIterator iteradorGrupo =
    		ficheroConfig.getSectionIterator();
 
    // Mientras haya grupos
    while (iteradorGrupo.hasMoreElements()) {
        // Tomamos el nombre del grupo
        nombreGrupo = iteradorGrupo.peekNextKey();
 
        // Vamos a recorrer los archivos del grupo
        Ogre::ConfigFile::SettingsMultiMap *opciones =
        		iteradorGrupo.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
 
        for (i = opciones->begin(); i != opciones->end(); ++i) {
            nombreTipo = i->first;
            nombreArchivo = i->second;
            
            cout << "Archivo: " << nombreArchivo << endl;
            cout << "Tipo: " << nombreTipo << endl;
 
            // Añadimos la localización del archivo con su nombre,
            // su tipo y grupo
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
            		nombreArchivo,
            		nombreTipo,
            		nombreGrupo
            );
        }
    }
    
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void EscenaSimple::configurarSceneManager() {
    // Creamos un SceneManager genérico
    gestorEscena = raiz->createSceneManager(Ogre::ST_GENERIC, "gestorEscena");
    
    // Elegimos el tipo de sombra
    gestorEscena->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
}

void EscenaSimple::crearCamara() {
    // Creamos y configuramos la cámara
    camara = gestorEscena->createCamera("camara");
    camara->setNearClipDistance(5);
    camara->setFarClipDistance(1000);
    camara->setPosition(40, 40, 0);
    camara->lookAt(0, 0, 0);
     
    // Añadimos un Viewport
    viewport = ventana->addViewport(camara);

    // Adaptamos la proporción de la cámara para que encaje con el viewport
    camara->setAspectRatio(Ogre::Real(viewport->getActualWidth()) /
                           Ogre::Real(viewport->getActualHeight()));
                            
    camaraNodo = gestorEscena->getRootSceneNode()->createChildSceneNode(
    		"camaraNodo");
    camaraNodo->attachObject(camara);

}

void EscenaSimple::crearEscena() {
    // Luz ambiente
    gestorEscena->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
    
    // Creamos al personaje
    entidadPersonaje = gestorEscena->createEntity("entidadPersonaje",
    		"Sinbad.mesh");
    entidadPersonaje->setCastShadows(true);
    nodoPersonaje = gestorEscena->getRootSceneNode()->createChildSceneNode(
    		"nodoPersonaje");
    nodoPersonaje->attachObject(entidadPersonaje);
    nodoPersonaje->setPosition(0, 5.75, 0);
    
    // Cargamos las animaciones del personaje
    animaciones["IdleBase"] = entidadPersonaje->getAnimationState("IdleBase");
    animaciones["IdleTop"] = entidadPersonaje->getAnimationState("IdleTop");
    animaciones["RunBase"] = entidadPersonaje->getAnimationState("RunBase");
    animaciones["RunTop"] = entidadPersonaje->getAnimationState("RunTop");
    animaciones["Dance"] = entidadPersonaje->getAnimationState("Dance");
    
    animArriba = animaciones["IdleTop"];
    animAbajo = animaciones["IdleBase"];
    
    // Animaciones iniciales
    animaciones["IdleBase"]->setEnabled(true);
    animaciones["IdleBase"]->setLoop(true);
    animaciones["IdleTop"]->setEnabled(true);
    animaciones["IdleTop"]->setLoop(true);
    
    direccionPersonaje = Ogre::Vector3(0, 0, 1);

    // Creamos el plano
    Ogre::Plane plano(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane(
    		"suelo",
    		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    		plano,
    		100,
    		200,
    		10,
    		10,
    		true,
    		1,
    		5,
    		5,
    		Ogre::Vector3::UNIT_Z
    );
    entidadPlano = gestorEscena->createEntity("entidadPlano", "suelo");
    nodoPlano = gestorEscena->getRootSceneNode()->createChildSceneNode(
    		"nodoPlano");
    nodoPlano->attachObject(entidadPlano);
    entidadPlano->setMaterialName("Rockwall");
    entidadPlano->setCastShadows(true);

    // Punto de luz 1
    luzPunto1 = gestorEscena->createLight("luzPunto1");
    luzPunto1->setType(Ogre::Light::LT_POINT);
    luzPunto1->setPosition(0, 20, 10);
    luzPunto1->setSpecularColour(0.9, 0.1, 0.1); // Roja
    luzPunto1->setDiffuseColour(0.9, 0.1, 0.1);

    // Punto de luz 2
    luzPunto2 = gestorEscena->createLight("luzPunto2");
    luzPunto2->setType(Ogre::Light::LT_POINT);
    luzPunto2->setPosition(0, 20, -10);
    luzPunto2->setSpecularColour(0.1, 0.1, 0.9); // Azul
    luzPunto2->setDiffuseColour(0.1, 0.1, 0.9);

    // Punto de luz 2
    luzPunto3 = gestorEscena->createLight("luzPunto3");
    luzPunto3->setType(Ogre::Light::LT_POINT);
    luzPunto3->setPosition(10, 20, 10);
    luzPunto3->setSpecularColour(0.1, 0.9, 0.1); // Verde
    luzPunto3->setDiffuseColour(0.1, 0.9, 0.1);

    // Punto de luz 4
    luzPunto4 = gestorEscena->createLight("luzPunto4");
    luzPunto4->setType(Ogre::Light::LT_POINT);
    luzPunto4->setPosition(10, 20, -10);
    luzPunto4->setSpecularColour(0.9, 0.8, 0.05); // Amarilla
    luzPunto4->setDiffuseColour(0.9, 0.8, 0.05);

    // Creamos un SkyDome
    gestorEscena->setSkyDome(true, "cielo", 30, 1.5, 5000, false);
}

void EscenaSimple::iniciarTemporizador() {
    temporizador = new Ogre::Timer;
    ultimoFrame = 0.0f;
    deltaT = 0.0;
}

void EscenaSimple::buclePrincipal() {
 
    // Lanzamos el timer
    temporizador->reset();
    
    // Mientras no se cierre la ventana
    while (!salir) {
        // Actualizamos la entrada
        teclado->capture();
        raton->capture();
        
        // Actualizamos personaje
        actualizarPersonaje(deltaT);
        
        // Sacamos todos los eventos de ventana de la pila
        Ogre::WindowEventUtilities::messagePump();
        
        // Renderizamos la escena
        raiz->renderOneFrame();
        
        // Calculamos deltaT
        deltaT = temporizador->getMilliseconds() - ultimoFrame;
        ultimoFrame = temporizador->getMilliseconds();
    }
}

bool EscenaSimple::keyPressed(const OIS::KeyEvent &arg) {
    if (arg.key == OIS::KC_ESCAPE)
        salir = true;
        
    // Bailar o dejar de bailar
    if (arg.key == OIS::KC_D) {
        if (animArriba->getAnimationName() != "Dance") {
            animArriba->setEnabled(false);
            animAbajo->setEnabled(false);
            animArriba = animaciones["Dance"];
            animAbajo = animaciones["Dance"];
            animArriba->setEnabled(true);
            animArriba->setLoop(true);
        } else {
            animArriba->setEnabled(false);
            animAbajo->setEnabled(false);
            animArriba = animaciones["IdleTop"];
            animAbajo = animaciones["IdleBase"];
            animArriba->setEnabled(true);
            animArriba->setLoop(true);
            animAbajo->setEnabled(true);
            animAbajo->setLoop(true);
        }
    }

    return true;
}

void EscenaSimple::actualizarPersonaje(Ogre::Real deltaT) {
    animArriba->addTime(deltaT);
    animAbajo->addTime(deltaT);
    
    bool andando = false;
    
    Ogre::Vector3 direccion;
    
    if (teclado->isKeyDown(OIS::KC_UP)) {
        direccion = Ogre::Vector3(-1, 0, 0);
        andando = true;
    } else if (teclado->isKeyDown(OIS::KC_DOWN)) {
        direccion = Ogre::Vector3(1, 0, 0);
        andando = true;
    } else if (teclado->isKeyDown(OIS::KC_RIGHT)) {
        direccion = Ogre::Vector3(0, 0, -1);
        andando = true;
    } else if (teclado->isKeyDown(OIS::KC_LEFT)) {
        direccion = Ogre::Vector3(0, 0, 1);
        andando = true;
    }
    
    direccion.normalise();
    
    // Si andamos nos movemos
    if (andando) {
        nodoPersonaje->translate(
        		Ogre::Vector3(0.02, 0.02, 0.02) * direccion * deltaT,
        		Ogre::Node::TS_WORLD
        );
        
        Ogre::Quaternion rotacion = direccionPersonaje.getRotationTo(
        		direccion,
        		Ogre::Vector3::UNIT_Y
        );
        nodoPersonaje->yaw(rotacion.getYaw());
        
        direccionPersonaje = direccion;
        
        // Si la animación de abajo era estar quieto
        if (animAbajo->getAnimationName() == "IdleBase") {
            animAbajo->setEnabled(false);
            animAbajo = animaciones["RunBase"];
            animAbajo->setEnabled(true);
            animAbajo->setLoop(true);
        }
        
        // Si la animación de arriba era estar quieto
        if (animArriba->getAnimationName() == "IdleTop") {
            animArriba->setEnabled(false);
            animArriba = animaciones["RunTop"];
            animArriba->setEnabled(true);
            animArriba->setLoop(true);
        } 
    } else {
        // Si la animación de abajo era correr
        if (animAbajo->getAnimationName() == "RunBase") {
            animAbajo->setEnabled(false);
            animAbajo = animaciones["IdleBase"];
            animAbajo->setEnabled(true);
            animAbajo->setLoop(true);
        }
        
        // Si la animación de arriba era correr
        if (animArriba->getAnimationName() == "RunTop") {
            animArriba->setEnabled(false);
            animArriba = animaciones["IdleTop"];
            animArriba->setEnabled(true);
            animArriba->setLoop(true);
        }
    }
}

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

#ifndef IBEROGRE_EJEMPLOS_OGRE_NODOS_INCLUDE_ACPLICACIONOGRE_H_
#define IBEROGRE_EJEMPLOS_OGRE_NODOS_INCLUDE_ACPLICACIONOGRE_H_

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreConfigFile.h>
#include <OGRE/OgreWindowEventUtilities.h>

#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include <iostream>
#include <string>
#include <utility>

//! Ventana para una aplicación Ogre, inicia los subsistemas y captura eventos

/**
 * @author David Saltares Márquez
 * @file aplicacionOgre.h
 * @date 26-09-2010
 *
 * Clase que se encarga de iniciar y cerrar Ogre, crear una ventana
 * y permanecer a la espera capturando eventos de ventana, teclado
 * y ratón.
 *
 * Es la que inicia el bucle principal, no obstantem está diseñada
 * para ser especializada y sorbrecargar sus métodos según nuestras
 * necesidades
 */

class AplicacionOgre:	public Ogre::WindowEventListener,
						public OIS::KeyListener,
						public OIS::MouseListener {

    public:
        /**
                Constructor

                Inicializa Ogre, crea una ventana y se prepara
                para capturar eventos.
        */
        AplicacionOgre(const Ogre::String& ventana);

        /**
                Destructor

                Destruye OIS y cierra Ogre.
        */
        virtual ~AplicacionOgre();

        /**
                Inicia el bucle de juego, mientras no se reciba
                un evento de salida de la aplicación continúa
                actualizándose.
                 
                A menos que se sobrecargue no se renderizará nada
                porque no hemos iniciado el SceneManager.
        */
        virtual void buclePrincipal();

    protected:
        /**
                Evento que se dispara cuando se cierra la ventana.
                Sobrecargar para ofrecer otro comportamiento.

                @param ventana Ventana de Ogre cerrada.
        */
        virtual void windowClosed(Ogre::RenderWindow* ventana);

        /**
                Evento que se dispara cuando se redimensiona la ventana.
                Sobrecargar para ofrecer otro comportamiento.

                @param ventana Ventana de Ogre redimensionada.
        */
        virtual void windowResized(Ogre::RenderWindow* ventana);

        /**
                Evento que se dispara cuando se va a cerrar la ventana.
                Sobrecargar para ofrecer otro comportamiento.

                @param ventana Ventana de Ogre que se va a cerrar.
                @return true si se confirma el cierre, false en caso contrario.
        */
        virtual bool windowClosing(Ogre::RenderWindow* ventana);

        /**
                Evento que se dispara cuando se pulsa una tecla.
                Sobrecargar para ofrecer otro comportamiento.

                @param arg tecla pulsada.
        */
        virtual bool keyPressed(const OIS::KeyEvent &arg);

        /**
                Evento que se dispara cuando se libera una tecla.
                Sobrecargar para ofrecer otro comportamiento.

                @param arg tecla liberada.
        */
        virtual bool keyReleased(const OIS::KeyEvent &arg);

        /**
                Evento que se dispara cuando se mueve el ratón
                Sobrecargar para ofrecer otro comportamiento.

                @param arg estado del ratón.
        */
        virtual bool mouseMoved(const OIS::MouseEvent &arg);

        /**
                Evento que se dispara cuando se pulsa un botón del ratón.
                Sobrecargar para ofrecer otro comportamiento.

                @param arg estado del ratón.
                @param id botón pulsado.
        */
        virtual bool mousePressed(
        		const OIS::MouseEvent &arg,
        		OIS::MouseButtonID id
        );

        /**
                Evento que se dispara cuando se suelta un botón del ratón.
                Sobrecargar para ofrecer otro comportamiento.

                @param arg estado del ratón.
                @param id botón soltado.
        */
        virtual bool mouseReleased(
        		const OIS::MouseEvent &arg,
        		OIS::MouseButtonID id
        );

        // Objetos Ogre
        Ogre::Root* raiz;
        Ogre::RenderWindow* ventana;
        Ogre::Log* log;
        
        // Nombre aplicación
        Ogre::String nombreVentana;

        // Ficheros de plugins, logging y configuración
        Ogre::String pluginsCfg;
        Ogre::String ogreLog;
        Ogre::String ogreCfg;

        // Managers de entrada OIS
        OIS::InputManager* gestorEntrada;
        OIS::Keyboard* teclado;
        OIS::Mouse* raton;

        // Flag para controlar la salida
        bool salir;

    private:
        bool iniciarOgre();
        bool iniciarOIS();
        
};

#endif  // IBEROGRE_EJEMPLOS_OGRE_NODOS_INCLUDE_ACPLICACIONOGRE_H_

/*
 * ThreadCJSR.cpp
 *
 *  Created on: 21/06/2012
 *      Author: jose
 */

#include "ThreadCJSR.h"

ThreadCJSR::ThreadCJSR(const CreadorEscena *escenaOGRE) {
	js = NULL;
	mEscenaOGRE = escenaOGRE;
}

ThreadCJSR::~ThreadCJSR() {
	if (js) {
		delete js;
	}
	v8::V8::Dispose();
}

void ThreadCJSR::run() {

	v8::V8::Initialize();
	v8::HandleScope handle_scope;
	v8::Local<v8::String> name(v8::String::New("(shell)"));

	while(true) {
		try {
			ServerSocket s(30000);

			ServerSocket newSocket;
			s.accept(newSocket);

			js = new JavascriptC(newSocket, mEscenaOGRE);

			while (js->isConnected()) {

				std::string data;
				newSocket >> data;

				js->ExecuteString(
						v8::String::New(data.data()),
						name,
						true,
						true
				);
			}
			delete js;
		} catch (SocketException &e) {
			//TODO: Logfile for the theads (I can use the OGRE Logs)
		}
	}
}

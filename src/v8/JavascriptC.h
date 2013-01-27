/*
 * Javascript.h
 *
 *  Created on: 16/06/2012
 *      Author: jose
 */

#ifndef JAVASCRIPTC_H_
#define JAVASCRIPTC_H_

#ifdef COMPRESS_STARTUP_DATA_BZ2
#error Using compressed startup data is not supported for this sample
#endif

#include <v8.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "../sockets/ServerSocket.h"
#include "../OGRE/CreadorEscena.h"

class JavascriptC {
public:
	JavascriptC(const ServerSocket &, const CreadorEscena *);
	virtual ~JavascriptC();

	bool ExecuteString(v8::Handle<v8::String>,
			v8::Handle<v8::Value>, bool, bool);

	bool isConnected() const;

protected:
    // Initialize the singleton
    void Init();
    static v8::Handle<v8::Value> PrintWrapper(const v8::Arguments& );
    void print(const v8::Arguments& );
    static v8::Handle<v8::Value> ReadWrapper(const v8::Arguments& );
    static v8::Handle<v8::Value> LoadWrapper(const v8::Arguments& );
    v8::Handle<v8::Value> load(const v8::Arguments& );
    static v8::Handle<v8::Value> VersionWrapper(const v8::Arguments& );
    void version();
    static v8::Handle<v8::Value> DisconnectWrapper(const v8::Arguments& );
    void disconnect(const v8::Arguments& );
    static v8::Handle<v8::String> ReadFile(const char *name);

    static v8::Handle<v8::Value> SceneLoader(const v8::Arguments&);

    void ReportException(v8::TryCatch *handler);
    static const char *ToCString(const v8::String::Utf8Value& );
    // Execution environment containing built-in functions.
    v8::Persistent<v8::Context> execution_context_;
    ServerSocket mSock;
    bool mConnected;

private:
    void assignGlobalsFunctions(v8::Handle<v8::ObjectTemplate>);
    void assignApiOgreFunctions(v8::Handle<v8::ObjectTemplate>);

    const CreadorEscena *mOgreInterface;
};

#endif /* JAVASCRIPT_H_ */

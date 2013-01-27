/*
 * Javascript.h
 *
 *  Created on: 16/06/2012
 *      Author: jose
 */

#ifndef JAVASCRIPT_H_
#define JAVASCRIPT_H_

#ifdef COMPRESS_STARTUP_DATA_BZ2
#error Using compressed startup data is not supported for this sample
#endif

#include <v8.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

class Javascript {
public:
	static int go(int argc, char **argv);
	static v8::Persistent<v8::Context> CreateShellContext();
	static void DestroyShellContext(v8::Persistent<v8::Context>);
	static std::string* ExecuteString(v8::Handle<v8::String>,
			v8::Handle<v8::Value>,
			bool print_result,
			bool report_exceptions);
	static v8::Handle<v8::String> ReadFile(const char* name);
protected:
	Javascript();
	virtual ~Javascript();
	static bool run_shell;
	static void RunShell(v8::Handle<v8::Context>);
	static int RunMain(int, char* []);
	static v8::Handle<v8::Value> Print(const v8::Arguments&);
	static v8::Handle<v8::Value> Read(const v8::Arguments&);
	static v8::Handle<v8::Value> Load(const v8::Arguments&);
	static v8::Handle<v8::Value> Quit(const v8::Arguments&);
	static v8::Handle<v8::Value> Version(const v8::Arguments&);
	static void ReportException(v8::TryCatch* handler);
	static const char* ToCString(const v8::String::Utf8Value&);
};

#endif /* JAVASCRIPT_H_ */

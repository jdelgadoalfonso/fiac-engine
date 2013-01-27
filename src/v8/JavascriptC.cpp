/*
 * Javascript.cpp
 *
 *  Created on: 16/06/2012
 *      Author: jose
 */

#include "JavascriptC.h"

JavascriptC::JavascriptC(const ServerSocket &sock,
		const CreadorEscena *ogreInterface) {
	mConnected = true;
	mSock = sock;
	mOgreInterface = ogreInterface;
	Init();
}

JavascriptC::~JavascriptC() {
}

// Extracts a C string from a V8 Utf8Value.
const char* JavascriptC::ToCString(const v8::String::Utf8Value& value) {
	return *value ? *value : "<string conversion failed>";
}

// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called. Prints its arguments on stdout separated by
// spaces and ending with a newline.
v8::Handle<v8::Value> JavascriptC::PrintWrapper(const v8::Arguments& args) {

	JavascriptC* puntero =
			(JavascriptC*) (v8::External::Unwrap(args.Data()));

	puntero->print(args);

	return v8::Undefined();
}

v8::Handle<v8::Value> JavascriptC::SceneLoader(const v8::Arguments& args) {

	CreadorEscena* puntero =
			(CreadorEscena*) (v8::External::Unwrap(args.Data()));

	v8::Handle<v8::Value> result = v8::String::New("OK");

	if (args.Length() == 3) {
		v8::String::Utf8Value str0(args[0]);
		const char* name = ToCString(str0);
		v8::String::Utf8Value str1(args[1]);
		const char* mesh = ToCString(str1);
		v8::String::Utf8Value str2(args[2]);
		const char* nodeName = ToCString(str2);
		puntero->sceneLoader(name, mesh, nodeName);
	} else {
		result = v8::String::New("Number of arguments incorrect");
	}

	return result;
}

void JavascriptC::print(const v8::Arguments& args) {

	v8::HandleScope handle_scope;
	v8::Handle<v8::String> result = v8::String::New("");
	bool first = true;

	for (int i = 0; i < args.Length(); i++) {
		if (first) {
			first = false;
		} else {
			result = v8::String::Concat(result, v8::String::New(" "));
		}
		v8::String::Utf8Value str(args[i]);
		const char* cstr = ToCString(str);
		result = v8::String::Concat(result, v8::String::New(cstr));
	}

	v8::String::Utf8Value str(result);
	const char* cstr = ToCString(str);
	std::string retorno(cstr);
	mSock << retorno << "\n";
}

// The callback that is invoked by v8 whenever the JavaScript 'read'
// function is called. This function loads the content of the file named in
// the argument into a JavaScript string.
v8::Handle<v8::Value> JavascriptC::ReadWrapper(const v8::Arguments& args) {
	if (args.Length() != 1) {
		return v8::ThrowException(v8::String::New("Bad parameters"));
	}
	v8::String::Utf8Value file(args[0]);
	if (*file == NULL) {
		return v8::ThrowException(v8::String::New("Error loading file"));
	}
	v8::Handle<v8::String> source = ReadFile(*file);
	if (source.IsEmpty()) {
		return v8::ThrowException(v8::String::New("Error loading file"));
	}
	return source;
}

// The callback that is invoked by v8 whenever the JavaScript 'load'
// function is called. Loads, compiles and executes its argument
// JavaScript file.
v8::Handle<v8::Value> JavascriptC::LoadWrapper(const v8::Arguments& args) {

	JavascriptC* puntero =
			(JavascriptC*) (v8::External::Unwrap(args.Data()));

	return puntero->load(args);
}

v8::Handle<v8::Value> JavascriptC::load(const v8::Arguments& args) {
	for (int i = 0; i < args.Length(); i++) {
		v8::HandleScope handle_scope;
		v8::String::Utf8Value file(args[i]);
		if (*file == NULL) {
			return v8::ThrowException(v8::String::New("Error loading file"));
		}
		v8::Handle<v8::String> source = ReadFile(*file);
		if (source.IsEmpty()) {
			return v8::ThrowException(v8::String::New("Error loading file"));
		}
		if (!ExecuteString(source, v8::String::New(*file), false, false)) {
			return v8::ThrowException(v8::String::New("Error executing file"));
		}
	}
	return v8::Undefined();
}

// The callback that is invoked by v8 whenever the JavaScript 'quit'
// function is called. Quits.
v8::Handle<v8::Value> JavascriptC::DisconnectWrapper(
		const v8::Arguments& args) {

	JavascriptC* puntero =
			(JavascriptC*) (v8::External::Unwrap(args.Data()));
	puntero->disconnect(args);

	return v8::Undefined();
}

void JavascriptC::disconnect(
		const v8::Arguments& args) {

	// If not arguments are given args[0] will yield undefined which
	// converts to the integer value 0.
	mConnected = false;
}

v8::Handle<v8::Value> JavascriptC::VersionWrapper(const v8::Arguments& args) {
	JavascriptC* puntero =
				(JavascriptC*) (v8::External::Unwrap(args.Data()));
	puntero->version();

	return v8::Undefined();
}

void JavascriptC::version() {
	v8::Local<v8::String> result = v8::String::New(v8::V8::GetVersion());
	v8::String::Utf8Value str(result);
	const char* cstr = ToCString(str);
	std::string retorno(cstr);
	mSock << retorno << "\n";
}

// Reads a file into a v8 string.
v8::Handle<v8::String> JavascriptC::ReadFile(const char* name) {
	FILE* file = fopen(name, "rb");
	if (file == NULL) return v8::Handle<v8::String>();

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	rewind(file);

	char* chars = new char[size + 1];
	chars[size] = '\0';
	for (int i = 0; i < size;) {
		int read = fread(&chars[i], 1, size - i, file);
		i += read;
	}
	fclose(file);
	v8::Handle<v8::String> result = v8::String::New(chars, size);
	delete[] chars;
	return result;
}

// Executes a string within the current v8 context.
bool JavascriptC::ExecuteString(v8::Handle<v8::String> source,
		v8::Handle<v8::Value> name,
		bool print_result,
		bool report_exceptions) {

	v8::HandleScope handle_scope;
	v8::Context::Scope context_scope(execution_context_);
	v8::TryCatch try_catch;
	v8::Handle<v8::Script> script = v8::Script::Compile(source, name);
	bool ok = false;
	if (script.IsEmpty()) {
		// Print errors that happened during compilation.
		if (report_exceptions) {
			ReportException(&try_catch);
		}
	} else {
		v8::Handle<v8::Value> result = script->Run();
		if (result.IsEmpty()) {
			assert(try_catch.HasCaught());
			// Print errors that happened during execution.
			if (report_exceptions) {
				ReportException(&try_catch);
			}
		} else {
			assert(!try_catch.HasCaught());
			if (print_result && !result->IsUndefined()) {
				// If all went well and the result wasn't undefined then print
				// the returned value.
				ok = true;
				v8::String::Utf8Value str(result);
				const char* cstr = ToCString(str);
				//printf("%s\n", cstr);
				mSock << cstr << "\n";
			}
		}
	}
	return ok;
}

void JavascriptC::ReportException(v8::TryCatch* try_catch) {
	v8::HandleScope handle_scope;
	v8::String::Utf8Value exception(try_catch->Exception());
	const char* exception_string = ToCString(exception);
	v8::Handle<v8::Message> message = try_catch->Message();
	if (message.IsEmpty()) {
		// V8 didn't provide any extra information about this error; just
		// print the exception.
		//printf("%s\n", exception_string);
		mSock << exception_string << "\n";
	} else {
		// Print (filename):(line number): (message).
		v8::String::Utf8Value filename(message->GetScriptResourceName());
		const char* filename_string = ToCString(filename);
		int linenum = message->GetLineNumber();
		// create a stringstream
		std::stringstream flujo;
		flujo << linenum; // add number to the stream
		//printf("%s:%i: %s\n", filename_string, linenum, exception_string);
		mSock << filename_string << ":" <<
				flujo.str() << ": "<< exception_string << "\n";
		// Print line of source code.
		v8::String::Utf8Value sourceline(message->GetSourceLine());
		const char* sourceline_string = ToCString(sourceline);
		//printf("%s\n", sourceline_string);
		mSock << sourceline_string << "\n";
		// Print wavy underline (GetUnderline is deprecated).
		int start = message->GetStartColumn();
		for (int i = 0; i < start; i++) {
			//printf(" ");
			mSock << " ";
		}
		int end = message->GetEndColumn();
		for (int i = start; i < end; i++) {
			//printf("^");
			mSock << "^";
		}
		//printf("\n");
		mSock << "\n";
		v8::String::Utf8Value stack_trace(try_catch->StackTrace());
		if (stack_trace.length() > 0) {
			const char* stack_trace_string = ToCString(stack_trace);
			//printf("%s\n", stack_trace_string);
			mSock << stack_trace_string << "\n";
		}
	}
}

// C++ code for MyClass::Init
void JavascriptC::Init() {
	v8::HandleScope handle_scope; // For allocation of local handles.

	// Create a template for the global object.
	v8::Handle<v8::ObjectTemplate> global_template = v8::ObjectTemplate::New();

	v8::Handle<v8::ObjectTemplate> myApi_template = v8::ObjectTemplate::New();

	// Bind C++ funtions as callbacks
	global_template->SetInternalFieldCount(0);
	myApi_template->SetInternalFieldCount(1);

	assignGlobalsFunctions(global_template);

	assignApiOgreFunctions(myApi_template);

	// Create a new execution environment containing the built-in
	// functions
	v8::Handle<v8::Context> context = v8::Context::New(NULL, global_template);

	execution_context_ = v8::Persistent<v8::Context>::New(context);

	/* ADD THIS LINE TO ENTER CONTEXT SCOPE: */
	v8::Context::Scope context_scope(execution_context_);

	/* NOW CREATE THE INSTANCE */
	// *** myApi_template->NewInstance crashes
	v8::Local<v8::Object> myApi_instance = myApi_template->NewInstance();
	myApi_instance->SetInternalField(0, v8::External::New(this));

	// Create a top level object named 'myApi' in the global object
	context->Global()->Set(v8::String::New("myApi"), myApi_instance);
}

bool JavascriptC::isConnected() const {
	return mConnected;
}

void JavascriptC::assignGlobalsFunctions(
		v8::Handle<v8::ObjectTemplate> global_template) {

	// Bind the global 'print' function to the C++ Print callback.
	global_template->Set(
			v8::String::New("print"),
			v8::FunctionTemplate::New(
					&JavascriptC::PrintWrapper,
					v8::External::Wrap(this)
			)
	);
	// Bind the global 'load' function to the C++ Load callback.
	global_template->Set(
			v8::String::New("load"),
			v8::FunctionTemplate::New(
					&JavascriptC::LoadWrapper,
					v8::External::Wrap(this)
			)
	);
	// Bind the 'read' function
	global_template->Set(
			v8::String::New("read"),
			v8::FunctionTemplate::New(&JavascriptC::ReadWrapper)
	);
	// Bind the 'disconnect' function
	global_template->Set(
			v8::String::New("disconnect"),
			v8::FunctionTemplate::New(
					&JavascriptC::DisconnectWrapper,
					v8::External::Wrap(this)
			)
	);
	// Bind the 'disconnect' function
	global_template->Set(
			v8::String::New("version"),
			v8::FunctionTemplate::New(
					&JavascriptC::VersionWrapper,
					v8::External::Wrap(this)
			)
	);
}

void JavascriptC::assignApiOgreFunctions(
		v8::Handle<v8::ObjectTemplate> myApi_template) {

	// Bind the global 'print' function to the C++ Print callback.
	myApi_template->Set(
			v8::String::New("cargarEscena"),
			v8::FunctionTemplate::New(
					&JavascriptC::SceneLoader,
					v8::External::Wrap(
							(CreadorEscena*) this->mOgreInterface)
			)
	);
}

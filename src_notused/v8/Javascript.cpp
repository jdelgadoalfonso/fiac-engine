/*
 * Javascript.cpp
 *
 *  Created on: 16/06/2012
 *      Author: jose
 */

#include "Javascript.h"

bool Javascript::run_shell;

Javascript::Javascript() {
	// TODO Auto-generated constructor stub

}

Javascript::~Javascript() {
	// TODO Auto-generated destructor stub
}

int Javascript::go(int argc, char **argv) {
	v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
	Javascript::run_shell = (argc == 1);
	v8::HandleScope handle_scope;
	v8::Persistent<v8::Context> context = Javascript::CreateShellContext();
	if (context.IsEmpty()) {
		printf("Error creating context\n");
		return 1;
	}
	context->Enter();
	Javascript::RunMain(argc, argv);
	if (Javascript::run_shell) Javascript::RunShell(context);
	DestroyShellContext(context);
	v8::V8::Dispose();
	return 0;
}

// Extracts a C string from a V8 Utf8Value.
const char* Javascript::ToCString(const v8::String::Utf8Value& value) {
	return *value ? *value : "<string conversion failed>";
}

// Creates a new execution environment containing the built-in
// functions.
v8::Persistent<v8::Context> Javascript::CreateShellContext() {
	// Create a template for the global object.
	v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
	// Bind the global 'print' function to the C++ Print callback.
	global->Set(v8::String::New("print"),
			v8::FunctionTemplate::New(Javascript::Print));
	// Bind the global 'read' function to the C++ Read callback.
	global->Set(v8::String::New("read"),
			v8::FunctionTemplate::New(Javascript::Read));
	// Bind the global 'load' function to the C++ Load callback.
	global->Set(v8::String::New("load"),
			v8::FunctionTemplate::New(Javascript::Load));
	// Bind the 'quit' function
	global->Set(v8::String::New("quit"),
			v8::FunctionTemplate::New(Javascript::Quit));
	// Bind the 'version' function
	global->Set(v8::String::New("version"),
			v8::FunctionTemplate::New(Javascript::Version));

	return v8::Context::New(NULL, global);
}

void Javascript::DestroyShellContext(v8::Persistent<v8::Context> context) {
	context->Exit();
	context.Dispose();
}

// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called. Prints its arguments on stdout separated by
// spaces and ending with a newline.
v8::Handle<v8::Value> Javascript::Print(const v8::Arguments& args) {
	bool first = true;
	v8::Handle<v8::String> result = v8::String::New("");;
	for (int i = 0; i < args.Length(); i++) {
		v8::HandleScope handle_scope;
		if (first) {
			first = false;
		} else {
			printf(" ");
			result = v8::String::Concat(result, v8::String::New(" "));
		}
		v8::String::Utf8Value str(args[i]);
		const char* cstr = ToCString(str);
		printf("%s", cstr);
		result = v8::String::Concat(result, v8::String::New(cstr));
	}
	printf("\n");
	fflush(stdout);
	return result;
}

// The callback that is invoked by v8 whenever the JavaScript 'read'
// function is called. This function loads the content of the file named in
// the argument into a JavaScript string.
v8::Handle<v8::Value> Javascript::Read(const v8::Arguments& args) {
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
v8::Handle<v8::Value> Javascript::Load(const v8::Arguments& args) {
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
v8::Handle<v8::Value> Javascript::Quit(const v8::Arguments& args) {
	// If not arguments are given args[0] will yield undefined which
	// converts to the integer value 0.
	int exit_code = args[0]->Int32Value();
	fflush(stdout);
	fflush(stderr);
	exit(exit_code);
	return v8::Undefined();
}

v8::Handle<v8::Value> Javascript::Version(const v8::Arguments& args) {
	return v8::String::New(v8::V8::GetVersion());
}

// Reads a file into a v8 string.
v8::Handle<v8::String> Javascript::ReadFile(const char* name) {
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

// Process remaining command line arguments and execute files
int Javascript::RunMain(int argc, char* argv[]) {
	for (int i = 1; i < argc; i++) {
		const char* str = argv[i];
		if (strcmp(str, "--shell") == 0) {
			run_shell = true;
		} else if (strcmp(str, "-f") == 0) {
			// Ignore any -f flags for compatibility with the other stand-
			// alone JavaScript engines.
			continue;
		} else if (strncmp(str, "--", 2) == 0) {
			printf("Warning: unknown flag %s.\nTry --help for options\n", str);
		} else if (strcmp(str, "-e") == 0 && i + 1 < argc) {
			// Execute argument given to -e option directly.
			v8::Handle<v8::String> file_name = v8::String::New("unnamed");
			v8::Handle<v8::String> source = v8::String::New(argv[++i]);
			if (!ExecuteString(source, file_name, false, true)) return 1;
		} else {
			// Use all other arguments as names of files to load and run.
			v8::Handle<v8::String> file_name = v8::String::New(str);
			v8::Handle<v8::String> source = ReadFile(str);
			if (source.IsEmpty()) {
				printf("Error reading '%s'\n", str);
				continue;
			}
			if (!ExecuteString(source, file_name, false, true)) return 1;
		}
	}
	return 0;
}

// The read-eval-execute loop of the shell.
void Javascript::RunShell(v8::Handle<v8::Context> context) {
	printf("V8 version %s [sample shell]\n", v8::V8::GetVersion());
	static const int kBufferSize = 256;
	// Enter the execution environment before evaluating any code.
	v8::Context::Scope context_scope(context);
	v8::Local<v8::String> name(v8::String::New("(shell)"));
	while (true) {
		char buffer[kBufferSize];
		printf("> ");
		char* str = fgets(buffer, kBufferSize, stdin);
		if (str == NULL) break;
		v8::HandleScope handle_scope;
		ExecuteString(v8::String::New(str), name, true, true);
	}
	printf("\n");
}

// Executes a string within the current v8 context.
std::string* Javascript::ExecuteString(v8::Handle<v8::String> source,
		v8::Handle<v8::Value> name,
		bool print_result,
		bool report_exceptions) {
	std::string* retorno = NULL;
	v8::HandleScope handle_scope;
	v8::TryCatch try_catch;
	v8::Handle<v8::Script> script = v8::Script::Compile(source, name);
	if (script.IsEmpty()) {
		// Print errors that happened during compilation.
		if (report_exceptions)
			ReportException(&try_catch);
		return NULL;
	} else {
		v8::Handle<v8::Value> result = script->Run();
		if (result.IsEmpty()) {
			assert(try_catch.HasCaught());
			// Print errors that happened during execution.
			if (report_exceptions)
				ReportException(&try_catch);
			return NULL;
		} else {
			assert(!try_catch.HasCaught());
			if (print_result && !result->IsUndefined()) {
				// If all went well and the result wasn't undefined then print
				// the returned value.
				v8::String::Utf8Value str(result);
				const char* cstr = ToCString(str);
				retorno = new std::string(cstr);
				printf("%s\n", cstr);
			}
			return retorno;
		}
	}
}

void Javascript::ReportException(v8::TryCatch* try_catch) {
	v8::HandleScope handle_scope;
	v8::String::Utf8Value exception(try_catch->Exception());
	const char* exception_string = ToCString(exception);
	v8::Handle<v8::Message> message = try_catch->Message();
	if (message.IsEmpty()) {
		// V8 didn't provide any extra information about this error; just
		// print the exception.
		printf("%s\n", exception_string);
	} else {
		// Print (filename):(line number): (message).
		v8::String::Utf8Value filename(message->GetScriptResourceName());
		const char* filename_string = ToCString(filename);
		int linenum = message->GetLineNumber();
		printf("%s:%i: %s\n", filename_string, linenum, exception_string);
		// Print line of source code.
		v8::String::Utf8Value sourceline(message->GetSourceLine());
		const char* sourceline_string = ToCString(sourceline);
		printf("%s\n", sourceline_string);
		// Print wavy underline (GetUnderline is deprecated).
		int start = message->GetStartColumn();
		for (int i = 0; i < start; i++) {
			printf(" ");
		}
		int end = message->GetEndColumn();
		for (int i = start; i < end; i++) {
			printf("^");
		}
		printf("\n");
		v8::String::Utf8Value stack_trace(try_catch->StackTrace());
		if (stack_trace.length() > 0) {
			const char* stack_trace_string = ToCString(stack_trace);
			printf("%s\n", stack_trace_string);
		}
	}
}

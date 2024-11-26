#define NOMINMAX
#include <iostream>
#include <string.h>
#include <windows.h>
#define V8_COMPRESS_POINTERS 1
#define V8_31BIT_SMIS_ON_64BIT_ARCH 1
#define V8_ENABLE_SANDBOX 1
#include <v8/v8.h>
#include <v8/libplatform/libplatform.h>
#include <v8/v8-context.h>
#include <v8/v8-initialization.h>
#include <v8/v8-isolate.h>
#include <v8/v8-local-handle.h>
#include <v8/v8-primitive.h>
#include <v8/v8-script.h>

#include "JSEngine.h"

using std::cout, std::endl;

JSEngine::JSEngine(const char* executionPath) {
	cout << "Launching V8..." << endl;
	v8::V8::InitializeICUDefaultLocation(executionPath);
	v8::V8::InitializeExternalStartupData(executionPath);
	platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(platform.get());
	v8::V8::Initialize();
	createParams.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	V8Isolate = v8::Isolate::New(createParams);
	evalToStdout("[][(([]+![])[+!![]+!![]+!![]])+(([]+{})[+!![]])+(([]+!![])[+!![]])+(([]+!![])[+[]])][(([]+{})[+!![]+!![]+!![]+!![]+!![]])+(([]+{})[+!![]])+(([]+[][[]])[+!![]])+(([]+![])[+!![]+!![]+!![]])+(([]+!![])[+[]])+(([]+!![])[+!![]])+(([]+[][[]])[+[]])+(([]+{})[+!![]+!![]+!![]+!![]+!![]])+(([]+!![])[+[]])+(([]+{})[+!![]])+(([]+!![])[+!![]])]((([]+!![])[+!![]])+(([]+!![])[+!![]+!![]+!![]])+(([]+!![])[+[]])+(([]+[][[]])[+[]])+(([]+!![])[+!![]])+(([]+[][[]])[+!![]])+(([]+{})[+!![]+!![]+!![]+!![]+!![]+!![]+!![]])+(([]+[][[]])[+[]])+(([]+[][[]])[+!![]])+(([]+!![])[+!![]+!![]+!![]])+(([]+![])[+!![]+!![]+!![]])+(([]+{})[+!![]+!![]+!![]+!![]+!![]])+(([]+![])[+!![]])+((+([]+(+!![]+!![])+(+!![]+!![]+!![]+!![]+!![])))[(([]+!![])[+[]])+(([]+{})[+!![]])+(([]+([]+[])[(([]+{})[+!![]+!![]+!![]+!![]+!![]])+(([]+{})[+!![]])+(([]+[][[]])[+!![]])+(([]+![])[+!![]+!![]+!![]])+(([]+!![])[+[]])+(([]+!![])[+!![]])+(([]+[][[]])[+[]])+(([]+{})[+!![]+!![]+!![]+!![]+!![]])+(([]+!![])[+[]])+(([]+{})[+!![]])+(([]+!![])[+!![]])])[+!![]+!![]+!![]+!![]+!![]+!![]+!![]+!![]+!![]])+(([]+!![])[+[]])+(([]+!![])[+!![]])+(([]+[][[]])[+!![]+!![]+!![]+!![]+!![]])+(([]+[][[]])[+!![]])+(([]+([]+[])[(([]+{})[+!![]+!![]+!![]+!![]+!![]])+(([]+{})[+!![]])+(([]+[][[]])[+!![]])+(([]+![])[+!![]+!![]+!![]])+(([]+!![])[+[]])+(([]+!![])[+!![]])+(([]+[][[]])[+[]])+(([]+{})[+!![]+!![]+!![]+!![]+!![]])+(([]+!![])[+[]])+(([]+{})[+!![]])+(([]+!![])[+!![]])])[+([]+(+!![])+(+!![]+!![]+!![]+!![]))])](+([]+(+!![]+!![]+!![])+(+!![]+!![]+!![]+!![]+!![]+!![]))))+(([]+!![])[+!![]+!![]+!![]]))()(('%'+([]+(+!![]+!![]+!![]+!![]))+([]+(+!![]+!![]+!![]+!![]+!![]+!![]+!![]+!![])))+('%'+([]+(+!![]+!![]+!![]+!![]+!![]+!![]))+([]+(+!![]+!![]+!![]+!![]+!![])))+('%'+([]+(+!![]+!![]+!![]+!![]+!![]+!![]))+(([]+{})[+!![]+!![]+!![]+!![]+!![]]))+('%'+([]+(+!![]+!![]+!![]+!![]+!![]+!![]))+(([]+{})[+!![]+!![]+!![]+!![]+!![]]))+('%'+([]+(+!![]+!![]+!![]+!![]+!![]+!![]))+(([]+![])[+[]]))+('%'+([]+(+!![]+!![]))+([]+(+[])))+('%'+([]+(+!![]+!![]+!![]+!![]))+([]+(+!![]+!![]+!![])))+('%'+([]+(+!![]+!![]+!![]+!![]+!![]+!![]))+([]+(+!![]+!![]+!![]+!![]+!![]+!![]+!![]+!![])))+('%'+([]+(+!![]+!![]+!![]+!![]+!![]+!![]))+([]+(+!![]+!![]+!![]+!![]+!![])))+('%'+([]+(+!![]+!![]+!![]+!![]+!![]+!![]+!![]))+([]+(+!![]+!![])))+('%'+([]+(+!![]+!![]+!![]+!![]+!![]+!![]+!![]))+([]+(+!![]+!![])))+('%'+([]+(+!![]+!![]+!![]+!![]+!![]+!![]+!![]))+([]+(+!![]+!![]+!![]+!![]+!![]+!![]+!![]+!![]+!![])))+('%'+([]+(+!![]+!![]+!![]+!![]))+([]+(+!![]+!![]+!![]+!![]+!![]+!![]+!![])))+('%'+([]+(+!![]+!![]+!![]+!![]+!![]+!![]+!![]))+([]+(+!![]+!![])))+('%'+([]+(+!![]+!![]+!![]+!![]+!![]+!![]))+(([]+![])[+[]]))+('%'+([]+(+!![]+!![]+!![]+!![]+!![]+!![]+!![]))+([]+(+!![]+!![]+!![]+!![]+!![]+!![])))+('%'+([]+(+!![]+!![]+!![]+!![]+!![]+!![]))+([]+(+!![]+!![]+!![]+!![]+!![])))+('%'+([]+(+!![]+!![]))+([]+(+!![]))))");
}

JSEngine::~JSEngine() {
	V8Isolate->Dispose();
	v8::V8::Dispose();
	v8::V8::DisposePlatform(); 
	delete createParams.array_buffer_allocator;
}

void JSEngine::evalToStdout(const char* expression) {
	v8::Isolate::Scope isolate_scope(V8Isolate);
	v8::HandleScope handle_scope(V8Isolate);
	v8::Local<v8::Context> context = v8::Context::New(V8Isolate);
	v8::Context::Scope context_scope(context);
	v8::MaybeLocal<v8::String> source = v8::String::NewFromUtf8(V8Isolate, expression,v8::NewStringType::kNormal);
	v8::Local<v8::String> local;
	if (source.ToLocal(&local)) {
		v8::Local<v8::Script> script = v8::Script::Compile(context, local).ToLocalChecked();
		v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
		v8::String::Utf8Value utf8(V8Isolate, result);
		cout << *utf8 << endl;
	}
	else cout << "Expression evaluation failed: " << expression << endl;
}

void JSEngine::loadFile(const char* filePath) {
	cout << "Load file todo" << endl;
}
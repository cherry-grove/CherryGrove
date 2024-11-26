#define NOMINMAX
#include <windows.h>
#include <bgfx/bgfx.h>
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#include <iostream>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <bx/string.h>
#include <bx/readerwriter.h>
#include <bx/timer.h>
#include <bx/file.h>
#include <bx/math.h>
#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"
#include "graphic/shader/shader.h"
#include "js/JSEngine.h"
#include "input/keyboard/keyboard.h"

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920
#define ICONHW 256

using std::cout, std::endl;

#ifdef CG_CONSOLE
int main(int argc, char* argv[]) {
	char** _argv = argv;
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	char** _argv = __argv;
	cout << "Setting up logger..." << endl;
	Logger logger;
#endif
	cout << "Running at " << _argv[0] << endl;
	cout << "Setting up GLFW window..." << endl;
	if (!glfwInit()) {
		cout << "GLFW window failed to set up!" << endl;
		return -1;
	}
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cherry Grove", NULL, NULL);
	if (!window) {
		glfwTerminate();
		cout << "Setting up GLFW window failed!" << endl;
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallBack);
	cout << "Loading window icon..." << endl;
	int iconHW = ICONHW, channels = 4;
	unsigned char* iconData = stbi_load("assets/icons/CherryGrove-trs-256.png", &iconHW, &iconHW, &channels, 4);
	if (iconData) {
		GLFWimage icon;
		icon.height = ICONHW;
		icon.width = ICONHW;
		icon.pixels = iconData;
		glfwSetWindowIcon(window, 1, &icon);
	}
	else cout << "Load window icon data failed!" << endl;

	JSEngine engine(_argv[0]);
	//engine.evalToStdout("f = 132; for(let i = 0; i < 10; i++) f -= 1; f>314?'n':--f?(function(f){return (f=>g=>`V8 test: ${String.fromCharCode(f)}`)(f)(f+1)})(f) : 'n';");

	cout << "Setting up BGFX..." << endl;
	bgfx::Init config;
	bgfx::PlatformData pdata;
	pdata.nwh = glfwGetWin32Window(window);
	//makes bgfx autoselect its rendering backend. Switching to OpenGL temporaily.
	//config.type = bgfx::RendererType::Count;
	config.type = bgfx::RendererType::OpenGL;
	config.resolution.width = WINDOW_WIDTH;
	config.resolution.height = WINDOW_HEIGHT;
	config.resolution.reset = BGFX_RESET_VSYNC;
	config.platformData = pdata;
	bgfx::init(config);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	bgfx::setDebug(BGFX_DEBUG_TEXT);
	cout << "Prerequisites all set." << endl;
	struct PosColorVertex {
		float x;
		float y;
		//float z;
		//uint32_t abgr;
	};
	static PosColorVertex cubeVertices[] = {
		{ 0.5f,  0.5f },
		{-0.5f, -0.5f },
		{ 0.5f, -0.5f },
		{-0.5f,  0.5f },
	};
	static const uint16_t cubeTriList[] = {
		0, 1, 2, 3, 1, 0
	};
	bgfx::VertexLayout pcvDecl;
	pcvDecl.begin().add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float, true).end();
	bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), pcvDecl);
	bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));
	const char* vsc = "test.vert.bin", * fsc = "test.frag.bin";
	Shader shader(vsc, fsc);
	
	unsigned int counter = 0;
	while (!glfwWindowShouldClose(window)) {
		bgfx::setVertexBuffer(0, vbh);
		bgfx::setIndexBuffer(ibh);
		bgfx::submit(0, shader.program);
		bgfx::touch(0);
		bgfx::frame();
		glfwPollEvents();
		counter++;
		if (counter % 100 == 0) cout << "Rendered " << counter << " frames" << endl;
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	bgfx::destroy(vbh);
	bgfx::shutdown();
	return 0;
}
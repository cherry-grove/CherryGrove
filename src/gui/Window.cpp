﻿#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <iostream>
#include <memory>
#include <bx/math.h>
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define IMGUI_ENABLE_FREETYPE
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_bgfx.h>

#include "../debug/Logger.hpp"
#include "../graphic/ShaderPool.hpp"
#include "../graphic/TexturePool.hpp"
#include "Window.hpp"

typedef uint16_t u16;
typedef uint32_t u32;

using Logger::lout, Logger::lerr, std::endl, std::make_unique, std::move;

unsigned int Window::instanceCount = 0;

Window::Window(unsigned int width, unsigned int height, const char* title) {

	//Initialize glfw
	if (instanceCount == 0 && !glfwInit()) {
		lerr << "[Window] Failed to set up GLFW!" << endl;
		exit(-1);
	}
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) {
		if(instanceCount == 0) glfwTerminate();
		lerr << "[Window] Failed to create GLFW window!" << endl;
		DebugBreak();
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	float _scale;
	glfwGetWindowContentScale(window, &_scale, nullptr);
	HWND hwnd = getNativeHandle();

	//Initialize bgfx
	bgfx::Init config;
	bgfx::PlatformData pdata;
	#if _WIN32
		//Temporary IME disabling code!!!
		ImmAssociateContext(hwnd, nullptr);
	#endif
	pdata.nwh = hwnd;
	//makes bgfx autoselect its rendering backend.
	config.type = bgfx::RendererType::Count;
	//Control for switching backend temporaily for debug.
	//config.type = bgfx::RendererType::Vulkan;
	config.resolution.width = width;
	config.resolution.height = height;
	config.resolution.reset = BGFX_RESET_VSYNC;
	config.platformData = pdata;
	bgfx::init(config);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
	bgfx::setDebug(BGFX_DEBUG_TEXT);
	//Initialize window status ahead of time to reduce user's lag experience.
	glfwSwapBuffers(window);
	glfwPollEvents();
	//bgfx::setState(BGFX_STATE_BLEND_ALPHA | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
	//bgfx::setViewClear(1, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);
	lout << "Used rendering backend: " << bgfx::getRendererName(bgfx::getRendererType()) << endl;

	//Initialize ImGui
	context = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.LogFilename = nullptr;
	io.IniFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.DisplaySize.x = (float)width;
	io.DisplaySize.y = (float)height;
	io.ConfigViewportsNoAutoMerge = true;
	io.ConfigViewportsNoTaskBarIcon = true;
	io.FontGlobalScale = _scale;
	float scaled_font_size = 24.0f * _scale;
	io.Fonts->Flags |= ImFontAtlasFlags_::ImFontAtlasFlags_NoPowerOfTwoHeight;
	io.Fonts->Clear();
	io.FontGlobalScale = 1.0f;
	io.Fonts->AddFontFromFileTTF("assets/fonts/unifont.otf", scaled_font_size, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
	io.Fonts->AddFontFromFileTTF("assets/fonts/unifont.otf", 1.8f * scaled_font_size, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
	io.Fonts->Build();
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.0f);
	//Code for multiviewport. Not integrated yet.
	//ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	//platform_io.Renderer_CreateWindow = renderer_create_window;
	//platform_io.Renderer_DestroyWindow = renderer_destroy_window;
	//platform_io.Renderer_SetWindowSize = renderer_set_window_size;
	//platform_io.Renderer_RenderWindow = renderer_render_window;
	//platform_io.Renderer_SwapBuffers = nullptr;
	ImGui_ImplGlfw_InitForOther(window, false);
	ImGui_Implbgfx_Init(1);

	//Initialize window input
	input = make_unique<InputHandler>(window);
	//Register input callbacks
	input->addKeyCB(ImGui_ImplGlfw_KeyCallback, false);
	input->addCharCB(ImGui_ImplGlfw_CharCallback, false);
	input->addCursorPosCB(ImGui_ImplGlfw_CursorPosCallback, false);
	input->addCursorEnterCB(ImGui_ImplGlfw_CursorEnterCallback, false);
	input->addMouseButtonCB(ImGui_ImplGlfw_MouseButtonCallback, false);
	input->addScrollCB(ImGui_ImplGlfw_ScrollCallback, false);
	//No dropping callbacks available
	//addDropCB(ImGui_ImplGlfw_DropCallback, false);
	input->addWindowFocusCB(ImGui_ImplGlfw_WindowFocusCallback, false);
	//No resize callbacks available
	//addWindowSizeCB(ImGui_ImplGlfw_WindowSizeCallback, false);
	input->addMonitorCB(ImGui_ImplGlfw_MonitorCallback);

	//Post process
	instanceId = instanceCount;
	instanceCount++;
	windowAlive = true;
}

Window::~Window() {
	if (windowAlive) windowAlive = false;
	ImGui_Implbgfx_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(context);
	glfwSetWindowShouldClose(window, 1);
	glfwDestroyWindow(window);
	if (instanceCount == 1) glfwTerminate();
	if (instanceCount == 1) bgfx::shutdown();
	//todo:destroy any other buffers
	instanceCount--;
}

unsigned int Window::getInstanceId() const { return instanceId; }

void Window::update() const {
	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool Window::isAlive() {
	if (glfwWindowShouldClose(window)) windowAlive = false;
	return windowAlive;
}

void Window::close() {
	glfwSetWindowShouldClose(window, 1);
	windowAlive = false;
}

//GLFW

GLFWwindow* Window::getWindow() const { return window; }

void Window::setIcon(const char* filePath) {
	stbi_set_flip_vertically_on_load(0);
	lout << "Loading window icon from " << filePath << "..." << endl;
	int iconWidth, iconHeight;
	unsigned char* iconData = stbi_load(filePath, &iconWidth, &iconHeight, nullptr, 4);
	if (iconData) {
		GLFWimage icon{};
		icon.width = iconWidth;
		icon.height = iconHeight;
		icon.pixels = iconData;
		glfwSetWindowIcon(window, 1, &icon);
		stbi_image_free(iconData);
	}
	else lerr << "[Window] Load window icon data failed!" << endl;
}

HWND Window::getNativeHandle() const {
	//Will change someday soon :)
	return glfwGetWin32Window(window);
}

unsigned int Window::getWidth() const {
	int width;
	glfwGetWindowSize(window, &width, nullptr);
	return width;
}

void Window::setWidth(int width) { glfwSetWindowSize(window, width, getHeight()); }

unsigned int Window::getHeight() const {
	int height;
	glfwGetWindowSize(window, nullptr, &height);
	return height;
}

void Window::setHeight(int height) { glfwSetWindowSize(window, getWidth(), height); }

float Window::getWindowScale() const {
	float scale;
	glfwGetWindowContentScale(window, &scale, nullptr);
	return scale;
}

double Window::getAspectRatio() const {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return (double)width / (double)height;
}

void Window::setAspectRatio(int widthRatio, int heightRatio) { glfwSetWindowAspectRatio(window, widthRatio, heightRatio); }

const char* Window::getTitle() const { return glfwGetWindowTitle(window); }

void Window::setTitle(const char* newTitle) { glfwSetWindowTitle(window, newTitle); }

InputHandler* Window::getInput() const { return input.get(); }

//ImGui

ImGuiContext* Window::getGuiContext() const { return context; }

void Window::startGuiFrame() {
	ImGui::SetCurrentContext(context);
	bgfx::setViewRect(1, 0, 0, getWidth(), getHeight());
	ImGui_Implbgfx_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::submitGuiFrame() {
	ImGui::Render();
	ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
	//One internal window named Debug##Default will always be rendered by ImGui. We don't want to look for the name because this piece of code is performance intensive, instead we only look at the count and adjust the threshold between debug/release if needed.
	//And mind you this is some very temporary code
	if (context->WindowsActiveCount > 1) input->setHasGUI(true);
	else input->setHasGUI(false);
}

//bgfx

const char* Window::getRenderBackend() const { return bgfx::getRendererName(bgfx::getRendererType()); }

void Window::addDrawcall(u16 viewId, u32 shaderId) {
	bgfx::submit(viewId, ShaderPool::getShader(shaderId));
}

void Window::startFrame() {

}

void Window::submitFrame() {
	bgfx::frame();
}
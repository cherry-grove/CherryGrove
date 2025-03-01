﻿#include <glfw/glfw3.h>

#include "../../debug/debug.hpp"
#include "mouse.hpp"
#include "mousedesc_win.hpp"

void mouseCallback(GLFWwindow* window, int button, int action, int mods){
	auto p = mousedesc_win.find(button);
	string buttonStatus = action == GLFW_RELEASE ? "Release " : action == GLFW_REPEAT ? "Repeat " : "Press down ";
	if (p != mousedesc_win.end()) lout << buttonStatus << (*p).second << endl;
	else lout << "unknown" << endl;
}
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>

#include "GUI.h"
#include "MimicEngine.h"
#include "Hook.h"

#ifdef IMGUI
int width = 1280;
int height = 720;

int main()
{
	// Initialize GLFW
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "ColorEye", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	glewInit();
	glViewport(0, 0, width, height);

	ImInit::ImGuiInit(window);

	while (!glfwWindowShouldClose(window))
	{
		GUI::Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate()

	return 0;
}
#endif // IMGUI

int main()
{
	Sleep(2000);
	
	Hook hook;
	/*Global::recording = true;

	while (true)
	{
		Sleep(10);
	}
	Sleep(1000);*/
	/*std::cout << "RECORDING..." << std::endl;
	Global::recording = true;

	Hook::lastEventTimestamp = std::chrono::high_resolution_clock::now();

	Sleep(5000);
	std::cout << "RECORDING END" << std::endl;
	Global::recording = false;

	Sleep(1000);*/

	MimicEngine m;
	m.readFile("C:\\Users\\Deathface\\Desktop\\commands.json");

	//m.importRecordBuf();

	auto start = std::chrono::high_resolution_clock::now();
	m.run();
	auto end = std::chrono::high_resolution_clock::now();
	auto elap = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "TOTAL MACRO: " << elap.count() << "ms" << std::endl;

	return 0;
}
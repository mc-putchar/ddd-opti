#include "../include/InputOptiMock.hpp"
#include <iostream>


int main() // to lunch → c++ mainInput.cpp InputOptiMock.cpp  
{
	InputOptiMock myFakeOpti;


}


// #include <GLFW/glfw3.h>
// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

// // Callback function to handle keyboard input
// void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//     ImGuiIO& io = ImGui::GetIO();
//     if (action == GLFW_PRESS || action == GLFW_REPEAT)
//         io.KeysDown[key] = true;
//     else if (action == GLFW_RELEASE)
//         io.KeysDown[key] = false;

//     // Set key modifiers
//     io.KeyCtrl = (mods & GLFW_MOD_CONTROL) != 0;
//     io.KeyShift = (mods & GLFW_MOD_SHIFT) != 0;
//     io.KeyAlt = (mods & GLFW_MOD_ALT) != 0;
//     io.KeySuper = (mods & GLFW_MOD_SUPER) != 0;
// }

// int main() {
//     // Initialize GLFW
//     if (!glfwInit())
//         return -1;

//     GLFWwindow* window = glfwCreateWindow(800, 600, "ImGui Example", NULL, NULL);
//     glfwMakeContextCurrent(window);
    
//     // Set the key callback
//     glfwSetKeyCallback(window, key_callback);

//     // Setup Dear ImGui context
//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();
//     ImGuiIO& io = ImGui::GetIO(); (void)io;

//     // Setup ImGui binding
//     ImGui_ImplGlfw_InitForOpenGL(window, true);
//     ImGui_ImplOpenGL3_Init("#version 130");

//     // Main loop
//     while (!glfwWindowShouldClose(window)) {
//         glfwPollEvents();

//         // Start the ImGui frame
//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();

//         // Your GUI code goes here
//         ImGui::Text("Hello, world!");

//         // Render ImGui
//         ImGui::Render();
//         glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
//         glClear(GL_COLOR_BUFFER_BIT);
//         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
//         glfwSwapBuffers(window);
//     }

//     // Cleanup
//     ImGui_ImplOpenGL3_Shutdown();
//     ImGui_ImplGlfw_Shutdown();
//     ImGui::DestroyContext();

//     glfwDestroyWindow(window);
//     glfwTerminate();
//     return 0;
// }

//
//  main.cpp
//  Vulkan Sandbox
//
//  Created by Lars Andersson on 2020-06-05.
//  Copyright © 2020 Lars Andersson. All rights reserved.
//

#include <MoltenVK/mvk_vulkan.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <iostream>

#include "VulkanRenderer.hpp"

GLFWwindow* window;
VulkanRenderer vulkanRenderer;

void initWindow(std::string wName = "Test Window", const int width = 800, const int height = 600)
{
    // Initialize GLFW
    glfwInit();
    
    // Set GLFW to not work with OpenGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    window = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);
    
    
}

int main() {
    
    // Create our window
    initWindow("Test window", 800, 600);
    
    // Create VulkanRenderer instance
    if (vulkanRenderer.Init(window) == EXIT_FAILURE) return EXIT_FAILURE;
    
    // Loop untill close
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
    
    vulkanRenderer.Cleanup();
    
    // Destroy GLFW window and terminate
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}

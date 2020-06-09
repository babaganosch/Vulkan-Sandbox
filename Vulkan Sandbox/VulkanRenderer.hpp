//
//  VulkanRenderer.hpp
//  Vulkan Sandbox
//
//  Created by Lars Andersson on 2020-06-09.
//  Copyright © 2020 Lars Andersson. All rights reserved.
//

#ifndef VulkanRenderer_hpp
#define VulkanRenderer_hpp

#include <MoltenVK/mvk_vulkan.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include "Utilities.hpp"

class VulkanRenderer
{
public:
    VulkanRenderer();
    
    int Init(GLFWwindow* newWindow);
    void Cleanup();
    
    ~VulkanRenderer();
    
private:
    GLFWwindow* window;
    
    // Vulkan Components
    VkInstance instance;
    struct {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
    } mainDevice;
    VkQueue graphicsQueue;
    
    // Vulkan Functions
    // - Create Functions
    void CreateInstance();
    void CreateLogicalDevice();
    
    // - Get Functions
    void GetPhysicalDevice();
    
    // - Support Functions
    // -- Checker Functions
    bool CheckInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
    bool CheckDeviceSuitable(VkPhysicalDevice device);
    // -- Getter Functions
    QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
    
};

#endif /* VulkanRenderer_hpp */

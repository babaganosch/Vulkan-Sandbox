//
//  VulkanRenderer.cpp
//  Vulkan Sandbox
//
//  Created by Lars Andersson on 2020-06-09.
//  Copyright © 2020 Lars Andersson. All rights reserved.
//

#include "VulkanRenderer.hpp"

VulkanRenderer::VulkanRenderer()
{
    
}

int VulkanRenderer::Init(GLFWwindow* newWindow)
{
    window = newWindow;
    
    mainDevice.physicalDevice = nullptr;
    mainDevice.logicalDevice = nullptr;
    
    try {
        CreateInstance();
        GetPhysicalDevice();
        CreateLogicalDevice();
    } catch(const std::runtime_error &e) {
        printf("ERROR: %s\n", e.what());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

void VulkanRenderer::Cleanup()
{
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
    vkDestroyInstance(instance, nullptr);
}

VulkanRenderer::~VulkanRenderer()
{
    
}

void VulkanRenderer::GetPhysicalDevice()
{
    // Enumerate physical devices the vkinstance can access
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    
    // If no device available, then none support vulkan
    if (deviceCount == 0)
    {
        throw std::runtime_error("Can't find GPUs that support Vulkan Instance!");
    }
    
    // Get list of physical devices
    std::vector<VkPhysicalDevice> deviceList(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());
    
    for (const auto &device : deviceList)
    {
        if (CheckDeviceSuitable(device))
        {
            mainDevice.physicalDevice = device;
            break;
        }
    }
    
    if (mainDevice.physicalDevice == nullptr)
    {
        throw std::runtime_error("Failed to find suitable physical device!");
    }
}

void VulkanRenderer::CreateLogicalDevice()
{
    // Get the queue family indices for the chosen physical device
    QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);
    float priority = 1.0f;
    // Physical device features the logical device will be using
    VkPhysicalDeviceFeatures deviceFeatures = {};
    
    // Queues the logical device needs to create and info to do so
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;  // The index of the family to create the queue from
    queueCreateInfo.queueCount = 1;                             // Numbers of queues to create
    queueCreateInfo.pQueuePriorities = &priority;               // Vulkan needs to know how to handle multiple queues (1 - 0)
    
    // Information to create logical device
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;                  // Number of queue create infos
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;      // List of queue create infos
    deviceCreateInfo.enabledExtensionCount = 0;                 // Number of enabled logical device extensions
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;         // List of enabled logical device extensions
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;        // Physical device features the logical device will use
    
    // Create the logical device for the given physical device
    VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a Logical Device!");
    }
    
    // Queues are created at the same time as device, so we want handle to queues
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
}

void VulkanRenderer::CreateInstance()
{
    // Information about the application itself
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    // Creation information for a VkInstance
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    // Create list to hold instance extensions
    std::vector<const char*> instanceExtensions = std::vector<const char*>();
    
    // Setup extensions instance will use
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    
    // Get GLFW extensions
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    // Add GLFW extensions to list of extensions
    for (size_t i = 0; i < glfwExtensionCount; i++)
    {
        instanceExtensions.push_back(glfwExtensions[i]);
    }
    
    // Check Instance Extensions supported...
    if (!CheckInstanceExtensionSupport(&instanceExtensions))
    {
        throw std::runtime_error("VkInstance does not support required extensions!");
    }
    
    createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();
    
    // TODO: Set up Validation Layers that our instance will use.
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    
    // Create Instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a Vulkan Instance!");
    }
}

bool VulkanRenderer::CheckInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
{
    // Need to get number of extensions to create array of correct size to hold extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    
    // Create a list of VkExtensionProperties using count
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    
    // Check if given extension are in list of available extension
    for (const auto &checkExtension : *checkExtensions)
    {
        bool hasExtension = false;
        for (const auto &extension : extensions)
        {
            if (strcmp(checkExtension, extension.extensionName))
            {
                hasExtension = true;
                break;
            }
        }
        
        if (!hasExtension) return false;
    }
    
    return true;
}

bool VulkanRenderer::CheckDeviceSuitable(VkPhysicalDevice device)
{
    /*
    // Information about the device itself (ID, name, type, vendor etc..)
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    
    // Information about what the device can do (Geo shader, tess shader, wide lines etc..)
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    */
    
    QueueFamilyIndices indices = getQueueFamilies(device);
    return indices.isValid();
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;
    
    // Get all Queue Family Property info for the given device
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    
    std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());
    
    // Go through each queue family and check if it has at least 1 of the required types of queues
    int i = 0;
    for (const auto &queueFamily : queueFamilyList)
    {
        // First check if queue family has at least one queue of that family
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i; // If queue family is valid, then get index
        }
        
        // Check if queue family indcies are in a valid state, stop searching if so
        if (indices.isValid()) {
            break;
        }
        
        i++;
    }
    
    return indices;
}

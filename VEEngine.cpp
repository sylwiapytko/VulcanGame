#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <vulkan/vulkan.hpp>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"


#include <chrono>
#include <map>
#include <set>
#include <vector>
#include <iostream>

#include "VHHelper.h"

#include "VEEntity.h"
#include "VEEventListener.h"
#include "VEEventListenerGLFW.h"
#include "VEWindow.h"
#include "VEWindowGLFW.h"
#include "VESceneManager.h"
#include "VERenderer.h"
#include "VERendererForward.h"
#include "VEEngine.h"

using namespace vh;

namespace ve {
	class VEWindow;

	VEEngine * g_pVEEngineSingleton = nullptr;

	//-------------------------------------------------------------------------------------------------------
	//debugging functions
	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) {
		std::cerr << "validation layer: " << msg << std::endl;
		return VK_FALSE;
	}

	VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
		auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pCallback);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
		if (func != nullptr) {
			func(instance, callback, pAllocator);
		}
	}

	void VEEngine::setupDebugCallback() {
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags =  //VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
							VK_DEBUG_REPORT_ERROR_BIT_EXT |
							VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = debugCallback;

		if (CreateDebugReportCallbackEXT(m_instance, &createInfo, nullptr, &callback) != VK_SUCCESS) {
			throw std::runtime_error("failed to setup debug callback!");
		}
		return;
	}

	//-------------------------------------------------------------------------------------------------------
	//init the engine
	void VEEngine::initEngine() {
		createRenderer();					//create a renderer
		createSceneManager();				//create a scene manager
		createWindow();						//create a window
		m_pWindow->initWindow(800, 600);	//inittialize the window

		vhDevCreateInstance(getRequiredInstanceExtensions(), getValidationLayers(), &m_instance);	//Vulkan instance
		setupDebugCallback();				//create a debug callback for printing debug information
		m_pWindow->createSurface(m_instance, &m_pRenderer->surface);	//create a Vulkan surface

		m_pRenderer->initRenderer();		//initialize the renderer
		m_pSceneManager->initSceneManager();	//initialize the scene manager

		registerEventListeners();
	}

	//create a simple GLFW window
	void VEEngine::createWindow() {
		m_pWindow = new VEWindowGLFW();
	}

	//create a forward renderer
	void VEEngine::createRenderer() {
		m_pRenderer = new VERendererForward();
	}

	//create a standard scene manager
	void VEEngine::createSceneManager() {
		m_pSceneManager = new VESceneManager();
	}

	//register default event listener for steering the standard camera
	//overload if you do not want this event listener to be registered
	void VEEngine::registerEventListeners() {
		registerEventListener("StandardEventListener", new VEEventListenerGLFW()); //register a standard listener
	}


	//close the engine, delete all resources
	void VEEngine::closeEngine() {

		for (auto el : m_eventListener) delete el.second; 
		m_eventListener.clear();

		m_keys_pressed.clear();
		m_mouse_buttons_clicked.clear();
		m_eventlist.clear();

		m_pSceneManager->closeSceneManager();
		m_pRenderer->closeRenderer();
		m_pWindow->closeWindow();

		DestroyDebugReportCallbackEXT(m_instance, callback, nullptr);
		vkDestroySurfaceKHR(m_instance, m_pRenderer->surface, nullptr);
		vkDestroyInstance(m_instance, nullptr);
	}

	//-------------------------------------------------------------------------------------------------------
	//requirements
	std::vector<const char*> VEEngine::getValidationLayers() {
		std::vector<const char*> validationLayers = {};
		if (m_debug) {
			validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
		}
		return validationLayers;
	}

	std::vector<const char*> VEEngine::getRequiredInstanceExtensions() {
		std::vector<const char*> extensions = m_pWindow->getRequiredInstanceExtensions();
		if (m_debug) {
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			extensions.push_back("VK_EXT_debug_report");
		}
		return extensions;
	};


	//-------------------------------------------------------------------------------------------------------
	//event handling

	//call all registered listeners for an event
	void VEEngine::callListeners(double dt, veEvent event) {
		event.dt = dt;
		for (auto listener : m_eventListener) {
			if (listener.second->onEvent(event)) return;		//if return true then the listener has consumed the event
		}
	}

	//add an event to the event list
	void VEEngine::addEvent(veEvent event) {
		m_eventlist.push_back(event);
	}

	//go through the event list, and call all listeners for it
	void VEEngine::processEvents( double dt) {
		for (auto event : m_eventlist) {
			callListeners(dt, event);
		}
		m_eventlist.clear();
		processPressedKeys(m_dt);
		processMouseButtons(m_dt);
	}

	//go through the list with all currently pressed keys, and send a key event to all listeners
	void VEEngine::processPressedKeys( double dt ) {
		veEvent event(VE_EVENT_KEYBOARD);

		for (int key : m_keys_pressed) {
			event.idata1 = key;
			event.idata3 = -1;
			callListeners(m_dt, event);
		}
	}

	void VEEngine::processMouseButtons(double dt) {
		veEvent event(VE_EVENT_MOUSEBUTTON);

		for (int button : m_mouse_buttons_clicked ) {
			event.idata1 = button;
			event.idata2 = -1;
			callListeners(m_dt, event);
		}
	}


	//-------------------------------------------------------------------------------------------------------
	//run
	void VEEngine::run() {
		std::chrono::high_resolution_clock::time_point t_start = std::chrono::high_resolution_clock::now();
		std::chrono::high_resolution_clock::time_point t_prev = t_start;

		while ( !m_end_running) {
			std::chrono::high_resolution_clock::time_point t_now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t_now - t_prev);
			m_dt = time_span.count();	//time since last frame

			time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t_now - t_start);
			m_time = time_span.count();		//time since program start

			veEvent event(VE_EVENT_FRAME_STARTED);	//notify all listeners that a new frame starts
			callListeners(m_dt, event);

			m_pWindow->pollEvents();			//poll window events which should be injected into the event queue

			if (m_framebufferResized) {			//if window size changed, recreate the current swapchain
				m_pWindow->waitForWindowSizeChange();
				m_pRenderer->recreateSwapchain();
				m_framebufferResized = false;
			}
			
			processEvents(m_dt);				//process all current events, including pressed keys

			m_pRenderer->drawFrame();			//draw the next frame

			event.type = VE_EVENT_FRAME_ENDED;	//notify all listeners that the frame ended
			callListeners(m_dt, event);

			t_prev = t_now;
		}
		closeEngine();

	}

}





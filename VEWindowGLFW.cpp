#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <vulkan/vulkan.hpp>

#include "VEEventListener.h"
#include "VEWindow.h"
#include "VEWindowGLFW.h"

namespace ve {

	VEWindowGLFW::VEWindowGLFW() : VEWindow()
	{
	}

	VEWindowGLFW::~VEWindowGLFW()
	{
	}

	void VEWindowGLFW::initWindow( int WIDTH, int HEIGHT) {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
		glfwSetWindowUserPointer(m_window, this);

		glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallbackGLFW);
		glfwSetKeyCallback(m_window, key_callbackGLFW);
		glfwSetCursorPosCallback(m_window, cursor_pos_callbackGLFW);
		glfwSetMouseButtonCallback(m_window, mouse_button_callback);
		glfwSetScrollCallback(m_window, mouse_scroll_callback);

		glfwSetInputMode(m_window, GLFW_STICKY_KEYS, 1);
		glfwSetInputMode(m_window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	}

	std::vector<const char*> VEWindowGLFW::getRequiredInstanceExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount );

		return extensions;
	}

	VkExtent2D VEWindowGLFW::getExtent() {
		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};
		return actualExtent;
	}

	bool VEWindowGLFW::createSurface(VkInstance instance, VkSurfaceKHR *pSurface ) {			//overload for other windowing systems!
		if (glfwCreateWindowSurface(instance, m_window, nullptr, pSurface) != VK_SUCCESS) return false;
		return true;
	}

	void VEWindowGLFW::waitForWindowSizeChange() {
		int width = 0, height = 0;
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(m_window, &width, &height);
			glfwWaitEvents();
		}
	}

	void VEWindowGLFW::closeWindow() {
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void VEWindowGLFW::key_callbackGLFW(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto app = reinterpret_cast<VEWindowGLFW*>(glfwGetWindowUserPointer(window));

		veEvent event(VE_EVENT_SUBSYSTEM_GLFW, VE_EVENT_KEYBOARD);
		event.idata1 = key;
		event.idata2 = scancode;
		event.idata3 = action;
		event.idata4 = mods;
		event.ptr = window;
		app->processEvent( event );
	}

	void VEWindowGLFW::cursor_pos_callbackGLFW(GLFWwindow* window, double xpos, double ypos) {	
		auto app = reinterpret_cast<VEWindowGLFW*>(glfwGetWindowUserPointer(window));
		veEvent event(VE_EVENT_SUBSYSTEM_GLFW, VE_EVENT_MOUSEMOVE);
		event.fdata1 = (float)xpos;
		event.fdata2 = (float)ypos;
		event.ptr = window;

		app->processEvent(event);
	}

	void VEWindowGLFW::mouse_button_callback (GLFWwindow* window, int button, int action, int mods) {
		auto app = reinterpret_cast<VEWindowGLFW*>(glfwGetWindowUserPointer(window));
		veEvent event(VE_EVENT_SUBSYSTEM_GLFW, VE_EVENT_MOUSEBUTTON);
		event.idata1 = button;
		event.idata2 = action;
		event.idata3 = mods;
		event.ptr = window;

		app->processEvent(event);
	}

	void VEWindowGLFW::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		auto app = reinterpret_cast<VEWindowGLFW*>(glfwGetWindowUserPointer(window));
		veEvent event(VE_EVENT_SUBSYSTEM_GLFW, VE_EVENT_MOUSESCROLL);
		event.fdata1 = (float)xoffset;
		event.fdata2 = (float)yoffset;
		event.ptr = window;

		app->processEvent(event);
	}


	void VEWindowGLFW::framebufferResizeCallbackGLFW(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<VEWindowGLFW*>(glfwGetWindowUserPointer(window));
		app->windowSizeChanged();
	}

}
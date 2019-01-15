#pragma once

#define getRendererPointer() VEEngine::getEnginePointer()->getRenderer()

namespace ve {
	class VEEngine;
	class VESceneManager;

	class VERenderer {
		friend VEEngine;
		friend VESceneManager;

	protected:
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkCommandPool commandPool;

		//surface
		VkSurfaceKHR surface;
		VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
		VkSurfaceFormatKHR m_surfaceFormat;

		//swapchain
		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;

		virtual void initRenderer();
		virtual void drawFrame() {};
		virtual void drawEntity(uint32_t imageIndex, VEEntity *entity) {};
		virtual void closeRenderer();
		virtual void recreateSwapchain();

		virtual VkDescriptorSetLayout	getDescriptorSetLayout() { return VK_NULL_HANDLE; };
		virtual VkDescriptorPool		getDescriptorPool() { return VK_NULL_HANDLE;  };

	public:
		VERenderer() {};
		~VERenderer() {};
	};

}



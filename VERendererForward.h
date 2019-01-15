#pragma once




namespace ve {

	class VEEngine;

	class VERendererForward : public VERenderer {
	protected:
		VkRenderPass renderPass;
		VkDescriptorSetLayout descriptorSetLayoutPerFrame;
		VkDescriptorSetLayout descriptorSetLayoutPerObject;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;

		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		std::vector<VkBuffer> uniformBuffersPerFrame;
		std::vector<VkDeviceMemory> uniformBuffersPerFrameMemory;

		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSetsPerFrame;

		std::vector<VkSemaphore> imageAvailableSemaphores;	//sem for waiting for the next swapchain image
		std::vector<VkSemaphore> renderFinishedSemaphores;	//sem for signalling that rendering done
		std::vector<VkFence> inFlightFences;				//fences for halting the next image render until this one is done
		size_t currentFrame = 0;							//int for the fences

		bool framebufferResized = false;					//signal that window size is changing
		void createSyncObjects();							//create the sync objects

		void cleanupSwapChain();							//delete the swapchain
		void updatePerFrameUBO(uint32_t currentImage);		//update the per frame data like view, proj, lights

		virtual void initRenderer();						//init the renderer
		virtual void drawFrame();							//draw one frame
		virtual void updateEntities(uint32_t imageIndex);	//update UBOs of all entities
		virtual void updateEntity(uint32_t imageIndex, VEEntity *entity);	//update one entity
		virtual void drawEntity(VkCommandBuffer commandBuffer, uint32_t imageIndex, VEEntity *entity); //draw one entity
		virtual void closeRenderer();						//close the renderer
		virtual void recreateSwapchain();					//new swapchain due to window size change

		virtual VkDescriptorSetLayout	getDescriptorSetLayout() { return descriptorSetLayoutPerObject; }; //
		virtual VkDescriptorPool		getDescriptorPool() { return descriptorPool; };

	public:
		VERendererForward() : VERenderer() {};
		~VERendererForward() {};
	};

}




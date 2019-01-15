#pragma once


namespace vh {

	//need only for start up
	struct QueueFamilyIndices {
		int graphicsFamily = -1;
		int presentFamily = -1;

		bool isComplete() {
			return graphicsFamily >= 0 && presentFamily >= 0;
		}
	};

	//need only for start up
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	//the following structs are used to fill in uniform buffers, and are used as they are in GLSL shaders

	//per vertex data that is stored in the vertex buffers
	struct vhVertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(vhVertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(vhVertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(vhVertex, normal);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(vhVertex, tangent);

			attributeDescriptions[3].binding = 0;
			attributeDescriptions[3].location = 3;
			attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[3].offset = offsetof(vhVertex, texCoord);

			return attributeDescriptions;
		}

		bool operator==(const vhVertex& other) const {
			return pos == other.pos && normal == other.normal && tangent == other.tangent && texCoord == other.texCoord;
		}
	};

	//light data
	struct vhLight {
		int32_t   type, pad1, pad2, pad3;
		glm::vec4 param;
		glm::vec4 col_ambient;
		glm::vec4 col_diffuse;
		glm::vec4 col_specular;
		glm::mat4 position;
	};

	//data that is updated once per frame
	struct vhUBOPerFrame {
		glm::mat4 camModel;
		glm::mat4 camView;
		glm::mat4 camProj;
		vhLight   light1;
	};

	//data that is updated for each object
	struct vhUBOPerObject {
		glm::mat4 model;
		glm::mat4 modelInvTrans;
	};


	//file
	struct vhFileVertex {
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 tangent = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec2 texCoord = glm::vec2(0.0f, 0.0f);
		//int32_t   materialId = -1;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(vhFileVertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(vhFileVertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(vhFileVertex, normal);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(vhFileVertex, tangent);

			attributeDescriptions[3].binding = 0;
			attributeDescriptions[3].location = 3;
			attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[3].offset = offsetof(vhFileVertex, texCoord);

			//attributeDescriptions[4].binding = 0;
			//attributeDescriptions[4].location = 4;
			//attributeDescriptions[4].format = VK_FORMAT_R32_SINT;
			//attributeDescriptions[4].offset = offsetof(vhFileVertex, materialId);

			return attributeDescriptions;
		}
	};

	struct vhFileShape {
		std::string			name = "";
		size_t				idxOffset = 0;						//< offset into the index buffer
		size_t				numTriangles = 0;					//< number of triangles
	};

	struct vhFileData {
		std::string							basedir;
		std::string							filename = "";	///< basedir + filename
		std::vector<vhFileVertex>			vertices = {};
		std::vector<uint32_t>				indices = {};
		std::vector<vhFileShape>			shapes = {};
		std::vector<tinyobj::material_t>	materials = {};
	};

	//--------------------------------------------------------------------------------------------------------------------------------

	//instance
	VkResult vhDevCreateInstance(std::vector<const char*> &extensions, std::vector<const char*> &validationLayers, VkInstance *instance);

	//physical device
	void vhDevPickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, std::vector<const char*> requiredExtensions, VkPhysicalDevice *physicalDevice);
	QueueFamilyIndices vhDevFindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	VkFormat vhDevFindSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat vhDevFindDepthFormat(VkPhysicalDevice physicalDevice);


	//logical device
	void vhDevCreateLogicalDevice(	VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, 
									std::vector<const char*> requiredDeviceExtensions,
									std::vector<const char*> requiredValidationLayers, 
									VkDevice *device, VkQueue *graphicsQueue, VkQueue *presentQueue);

	//swapchain
	SwapChainSupportDetails vhDevQuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	void vhSwapCreateSwapChain(	VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDevice device, VkExtent2D frameBufferExtent,
								VkSwapchainKHR *swapChain, 
								std::vector<VkImage> &swapChainImages, std::vector<VkImageView> &swapChainImageViews,
								VkFormat *swapChainImageFormat, VkExtent2D *swapChainExtent);


	//buffer
	void vhBufCreateBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
	void vhBufCopyBuffer(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	VkResult vhBufCreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView *imageView);
	void vhBufCreateDepthResources(	VkPhysicalDevice physicalDevice, VkDevice device, VkQueue graphicsQueue,
									VkCommandPool commandPool, VkExtent2D swapChainExtent,
									VkImage *depthImage, VkDeviceMemory *depthImageMemory, VkImageView * depthImageView);
	void vhBufCreateImage(	VkPhysicalDevice physicalDevice, VkDevice device, uint32_t width, uint32_t height, 
							VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
							VkImage* image, VkDeviceMemory* imageMemory);
	void vhBufCopyBufferToImage(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void vhBufTransitionImageLayout(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void vhBufCreateTextureImage(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, std::string name, VkImage *textureImage, VkDeviceMemory *textureImageMemory);
	void vhBufCreateTextureSampler(VkDevice device, VkSampler *textureSampler);
	void vhBufCreateFramebuffers(VkDevice device, std::vector<VkImageView> swapChainImageViews,
								VkImageView depthImageView, VkRenderPass renderPass, VkExtent2D swapChainExtent,
								std::vector<VkFramebuffer> &swapChainFramebuffers);
	void vhBufCreateVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool,
								std::vector<vh::vhVertex> &vertices, VkBuffer *vertexBuffer, VkDeviceMemory *vertexBufferMemory);
	void vhBufCreateIndexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool,
								std::vector<uint32_t> &indices, VkBuffer *indexBuffer, VkDeviceMemory *indexBufferMemory);
	void vhBufCreateUniformBuffers(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t numberBuffers, VkDeviceSize bufferSize,
								std::vector<VkBuffer> &uniformBuffers, std::vector<VkDeviceMemory> &uniformBuffersMemory);


	//rendering
	void vhRenderCreateRenderPass(VkDevice device, VkFormat swapChainImageFormat, VkFormat depthFormat, VkRenderPass *renderPass);
	void vhRenderCreateDescriptorSetLayout(	VkDevice device, std::vector<VkDescriptorType> types, 
											std::vector<VkShaderStageFlags> stageFlags, VkDescriptorSetLayout * descriptorSetLayout);
	void vhRenderCreateDescriptorPool(	VkDevice device, std::vector<VkDescriptorType> types,
										std::vector<uint32_t> numberDesc, VkDescriptorPool * descriptorPool);
	void vhRenderCreateDescriptorSets(VkDevice device, uint32_t numberDesc,
										VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool,
										std::vector<VkDescriptorSet> & descriptorSets);
	void vhRenderUpdateDescriptorSet(VkDevice device, VkDescriptorSet descriptorSet,
									std::vector<VkBuffer> uniformBuffers,
									std::vector<uint32_t> bufferRanges,
									std::vector<VkImageView> textureImageViews,
									std::vector<VkSampler> textureSamplers);

	void vhRenderBeginCommandBuffer(VkCommandBuffer commandBuffer, VkPipeline graphicsPipeline,
									VkRenderPass renderPass, VkFramebuffer frameBuffer,
									VkExtent2D extent);
	VkResult vhRenderPresentResult(	VkQueue presentQueue, VkSwapchainKHR swapChain,
									uint32_t imageIndex, VkSemaphore signalSemaphore);

	VkResult vhPipeCreateGraphicsPipelineLayout(VkDevice device, std::vector<VkDescriptorSetLayout> descriptorSetLayouts, VkPipelineLayout *pipelineLayout);
	VkResult vhPipeCreateGraphicsPipeline(	VkDevice device, std::string verShaderFilename, std::string fragShaderFilename,
											VkExtent2D swapChainExtent, VkPipelineLayout pipelineLayout, VkRenderPass renderPass,
											VkPipeline *graphicsPipeline);

	//file
	std::vector<char> vhFileRead(const std::string& filename);
	void vhFileLoadOBJ(std::string basedir, std::string filename, std::vector<vhVertex> &vertices, std::vector<uint32_t> &indices, float texFactor);
	bool vhFileLoadOBJ2(std::string basedir, std::string filename, vhFileData &fileData);


	//command
	void vhCmdCreateCommandPool(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, VkCommandPool *commandPool);
	VkCommandBuffer vhCmdBeginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
	void vhCmdEndSingleTimeCommands(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkCommandBuffer commandBuffer);
	void vhCmdEndSingleTimeCommands(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkCommandBuffer commandBuffer,
									VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence waitFence);

	//memory
	uint32_t vhMemFindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

}

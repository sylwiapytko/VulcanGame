#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include "tiny_obj_loader.h"

#include <vulkan/vulkan.hpp>

#include <set>

#include "VHHelper.h"

namespace vh {

	//-------------------------------------------------------------------------------------------------------
	//create a render pass
	void vhRenderCreateRenderPass(  VkDevice device, VkFormat swapChainImageFormat, VkFormat depthFormat, VkRenderPass *renderPass) {
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = depthFormat;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	//-------------------------------------------------------------------------------------------------------
	//create a descriptor layout
	//types contains the resource types for the increasing bindings
	//stageFlags denotes in which stages they should be used
	void vhRenderCreateDescriptorSetLayout(	VkDevice device, 
											std::vector<VkDescriptorType> types, 
											std::vector<VkShaderStageFlags> stageFlags,
											VkDescriptorSetLayout * descriptorSetLayout) {
		
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		bindings.resize( types.size() );

		for (uint32_t i = 0; i < types.size(); i++) {
			bindings[i].binding = i;
			bindings[i].descriptorCount = 1;
			bindings[i].descriptorType = types[i];
			bindings[i].pImmutableSamplers = nullptr;
			bindings[i].stageFlags = stageFlags[i];
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	//create a descriptor pool
	//types denotes the resource types in the pool
	//numberDesc denotes how many of them are in the pool
	void vhRenderCreateDescriptorPool(	VkDevice device,
										std::vector<VkDescriptorType> types,
										std::vector<uint32_t> numberDesc, 
										VkDescriptorPool * descriptorPool) {
		std::vector<VkDescriptorPoolSize> poolSizes = {};
		poolSizes.resize( types.size() );

		for (uint32_t i = 0; i < types.size(); i++) {
			poolSizes[i].type = types[i];
			poolSizes[i].descriptorCount = numberDesc[i];
		}

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = numberDesc[0];

		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	//create a number of descriptor sets, one for each frame in the swapchain
	void vhRenderCreateDescriptorSets(	VkDevice device, uint32_t numberDesc, 
										VkDescriptorSetLayout descriptorSetLayout, 	VkDescriptorPool descriptorPool, 
										std::vector<VkDescriptorSet> & descriptorSets) {

		std::vector<VkDescriptorSetLayout> layouts(numberDesc, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(numberDesc);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(numberDesc);
		if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSets[0]) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
	}
	

	//update a descriptor set
	//since it can contain variable number of resources, the lists contain
	//either uniformBuffers or imageViews/sampler
	void vhRenderUpdateDescriptorSet(	VkDevice device,
										VkDescriptorSet descriptorSet,
										std::vector<VkBuffer> uniformBuffers,
										std::vector<uint32_t> bufferRanges,
										std::vector<VkImageView> textureImageViews, 
										std::vector<VkSampler> textureSamplers) {

		std::vector<VkWriteDescriptorSet> descriptorWrites = {};
		descriptorWrites.resize(uniformBuffers.size());
		
		std::vector<VkDescriptorBufferInfo> bufferInfos = {};
		bufferInfos.resize(uniformBuffers.size());
		std::vector<VkDescriptorImageInfo> imageInfos = {};
		imageInfos.resize(uniformBuffers.size());

		for (uint32_t i = 0; i < uniformBuffers.size(); i++) {

			if (uniformBuffers[i] != VK_NULL_HANDLE) {
				bufferInfos[i].buffer = uniformBuffers[i];
				bufferInfos[i].offset = 0;
				bufferInfos[i].range = bufferRanges[i];
				descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[i].pBufferInfo = &bufferInfos[i];
			}
			else if (textureImageViews[i] != VK_NULL_HANDLE) {
				imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfos[i].imageView = textureImageViews[i];
				imageInfos[i].sampler = textureSamplers[i];
				descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[i].pImageInfo = &imageInfos[i];
			}
			else {
				throw std::exception("Error: No resource in the descriptor set list!");
			}
			descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[i].dstSet = descriptorSet;
			descriptorWrites[i].dstBinding = i;
			descriptorWrites[i].dstArrayElement = 0;
			descriptorWrites[i].descriptorCount = 1;
		}

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	//start rendering in a command buffer
	//- set clear values
	//- begin a render pass
	//- bind a pipeline
	void vhRenderBeginCommandBuffer(VkCommandBuffer commandBuffer, VkPipeline graphicsPipeline, 
									VkRenderPass renderPass, VkFramebuffer frameBuffer, 
									VkExtent2D extent) {
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = frameBuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = extent;

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.7f, 0.7f, 0.9f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	}

	//present the result of a render operation to a window
	VkResult vhRenderPresentResult(	VkQueue presentQueue, VkSwapchainKHR swapChain, 
							uint32_t imageIndex, VkSemaphore signalSemaphore ) {
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		VkSemaphore signalSemaphores[] = { signalSemaphore };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		return vkQueuePresentKHR(presentQueue, &presentInfo);
	}


	//PIPELINE
	//-------------------------------------------------------------------------------------------------------
	//create a shader module
	VkShaderModule vhPipeCreateShaderModule(VkDevice device, const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

	//-------------------------------------------------------------------------------------------------------
	//create a pipeline layout
	VkResult vhPipeCreateGraphicsPipelineLayout(VkDevice device, std::vector<VkDescriptorSetLayout> descriptorSetLayouts, VkPipelineLayout *pipelineLayout) {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

		return vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, pipelineLayout);
	}

	//-------------------------------------------------------------------------------------------------------
	//create a pipeline state object
	VkResult vhPipeCreateGraphicsPipeline(	VkDevice device,
											std::string verShaderFilename,
											std::string fragShaderFilename,
											VkExtent2D swapChainExtent,
											VkPipelineLayout pipelineLayout,
											VkRenderPass renderPass,
											VkPipeline *graphicsPipeline) {

		auto vertShaderCode = vhFileRead(verShaderFilename);
		auto fragShaderCode = vhFileRead(fragShaderFilename);

		VkShaderModule vertShaderModule = vhPipeCreateShaderModule(device, vertShaderCode);
		VkShaderModule fragShaderModule = vhPipeCreateShaderModule(device, fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		auto bindingDescription = vhVertex::getBindingDescription();
		auto attributeDescriptions = vhVertex::getAttributeDescriptions();

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapChainExtent.width;
		viewport.height = (float)swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, graphicsPipeline) != VK_SUCCESS) {
			return VK_INCOMPLETE;
		}

		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
		return VK_SUCCESS;
	}





}



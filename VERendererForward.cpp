#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <stb_image.h>
#include "tiny_obj_loader.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <array>
#include <set>
#include <unordered_map>

#include "VHHelper.h"

#include "VEEventListener.h"
#include "VEWindow.h"
#include "VEWindowGLFW.h"
#include "VEEngine.h"
#include "VEEntity.h"
#include "VESceneManager.h"
#include "VERenderer.h"
#include "VERendererForward.h"

const std::string MODEL_PATH = "models\\test\\cube_t_n_s.obj";
const std::string TEXTURE_PATH = "models\\test\\cube.png";

const int MAX_FRAMES_IN_FLIGHT = 2;


namespace ve {

	//initialize the renderer
	//create Vulkan objects like physical and logical device, swapchain, renderpasses
	//descriptor set layouts, etc.
	void VERendererForward::initRenderer() {
		VERenderer::initRenderer();

		const std::vector<const char*> requiredDeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		const std::vector<const char*> requiredValidationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		vh::vhDevPickPhysicalDevice(VEEngine::getEnginePointer()->m_instance, surface, requiredDeviceExtensions, &physicalDevice);
		vh::vhDevCreateLogicalDevice(physicalDevice, surface, requiredDeviceExtensions, requiredValidationLayers,
									&device, &graphicsQueue, &presentQueue);

		vh::vhSwapCreateSwapChain(	physicalDevice, surface, device, getWindowPointer()->getExtent(),
									&swapChain, swapChainImages, swapChainImageViews,
									&swapChainImageFormat, &swapChainExtent);

		vh::vhRenderCreateRenderPass(device, swapChainImageFormat, vh::vhDevFindDepthFormat(physicalDevice), &renderPass);

		//per frame resources
		vh::vhRenderCreateDescriptorSetLayout(device,
											{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
											{ VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT },
											&descriptorSetLayoutPerFrame);
		//per object resources
		vh::vhRenderCreateDescriptorSetLayout(device,
											{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER },
											{ VK_SHADER_STAGE_VERTEX_BIT,			VK_SHADER_STAGE_FRAGMENT_BIT },
											&descriptorSetLayoutPerObject);

		vh::vhPipeCreateGraphicsPipelineLayout(	device,
												{ descriptorSetLayoutPerObject, descriptorSetLayoutPerFrame },
												&pipelineLayout);
		vh::vhPipeCreateGraphicsPipeline(device, "shader\\vert.spv", "shader\\frag.spv",
										swapChainExtent, pipelineLayout, renderPass, &graphicsPipeline);

		vh::vhCmdCreateCommandPool(physicalDevice, device, surface, &commandPool);

		vh::vhBufCreateDepthResources(physicalDevice, device, graphicsQueue, commandPool, swapChainExtent,
										&depthImage, &depthImageMemory, &depthImageView);

		vh::vhBufCreateFramebuffers(device, swapChainImageViews, depthImageView, renderPass, swapChainExtent, swapChainFramebuffers);

		vh::vhRenderCreateDescriptorPool(device,
										{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER , VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }, 
										{ 4000, 2000 }, 
										&descriptorPool);

		vh::vhBufCreateUniformBuffers(physicalDevice, device, swapChainImages.size(), sizeof(vh::vhUBOPerFrame), uniformBuffersPerFrame, uniformBuffersPerFrameMemory);

		vh::vhRenderCreateDescriptorSets(device, swapChainImages.size(), descriptorSetLayoutPerFrame, getDescriptorPool(), descriptorSetsPerFrame);

		createSyncObjects();
	}

	//desctroy the swapchain because window soie chain or close down
	void VERendererForward::cleanupSwapChain() {
		vkDestroyImageView(device, depthImageView, nullptr);
		vkDestroyImage(device, depthImage, nullptr);
		vkFreeMemory(device, depthImageMemory, nullptr);

		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyRenderPass(device, renderPass, nullptr);

		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}

		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}

	//close the renderer, destroy all local resources
	void VERendererForward::closeRenderer() {
		cleanupSwapChain();

		vkDestroyDescriptorPool(device, descriptorPool, nullptr);

		vkDestroyDescriptorSetLayout(device, descriptorSetLayoutPerFrame, nullptr);
		vkDestroyDescriptorSetLayout(device, descriptorSetLayoutPerObject, nullptr);

		for (size_t i = 0; i < uniformBuffersPerFrame.size(); i++) {
			vkDestroyBuffer(device, uniformBuffersPerFrame[i], nullptr);
			vkFreeMemory(device, uniformBuffersPerFrameMemory[i], nullptr);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(device, commandPool, nullptr);

		vkDestroyDevice(device, nullptr);
	}

	//recreate the swapchain because the window has changed
	void VERendererForward::recreateSwapchain() {

		vkDeviceWaitIdle(device);

		cleanupSwapChain();

		vh::vhSwapCreateSwapChain(physicalDevice, surface, device, getWindowPointer()->getExtent(),
			&swapChain, swapChainImages, swapChainImageViews,
			&swapChainImageFormat, &swapChainExtent);

		vh::vhRenderCreateRenderPass(device, swapChainImageFormat, vh::vhDevFindDepthFormat(physicalDevice), &renderPass);
		vh::vhPipeCreateGraphicsPipelineLayout(	device, 
												{ descriptorSetLayoutPerObject, descriptorSetLayoutPerFrame  }, 
												&pipelineLayout);
		vh::vhPipeCreateGraphicsPipeline(device, "shader\\vert.spv", "shader\\frag.spv",
										swapChainExtent, pipelineLayout, renderPass, &graphicsPipeline);
		vh::vhBufCreateDepthResources(	physicalDevice, device, graphicsQueue, commandPool, swapChainExtent,
										&depthImage, &depthImageMemory, &depthImageView);

		vh::vhBufCreateFramebuffers(device, swapChainImageViews, depthImageView, renderPass, swapChainExtent, swapChainFramebuffers);
	}
	

	//create the semaphores and fences for syncing command buffers and swapchain
	void VERendererForward::createSyncObjects() {
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT); //for wait for the next swap chain image
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT); //for wait for render finished
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);		   //for wait for at least one image in the swap chain

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS ) {
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}

	//update the per Frame UBO and its descriptor set
	void VERendererForward::updatePerFrameUBO( uint32_t imageIndex) {
		VECamera * camera = (VECamera*)getSceneManagerPointer()->getEntity(getSceneManagerPointer()->m_cameraName);
		if (camera == nullptr) {
			throw std::exception("Error: did not find camera in Scene Manager!");
		}

		vh::vhUBOPerFrame ubo = {};

		//fill in camera data
		ubo.camModel = camera->getWorldTransform();
		ubo.camView = glm::inverse(ubo.camModel);
		ubo.camProj = camera->getProjectionMatrix(swapChainExtent.width, swapChainExtent.height);
		ubo.camProj[1][1] *= -1;

		//fill in light data
		ubo.light1.type = -1;
		if (getSceneManagerPointer()->m_lightNames.size() > 0) {
			std::string l1 = *getSceneManagerPointer()->m_lightNames.begin();
			VEEntity *e1 = getSceneManagerPointer()->getEntity(l1);
			if (e1 != nullptr) {
				VELight *plight = static_cast<VELight*>(e1);
				plight->fillVhLightStructure(&ubo.light1);
			}
		}

		void* data;
		vkMapMemory(device, uniformBuffersPerFrameMemory[imageIndex], 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, uniformBuffersPerFrameMemory[imageIndex]);

		//update the descriptor set for the per frame data
		vh::vhRenderUpdateDescriptorSet(device, descriptorSetsPerFrame[imageIndex],
											{ uniformBuffersPerFrame[imageIndex] }, //UBOs
											{ sizeof(vh::vhUBOPerFrame) },			//UBO sizes
											{ VK_NULL_HANDLE },						//textureImageViews
											{ VK_NULL_HANDLE }						//samplers
											);

	}


	//update a specific entity (UBOs and descriptor sets)
	void VERendererForward::updateEntity( uint32_t imageIndex, VEEntity *entity) {
		VECamera * camera = (VECamera*)getSceneManagerPointer()->getEntity(VEEngine::getEnginePointer()->getSceneManager()->m_cameraName);
		if (camera == nullptr) {
			throw std::exception("Error: did not find camera in Scene Manager!");
		}

		vh::vhUBOPerObject ubo = {};
		ubo.model = entity->getWorldTransform();
		ubo.modelInvTrans = glm::transpose(glm::inverse(entity->getWorldTransform()));

		void* data;
		vkMapMemory(device, entity->uniformBuffersMemory[imageIndex], 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, entity->uniformBuffersMemory[imageIndex]);

		vh::vhRenderUpdateDescriptorSet(device, entity->descriptorSets[imageIndex],
										{ entity->uniformBuffers[imageIndex], VK_NULL_HANDLE }, //UBOs
										{ sizeof(vh::vhUBOPerObject) ,		  VK_NULL_HANDLE },	//UBO sizes
										{ VK_NULL_HANDLE, entity->pEntityData->materials[0].map_Kd.imageView },	//textureImageViews
										{ VK_NULL_HANDLE, entity->pEntityData->materials[0].map_Kd.sampler }	//samplers
										);
	}

	//update all entities for drawing
	void VERendererForward::updateEntities(uint32_t imageIndex) {
		//update all entities
		for (auto &rEntity : getSceneManagerPointer()->m_entities) {
			VEEntity *entity = rEntity.second;
			if (entity->drawEntity && entity->entityType == VEEntity::VE_ENTITY_TYPE_OBJECT) {
				updateEntity(imageIndex, entity);
			}
		}
	}

	//draw a specific entity
	void VERendererForward::drawEntity( VkCommandBuffer commandBuffer, uint32_t imageIndex, VEEntity *entity ) {

		VkBuffer vertexBuffers[] = { entity->pEntityData->vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffer, entity->pEntityData->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		//set 0...per object
		//set 1...per frame
		std::vector<VkDescriptorSet> sets = { entity->descriptorSets[imageIndex], descriptorSetsPerFrame[imageIndex] };
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 2, sets.data(), 0, nullptr);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(entity->pEntityData->indices.size()), 1, 0, 0, 0);

	}

	//draw the frame:
	//- wait for draw completion using a fence, so there is at least one frame in the swapchain
	//- acquire the next image from the swap chain
	//- update all UBOs
	//- get a single time command buffer from the pool, bind pipeline and begin the render pass
	//- loop through all entities and draw them
	//- end the command buffer and submit it
	//- wait for the result and present it
	void VERendererForward::drawFrame() {
		vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

		//acquire the next image
		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), 
												imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapchain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		updatePerFrameUBO( imageIndex );	//update camera data UBO

		updateEntities( imageIndex );		//update model matrix of each entity

		//prepare command buffer for drawing
		VkCommandBuffer commandBuffer = vh::vhCmdBeginSingleTimeCommands(device, commandPool);

		vh::vhRenderBeginCommandBuffer(commandBuffer, graphicsPipeline, renderPass, swapChainFramebuffers[imageIndex], swapChainExtent);

		//go through all entities
		for (auto &rEntity : VEEngine::getEnginePointer()->getSceneManager()->m_entities) {
			VEEntity *entity = rEntity.second;
			if (entity->drawEntity && entity->entityType== VEEntity::VE_ENTITY_TYPE_OBJECT ) {
				drawEntity( commandBuffer, imageIndex, entity );
			}
		}

		vkCmdEndRenderPass(commandBuffer);

		vh::vhCmdEndSingleTimeCommands(device, graphicsQueue, commandPool, commandBuffer,
			imageAvailableSemaphores[currentFrame], renderFinishedSemaphores[currentFrame], inFlightFences[currentFrame]);

		result = vh::vhRenderPresentResult( presentQueue, swapChain, imageIndex, 
											renderFinishedSemaphores[currentFrame]);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
			framebufferResized = false;
			recreateSwapchain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

}











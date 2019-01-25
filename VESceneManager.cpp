#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>

#include "tiny_obj_loader.h"

#include <vector>
#include <set>

#include <vulkan/vulkan.hpp>

#include "VHHelper.h"

#include "VEEventListener.h"
#include "VEEngine.h"
#include "VEEntity.h"
#include "VERenderer.h"
#include "VESceneManager.h"

#include <iostream>

namespace ve {

	void VESceneManager::initSceneManager() {
		//camera parent is used for translation rotations
		VEEntity *cameraParent = new VEEntity("StandardCameraParent");
		cameraParent->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
		addEntity(cameraParent);

		//camera can only do yaw (parent y-axis) and pitch (local x-axis) rotations
		VECamera *camera = new VECamera("StandardCamera");
		camera->localToParentTransform = glm::inverse(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-2.0f, -2.0f, -2.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		camera->fov = glm::radians(45.0f);
		camera->pEntityParent = cameraParent;
		addEntity(camera);
		m_cameraName = camera->entityName;

		//use one light source
		VELight *light = new VELight("StandardLight");
		light->type = 0;	//directional
		light->localToParentTransform = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		light->localToParentTransform = glm::inverse(light->localToParentTransform); //LookAt produces a view matrix, not a model matrix
		addEntity(light);
		m_lightNames.insert(light->entityName);
	};

	//---------------------------------------------------------------------------------------------
	//simple OBJ loader
	//
	VEEntity * VESceneManager::loadOBJ( std::string entityName, std::string basedir, std::string filename, std::string texName, float texFactor ) {
		if (m_entities.count(entityName) > 0)
			return m_entities[entityName];

		VEEntity *entity = new VEEntity(entityName);

		std::string filekey = "";
		if (filename.length() > 0) {
			entity->drawEntity = true;

			filekey = basedir + "\\" + filename + "+" + texName;

			if (m_entityData.count(filekey) == 0) {						// did I load this file before?
				veEntityData *entityData = new veEntityData();
				entityData->entityDataName = filekey;
				veMaterial material;

				vh::vhFileLoadOBJ(basedir, filename, entityData->vertices, entityData->indices, texFactor);

				if (entityData->vertices.size() > 0 && entityData->indices.size() > 0) {

					//vertex and index buffers
					vh::vhBufCreateVertexBuffer(getRendererPointer()->physicalDevice, getRendererPointer()->device,
												getRendererPointer()->graphicsQueue,  getRendererPointer()->commandPool,
												entityData->vertices, &entityData->vertexBuffer, &entityData->vertexBufferMemory);

					vh::vhBufCreateIndexBuffer(	getRendererPointer()->physicalDevice, getRendererPointer()->device, getRendererPointer()->graphicsQueue,
												getRendererPointer()->commandPool,
												entityData->indices, &entityData->indexBuffer, &entityData->indexBufferMemory);

					//texture image buffer and view
					vh::vhBufCreateTextureImage(getRendererPointer()->physicalDevice, getRendererPointer()->device, 
												getRendererPointer()->graphicsQueue, getRendererPointer()->commandPool,
												basedir+"\\"+texName, &material.map_Kd.image, &material.map_Kd.deviceMemory);
					vh::vhBufCreateImageView(	getRendererPointer()->device, material.map_Kd.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, 
												&material.map_Kd.imageView);
					vh::vhBufCreateTextureSampler(getRendererPointer()->device, &material.map_Kd.sampler);

					entityData->materials.push_back(material);		//store in the entityData material array
				
				}
				else entity->drawEntity = false;
				m_entityData[filekey] = entityData;								//store file data in array
			}

			if (entity->drawEntity) {
				//uniform buffers, one for each swapchain image
				vh::vhBufCreateUniformBuffers(  getRendererPointer()->physicalDevice, getRendererPointer()->device, 
												getRendererPointer()->swapChainImages.size(), sizeof(vh::vhUBOPerObject), 
												entity->uniformBuffers, entity->uniformBuffersMemory);
				vh::vhRenderCreateDescriptorSets(getRendererPointer()->device, getRendererPointer()->swapChainImages.size(), 
												getRendererPointer()->getDescriptorSetLayout(), getRendererPointer()->getDescriptorPool(), 
												entity->descriptorSets);
			}
			entity->boundingBox = calculateEntityBoundingBox(m_entityData[filekey]->vertices);
		}
		else entity->drawEntity = false;

		entity->pEntityData = m_entityData[filekey];	//store link to file data that is used by this entity, if empty then this is a pure transform node
		m_entities[entityName] = entity;				// store entity in the entity array

		return m_entities[entityName];
	}


	//---------------------------------------------------------------------------------------------------------------
	//complex OBJ loader
	//

	void VESceneManager::createTexture(std::string &basedir, std::string &texName, veTexture &texture) {
		if (texName.length() == 0) return;

		vh::vhBufCreateTextureImage(getRendererPointer()->physicalDevice, getRendererPointer()->device,
									getRendererPointer()->graphicsQueue, getRendererPointer()->commandPool,
									basedir + "\\" + texName, &texture.image, &texture.deviceMemory);

		vh::vhBufCreateImageView(	getRendererPointer()->device, texture.image, getRendererPointer()->m_surfaceFormat.format,  
									VK_IMAGE_ASPECT_COLOR_BIT, &texture.imageView);

		vh::vhBufCreateTextureSampler(getRendererPointer()->device, &texture.sampler);

	}

	void VESceneManager::copyMaterial(std::string &basedir, tinyobj::material_t &materialFileData, veMaterial & vematerial) {

		//lambertian
		vematerial.materialBuffer.ambient = glm::vec3(materialFileData.ambient[0], materialFileData.ambient[1], materialFileData.ambient[2]);
		vematerial.materialBuffer.diffuse = glm::vec3(materialFileData.diffuse[0], materialFileData.diffuse[1], materialFileData.diffuse[2]);
		vematerial.materialBuffer.specular = glm::vec3(materialFileData.specular[0], materialFileData.specular[1], materialFileData.specular[2]);
		vematerial.materialBuffer.transmittance = glm::vec3(materialFileData.transmittance[0], materialFileData.transmittance[1], materialFileData.transmittance[2]);
		vematerial.materialBuffer.emission = glm::vec3(materialFileData.emission[0], materialFileData.emission[1], materialFileData.emission[2]);
		vematerial.materialBuffer.shininess = materialFileData.shininess;
		vematerial.materialBuffer.ior = materialFileData.ior;
		vematerial.materialBuffer.dissolve = materialFileData.dissolve;

		createTexture(basedir, materialFileData.ambient_texname, vematerial.map_Ka);
		createTexture(basedir, materialFileData.diffuse_texname, vematerial.map_Kd);
		createTexture(basedir, materialFileData.specular_texname, vematerial.map_Ks);
		createTexture(basedir, materialFileData.bump_texname, vematerial.map_Bump);
		createTexture(basedir, materialFileData.normal_texname, vematerial.map_Normal);
		createTexture(basedir, materialFileData.alpha_texname, vematerial.map_Alpha);

		//PBR
		vematerial.materialBuffer.roughness = materialFileData.roughness;				// [0, 1] default 0
		vematerial.materialBuffer.metallic = materialFileData.metallic;				// [0, 1] default 0
		vematerial.materialBuffer.sheen = materialFileData.sheen;					// [0, 1] default 0
		vematerial.materialBuffer.clearcoat_thickness = materialFileData.clearcoat_thickness;		// [0, 1] default 0
		vematerial.materialBuffer.clearcoat_roughness = materialFileData.clearcoat_roughness;		// [0, 1] default 0
		vematerial.materialBuffer.anisotropy = materialFileData.anisotropy;				// aniso. [0, 1] default 0
		vematerial.materialBuffer.anisotropy_rotation = materialFileData.anisotropy_rotation;		// anisor. [0, 1] default 0

		std::string roughness_texname;  // map_Pr
		std::string metallic_texname;   // map_Pm
		std::string sheen_texname;      // map_Ps
		std::string emissive_texname;   // map_Ke
		std::string normal_texname;     // norm. For normal mapping.

		createTexture(basedir, materialFileData.roughness_texname, vematerial.map_Pr);
		createTexture(basedir, materialFileData.metallic_texname, vematerial.map_Pm);
		createTexture(basedir, materialFileData.sheen_texname, vematerial.map_Ps);
		createTexture(basedir, materialFileData.emissive_texname, vematerial.map_Ke);
	}


	VEEntity * VESceneManager::loadOBJ2(std::string entityName, std::string basedir, std::string filename, int32_t shapeIdx) {

		if (m_entities.count(entityName) > 0)
			return m_entities[entityName];

		VEEntity *entity = new VEEntity(entityName);
		entity->shapeIdx = shapeIdx;

		std::string filekey = "";
		/*if (filename.length() > 0) {
			entity->drawEntity = true;

			filekey = basedir + "\\" + filename;

			if (m_entityData.count(filekey) == 0) {						// did I load this file before?
				veEntityData *entityData = new veEntityData();
				entityData->entityDataName = filekey;

				//if (!vhFileLoadOBJ2(basedir, filename, entityData->fileData)) return nullptr;	// no -> load it from file

				if (entityData->fileData.vertices.size() > 0 && entityData->fileData.indices.size() > 0) {
					//vhBufCreateVertexBuffer(m_pRenderer->physicalDevice, m_pRenderer->device,
					//						m_pRenderer->graphicsQueue, m_pRenderer->commandPool,
					//						entityData->fileData.vertices, &entityData->vertexBuffer, &entityData->vertexBufferMemory);

					//vhBufCreateIndexBuffer(	m_pRenderer->physicalDevice, m_pRenderer->device, m_pRenderer->graphicsQueue,
					//						m_pRenderer->commandPool,
					//						entityData->fileData.indices, &entityData->indexBuffer, &entityData->indexBufferMemory);
				}
				else entity->drawEntity = false;

				for (auto &materialFileData : entityData->fileData.materials) {	//copy the file materials so I can use them
					veMaterial vematerial;
					copyMaterial(basedir, materialFileData, vematerial);				//copy material parameters and create textures
					entityData->materials.push_back(vematerial);					//store in the entityData material array
				}

				m_entityData[filekey] = entityData;								//store file data in array
			}
		}
		else entity->drawEntity = false;*/

		entity->pEntityData = m_entityData[filekey];		//store link to file data that is used by this entity, if empty then this is a pure transform node
		m_entities[entityName] = entity;		// store entity in the entity array

		return m_entities[entityName];
	}

	//----------------------------------------------------------------------------------------------------------------
	//scene management stuff
	//

	VEEntity * VESceneManager::getEntity(std::string objectname) {
		if (m_entities.count(objectname) > 0) return m_entities[objectname];
		return nullptr;
	}

	VEEntity * VESceneManager::getRemovedEntity(std::string objectname) {
		if (m_removedEntities.count(objectname) > 0) return m_removedEntities[objectname];
		return nullptr;
	}

	veEntityData *	VESceneManager::getEntityData(std::string entityDataName) {
		if (m_entityData.count(entityDataName) == 0) return nullptr;
		return m_entityData[entityDataName];
	}

	void VESceneManager::removeEntity(std::string name) {
		for (auto entity : m_entities) {
		//	if (entity.second->pEntityParent->entityName == name) entity.second->pEntityParent->entityName = nullptr;
		}
		VEEntity *e9 = getEntity(name);
		if (e9 != nullptr) {
			addRemovedEntity(e9);
		}
		m_entities.erase(name);
	}

	void VESceneManager::returnRemovedEntity(std::string name) {
		VEEntity *e9 = getRemovedEntity(name);
		if (e9 != nullptr) {
			addEntity(e9);
		}
		m_removedEntities.erase(name);
	}

	void VESceneManager::removeEntityData(std::string name) {
		veEntityData * pEntityData = m_entityData[name];
		std::vector<std::string> remove_ent = {};
		for (auto &entity : m_entities) {
			if (entity.second->pEntityData == pEntityData) {
				remove_ent.push_back(entity.second->entityName);
			}
		}
		for (auto &ename : remove_ent) {
			m_entities.erase(ename);
		}
		if (pEntityData->vertexBuffer != VK_NULL_HANDLE) vkDestroyBuffer(VEEngine::getEnginePointer()->getRenderer()->device, pEntityData->vertexBuffer, nullptr);
		if (pEntityData->vertexBufferMemory != VK_NULL_HANDLE) vkFreeMemory(VEEngine::getEnginePointer()->getRenderer()->device, pEntityData->vertexBufferMemory, nullptr);
		if (pEntityData->indexBuffer != VK_NULL_HANDLE) vkDestroyBuffer(VEEngine::getEnginePointer()->getRenderer()->device, pEntityData->indexBuffer, nullptr);
		if (pEntityData->indexBufferMemory != VK_NULL_HANDLE) vkFreeMemory(VEEngine::getEnginePointer()->getRenderer()->device, pEntityData->indexBufferMemory, nullptr);
		for (auto material : pEntityData->materials) {
			freeTexture(material.map_Ka);
			freeTexture(material.map_Kd);
			freeTexture(material.map_Ks);
			freeTexture(material.map_Bump);
			freeTexture(material.map_Normal);
			freeTexture(material.map_Alpha);
			freeTexture(material.map_Pr);
			freeTexture(material.map_Pm);
			freeTexture(material.map_Ps);
			freeTexture(material.map_Ke);
		}
		m_entityData.erase(name);
	}

	void VESceneManager::removeEntityBoundingBox(std::string name){
		m_entityBoundingBox.erase(name);
	};

	veEntityBoundingBox * VESceneManager::calculateEntityBoundingBox(std::vector<vh::vhVertex> vertices) {
		veEntityBoundingBox *boundingBox = new veEntityBoundingBox();
		glm::vec3 maxVertex = vertices.front().pos;
		glm::vec3 minVertex = vertices.front().pos;

		for (auto const& v : vertices) {
			if (v.pos.x > maxVertex.x) maxVertex.x = v.pos.x;
			if (v.pos.y > maxVertex.y) maxVertex.y = v.pos.y;
			if (v.pos.z > maxVertex.z) maxVertex.z = v.pos.z;

			if (v.pos.x < minVertex.x) minVertex.x = v.pos.x;
			if (v.pos.y < minVertex.y) minVertex.y = v.pos.y;
			if (v.pos.z < minVertex.z) minVertex.z = v.pos.z;
		}
		boundingBox->maxVertex = glm::vec4(maxVertex, 1);
		boundingBox->minVertex = glm::vec4(minVertex, 1);

		boundingBox->maxVertexCurr = boundingBox->maxVertex;
		boundingBox->minVertexCurr = boundingBox->minVertex;

		return boundingBox;
	}

	void VESceneManager::updateEntityCurrentBoundingBox(VEEntity * entity)
	{
		veEntityBoundingBox * entityBoundingBox = entity->boundingBox;
		entityBoundingBox->maxVertexCurr = entity->localToParentTransform * entityBoundingBox->maxVertex;
		entityBoundingBox->minVertexCurr = entity->localToParentTransform * entityBoundingBox->minVertex;
	}

	std::set<VEEntity*> VESceneManager::findUserCollision(std::string entityName)
	{
		VEEntity *eUser = getSceneManagerPointer()->getEntity(entityName);
		updateEntityCurrentBoundingBox(eUser);
		glm::vec4 eUserBBMax = eUser->boundingBox->maxVertexCurr;
		glm::vec4 eUserBBMin = eUser->boundingBox->minVertexCurr;

		std::set<VEEntity*> entitiesColided = {};

		for (auto const& entity : m_entities)
		{
			if (entity.second != eUser && entity.first!= "The Plane" && entity.second->boundingBox!=nullptr) {
				glm::vec4 entityBBMax = entity.second->boundingBox->maxVertexCurr;
				glm::vec4 entityBBMin = entity.second->boundingBox->minVertexCurr;

				if (eUserBBMax.x >= entityBBMin.x && entityBBMax.x >= eUserBBMin.x && eUserBBMax.y >= entityBBMin.y && entityBBMax.y >= eUserBBMin.y) {
					VEEntity *colidedEntity = entity.second;
					entitiesColided.insert(colidedEntity);
					std::cout << entity.first ;
				}
			}			
		}
		return entitiesColided;
	}

	void VESceneManager::freeTexture(veTexture & texture) {
		if (texture.imageView != VK_NULL_HANDLE) vkDestroyImageView(VEEngine::getEnginePointer()->getRenderer()->device, texture.imageView, nullptr);
		if (texture.image != VK_NULL_HANDLE) vkDestroyImage(VEEngine::getEnginePointer()->getRenderer()->device, texture.image, nullptr);
		if (texture.sampler != VK_NULL_HANDLE) vkDestroySampler(VEEngine::getEnginePointer()->getRenderer()->device, texture.sampler, nullptr);
		if (texture.deviceMemory != VK_NULL_HANDLE) vkFreeMemory(VEEngine::getEnginePointer()->getRenderer()->device, texture.deviceMemory, nullptr);
	}

	void VESceneManager::closeSceneManager() {

		for (auto ent : m_entities) {
			VEEntity *pEnt = ent.second;
			if (pEnt->drawEntity) {
				for (int i = 0; i < pEnt->uniformBuffers.size(); i++) {
					vkDestroyBuffer(VEEngine::getEnginePointer()->getRenderer()->device, pEnt->uniformBuffers[i], nullptr);
					vkFreeMemory(VEEngine::getEnginePointer()->getRenderer()->device, pEnt->uniformBuffersMemory[i], nullptr);
				}
			}
		}

		for (auto entdata : m_entityData) {
			veEntityData *pDat = entdata.second;

			vkDestroyBuffer(VEEngine::getEnginePointer()->getRenderer()->device, pDat->indexBuffer, nullptr);
			vkFreeMemory(VEEngine::getEnginePointer()->getRenderer()->device, pDat->indexBufferMemory, nullptr);

			vkDestroyBuffer(VEEngine::getEnginePointer()->getRenderer()->device, pDat->vertexBuffer, nullptr);
			vkFreeMemory(VEEngine::getEnginePointer()->getRenderer()->device, pDat->vertexBufferMemory, nullptr);

			for (auto mat : pDat->materials) {
				if (mat.map_Kd.image != VK_NULL_HANDLE ) {
					vkDestroySampler(VEEngine::getEnginePointer()->getRenderer()->device, mat.map_Kd.sampler, nullptr);
					vkDestroyImageView(VEEngine::getEnginePointer()->getRenderer()->device, mat.map_Kd.imageView, nullptr);
					vkDestroyImage(VEEngine::getEnginePointer()->getRenderer()->device, mat.map_Kd.image, nullptr);
					vkFreeMemory(VEEngine::getEnginePointer()->getRenderer()->device, mat.map_Kd.deviceMemory, nullptr);
				}
			}
		}
	}



}
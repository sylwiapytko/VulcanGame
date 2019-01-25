#pragma once


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <map>
#include <set>


#define getSceneManagerPointer() VEEngine::getEnginePointer()->getSceneManager()

namespace ve {
	class VEEngine;
	class VERenderer;
	class VERendererForward;


	class VESceneManager {
		friend VEEngine;
		friend VERenderer;
		friend VERendererForward;

	protected:
		std::map<std::string, VEEntity*>	 m_entities = {};	///< storage of all entities currently in the engine
		std::map<std::string, VEEntity*>	 m_removedEntities = {};	
		std::map<std::string, veEntityData*> m_entityData = {};	///< storage of all OBJ file data loaded so far

		std::map<std::string, veEntityBoundingBox*> m_entityBoundingBox = {};	

		std::string			  m_cameraName = "";				//entity name of the current camera
		std::set<std::string> m_lightNames = {};				//names of the lights to use

		virtual void initSceneManager();						///< initialization (empty)
		virtual void closeSceneManager();

		void createTexture(std::string &basedir, std::string &texName, veTexture &texture);
		void copyMaterial(std::string &basedir, tinyobj::material_t &materialFileData, veMaterial & vematerial);
		void freeTexture(veTexture & texture);

	public:
		VESceneManager() {};
		~VESceneManager() {};

		VEEntity *		loadOBJ(std::string entityName, std::string basename, 
								std::string filename, std::string texname, float texFactor=1.0f );	 	///< simple load

		VEEntity *		loadOBJ2(std::string entityName, std::string basename, 
								std::string filename, int32_t shapeIdx = -1 );		///< full load

		void			addEntity(VEEntity *entity) { m_entities[entity->entityName] = entity; };
		void			addRemovedEntity(VEEntity *entity) { m_removedEntities[entity->entityName] = entity; };
		VEEntity *		getEntity(std::string entityName);		 								///< return a pointer to an entity
		VEEntity *		getRemovedEntity(std::string entityName);		 								///< return a pointer to an entity
		void			removeEntity(std::string name);
		void			returnRemovedEntity(std::string name);

		void			addEntityData(veEntityData *entityData) { m_entityData[entityData->entityDataName] = entityData; };
		veEntityData *	getEntityData(std::string entityDataName);
		void			removeEntityData(std::string name);

		void			addEntityBoundingBox(veEntityBoundingBox *entityBoundingBox) { m_entityBoundingBox[entityBoundingBox->entityBoxName] = entityBoundingBox; };
		void			removeEntityBoundingBox(std::string name);
		veEntityBoundingBox * calculateEntityBoundingBox(std::vector<vh::vhVertex> vertices);

		void updateEntityCurrentBoundingBox(VEEntity *entity);
		std::set<VEEntity*> findUserCollision(std::string entityName);

		std::string		getCameraName() { return m_cameraName; };
		void			setCameraName(std::string name) { m_cameraName = name; };

		std::set<std::string> & getLightnames() { return m_lightNames;  };
	};
}


#pragma once

namespace ve {

	struct veTexture {
		VkImage			image = VK_NULL_HANDLE;
		VkImageView		imageView = VK_NULL_HANDLE;
		VkDeviceMemory	deviceMemory = VK_NULL_HANDLE;
		VkSampler		sampler = VK_NULL_HANDLE;
	};

	struct veMaterialBuffer {
		glm::vec3 ambient;
		int32_t	  map_Ka_idx = -1;
		glm::vec3 diffuse;
		int32_t	  map_Kd_idx = -1;
		glm::vec3 specular;
		int32_t	  map_Ks_idx = -1;
		glm::vec3 transmittance;
		int32_t	  map_Bump_idx = -1;
		glm::vec3 emission;
		int32_t	  map_Normal_idx = -1;
		glm::float32 shininess;
		glm::float32 ior;       // index of refraction
		glm::float32 dissolve;  // 1 == opaque; 0 == fully transparent
		int32_t	  map_Alpha_idx = -1;

		glm::float32 roughness;            // [0, 1] default 0
		glm::float32 metallic;             // [0, 1] default 0
		glm::float32 sheen;                // [0, 1] default 0
		glm::float32 clearcoat_thickness;  // [0, 1] default 0
		glm::float32 clearcoat_roughness;  // [0, 1] default 0
		glm::float32 anisotropy;           // aniso. [0, 1] default 0
		glm::float32 anisotropy_rotation;  // anisor. [0, 1] default 0
		int32_t		 map_Pr_idx = -1;
		int32_t		 map_Pm_idx = -1;
		int32_t		 map_Ps_idx = -1;
		int32_t		 map_Ke_idx = -1;

		glm::vec3	 pad;	   // pad for alignment to multiple of 4 floats (16 bytes)
	};


	struct veMaterial {
		veMaterialBuffer materialBuffer = {};	//this goes into a UBO

		//lambertian
		veTexture map_Ka = {};
		veTexture map_Kd = {};
		veTexture map_Ks = {};
		veTexture map_Bump = {};
		veTexture map_Normal = {};
		veTexture map_Alpha = {};

		//pbr
		veTexture map_Pr = {};
		veTexture map_Pm = {};
		veTexture map_Ps = {};
		veTexture map_Ke = {};
	};

	struct veEntityBoundingBox {
		std::string	entityBoxName = "";
		glm::vec4 maxVertex;
		glm::vec4 minVertex;
		glm::vec4 maxVertexCurr;
		glm::vec4 minVertexCurr;
	};

	struct veEntityData {
		std::string				entityDataName = "";
		//vh::vhFileData			fileData = {};
		std::vector<vh::vhVertex> vertices;
		std::vector<uint32_t>	indices;
		VkBuffer				vertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory			vertexBufferMemory = VK_NULL_HANDLE;
		VkBuffer				indexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory			indexBufferMemory = VK_NULL_HANDLE;
		std::vector<veMaterial> materials = {};

		veEntityBoundingBox	*	boundingBox;
	};


	class VEEntity {
	public:
		enum veEntityType {
			VE_ENTITY_TYPE_OBJECT = 1,
			VE_ENTITY_TYPE_CAMERA = 2,
			VE_ENTITY_TYPE_LIGHT = 4,
			VE_ENTITY_TYPE_LIGHT_DIRECTIONAL = 8,
			VE_ENTITY_TYPE_LIGHT_POINT = 16,
			VE_ENTITY_TYPE_LIGHT_SPOT = 32
		};

		uint32_t	    entityType = VE_ENTITY_TYPE_OBJECT;
		std::string		entityName = "";
		veEntityData*	pEntityData = nullptr;
		VEEntity *		pEntityParent = nullptr;
		glm::mat4x4		localToParentTransform = glm::mat4(1.0);
		int32_t			shapeIdx = -1;		///< index of the sub shape that is selected, -1 if all shapes are used
		bool			drawEntity = false;

		std::vector<VkBuffer>			uniformBuffers;			//one for each framebuffer frame
		std::vector<VkDeviceMemory>		uniformBuffersMemory;
		std::vector<VkDescriptorSet>	descriptorSets;

		VEEntity( std::string name ) : entityName(name) {};

		glm::mat4x4 getWorldTransform() {
			if (pEntityParent != nullptr) return pEntityParent->getWorldTransform() * localToParentTransform;
			return localToParentTransform;
		};
	};

	class VECamera : public VEEntity {
	public:
		float fov;
		VECamera(std::string name) : VEEntity(name) { entityType = VE_ENTITY_TYPE_CAMERA; };

		glm::mat4 getProjectionMatrix( uint32_t width, uint32_t height ) {
			return glm::perspective(glm::radians(45.0f), width / (float)height, 0.1f, 200.0f);
		}
	};

	class VELight : public VEEntity {
	public:
		int32_t   type;											//0-directional , 1-point, 2-spot
		glm::vec4 param = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);	//1-2: attenuation, 3: Ns
		glm::vec4 col_ambient  = 0.4f * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 col_diffuse  = 0.8f * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 col_specular = 0.9f * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		void fillVhLightStructure(vh::vhLight *pLight) {
			pLight->type = type;
			pLight->param = param;
			pLight->col_ambient = col_ambient;
			pLight->col_diffuse = col_diffuse;
			pLight->col_specular = col_specular;
			pLight->position = getWorldTransform();
		}

		VELight(std::string name) : VEEntity(name) { entityType = VE_ENTITY_TYPE_LIGHT | VE_ENTITY_TYPE_LIGHT_DIRECTIONAL; };
	};


}



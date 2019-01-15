
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
#include "VEEventListener.h"
#include "VEEngine.h"
#include "VEEntity.h"
#include "VESceneManager.h"
#include "VERenderer.h"

class VEEngine;


namespace ve {

	void VERenderer::initRenderer() {
	}


	void VERenderer::closeRenderer() {

	}

	void VERenderer::recreateSwapchain() {

	}

}



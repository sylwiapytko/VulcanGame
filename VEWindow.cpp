
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include "tiny_obj_loader.h"

#include <vulkan/vulkan.hpp>

#include <map>

#include "VHHelper.h"

#include "VEEntity.h"
#include "VEEventListener.h"
#include "VESceneManager.h"
#include "VEEngine.h"
#include "VEWindow.h"


namespace ve {

	void VEWindow::windowSizeChanged() {
		VEEngine::getEnginePointer()->windowSizeChanged();
	};

	void VEWindow::processEvent(veEvent event) {
		VEEngine::getEnginePointer()->addEvent( event );
	};

}


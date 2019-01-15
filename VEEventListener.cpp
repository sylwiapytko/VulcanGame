
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <vector>
#include <set>
#include <vulkan/vulkan.hpp>

#include "tiny_obj_loader.h"

#include "VHHelper.h"

#include "VEEventListener.h"
#include "VEEngine.h"
#include "VEEntity.h"
#include "VESceneManager.h"
#include "VEEngine.h"


namespace ve {

	//depending on the event type, call the respective callback
	bool VEEventListener::onEvent( veEvent event ) {
		switch ( event.type ) {
		case VE_EVENT_FRAME_STARTED :
			onFrameStarted(event);
			return false;
			break;
		case VE_EVENT_FRAME_ENDED:
			onFrameEnded(event);
			return false;
			break;
		case VE_EVENT_KEYBOARD:
			return onKeyboard(event);
			break;
		case VE_EVENT_MOUSEMOVE:
			return onMouseMove(event);
			break;
		case VE_EVENT_MOUSEBUTTON:
			return onMouseButton(event);
			break;
		case VE_EVENT_MOUSESCROLL:
			return onMouseScroll(event);
			break;

		default:
			break;
		}
		return false;
	}


}


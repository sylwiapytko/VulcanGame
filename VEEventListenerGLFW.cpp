
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
#include "VEEventListenerGLFW.h"
#include "VEEngine.h"
#include "VEEntity.h"
#include "VESceneManager.h"
#include "VEEngine.h"

#include <iostream>

namespace ve {

	void VEEventListenerGLFW::onFrameStarted(veEvent event) {
	}

	void VEEventListenerGLFW::onFrameEnded(veEvent event) {
	}

	//callback for GLFW keyboard events
	bool VEEventListenerGLFW::onKeyboard(veEvent event) {
		if (event.idata1 == GLFW_KEY_ESCAPE && event.idata3 == GLFW_PRESS) { //ESC pressed -> end the engine
			VEEngine::getEnginePointer()->m_end_running = true;
			return true;
		}

		if (event.idata3 == GLFW_REPEAT) return true;	//do I need this?
		
		if (event.idata3 == GLFW_PRESS) {				//just started to press a key -> remember it in the engine
			
			VEEngine::getEnginePointer()->m_keys_pressed.insert(event.idata1);

			// Movment keys - rotation I=73 J=74 K=75 L=76 
			/*
			if (event.idata1 == 73 || event.idata1 == 74 || event.idata1 == 75 || event.idata1 == 76) {
				int key_pressed_last = VEEngine::getEnginePointer()->m_keys_pressed_last;
				VEEngine::getEnginePointer()->m_keys_pressed_last = event.idata1;

				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Bird");
				if (e9 != nullptr) {
					glm::mat4 rotate = glm::mat4(1.0f);
					if (key_pressed_last == event.idata1 + 2 || key_pressed_last == event.idata1 - 2) {
						rotate = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					}
					else if (key_pressed_last == 73 && event.idata1 == 74 || key_pressed_last == 76 && event.idata1 == 73 || key_pressed_last == 75 && event.idata1 == 76 || key_pressed_last == 74 && event.idata1 == 75) {
						rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					}
					else if (key_pressed_last == 74 && event.idata1 == 73 || key_pressed_last == 73 && event.idata1 == 76 || key_pressed_last == 76 && event.idata1 == 75 || key_pressed_last == 75 && event.idata1 == 74) {
						rotate = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					}
					e9->localToParentTransform = e9->localToParentTransform *rotate;
				}
			}			
			*/
			return true;
		}

		if (event.idata3 == GLFW_RELEASE) {				//just released a key -> remove from pressed keys list
			
			VEEngine::getEnginePointer()->m_keys_pressed.erase(event.idata1);
			return true;
		}

		//create some default constants for the actions 
		glm::vec4 translate = glm::vec4(0.0, 0.0, 0.0, 1.0);	//total translation
		glm::vec4 rot4 = glm::vec4(1.0);						//total rotation around the axes, is 4d !
		float angle = 0.0;
		
		VECamera *pCamera = (VECamera*)VEEngine::getEnginePointer()->m_pSceneManager->getEntity(VEEngine::getEnginePointer()->getSceneManager()->getCameraName());
		VEEntity *pParent = pCamera->pEntityParent;

		switch (event.idata1) {
			/*
		case GLFW_KEY_A:
			translate = glm::vec4(-1.0, 0.0, 0.0, 1.0);
			break;
		case GLFW_KEY_D:
			translate = glm::vec4(1.0, 0.0, 0.0, 1.0);
			break;
		case GLFW_KEY_W:
			translate = glm::vec4(0.0, 0.0, -1.0, 1.0);
			break;
		case GLFW_KEY_S:
			translate = glm::vec4(0.0, 0.0, 1.0, 1.0);
			break;
		case GLFW_KEY_Q:
			translate = glm::vec4(0.0, -1.0, 0.0, 1.0);
			break;
		case GLFW_KEY_E:
			translate = glm::vec4(0.0, 1.0, 1.0, 1.0);
			break;
		case GLFW_KEY_LEFT:							//yaw rotation is already in parent space
			angle = (float)event.dt * 1.0;
			rot4 = glm::vec4(0.0, 0.0, 1.0, 1.0);
			break;
		case GLFW_KEY_RIGHT:						//yaw rotation is already in parent space
			angle = (float)event.dt * -1.0;
			rot4 = glm::vec4(0.0, 0.0, 1.0, 1.0);
			break;
		case GLFW_KEY_UP:							//pitch rotation is in cam/local space
			angle = (float)event.dt * -1.0;			//pitch angle
			rot4 = pCamera->localToParentTransform * glm::vec4(1.0, 0.0, 0.0, 1.0); //x axis from local to parent space!
			break;
		case GLFW_KEY_DOWN:							//pitch rotation is in cam/local space
			angle = (float)event.dt * 1.0;			//pitch angle
			rot4 = pCamera->localToParentTransform * glm::vec4(1.0, 0.0, 0.0, 1.0); //x axis from local to parent space!
			break;
		*/
		default:
			return false;
		};

		if (pParent == nullptr) {
			pParent = pCamera;
		}
		else {
			//so far the translation vector was defined in cam local space. But the camera frame of reference 
			//is defined wrt its parent space, so we must transform this vector to parent space
			translate = pCamera->localToParentTransform * translate;	//transform from local camera space to parent space
		}

		//add the new translation vector to the previous one
		float speed = 10.0f;
		glm::vec3 trans = speed * glm::vec3(translate.x, translate.y, translate.z);
		pParent->localToParentTransform = glm::translate(glm::mat4(1.0f), (float)event.dt * trans) * pParent->localToParentTransform;

		//combination of yaw and pitch, both wrt to parent space
		glm::vec3    rot3 = glm::vec3(rot4.x, rot4.y, rot4.z);
		glm::mat4x4  rotate = glm::rotate(glm::mat4(1.0), angle, rot3);
		pCamera->localToParentTransform = rotate * pCamera->localToParentTransform;
		
		return true;
	}

	//default behavior when the moue is moved
	//if left button is clicked then is is equivalent of UP/DOWN LEFT/RIGHT keys
	//will rotate the camera
	bool VEEventListenerGLFW::onMouseMove(veEvent event) {

		auto button = VEEngine::getEnginePointer()->m_mouse_buttons_clicked.find(GLFW_MOUSE_BUTTON_LEFT);
		if (button == VEEngine::getEnginePointer()->m_mouse_buttons_clicked.end()) return true;

		float x = event.fdata1;
		float y = event.fdata2;

		if (!m_usePrevCursorPosition) {
			m_cursorPrevX = x;
			m_cursorPrevY = y;
			m_usePrevCursorPosition = true;
			return true;
		}

		float dx = x - m_cursorPrevX;
		float dy = y - m_cursorPrevY;

		m_cursorPrevX = x;
		m_cursorPrevY = y;
		/*
		VECamera *pCamera = (VECamera*)VEEngine::getEnginePointer()->getSceneManager()->getEntity(VEEngine::getEnginePointer()->getSceneManager()->getCameraName());
		VEEntity *pParent = pCamera->pEntityParent;

		float slow = 2.0;

		//dx
		float angledx = -slow*(float)event.dt * dx;
		glm::vec4 rot4dx = glm::vec4(0.0, 0.0, 1.0, 1.0);
		glm::vec3 rot3dx = glm::vec3(rot4dx.x, rot4dx.y, rot4dx.z);
		glm::mat4x4 rotatedx = glm::rotate(glm::mat4(1.0), angledx, rot3dx);

		//dy
		float angledy = -slow*(float)event.dt * dy;			//pitch angle
		glm::vec4 rot4dy = pCamera->localToParentTransform * glm::vec4(1.0, 0.0, 0.0, 1.0); //x axis from local to parent space!
		glm::vec3 rot3dy = glm::vec3(rot4dy.x, rot4dy.y, rot4dy.z);
		glm::mat4x4 rotatedy = glm::rotate(glm::mat4(1.0), angledy, rot3dy);

		pCamera->localToParentTransform = rotatedx * rotatedy * pCamera->localToParentTransform;
		*/
		return false;
	}

	//track buttons of the mouse
	//if a button is clicked or released then this is noted in the engine m_mouse_buttons_clicked set
	bool VEEventListenerGLFW::onMouseButton(veEvent event) {

		if (event.idata2 == GLFW_REPEAT) return true;	//do I need this?

		if (event.idata2 == GLFW_PRESS) {				//just started to press a key -> remember it in the engine
			VEEngine::getEnginePointer()->m_mouse_buttons_clicked.insert(event.idata1);
			m_usePrevCursorPosition = false;
			return true;
		}

		if (event.idata2 == GLFW_RELEASE) {				//just released a key -> remove from pressed keys list
			VEEngine::getEnginePointer()->m_mouse_buttons_clicked.erase(event.idata1);
			m_usePrevCursorPosition = false;
			return true;
		}

		return false;
	}

	//react to mouse scrolls
	//they are the same as key W/S
	bool VEEventListenerGLFW::onMouseScroll(veEvent event) {

		float xoffset = event.fdata1;
		float yoffset = event.fdata2;

		VECamera *pCamera = (VECamera*)VEEngine::getEnginePointer()->m_pSceneManager->getEntity(VEEngine::getEnginePointer()->getSceneManager()->getCameraName());
		VEEntity *pParent = pCamera->pEntityParent;
		glm::vec4 translate = 1000 * yoffset * glm::vec4(0.0, 0.0, -1.0, 1.0);

		if (pParent == nullptr) {
			pParent = pCamera;
		}
		else {
			//so far the translation vector was defined in cam local space. But the camera frame of reference 
			//is defined wrt its parent space, so we must transform this vector to parent space
			translate = pCamera->localToParentTransform * translate;	//transform from local camera space to parent space
		}

		//add the new translation vector to the previous one
		glm::vec3 trans = glm::vec3(translate.x, translate.y, translate.z);
		pParent->localToParentTransform = glm::translate(pParent->localToParentTransform, (float)event.dt * trans);

		return false;
	}



}









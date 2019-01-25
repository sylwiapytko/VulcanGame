
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stb_image.h>
#include "tiny_obj_loader.h"
#include <vulkan/vulkan.hpp>
#include "vk_mem_alloc.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <stdexcept>
#include <set>

#include "VHHelper.h"

#include "VEEventListener.h"
#include "VEEngine.h"
#include "VEEntity.h"
#include "VESceneManager.h"
#include "VERenderer.h"
#include "VERendererForward.h"


namespace ve {

	class MyVulkanEngine : public VEEngine {
	protected:

	public:
		MyVulkanEngine() {};
		~MyVulkanEngine() {};
		void loadLevel() {
			
			/*
			VEEntity *e1 = m_pSceneManager->loadOBJ( "The Cube", "models\\test", "cube_t_n_s.obj", "cube.png");
			e1->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			VEEntity *e2 = m_pSceneManager->loadOBJ( "The Cube2", "models\\test", "cube_t_n_s.obj", "crate0\\crate0_diffuse.png");
			e2->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 1.0f ));
			VEEntity *e3 = m_pSceneManager->loadOBJ("The Cube3", "models\\test", "cube_t_n_s.obj", "crate2\\crate2_diffuse.png");
			e3->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 4.0f, 1.0f));

			VEEntity *e5 = m_pSceneManager->loadOBJ("The Chalet", "models", "chalet.obj", "chalet.jpg");
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
			trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -15.0f, 0.0f));
			e5->localToParentTransform = trans * scale;
			*/

			VEEntity *e4 = m_pSceneManager->loadOBJ("The Plane", "models\\test", "plane_t_n_s.obj", "grass.jpg", 200.0f);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1000.0f, 1000.0f, 1.0f));
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			e4->localToParentTransform = trans * scale;

			/*
			
			VEEntity *e6 = m_pSceneManager->loadOBJ("The Bird", "models\\test", "Bird1\\12248_Bird_v1_L2.obj", "Bird1\\12248_Bird_v1_diff.jpg");
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
			trans = glm::translate(glm::mat4(1.0f), glm::vec3(00.0f, -2.0f, 1.0f));
			glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			e6->localToParentTransform = trans *rotate*  scale;
			getSceneManagerPointer()->updateEntityCurrentBoundingBox(e6);
			std::cout << "Duck" << std::endl;
			std::cout << glm::to_string(e6->boundingBox->maxVertex) << std::endl;
			std::cout << glm::to_string(e6->boundingBox->minVertex) << std::endl;
			std::cout << "Duck" << std::endl;
			std::cout << glm::to_string(e6->boundingBox->maxVertexCurr) << std::endl;
			std::cout << glm::to_string(e6->boundingBox->minVertexCurr) << std::endl;
			*/

			//veEntityBoundingBox * e6BoundingBox = e6->pEntityData->boundingBox;
			//e6BoundingBox->maxVertex = e6->localToParentTransform * e6BoundingBox->maxVertex;
			//e6BoundingBox->minVertex = e6->localToParentTransform * e6BoundingBox->minVertex;

			VEEntity *e1 = m_pSceneManager->loadOBJ("The Cube", "models\\test", "cube_t_n_s.obj", "cube.png");
			e1->entityObjectType = "User";
			e1->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f));
			getSceneManagerPointer()->updateEntityCurrentBoundingBox(e1);
			/*			
			std::cout << "cube" << std::endl;
			std::cout << glm::to_string(e1->boundingBox->maxVertex) << std::endl;
			std::cout << glm::to_string(e1->boundingBox->minVertex) << std::endl;
			std::cout << glm::to_string(e1->boundingBox->maxVertexCurr) << std::endl;
			std::cout << glm::to_string(e1->boundingBox->minVertexCurr) << std::endl;
			*/

			//VEEntity *e2 = m_pSceneManager->loadOBJ("The Cube2", "models\\test", "cube_t_n_s.obj", "crate0\\crate0_diffuse.png");
			VEEntity *e2 = m_pSceneManager->loadOBJ("The Cube2", "models\\test", "cube_t_n_s.obj", "cube.png");
			e2->entityObjectType = "Box";
			e2->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 1.0f));
			getSceneManagerPointer()->updateEntityCurrentBoundingBox(e2);


			VEEntity *e3 = m_pSceneManager->loadOBJ("The Cube3", "models\\test", "cube_t_n_s.obj", "cube.png");
			e3->entityObjectType = "Box";
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f));
			trans = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 0.0f, 1.0f));
			e3->localToParentTransform = trans * scale;
			getSceneManagerPointer()->updateEntityCurrentBoundingBox(e3);

			VEEntity *ePineapple = m_pSceneManager->loadOBJ("The Pineapple", "models\\test", "Pineapple\\10200_Pineapple_v1-L2.obj", "Pineapple\\10200_Pineapple.jpg");
			ePineapple->entityObjectType = "Fruit";
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
			trans = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 2.0f, 0.5f));
			ePineapple->localToParentTransform = trans * scale;
			getSceneManagerPointer()->updateEntityCurrentBoundingBox(ePineapple);

			VEEntity *ePineapple2 = m_pSceneManager->loadOBJ("The Pineapple2", "models\\test", "Pineapple\\10200_Pineapple_v1-L2.obj", "Pineapple\\10200_Pineapple.jpg");
			ePineapple2->entityObjectType = "Fruit";
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
			trans = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 4.0f, 0.5f));
			ePineapple2->localToParentTransform = trans * scale;
			getSceneManagerPointer()->updateEntityCurrentBoundingBox(ePineapple2);
			/*
			
			VEEntity *e7 = m_pSceneManager->loadOBJ("The Bird2", "models\\test", "Bird1\\12248_Bird_v1_L2.obj", "Bird1\\12248_Bird_v1_diff.jpg");
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
			trans = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, 4.0f, 1.0f));
			rotate = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			e7->localToParentTransform = trans * rotate*  scale;

			VEEntity *e8 = m_pSceneManager->loadOBJ("The Bird3", "models\\test", "Bird2\\12249_Bird_v1_L2.obj", "Bird2\\12249_Bird_v1_diff.jpg");
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
			trans = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 6.0f, 1.0f));
			rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			e8->localToParentTransform = trans * rotate*  scale;
			*/

		};
	};

	class LightListener : public VEEventListener {
		float time = 0.0f;
	public:
		LightListener() : VEEventListener() {};

		void onFrameStarted(veEvent event) {
			std::set<std::string> &lightnames = getSceneManagerPointer()->getLightnames(); //names of lights in the scene
			
			if (lightnames.size() > 0) {
				std::string l1 = *lightnames.begin();						//first light name
				VEEntity *e1 = getSceneManagerPointer()->getEntity(l1);
				if (e1 != nullptr) {
					VELight *plight = static_cast<VELight*>(e1);			//cast from entity to light
					time += event.dt;
					//rotate the light in parent space around parent y-axis (up)
					plight->localToParentTransform = glm::rotate(glm::mat4(1.0), 0.5f*(float)event.dt, glm::vec3( 0.0f, 1.0f, 0.0f)) * plight->localToParentTransform;

				}
			}
		};
	};

	class BirdListener : public VEEventListener {
	public:
		BirdListener() : VEEventListener() {};

		bool onKeyboard(veEvent event) {
			if (event.idata1 == GLFW_KEY_J) {
				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Cube");
				if (e9 != nullptr) {				
					e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.0f, 0.0f)) * e9->localToParentTransform;;
					return false;
				}
			}
			if (event.idata1 == GLFW_KEY_L) {
				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Cube");
				if (e9 != nullptr) {
					e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, 0.0f, 0.0f)) * e9->localToParentTransform;;
					return false;
				}
			}
			
			if (event.idata1 == GLFW_KEY_I) {
				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Cube");
				if (e9 != nullptr) {
					e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,-0.1f, 0.0f)) * e9->localToParentTransform;;
					return false;
				}
			}
			if (event.idata1 == GLFW_KEY_K) {
				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Cube");
				if (e9 != nullptr) {
					e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.1f, 0.0f)) * e9->localToParentTransform;;
					return false;
				}
			}
			//std::set<std::string> entitiesColided = getSceneManagerPointer()->findUserCollision("The Cube");
			
			
			return false;
		};
	};
	class TestListener : public VEEventListener {
	public:
		TestListener() : VEEventListener() {};

		bool onKeyboard(veEvent event) {
			if (event.idata1 == GLFW_KEY_V) {
				getSceneManagerPointer()->removeEntity("The Cube");
				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Cube");
			}
			if (event.idata1 == GLFW_KEY_B) {
				getSceneManagerPointer()->returnRemovedEntity("The Cube");
				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Cube");				
			}

			if (event.idata1 == GLFW_KEY_N) {
				
			}
			return false;
		};

		void onFrameStarted(veEvent event) {
			std::set<VEEntity*> entitiesColided = getSceneManagerPointer()->findUserCollision("The Cube");
			for (auto entityColided : entitiesColided) {
				if (entityColided->entityObjectType == "Fruit") {
					getSceneManagerPointer()->removeEntity(entityColided->entityName);
					std::cout << "Omnomnom! ";
				}
			}

		};
	};

}

using namespace ve;

int main() {

	MyVulkanEngine mve;
	mve.initEngine();
	mve.loadLevel();

	LightListener * listener = new LightListener();
	mve.registerEventListener("LightListener", listener);

	BirdListener * listener1 = new BirdListener();
	mve.registerEventListener("BirdListener", listener1);

	TestListener * listener2 = new TestListener();
	mve.registerEventListener("TestListener", listener2);

	try {
		mve.run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return 0;
}


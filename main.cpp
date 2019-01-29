
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

			VEEntity *ePlane = m_pSceneManager->loadOBJ("The Plane", "models\\test", "plane_t_n_s.obj", "grass.jpg", 200.0f);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 1.0f));
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			ePlane->localToParentTransform = trans * scale;

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
			
			/*

			VEEntity *eBomb = m_pSceneManager->loadOBJ("The Bomb", "models\\test", "Bomb1\\dinamite.obj", "Bomb1\\D.png");
			//scale = glm::scale(glm::mat4(1.0f), glm::vec3(10.1f, 10.1f, 10.1f));
			trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));
			eBomb->localToParentTransform = trans;// *scale;
			*/


			VEEntity *e1 = m_pSceneManager->loadOBJ("The Cube", "models\\test", "cube_t_n_s.obj", "cube.png");
			e1->entityObjectType = "User";
			e1->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f));
			getSceneManagerPointer()->updateEntityCurrentBoundingBox(e1);

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

			///////FRUITS///////////////////////////////////////////////////////////////////////////////////////////////

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

			VEEntity *eCherry = m_pSceneManager->loadOBJ("The Cherry", "models\\test", "Cherry\\10174_Cherry_v01_l3.obj", "Cherry\\Cherry.jpg");
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
			trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));
			eCherry->localToParentTransform = trans * scale;


			/*
			VEEntity *eCherry = m_pSceneManager->loadOBJ("The Cherry", "models\\test", "Cherry\\10174_Cherry_v01_l3.obj", "Cherry\\Cherry.jpg");
			eCherry->pEntityParent = getSceneManagerPointer()->getEntity("StandardCamera");
			eCherry->entityObjectType = "Fruit";
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.02f, 0.02f, 0.02f));
			trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.7f, 1.22f, -3.4f));
			eCherry->localToParentTransform = trans*scale;
			getSceneManagerPointer()->updateEntityCurrentBoundingBox(eCherry);

			VEEntity *eCherry2 = m_pSceneManager->loadOBJ("The Cherry", "models\\test", "Cherry\\10174_Cherry_v01_l3.obj", "Cherry\\Cherry.jpg");
			eCherry2->pEntityParent = getSceneManagerPointer()->getEntity("StandardCamera");
			eCherry2->entityObjectType = "Fruit";
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.02f, 0.02f, 0.02f));
			trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.9f, 1.22f, -3.4f));
			eCherry2->localToParentTransform = trans * scale;
			getSceneManagerPointer()->updateEntityCurrentBoundingBox(eCherry2);
			
			
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
		void loadBase() {
			VEEntity *ePlane = m_pSceneManager->loadOBJ("The Plane", "models\\test", "plane_t_n_s.obj", "grass.jpg", 200.0f);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 1.0f));
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			ePlane->localToParentTransform = trans * scale;

			///////WALLS///////////////////////////////////////////////////////////////////////////////////////////////
			{
				VEEntity *eWall1 = m_pSceneManager->loadOBJ("The Wall1", "models\\test", "cube_t_n_s.obj", "cube.png");
				eWall1->entityObjectType = "Wall";
				scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 10.0f, 0.5f));
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 1.0f));
				eWall1->localToParentTransform = trans * scale;
				getSceneManagerPointer()->updateEntityCurrentBoundingBox(eWall1);

				VEEntity *eWall2 = m_pSceneManager->loadOBJ("The Wall2", "models\\test", "cube_t_n_s.obj", "cube.png");
				eWall2->entityObjectType = "Wall";
				scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 10.0f, 0.5f));
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5f, 0.0f, 1.0f));
				eWall2->localToParentTransform = trans * scale;
				getSceneManagerPointer()->updateEntityCurrentBoundingBox(eWall2);

				VEEntity *eWall3 = m_pSceneManager->loadOBJ("The Wall3", "models\\test", "cube_t_n_s.obj", "cube.png");
				eWall3->entityObjectType = "Wall";
				scale = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 0.5f, 0.5f));
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 1.0f));
				eWall3->localToParentTransform = trans * scale;
				getSceneManagerPointer()->updateEntityCurrentBoundingBox(eWall3);

				VEEntity *eWall4 = m_pSceneManager->loadOBJ("The Wall4", "models\\test", "cube_t_n_s.obj", "cube.png");
				eWall4->entityObjectType = "Wall";
				scale = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 0.5f, 0.5f));
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.5f, 1.0f));
				eWall4->localToParentTransform = trans * scale;
				getSceneManagerPointer()->updateEntityCurrentBoundingBox(eWall4);
			}
		}
		void loadLevel1() {
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

			VEEntity *e1 = m_pSceneManager->loadOBJ("The Cube", "models\\test", "cube_t_n_s.obj", "cube.png");
			e1->entityObjectType = "User";
			e1->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			getSceneManagerPointer()->updateEntityCurrentBoundingBox(e1);

			///////FRUITS///////////////////////////////////////////////////////////////////////////////////////////////
			{
				VEEntity *ePineapple = m_pSceneManager->loadOBJ("The Pineapple", "models\\test", "Pineapple\\10200_Pineapple_v1-L2.obj", "Pineapple\\10200_Pineapple.jpg");
				ePineapple->entityObjectType = "Fruit";
				scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 2.0f, 0.5f));
				ePineapple->localToParentTransform = trans * scale;
				getSceneManagerPointer()->updateEntityCurrentBoundingBox(ePineapple);

				VEEntity *ePineapple2 = m_pSceneManager->loadOBJ("The Pineapple2", "models\\test", "Pineapple\\10200_Pineapple_v1-L2.obj", "Pineapple\\10200_Pineapple.jpg");
				ePineapple2->entityObjectType = "Fruit";
				scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.5f));
				ePineapple2->localToParentTransform = trans * scale;
				getSceneManagerPointer()->updateEntityCurrentBoundingBox(ePineapple2);

				VEEntity *ePineapple3 = m_pSceneManager->loadOBJ("The Pineapple3", "models\\test", "Pineapple\\10200_Pineapple_v1-L2.obj", "Pineapple\\10200_Pineapple.jpg");
				ePineapple3->entityObjectType = "Fruit";
				scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 0.5f));
				ePineapple3->localToParentTransform = trans * scale;
				getSceneManagerPointer()->updateEntityCurrentBoundingBox(ePineapple3);

				VEEntity *ePineapple4 = m_pSceneManager->loadOBJ("The Pineapple4", "models\\test", "Pineapple\\10200_Pineapple_v1-L2.obj", "Pineapple\\10200_Pineapple.jpg");
				ePineapple4->entityObjectType = "Fruit";
				scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -2.0f, 0.5f));
				ePineapple4->localToParentTransform = trans * scale;
				getSceneManagerPointer()->updateEntityCurrentBoundingBox(ePineapple4);

				VEEntity *ePineapple5 = m_pSceneManager->loadOBJ("The Pineapple5", "models\\test", "Pineapple\\10200_Pineapple_v1-L2.obj", "Pineapple\\10200_Pineapple.jpg");
				ePineapple5->entityObjectType = "Fruit";
				scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -3.0f, 0.5f));
				ePineapple5->localToParentTransform = trans * scale;
				getSceneManagerPointer()->updateEntityCurrentBoundingBox(ePineapple5);
			}

			

		};
		
	};

	class LightListener : public VEEventListener {
		float time = 0.0f;
	public:
		LightListener() : VEEventListener() {};

		void onFrameStarted(veEvent event) {
			/*
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
			*/
			
		};
	};

	class BirdListener : public VEEventListener {
	public:
		BirdListener() : VEEventListener() {};

		bool onKeyboard(veEvent event) {
			if (event.idata1 == GLFW_KEY_J) {
				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Cube");
				if (e9 != nullptr) {
					e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.0f, 0.0f)) * e9->localToParentTransform;
					if (getSceneManagerPointer()->findUserBoxCollision("The Cube")) {
						e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, 0.0f, 0.0f)) * e9->localToParentTransform;
					}
				}
				return false;
			}
			if (event.idata1 == GLFW_KEY_L) {
				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Cube");
				if (e9 != nullptr) {
					e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, 0.0f, 0.0f)) * e9->localToParentTransform;
					if (getSceneManagerPointer()->findUserBoxCollision("The Cube")) {
						e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.0f, 0.0f)) * e9->localToParentTransform;
					}
				}
				return false;
			}
			
			if (event.idata1 == GLFW_KEY_I) {
				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Cube");
				if (e9 != nullptr ) {
					e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, 0.0f)) * e9->localToParentTransform;
					if (getSceneManagerPointer()->findUserBoxCollision("The Cube")) {
						e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.1f, 0.0f)) * e9->localToParentTransform;
					}
				}
				return false;
			}
			if (event.idata1 == GLFW_KEY_K) {
				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Cube");
				if (e9 != nullptr ) {
					e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.1f, 0.0f)) * e9->localToParentTransform;
					if (getSceneManagerPointer()->findUserBoxCollision("The Cube")) {
						e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, 0.0f)) * e9->localToParentTransform;
					}
				}
				return false;
			}
			if (event.idata1 == GLFW_KEY_U) {
				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Cube");
				if (e9 != nullptr) {
					e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.1f)) * e9->localToParentTransform;	
				}
				return false;
			}
			if (event.idata1 == GLFW_KEY_O) {
				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Cube");
				if (e9 != nullptr) {
					e9->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.1f)) * e9->localToParentTransform;
				}
				return false;
			}
			return false;
		};
	};
	class TestListener : public VEEventListener {
		float time = 0.0f;
		bool bomb1 = true;
		bool bomb2 = false;
	public:
		TestListener() : VEEventListener() {};

		
		void loadSuccess() {

			VEEntity *ePineappleReward = getSceneManagerPointer()->loadOBJ("The PineappleReward", "models\\test", "Pineapple\\10200_Pineapple_v1-L2.obj", "Pineapple\\10200_Pineapple.jpg");
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));
			ePineappleReward->localToParentTransform = trans * scale;
		}
		void loadDead() {

			VEEntity *eBombDead = getSceneManagerPointer()->loadOBJ("The BombDead", "models\\test", "Bomb1\\dinamite.obj", "Bomb1\\D.png");
			eBombDead->entityObjectType = "Dead";
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.5f, 2.5f, 2.5f));
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 3.0f));
			eBombDead->localToParentTransform = trans * scale;
		}
		void loadUser() {

			VEEntity *e1 = getSceneManagerPointer()->loadOBJ("The Cube", "models\\test", "cube_t_n_s.obj", "cube.png");
			e1->entityObjectType = "User";
			e1->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			getSceneManagerPointer()->updateEntityCurrentBoundingBox(e1);
		}
		void loadFruits() {
			
			getSceneManagerPointer()->returnRemovedEntity("The Pineapple");
			getSceneManagerPointer()->returnRemovedEntity("The Pineapple2");
			getSceneManagerPointer()->returnRemovedEntity("The Pineapple3");
			getSceneManagerPointer()->returnRemovedEntity("The Pineapple4");
			getSceneManagerPointer()->returnRemovedEntity("The Pineapple5");
			
		}
		void loadBombs() {
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

			VEEntity *eBomb = getSceneManagerPointer()->loadOBJ("The Bomb", "models\\test", "Bomb1\\dinamite.obj", "Bomb1\\D.png");
			eBomb->entityObjectType = "Bomb";
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
			trans = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.0f, 1.0f));
			eBomb->localToParentTransform = trans * scale;
			getSceneManagerPointer()->updateEntityCurrentBoundingBox(eBomb);

			VEEntity *eBomb2 = getSceneManagerPointer()->loadOBJ("The Bomb2", "models\\test", "Bomb1\\dinamite.obj", "Bomb1\\D.png");
			eBomb2->entityObjectType = "Bomb";
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
			trans = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 3.5f, 1.0f));
			eBomb2->localToParentTransform = trans * scale;
			getSceneManagerPointer()->updateEntityCurrentBoundingBox(eBomb2);
		}

		bool onKeyboard(veEvent event) {
			
			if (event.idata1 == GLFW_KEY_V) {
				VEEntity *e9 = getSceneManagerPointer()->getEntity("The Cube");
				std::cout << glm::to_string(e9->getWorldTransform()) << std::endl;
			}
			if (event.idata1 == GLFW_KEY_B) {
				VEEntity *e9 = getSceneManagerPointer()->getEntity("StandardCamera");
				glm::mat4 transobj = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 2.0f, 4.0f));
				std::cout << glm::to_string(glm::inverse(e9->getWorldTransform())*transobj) << std::endl;
			}
			if (event.idata1 == GLFW_KEY_SPACE) {
				if (getSceneManagerPointer()->checkLevelSuccess()) {
					getSceneManagerPointer()->cleanBoard();
					loadFruits();
					loadUser();
					getSceneManagerPointer()->updateLevel();
					if (getSceneManagerPointer()->getLevel() >= 2) {
						std::cout << getSceneManagerPointer()->getLevel();
						loadBombs();
					}

					getSceneManagerPointer()->removeEntity("The PineappleReward");
				}
				if (getSceneManagerPointer()->checkDead()) {
					getSceneManagerPointer()->cleanBoard();
					loadFruits();
					loadUser();
					getSceneManagerPointer()->repeatGame();

					getSceneManagerPointer()->removeEntity("The BombDead");
				}
			}

			return false;
		};

		void onFrameStarted(veEvent event) {
			getSceneManagerPointer()->findUserFoodCollision("The Cube");	

			if (getSceneManagerPointer()->findUserBombCollision("The Cube")) {

				getSceneManagerPointer()->removeEntity("The Cube");
				loadDead();
			}
			if (getSceneManagerPointer()->checkDead()) {
				VEEntity *e1 = getSceneManagerPointer()->getEntity("The BombDead");
				if (e1 != nullptr) {
					time += event.dt;
					e1->localToParentTransform = e1->localToParentTransform *  glm::rotate(glm::mat4(1.0), 0.5f*(float)event.dt, glm::vec3(0.0f, 0.0f, 1.0f));
				}
			}

			if (getSceneManagerPointer()->checkLevelSuccess()) {

				getSceneManagerPointer()->removeEntity("The Cube");				
				loadSuccess();
			}

			if (getSceneManagerPointer()->getLevel() >= 3) {
				VEEntity *eBomb1 = getSceneManagerPointer()->getEntity("The Bomb");
				if (eBomb1 != nullptr) {
					time += event.dt;
					if (getSceneManagerPointer()->findUserBoxCollision("The Bomb")) {
						bomb1 = !bomb1;
					}
					if (bomb1 == true) {
						eBomb1->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.01f, 0.0f)) * eBomb1->localToParentTransform;
					}
					else {
						eBomb1->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.01f, 0.0f)) * eBomb1->localToParentTransform;
					}
				}
				VEEntity *eBomb2 = getSceneManagerPointer()->getEntity("The Bomb2");
				if (eBomb2 != nullptr) {
					time += event.dt;
					if (getSceneManagerPointer()->findUserBoxCollision("The Bomb2")) {
						bomb2 = !bomb2;
					}
					if (bomb2 == true) {
						eBomb2->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.01f, 0.0f)) * eBomb2->localToParentTransform;
					}
					else {
						eBomb2->localToParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.01f, 0.0f)) * eBomb2->localToParentTransform;
					}
				}
			}


		};		
	};

}

using namespace ve;

int main() {

	MyVulkanEngine mve;
	mve.initEngine();
	mve.loadBase();
	mve.loadLevel1();

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


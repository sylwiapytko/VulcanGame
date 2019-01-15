#pragma once

#include <vector>

#define getWindowPointer() VEEngine::getEnginePointer()->getWindow()

namespace ve {
	class VEEngine;
	class VERendererForward;

	class VEWindow {
		friend VEEngine;
		friend VERendererForward;

	protected:
		virtual void		initWindow(int width, int height) {};
		virtual std::vector<const char*> getRequiredInstanceExtensions() { return {}; };
		virtual bool		createSurface(VkInstance instance, VkSurfaceKHR *pSurface ) { return false; };
		virtual VkExtent2D	getExtent() { return VkExtent2D() = { 0,0 }; };
		virtual void		pollEvents() {};
		virtual void		processEvent(veEvent event);
		void				windowSizeChanged();
		virtual void		waitForWindowSizeChange() {};
		virtual bool		windowShouldClose() { return false; };
		virtual void		closeWindow() {};

	public:
		VEWindow() {};
		~VEWindow() {};

	};

}



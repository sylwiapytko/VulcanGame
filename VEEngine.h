#pragma once



namespace ve {

	class VEWindow;
	class VERenderer;
	class VERendererForward;
	class VEEventListener;
	class VEEventListenerGLFW;
	class VESceneManager;

	class VEEngine;
	extern VEEngine* g_pVEEngineSingleton;

	class VEEngine {
		friend VEWindow;
		friend VERenderer;
		friend VERendererForward;
		friend VEEventListener;
		friend VEEventListenerGLFW;
		friend VESceneManager;

	protected:
		VkInstance m_instance = VK_NULL_HANDLE;
		VEWindow * m_pWindow = nullptr;
		VERenderer * m_pRenderer = nullptr;
		VESceneManager * m_pSceneManager = nullptr;
		VkDebugReportCallbackEXT callback;

		std::vector<veEvent> m_eventlist;
		std::map<std::string, VEEventListener*> m_eventListener;		///<registered event listeners
		std::set<int> m_keys_pressed = {};
		std::set<int> m_mouse_buttons_clicked = {};

		double m_dt=0.0, m_time=0.0;
		bool m_framebufferResized = false;
		bool m_end_running = false;
		bool m_debug = true;

		void setupDebugCallback();
		virtual std::vector<const char*> getRequiredInstanceExtensions();
		virtual std::vector<const char*> getValidationLayers();
		void callListeners(double dt, veEvent event);
		void addEvent(veEvent event);
		void processEvents(double dt);
		void processPressedKeys( double dt );
		void processMouseButtons(double dt);
		void windowSizeChanged() { m_framebufferResized = true; };

		//startup routines, can be overloaded to create different managers
		virtual void createWindow();
		virtual void createRenderer();
		virtual void createSceneManager();
		virtual void registerEventListeners();
		virtual void closeEngine();

	public:
		VEEngine() { g_pVEEngineSingleton = this; };
		~VEEngine() {};

		static VEEngine * getEnginePointer() { return g_pVEEngineSingleton; };

		virtual void initEngine();
		virtual void run();
		void registerEventListener(std::string name, VEEventListener *listener) { m_eventListener[name] = listener; };
		void removeEventListener(std::string name) { m_eventListener.erase(name); };

		VEWindow       * getWindow() { return m_pWindow;  };
		VESceneManager * getSceneManager() { return m_pSceneManager;  };
		VERenderer     * getRenderer() { return m_pRenderer;  };
	};

}
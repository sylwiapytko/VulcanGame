#pragma once


namespace ve {
	class VEEngine;

	enum veEventSubsystem {
		VE_EVENT_SUBSYSTEM_GENERIC = 0,
		VE_EVENT_SUBSYSTEM_GLFW = 1
	};

	enum veEventType {
		VE_EVENT_NONE = 0,
		VE_EVENT_FRAME_STARTED = 1,
		VE_EVENT_FRAME_ENDED = 2,
		VE_EVENT_KEYBOARD = 3,
		VE_EVENT_MOUSEMOVE = 4,
		VE_EVENT_MOUSEBUTTON = 5,
		VE_EVENT_MOUSESCROLL = 6
	};

	struct veEvent {
		veEventSubsystem subsystem = VE_EVENT_SUBSYSTEM_GENERIC;
		veEventType type = VE_EVENT_NONE;
		int subtype = 0;
		double dt;
		int   idata1=0, idata2=0, idata3=0, idata4=0;
		float fdata1 = 0.0f, fdata2 = 0.0f, fdata3 = 0.0f, fdata4 = 0.0f;
		void *ptr=nullptr;

		veEvent(veEventType evt) { subsystem = VE_EVENT_SUBSYSTEM_GENERIC;  type = evt; };
		veEvent(veEventSubsystem sub, veEventType evt) { subsystem = sub; type = evt; };
	};

	class VEEngine;

	class VEEventListener {
		friend VEEngine;

	protected:
		virtual bool onEvent(veEvent event);
		virtual void onFrameStarted(veEvent event) {};
		virtual void onFrameEnded(veEvent event) {};
		virtual bool onKeyboard(veEvent event) { return false; };
		virtual bool onMouseMove(veEvent event) { return false; };
		virtual bool onMouseButton(veEvent event) { return false; };
		virtual bool onMouseScroll(veEvent event) { return false; };

	public:
		VEEventListener() {};
		virtual ~VEEventListener() {};
	};
}

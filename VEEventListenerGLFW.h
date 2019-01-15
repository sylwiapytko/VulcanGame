#pragma once

namespace ve {

	class VEEventListenerGLFW : public VEEventListener {

	protected:
		bool  m_usePrevCursorPosition = false;
		float m_cursorPrevX = 0;
		float m_cursorPrevY = 0;

		virtual void onFrameStarted(veEvent event);
		virtual void onFrameEnded(veEvent event);
		virtual bool onKeyboard(veEvent event);
		virtual bool onMouseMove(veEvent event);
		virtual bool onMouseButton(veEvent event);
		virtual bool onMouseScroll(veEvent event);

	public:
		VEEventListenerGLFW() : VEEventListener() { };
		virtual ~VEEventListenerGLFW() {};
	};
}


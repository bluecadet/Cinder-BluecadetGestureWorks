#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "GestureHandler.h"

namespace bluecadet {
namespace gestureworks {
namespace handlers {

	typedef std::shared_ptr<class ManipulateGestureHandler> ManipulateGestureHandlerRef;

	class ManipulateGestureHandler : public GestureHandler {

	public:
		static const char * sGestureId;

		ManipulateGestureHandler(views::TouchViewRef view, bool dragging = true, bool scaling = true, bool rotating = true);
		~ManipulateGestureHandler();

		bool getDraggingEnabled() const { return mDraggingEnabled; }
		void setDraggingEnabled(const bool value) { mDraggingEnabled = value; }

		bool getScalingEnabled() const { return mScalingEnabled; }
		void setScalingEnabled(const bool value) { mScalingEnabled = value; }

		bool getRotatingEnabled() const { return mRotatingEnabled; }
		void setRotatingEnabled(const bool value) { mRotatingEnabled = value; }

		//! Enables dynamic changes to the transform origin, allowing scaling and rotating around the center point of the gesture. Defaults to true.
		bool getDynamicTransformOrigin() const { return mDynamicTransformOrigin; }
		void setDynamicTransformOrigin(const bool value) { mDynamicTransformOrigin = value; }

	protected:
		void handleGestureEvent(const GestureEvent & event) override;

		bool mDraggingEnabled = true;
		bool mScalingEnabled = true;
		bool mRotatingEnabled = true;

		bool mDynamicTransformOrigin = true;

		ci::vec2 mInitialOrigin;

	};

}
}
}

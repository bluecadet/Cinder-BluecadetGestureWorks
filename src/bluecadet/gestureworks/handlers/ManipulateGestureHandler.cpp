#include "ManipulateGestureHandler.h"

#include "ManipulateGestureHandler.h"

#include "bluecadet/core/ScreenLayout.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace gestureworks {
namespace handlers {

const char * ManipulateGestureHandler::sGestureId = "nmanipulate";

ManipulateGestureHandler::ManipulateGestureHandler(views::TouchViewRef view, bool dragging, bool scaling, bool rotating) :
	GestureHandler(view, { sGestureId }, true),
	mDraggingEnabled(dragging),
	mScalingEnabled(scaling),
	mRotatingEnabled(rotating)
{
}

ManipulateGestureHandler::~ManipulateGestureHandler() {
}

void ManipulateGestureHandler::handleGestureEvent(const GestureEvent & event) {
	auto view = getView();

	if (!view) {
		return;
	}

	const auto & gestureInfo = event.gestureInfo;

	if (strcmp(gestureInfo.gesture_id, sGestureId) != 0) {
		// only accept n-manipulate gestures
		return;
	}

	const auto appSize = vec2(bluecadet::core::ScreenLayout::getInstance()->getAppSize());

	if (gestureInfo.phase == 0) {
		if (mDynamicTransformOrigin) {
			mInitialOrigin = view->getTransformOrigin();
			const vec2 gesturePos = vec2(gestureInfo.x, gestureInfo.y) * appSize;
			const vec2 localGesturePos = view->convertGlobalToLocal(gesturePos);
			view->setTransformOrigin(localGesturePos, true);
		}

	} else if (gestureInfo.phase == 1) {

		if (mDraggingEnabled) {
			const float deltaX = gestureInfo.getValue("dx") * appSize.x;
			const float deltaY = gestureInfo.getValue("dy") * appSize.y;
			vec2 position = view->getGlobalPosition();
			position.x += deltaX;
			position.y += deltaY;
			view->setGlobalPosition(position);
		}

		if (mScalingEnabled) {
			// only use component between x/y, otherwise scale gets skewed
			const float deltaScaleX = gestureInfo.getValue("dsx") * appSize.x * 0.01f;
			//const float deltaScaleY = gestureInfo.getValue("dsy") * appSize.y * 0.01f;
			vec2 scale = view->getScale();
			scale.x += deltaScaleX;
			scale.y += deltaScaleX;
			view->setScale(scale);
		}

		if (mRotatingEnabled) {
			const float deltaTheta = toRadians(gestureInfo.getValue("dtheta"));
			float rotation = glm::angle(view->getRotation().value());
			rotation += deltaTheta;
			// keep between -/+PI
			rotation = fmodf(rotation, glm::two_pi<float>());
			if (rotation < 0) rotation += glm::two_pi<float>();
			view->setRotation(rotation);
		}

	} else if (gestureInfo.phase == 2) {
		if (mDynamicTransformOrigin) {
			view->setTransformOrigin(mInitialOrigin, true);
		}
	}
}



}
}
}


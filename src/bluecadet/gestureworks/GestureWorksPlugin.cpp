#include "GestureWorksPlugin.h"
#include "GestureEvent.h"

#include "cinder/Log.h"
#include "bluecadet/views/TouchView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::views;

namespace bluecadet {
namespace gestureworks {

GestureWorksPlugin::GestureWorksPlugin(ci::fs::path optGmlPath) :
	mGmlPath(optGmlPath),
	mGestureWorks(nullptr),
	mAppSize(0),
	mAutoRegistrationEnabled(false),
	mDefaultGestures({"ndrag", "nscale", "nrotate"}) {
}

GestureWorksPlugin::~GestureWorksPlugin() {
}

void GestureWorksPlugin::setup() {
	if (mAppSize.x == 0 || mAppSize.y == 0) {
		// Auto-detect window size if app size is not set yet (usually app size is retrieved from touch mgr)
		mAppSize = getWindowSize();
	}

	if (mGestureWorks) {
		CI_LOG_W("Setup() called on initialized instance. Aborting (call destroy() first if you want to re-initialize).");
		return;
	}

	const bool gmlExists = fs::exists(mGmlPath);

	if (!gmlExists) {
		CI_LOG_E("Could not find GML at '" + mGmlPath.string() + "'");
	}

	// Usually GW works in screen space, but since we're passing in our own events, we want to stick to our app space
	if (!gmlExists || mGmlPath.empty()) {
		mGestureWorks = initializeGestureWorks(mAppSize.x, mAppSize.y);
		CI_LOG_I("Initializedgesture works with default gestures (no custom GML).");

	} else {
		mGestureWorks = initializeGestureWorksGML(mAppSize.x, mAppSize.y, mGmlPath.string().c_str());
		CI_LOG_I("Initialized gesture works with custom gestures (" + mGmlPath.string() + ").");
	}

	if (!mGestureWorks) {
		CI_LOG_E("Could not initialize Gesture Works");
	}
}

void GestureWorksPlugin::destroy() {
	if (!mGestureWorks) {
		return;
	}

	destroyGestureWorks(mGestureWorks);
	mGestureWorks = nullptr;
}

void GestureWorksPlugin::wasAddedTo(touch::TouchManager * manager) {
	destroy();
	mAppSize = manager->getAppSize();
	setup();
}

void GestureWorksPlugin::willBeRemovedFrom(touch::TouchManager * manager) {
	destroy();
}

void GestureWorksPlugin::preUpdate(touch::TouchManager * manager, std::deque<bluecadet::touch::Touch> & touches) {
	const ivec2 appSize = manager->getAppSize();

	if (appSize != mAppSize) {
		// create a new GW instance with the correct app size
		destroy();
		mAppSize = appSize;
		setup();
	}
}

void GestureWorksPlugin::processEvent(touch::TouchManager * manager, const bluecadet::touch::TouchEvent & event) {
	vec2 touchPos = toPixels(event.globalPosition);

	int touchStatus = 0;

	switch (event.touchPhase) {
	case bluecadet::touch::TouchPhase::Began: touchStatus = GW_TOUCHADDED; break;
	case bluecadet::touch::TouchPhase::Moved: touchStatus = GW_TOUCHUPDATE; break;
	case bluecadet::touch::TouchPhase::Ended: touchStatus = GW_TOUCHREMOVED; break;
	}

	addTouchEvent(mGestureWorks, event.touchId, touchStatus, touchPos.x, touchPos.y);

	if (event.touchTarget) {
		const auto touchView = event.touchTarget;
		const auto viewId = event.touchTarget->getViewIdStr();
		bool isRegistered = mViewInfos.find(viewId) != mViewInfos.end();

		if (!isRegistered && mAutoRegistrationEnabled) {
			// add gestures if we don't have any yet
			addGesturesForView(touchView, mDefaultGestures);
			isRegistered = true;
		}

		if (isRegistered && touchStatus == GW_TOUCHADDED && touchView->getNumTouches() == 1) {
			// activate on first touch start
			activateView(viewId);
		}

		if (isRegistered) {
			addTouchPointToObject(mGestureWorks, viewId.c_str(), event.touchId);
		}
	}
}

void GestureWorksPlugin::postUpdate(touch::TouchManager * manager, std::deque<bluecadet::touch::Touch> & touches) {
	processGestures();
	cleanUpActiveViews();
}

void GestureWorksPlugin::addGesturesForView(views::TouchViewRef view, const std::set<std::string> & gestureIds) {
	const auto & viewId = view->getViewIdStr();

	auto & info = mViewInfos[viewId];

	info.view = views::TouchViewWeakRef(view);
	info.registeredGetures.insert(gestureIds.begin(), gestureIds.end());

	//CI_LOG_D("Added " + to_string(gestureIds.size()) + " gestures to view '" + viewId + "'");
}

void GestureWorksPlugin::removeGesturesFromView(views::TouchViewRef view, const std::set<std::string> & gestureIds) {
	const auto infoIt = mViewInfos.find(view->getViewIdStr());

	if (infoIt == mViewInfos.end()) {
		return;
	}

	auto & registeredGetures = infoIt->second.registeredGetures;

	// remove gestures
	for (const auto & gestureId : gestureIds) {
		auto firstResult = registeredGetures.find(gestureId);

		if (firstResult != registeredGetures.end()) {
			registeredGetures.erase(firstResult);
		}
	}

	// remove info object if all gestures are empty
	if (registeredGetures.empty()) {
		mViewInfos.erase(infoIt);
	}

	//CI_LOG_D("Removing view info for view '" + view->getViewIdStr() + "'");
}


void GestureWorksPlugin::processGestures() {
	updateFrame(mGestureWorks);

	GestureInfo * gestures;

	const int numGestureEvents = getGestureEvents(mGestureWorks, &gestures);

	// process gestures
	for (int i = 0; i < numGestureEvents; ++i) {
		const GestureInfo & gesture = gestures[i];
		const string viewId = gesture.target;
		const auto infoIt = mViewInfos.find(viewId);

		TouchViewRef view = nullptr;

		if (infoIt != mViewInfos.end()) {
			view = infoIt->second.view.lock();
		}

		if (view) {
			// process gesture on view
			GestureEvent gestureEvent(gesture, view);
			view->dispatchEvent(gestureEvent);
		}
	}
}

inline void GestureWorksPlugin::cleanUpActiveViews() {
	// deactivate and clean up unused views
	for (auto infoIt = mViewInfos.begin(); infoIt != mViewInfos.end(); ) {
		const auto & info = infoIt->second;

		if (!info.isActive) {
			infoIt++;
			continue;
		}

		const auto & viewId = infoIt->first;
		const auto view = info.view.lock();

		//CI_LOG_D("view '" + viewId + "' num touches: " + to_string(view->getNumTouches()));

		if (!view || view->getNumTouches() <= 0) {
			// deactivate on last touch end
			deactivateView(viewId);
		}

		if (!view) {
			infoIt = mViewInfos.erase(infoIt);
		} else {
			infoIt++;
		}
	}
}

void GestureWorksPlugin::activateView(const std::string & viewId) {
	registerTouchObject(mGestureWorks, viewId.c_str());

	const auto infoIt = mViewInfos.find(viewId);

	if (infoIt == mViewInfos.end()) {
		CI_LOG_W("Trying to activate unregistered view '" + viewId + "'");
	}

	auto & info = infoIt->second;
	info.isActive = true;

	for (const auto & gesture : info.registeredGetures) {
		addGesture(mGestureWorks, viewId.c_str(), gesture.c_str());
	}

	//CI_LOG_D("Activated view '" << viewId << "'");
}

void GestureWorksPlugin::deactivateView(const std::string & viewId) {
	const auto infoIt = mViewInfos.find(viewId);

	if (infoIt == mViewInfos.end()) {
		CI_LOG_W("Trying to deactivate unregistered view '" + viewId + "'");
	} else {
		infoIt->second.isActive = false;
	}

	removeTouchObject(mGestureWorks, viewId.c_str());
	//CI_LOG_D("Deactivated view '" << viewId << "'");
}

} // gestureworks
} // bluecadet


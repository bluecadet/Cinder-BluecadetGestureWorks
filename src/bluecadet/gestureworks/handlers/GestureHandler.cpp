#include "GestureHandler.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::touch;

namespace bluecadet {
namespace gestureworks {
namespace handlers {

	bool GestureHandler::sAutoRegisterPlugin = true;

	GestureHandler::GestureHandler(views::TouchViewRef view, const std::set<std::string> & gestureIds, const bool enableMultitouch) :
		mView(view),
		mGestureIds(gestureIds)
	{

		auto lockedView = mView.lock();

		if (!lockedView) {
			CI_LOG_E("View is empty, so no gesture listener will be added");

		} else {
			auto gesturePlugin = GestureWorksPlugin::getInstance();

			if (sAutoRegisterPlugin && !TouchManager::getInstance()->hasPlugin(gesturePlugin)) {
				TouchManager::getInstance()->addPlugin(gesturePlugin);
			}

			mCallbackConnection = lockedView->addEventCallback(GestureEvent::Type::GESTURE, bind(&GestureHandler::handleViewEvent, this, placeholders::_1));
			gesturePlugin->addGesturesForView(lockedView, mGestureIds);

			if (enableMultitouch) {
				lockedView->setMultiTouchEnabled(true);
			}
		}
	}

	GestureHandler::~GestureHandler() {
		auto lockedView = mView.lock();
		if (lockedView) {
			GestureWorksPlugin::getInstance()->removeGesturesFromView(lockedView, mGestureIds);
		}
		mCallbackConnection.disconnect();
	}

	void GestureHandler::handleViewEvent(views::ViewEvent & event) {
		if (!mEnabled) {
			return;
		}

		if (event.type != bluecadet::gestureworks::GestureEvent::Type::GESTURE) {
			CI_LOG_E("Received gesture event with type '" + event.type + "', which should be '" + GestureEvent::Type::GESTURE + "'");
			return;
		}

		auto gestureEvent = dynamic_cast<GestureEvent *>(&event);

		if (gestureEvent == nullptr) {
			CI_LOG_E("Received gesture event with type '" + event.type + "' that couldn't be cast to bluecadet::gestureworks::GestureEvent");
			return;
		}

		handleGestureEvent(*gestureEvent);
	}

	void GestureHandler::handleGestureEvent(const GestureEvent & event) {
		
	}

}
}
}

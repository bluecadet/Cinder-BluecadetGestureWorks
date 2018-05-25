#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "ideum/gestureworks/GestureWorks2.h"

#include "bluecadet/touch/TouchManager.h"
#include "bluecadet/touch/TouchManagerPlugin.h"
#include "bluecadet/views/TouchView.h"

namespace bluecadet {
namespace gestureworks {

typedef std::shared_ptr<class GestureWorksPlugin> GestureWorksPluginRef;

class GestureWorksPlugin : public touch::TouchManagerPlugin {


public:

	static GestureWorksPluginRef getInstance() {
		static GestureWorksPluginRef instance = nullptr;
		if (!instance) {
			instance = std::make_shared<GestureWorksPlugin>();
		}
		return instance;
	}

	GestureWorksPlugin(ci::fs::path optGmlPath = "");
	~GestureWorksPlugin();

	void setGmlPath(ci::fs::path gmlPath = "") { if (gmlPath == mGmlPath) return; mGmlPath = gmlPath; destroy(); setup(); };
	ci::fs::path getGmlPath() const { return mGmlPath; }

	void wasAddedTo(touch::TouchManager * manager) override;
	void willBeRemovedFrom(touch::TouchManager * manager) override;

	void preUpdate(touch::TouchManager * manager, std::deque<touch::Touch> & touches) override;
	void processEvent(touch::TouchManager * manager, const touch::TouchEvent & event) override;
	void postUpdate(touch::TouchManager * manager, std::deque<touch::Touch> & touches) override;

	//! Registers a view to receive a set of gestures.
	void addGesturesForView(views::TouchViewRef view, const std::set<std::string> & gestureIds);

	//! Unregisters a view for a specific set of gestures.
	void removeGesturesFromView(views::TouchViewRef view, const std::set<std::string> & gestureIds);

	//! Will automatically registered any touched view for default gestures if set to true. Defaults to false.
	bool getAutoRegistrationEnabled() const { return mAutoRegistrationEnabled; }
	void setAutoRegististrationEnabled(const bool value) { mAutoRegistrationEnabled = value; }

	//! The default gestures added to views when auto-regististration is enabled.
	const std::set<std::string> & getDefaultGestures() const { return mDefaultGestures; }
	void setDefaultGestures(const std::set<std::string> value) { mDefaultGestures = value; }

protected:

	struct ViewInfo {
		views::TouchViewWeakRef view;
		std::multiset<std::string> registeredGetures;
		bool isActive = false;
	};

	inline void processGestures();
	inline void cleanUpActiveViews();

	//! Registers a view for custom set of gestures
	void activateView(const std::string & viewId);

	//! Internal helper for efficiently unregistering views
	void deactivateView(const std::string & viewId);

	void setup();
	void destroy();

	std::set<std::string> mDefaultGestures;
	bool mAutoRegistrationEnabled;

	GestureWorks * mGestureWorks = nullptr;
	std::map<std::string, ViewInfo> mViewInfos;

	ci::fs::path mGmlPath;
	ci::ivec2 mAppSize;
};

}
}

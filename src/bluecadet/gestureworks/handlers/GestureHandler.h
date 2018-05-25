#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "bluecadet/gestureworks/GestureEvent.h"
#include "bluecadet/gestureworks/GestureWorksPlugin.h"
#include "bluecadet/views/TouchView.h"

namespace bluecadet {
namespace gestureworks {
namespace handlers {

	typedef std::shared_ptr<class GestureHandler> GestureHandlerRef;

	//! Base gesture handler that does nothing but can be extended to handle gesture events
	class GestureHandler {

	public:

		//! If set to true, this will automatically register the gesture works plugin with the touch manager
		//! as soon as a GestureHandler instance is created if necessary. Defaults to true
		static bool sAutoRegisterPlugin;

		GestureHandler(views::TouchViewRef view, const std::set<std::string> & gestureIds, const bool enableMultitouch = true);
		virtual ~GestureHandler();

		//! Returns the current view, which is pulled from a weak pointer. Make sure to check for nullptr before using this view.
		inline views::TouchViewRef getView() const { return mView.lock(); }

		bool isEnabled() const { return mEnabled; }
		void setEnabled(const bool value) { mEnabled = value; }

	protected:
		virtual void handleGestureEvent(const GestureEvent & event);

	private:
		virtual void handleViewEvent(views::ViewEvent & event);
		
		ci::signals::Connection mCallbackConnection;

		views::TouchViewWeakRef mView;
		std::set<std::string> mGestureIds;

		bool mEnabled = true;

	};
}
}
}

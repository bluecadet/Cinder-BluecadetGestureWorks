#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "ideum/gestureworks/GestureWorks2.h"

#include "bluecadet/views/TouchView.h"
#include "bluecadet/views/ViewEvent.h"

namespace bluecadet {
namespace gestureworks {

struct GestureEvent : public bluecadet::views::ViewEvent {
	// Types
	struct Type {
		static const std::string GESTURE;
	};

	GestureEvent(GestureInfo gestureInfo, bluecadet::views::TouchViewRef target);
	~GestureEvent();

	//! The original GestureWorks event
	GestureInfo gestureInfo;
};

}
}

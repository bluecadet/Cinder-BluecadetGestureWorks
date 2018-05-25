#include "GestureEvent.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace gestureworks {

const string GestureEvent::Type::GESTURE = "gesture";

GestureEvent::GestureEvent(GestureInfo gestureInfo, bluecadet::views::TouchViewRef target) :
	bluecadet::views::ViewEvent(Type::GESTURE, target),
	gestureInfo(gestureInfo)
{
}

GestureEvent::~GestureEvent() {
}

}
}

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Rand.h"

#include "bluecadet/core/BaseApp.h"
#include "bluecadet/views/TouchView.h"

#include "bluecadet/gestureworks/GestureEvent.h"
#include "bluecadet/gestureworks/GestureWorksPlugin.h"
#include "bluecadet/gestureworks/handlers/ManipulateGestureHandler.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;
using namespace bluecadet::gestureworks;

class GestureWorksSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;

	TouchViewRef createTransformableView();
	void makeViewTransformable(TouchViewRef view, BaseViewRef anchor);

	set<handlers::ManipulateGestureHandlerRef> mHandlers;

};

void GestureWorksSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	SettingsManager::getInstance()->setup(settings, "", [=](SettingsManager * manager) {
		manager->mWindowSize = ivec2(1280, 720);
		manager->mFullscreen = false;
		manager->mBorderless = false;
		manager->mConsole = false;
		manager->mShowTouches = true;
		manager->mShowCursor = true;
		manager->mNativeTouchEnabled = true;
		manager->mTuioTouchEnabled = true;
		manager->mMouseEnabled = true;
		manager->mDebugEnabled = true;
	});
}

void GestureWorksSampleApp::setup() {
	BaseApp::setup();

	// configure app
	BaseApp::addTouchSimulatorParams();

	// configure gestures
	fs::path gmlPath = getAssetPath("bluecadet_gestures.gml");
	GestureWorksPlugin::getInstance()->setGmlPath(gmlPath);
	TouchManager::getInstance()->addPlugin(GestureWorksPlugin::getInstance());

	// configure views
	getRootView()->setBackgroundColor(Color::gray(0.5f));
	getRootView()->setSize(ScreenLayout::getInstance()->getAppSize());

	// create views
#ifdef _DEBUG
	for (int i = 0; i < 50; ++i) {
#else
	for (int i = 0; i < 500; ++i) {
#endif
		auto view = createTransformableView();

		view->setScale(vec2(randFloat(0.5, 1.5f)));
		view->setPosition(vec2(randFloat((float)getWindowWidth()), randFloat((float)getWindowHeight())));
		view->setBackgroundColor(hsvToRgb(vec3(randFloat(), 1.0f, 1.0f)));
		view->setRotation(randFloat(glm::two_pi<float>()));

		float alphaA = randFloat(0.5, 1.0);
		float alphaB = randFloat(0.5, 1.0);
		float duration = randFloat(1.0f, 4.0f);
		view->setAlpha(alphaA);
		//view->getTimeline()->apply(&view->getAlpha(), alphaA, alphaB, duration, easeInOutQuad).pingPong().loop().delay(randFloat(duration));

		getRootView()->addChild(view);
	}
}

TouchViewRef GestureWorksSampleApp::createTransformableView() {
	auto view = make_shared<TouchView>();
	view->setSize(vec2(100, 100));
	view->setPosition((vec2(ScreenLayout::getInstance()->getAppSize()) - view->getSize()) * 0.5f);
	view->setTransformOrigin(view->getSize() * 0.5f);
	view->setScale(vec2(2));
	view->setBackgroundColor(Color(0, 1, 1));

	mHandlers.insert(make_shared<handlers::ManipulateGestureHandler>(view));

	/*auto anchor = make_shared<BaseView>();
	anchor->setBackgroundColor(Color(1, 0, 0));
	anchor->setSize(vec2(10));
	anchor->setTransformOrigin(anchor->getSize() * 0.5f);
	anchor->setPosition(view->getTransformOrigin().value() - anchor->getSize() * 0.5f);
	view->addChild(anchor);

	makeViewTransformable(view, anchor);*/

	return view;
}

void GestureWorksSampleApp::makeViewTransformable(TouchViewRef view, BaseViewRef anchor) {

	mHandlers.insert(make_shared<handlers::ManipulateGestureHandler>(view));

	view->addEventCallback(bluecadet::gestureworks::GestureEvent::Type::GESTURE, [=](bluecadet::views::ViewEvent & event) {
		// update anchor
		anchor->setPosition(view->getTransformOrigin().value() - anchor->getSize() * 0.5f);
	});
}

CINDER_APP(GestureWorksSampleApp, RendererGl(RendererGl::Options().msaa(4)), GestureWorksSampleApp::prepareSettings);
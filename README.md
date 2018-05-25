# Cinder-BluecadetGestureWorks

[GestureWorks](http://gestureworks.com/) plugin for Cinder-BluecadetViews block.

## Features

- Integrates GestureWorks gesture handling with BluecadetViews
- Supports optional GML
- Supports use of BluecadetView multitouch simulation (using CTRL and Shift while moving/dragging the mouse cursor)
- Extendable `GestureHandler` class that binds views to gestures
- Flexible `ManipulateGestureHandler` class that supports dragging, scaling and rotating views using the `nmanipulate` gesture provided in the sample GML file
- Supports multiple gesture handlers per view
- Supports multiple gesture handlers that use the same gestures (e.g. two handlers that both use `ndrag`)
- Gesture handlers can be added and removed dynamically while preserving remaining active gesture handlers

## Assets/Includes

- Required header
- **Trial** static libs and dynamic libs for MSW x64/x86 (TinderBox projects are hardcoded to x64 due to limited platform support in TinderBox)
- Sample GML with nmanipulate gesture to drag, scale and rotate

## Licensing

This block only includes the **trial** libraries for [GestureWorks](http://gestureworks.com/) will require a [GestureWorks](http://gestureworks.com/) license for production.

## Example

```c++
// configure gestures in setup()
fs::path gmlPath = getAssetPath("bluecadet_gestures.gml");
GestureWorksPlugin::getInstance()->setGmlPath(gmlPath);
TouchManager::getInstance()->addPlugin(GestureWorksPlugin::getInstance());


// add gesture handlers to views (handlers are not self-retaining, so they have to be stored somewhere to remain in memory and active)
myHandler = make_shared<ManipulateGestureHandler>(myView);

```

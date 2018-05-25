#pragma once

#include <string>

#define GESTUREWORKS_API __declspec(dllexport) 

class GestureWorks;

extern "C"
{
	/*!
		Initializes GestureWorks with a default set of gestures. This returns an instance of GestureWorks to use on future function calls.
	*/
	GESTUREWORKS_API GestureWorks* initializeGestureWorks(int screen_width, int screen_height);

	/*!
		Initializes GestureWorks with default options and a default set of gestures. This is used for screen mode, where gestures are global and not attached to individual touch objects.
	*/
	GESTUREWORKS_API GestureWorks* initializeGestureWorksScreenGestures(int screen_width, int screen_height);

	/*!
		Initializes GestureWorks with a custom gml file.
	*/
	GESTUREWORKS_API GestureWorks* initializeGestureWorksGML(int screen_width, int screen_height, const char *gml_file_path);

	/*!
		Deletes an instance of GestureWorks.
	*/
	GESTUREWORKS_API void destroyGestureWorks(GestureWorks *gesture_works);

	/*!
		Registers a touch object with GestureWorks.
	*/
	GESTUREWORKS_API void registerTouchObject(GestureWorks *gesture_works, const char *name);

	/*
		Removes a touch object from GestureWorks.
	*/
	GESTUREWORKS_API bool removeTouchObject(GestureWorks *gesture_works, const char *name);

	/*!
		Adds a gesture to an object. This passes in the name of the object and the gesture to associate.
	*/
	GESTUREWORKS_API bool addGesture(GestureWorks *gesture_works, const char *object_id, const char *gesture_id);

	const int GW_TOUCHUPDATE = 0;
	const int GW_TOUCHADDED = 1;
	const int GW_TOUCHREMOVED = 2;

	/*!
		Associates a touch event with a TouchObject. The touch_event_id is the point id, x and y are the coordinates for the point,
		and touch_status is one of:
		const int GW_TOUCHUPDATE = 0;
		const int GW_TOUCHADDED = 1;
		const int GW_TOUCHREMOVED = 2;
	*/
	GESTUREWORKS_API void addTouchEvent(GestureWorks *gesture_works, int touch_event_id, int touch_status, float x, float y);

	static const int MAX_POINTS_FRAME = 100;

	struct PointInfo
	{
		int point_id;

		int status;

		float position_x;

		float position_y;

		int timestamp;
	};

	/*!
		Retrieves the current touch events GestureWorks has. These points are retrieved and then addTouchPointToObject is called
		to associate a touch event with a GestureWorks TouchObject.
	*/
	GESTUREWORKS_API int getPointEvents(GestureWorks *gesture_works, PointInfo **points);

	/*!
		Associates a touch event with a TouchObject.
	*/
	GESTUREWORKS_API bool addTouchPointToObject(GestureWorks *gesture_works, const char *object_id, int point_id);

	/*!
		Tests to see if any current TouchObject already has the touch event.
	*/
	GESTUREWORKS_API bool objectHasTouchPoint(GestureWorks *gesture_works, int point_id);

	/*!
		Updates a frame of GestureWorks.
	*/
	GESTUREWORKS_API void updateFrame(GestureWorks *gesture_works);

	/*! 
		Updates a frame of GestureWorks running in screen mode.
	*/
	GESTUREWORKS_API void updateScreenFrame(GestureWorks *gesture_works);

	static const int MAX_GESTURES_FRAME = 100;

	static const int MAX_GESTURE_VALUES = 25;

	static const int MAX_GESTURE_NAME = 100;

	struct GestureInfo
	{
		int id;

		char gesture_type[MAX_GESTURE_NAME];
		char gesture_id[MAX_GESTURE_NAME];
		char target[MAX_GESTURE_NAME];

		int source;
		int n;
		int hold_n;
		float x;
		float y;

		int timestamp;
		int phase;

		struct GestureValue
		{
			char name[MAX_GESTURE_NAME];
			float value;
		};

		GestureValue values[MAX_GESTURE_VALUES];

		int gesture_values_count;

        inline float getValue(const char* name) const
        {
            for(int i = 0; i != gesture_values_count; ++i)
            {
                GestureValue gesture_value = values[i];
                if(strcmp(gesture_value.name, name) == 0)
                {
                    return gesture_value.value;
                }
            }

            return 0.0f;
        }
	};

	/*!
		Retrieves all of the gestures for a frame.
	*/
	GESTUREWORKS_API int getGestureEvents(GestureWorks *gesture_works, GestureInfo **gestures);
}

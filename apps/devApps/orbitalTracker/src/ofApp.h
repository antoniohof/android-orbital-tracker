#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "manager.h"
#include "ofxSunCalc.h"
#include "ofxTweenzor.h"
#include "UI.h"

class ofApp : public ofxAndroidApp{
	
public:
		
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void windowResized(int w, int h);

	void touchDown(int x, int y, int id);
	void touchMoved(int x, int y, int id);
	void touchUp(int x, int y, int id);
	void touchDoubleTap(int x, int y, int id);
	void touchCancelled(int x, int y, int id);
	void swipe(ofxAndroidSwipeDir swipeDir, int id);
	void scale(ofxAndroidScaleEventArgs& scale);

	void pause();
	void stop();
	void resume();
	void reloadTextures();

	bool backPressed();
	void okPressed();
	void cancelPressed();

	//objects manager
	manager Manager;

	//earth
	ofSpherePrimitive earthSphere;
	ofImage colorMap;
	float scaler;
	float rot;
	ofEasyCam cam;

	//background
	ofImage backgroundMap;
	ofSpherePrimitive backgroundSphere;

	//sunlight
	ofLight sun;
	void updateSunPosition();
	unsigned long long sunTimer;
	cSunCoordinates sunCalc;
	ofVec2f sunCoordinates;

	//pinch zoom
	float zoomDistance;
	float previousDistance;

	//UI
	UI interface;
};

#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "manager.h"
#include "ofxSunCalc.h"
#include "ofxDatGui.h"


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
	void scaleBegin(ofxAndroidScaleEventArgs& scale);

	void pause();
	void stop();
	void resume();
	void reloadTextures();

	bool backPressed();
	void okPressed();
	void cancelPressed();

	//satellites and update timer
	manager satelliteManager;

	//background
	ofImage backgroundMap;
	ofSpherePrimitive backgroundSphere;

	//earth
	ofSpherePrimitive earthSphere;
	ofImage colorMap;
	float scaler;
	float rot;
	ofEasyCam cam;

	//sun / light
	ofLight sun;
	void calcSunPosition();
	unsigned long long sunTimer;
	cSunCoordinates sunCalc;
	ofVec2f sunCoordinates;

	//pinch zoom
	float zoomDistance;
	float previousDistance;

	//interface header
	void loadHeader();
	bool headerLoaded;
	ofxDatGui * header;

	//menus
	void loadMenus();
	bool menusLoaded;

	//scroll lists
	ofxDatGuiScrollView* stationsList;

	//scroll buttons
	ofxDatGuiButton* scrollUp;
	ofxDatGuiButton *scrollDown;

	void onDropdownEvent(ofxDatGuiDropdownEvent e);
	void onButtonEvent(ofxDatGuiButtonEvent e);
	void onScrollListEvent(ofxDatGuiScrollViewEvent e);

};

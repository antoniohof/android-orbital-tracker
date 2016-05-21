#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class globals
{
public:
	static void setup();
    static ofxXmlSettings xml;
	static ofRectangle window;
	static bool fullscreen;
	static int fps;
    static vector<string> satellitesIds;
    static int counter;
};


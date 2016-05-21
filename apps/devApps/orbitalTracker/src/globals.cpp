#include "globals.h"

ofxXmlSettings globals::xml;
ofRectangle globals::window;
bool globals::fullscreen;
int globals::fps;
vector<string> globals::satellitesIds;
int globals::counter = 0;

void globals::setup()
{	
	//CONFIG
	if(xml.loadFile("config.xml")){
		xml.pushTag("root");
        
			window.x = xml.getAttribute("window", "x", 0, 0);
			window.y = xml.getAttribute("window", "y", 0, 0);
			window.width = xml.getAttribute("window", "width", 0, 0);
			window.height = xml.getAttribute("window", "height", 0, 0);
			fullscreen = xml.getAttribute("window", "fullscreen", "1", 0) == "1";
			fps = xml.getValue("fps", 60);

		xml.popTag();

	}else{
		ofLogError() << "error loading configuration data ";
	}
}
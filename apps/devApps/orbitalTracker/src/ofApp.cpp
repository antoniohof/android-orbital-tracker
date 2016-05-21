#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofSetWindowTitle("Orbital Tracker");
	ofEnableLighting();
	ofSetFullscreen(true);

	//earth sphere
	rot = 0;
	scaler = 300 / ofx::Geo::GeoUtils::EARTH_RADIUS_KM;
	if(colorMap.load("images/color_map_1024.jpg")) ofLog() << "earth image loaded!"; else ofLogError() << "failed to load earth image!";
	earthSphere.set(ofx::Geo::GeoUtils::EARTH_RADIUS_KM, 36);
	ofQuaternion quat;
	quat.makeRotate(180, 0, 1, 0);
	earthSphere.rotate(quat);
	earthSphere.mapTexCoords(0, colorMap.getTexture().getTextureData().tex_u, colorMap.getTexture().getTextureData().tex_t, 0);

	//background sphere
	if(backgroundMap.load("images/starbackground.png")) ofLog() << "background image loaded!"; else ofLogError() << "failed to load background image!";
	backgroundSphere.set(ofx::Geo::GeoUtils::EARTH_RADIUS_KM*30, 36);
	backgroundSphere.mapTexCoords(0, backgroundMap.getTexture().getTextureData().tex_u, backgroundMap.getTexture().getTextureData().tex_t, 0);

	//cam and 3d settings
	cam.setRotationSensitivity(0.3,0.3,0.3);
    cam.setTranslationSensitivity(0.2,0.2,0.2);
	cam.setPosition(0, 0, 0);
	cam.setFarClip(100000);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);

	//satellites names
	//font.load("fonts/calibri.ttf", 250, true, true);

	//initialize manager
	satelliteManager.setup();

	//zoom
	ofAddListener(ofxAndroidEvents().scaleBegin ,this,&ofApp::scaleBegin);
    previousDistance = 0;
    zoomDistance = 1000;

    //sun
    sun.setup();
    sun.setSpotlight(40, 5);
	sunTimer = ofGetElapsedTimeMillis()+500;

	//interface
    headerLoaded = false;
	menusLoaded = false;
	loadHeader();
}

//--------------------------------------------------------------
void ofApp::update() {

	satelliteManager.update();

	if (ofGetElapsedTimeMillis() > sunTimer) {
        calcSunPosition();
        sunTimer = ofGetElapsedTimeMillis() + 5000;
        if(headerLoaded) {
            if (ofToString(satelliteManager.myCoordenates) == "0,0,0") {
                header->getTextInput("Lat/Long/Alt:")->setText("GPS DISABLED");
            } else header->getTextInput("Lat/Long/Alt:")->setText(ofToString(satelliteManager.myCoordenates));
        }
    }

    if (headerLoaded)header->update();

    if (menusLoaded) {
		stationsList->update();
		scrollUp->update();
		scrollDown->update();
	}else loadMenus();

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);
	ofBackground(0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LEQUAL);
	glDepthMask(true);

    //zoom / distance
    if(zoomDistance < 400) zoomDistance = 400;
    if(zoomDistance > 6000) zoomDistance = 6000;
	cam.setDistance(zoomDistance);

	cam.begin();

	ofPushMatrix();
		//rotate earth
		//rot += 1;
		//ofRotate(rot, 0, 1, 0);
		ofScale(scaler, scaler, scaler);

		sun.enable();
		sun.orbit(sunCoordinates.x, sunCoordinates.y, earthSphere.getRadius(), earthSphere.getGlobalPosition());
		ofSetColor(255);
		colorMap.bind();
			earthSphere.draw();
		colorMap.unbind();
		satelliteManager.draw();
		sun.draw();
		sun.disable();
	ofPopMatrix();

	ofPushMatrix();
		ofScale(scaler, scaler, scaler);
		ofSetColor(255,255,255,180);
		backgroundMap.bind();
			backgroundSphere.draw();
		backgroundMap.unbind();
	ofPopMatrix();

	cam.end();

	glDisable(GL_DEPTH_TEST);
	ofDisableAlphaBlending();
	ofDisableBlendMode();
	ofDisableLighting();

    if(headerLoaded) header->draw();

	if(menusLoaded){
		stationsList->draw();
		scrollUp->draw();
		scrollDown->draw();
	}
}

void ofApp::calcSunPosition(){
    Poco::DateTime now; //UTC
	ofLog() << "updating sun position :";
	ofLog() << now.day() << " " << now.month() << " " << now.year() << " " << now.hour() << " " << now.minute();

	cTime ctime;
	ctime.iYear = now.year();
	ctime.iMonth = now.month();
	ctime.iDay = now.day();
	ctime.dHours = now.hour(); //UTC
	ctime.dMinutes = now.minute();
	ctime.dSeconds = now.second();

	cLocation location;
	location.dLatitude = satelliteManager.myCoordenates.getLatitude();
	location.dLongitude = satelliteManager.myCoordenates.getLongitude();
	ofVec2f data = sunpos(ctime, location, &sunCalc);

    double angle = 15 * (ctime.dHours + ctime.dMinutes/60);  //convert time to angle
    sunCoordinates.x = ofMap(angle, 0, 360, 180, -180);      //map angle to longitude
	sunCoordinates.y =  -data.y *180/PI;	                 //inclination of earth - convert from rad to degrees and invert

    ofLog() << "sun coordinates: longitude " << sunCoordinates.x << " / latitude:" << sunCoordinates.y;
}

void ofApp::scaleBegin(ofxAndroidScaleEventArgs& _distance){
    float distance = _distance.getCurrentSpan();
    if(previousDistance != distance) {
        if (distance > previousDistance) zoomDistance = zoomDistance - 30;
        else zoomDistance = zoomDistance + 30;
    }
    previousDistance = distance;
}


void ofApp::loadHeader() {
	ofLog() << "loading header...";

	header = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);
	header->setTheme(new ofxDatGuiThemeOrbital());
	header->setPosition(2, 2);
	header->setWidth(ofGetWidth());
	header->addHeader("Orbital Tracker", false);
	header->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	header->addTextInput("Lat/Long/Alt:", "INITIALIZING...");

	headerLoaded = true;
	ofLog() << "done!";
}

void ofApp::loadMenus(){
	if(satelliteManager.satellites.TLE.size()>=1) {
		ofLog() << "loading menus...";

		//buttons
		scrollUp = new ofxDatGuiButton("up");
		scrollUp->setTheme(new ofxDatGuiThemeOrbital());
		scrollUp->setWidth(ofGetWidth() / 8);
		scrollUp->setPosition(2, 110);

		scrollDown = new ofxDatGuiButton("down");
		scrollDown->setTheme(new ofxDatGuiThemeOrbital());
		scrollDown->setWidth(ofGetWidth() / 8);
		scrollDown->setPosition(100, 110);
		scrollUp->onButtonEvent(this, &ofApp::onButtonEvent);
		scrollDown->onButtonEvent(this, &ofApp::onButtonEvent);

		stationsList = new ofxDatGuiScrollView("stations", 16);
		stationsList->setTheme(new ofxDatGuiThemeOrbital());
		stationsList->setWidth(ofGetWidth() / 4);
		stationsList->setPosition(2, 200);

		for (int i = 1; i < satelliteManager.satellites.stations.size(); i++) {
			string station = satelliteManager.satellites.stations[i].Name();
			stationsList->add(station);
			if (ofContains(globals::satellitesIds, station)) {
				stationsList->get(station)->setStripeColor(ofColor::blue);
			} else stationsList->get(station)->setStripeColor(ofColor::grey);
		}
		stationsList->onScrollViewEvent(this, &ofApp::onScrollListEvent);
		ofLog() << "done!";

		menusLoaded = true;
	}
}


void ofApp::onButtonEvent(ofxDatGuiButtonEvent e){
    if(e.target->getName() == "up"){
		stationsList->scroll(10);
    }else{
		stationsList->scroll(-10);
    }
}

void ofApp::onScrollListEvent(ofxDatGuiScrollViewEvent e){
	string id = e.target->getName();
	if (ofContains(globals::satellitesIds, id)) {
		stationsList->get(id)->setStripeColor(ofColor::grey);
		satelliteManager.remove(id);
	} else {
		stationsList->get(id)->setStripeColor(ofColor::blue);
		satelliteManager.add(id);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ 
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id){
}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchCancelled(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::swipe(ofxAndroidSwipeDir swipeDir, int id){

}

//--------------------------------------------------------------
void ofApp::pause(){

}

//--------------------------------------------------------------
void ofApp::stop(){
	exit();
}

//--------------------------------------------------------------
void ofApp::resume(){

}

//--------------------------------------------------------------
void ofApp::reloadTextures(){

}

//--------------------------------------------------------------
bool ofApp::backPressed(){
	return false;
}

//--------------------------------------------------------------
void ofApp::okPressed(){

}

//--------------------------------------------------------------
void ofApp::cancelPressed(){

}

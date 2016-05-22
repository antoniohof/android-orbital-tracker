#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofSetWindowTitle("Orbital Tracker");
	ofEnableLighting();
	ofSetFullscreen(true);

	Tweenzor::init();

	//earth sphere
	rot = 0;
	scaler = 300 / ofx::Geo::GeoUtils::EARTH_RADIUS_KM;
	if(colorMap.load("images/color_map_1024.jpg")) ofLogVerbose() << "earth image loaded!"; else ofLogError() << "failed to load earth image!";
	earthSphere.set(ofx::Geo::GeoUtils::EARTH_RADIUS_KM, 36);
	ofQuaternion quat;
	quat.makeRotate(180, 0, 1, 0);
	earthSphere.rotate(quat);
	earthSphere.mapTexCoords(0, colorMap.getTexture().getTextureData().tex_u, colorMap.getTexture().getTextureData().tex_t, 0);

	//background sphere
	if(backgroundMap.load("images/starbackground.png")) ofLogVerbose() << "background image loaded!"; else ofLogError() << "failed to load background image!";
	backgroundSphere.set(ofx::Geo::GeoUtils::EARTH_RADIUS_KM*30, 36);
	backgroundSphere.mapTexCoords(0, backgroundMap.getTexture().getTextureData().tex_u, backgroundMap.getTexture().getTextureData().tex_t, 0);

	//cam and 3d settings
	cam.setRotationSensitivity(0.3,0.3,0.3);
    cam.setTranslationSensitivity(0.2,0.2,0.2);
	cam.setPosition(0, 0, 0);
	cam.setFarClip(100000);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);

	//initialize manager
	Manager.setup();

	//zoom
	ofAddListener(ofxAndroidEvents().scaleBegin ,this,&ofApp::scale);
    previousDistance = 0;
    zoomDistance = 1000;

    //sun
    sun.setup();
    sun.setSpotlight(40, 5);
	sunTimer = ofGetElapsedTimeMillis()+500;

	//UI
	interface.setup(&Manager);
}

//--------------------------------------------------------------
void ofApp::update() {
	Tweenzor::update(ofGetElapsedTimeMillis());
	Manager.update();
	interface.update();

	if (ofGetElapsedTimeMillis() > sunTimer) {
        updateSunPosition();
        sunTimer = ofGetElapsedTimeMillis() + 5000;
    }
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
		//earth
		//rot += 1;
		//ofRotate(rot, 0, 1, 0);
		ofScale(scaler, scaler, scaler);
		sun.enable();
		sun.orbit(sunCoordinates.x, sunCoordinates.y, earthSphere.getRadius(), earthSphere.getGlobalPosition());
		ofSetColor(255);
		colorMap.bind();
			earthSphere.draw();
		colorMap.unbind();
		Manager.draw();
		sun.draw();
		sun.disable();

		//background
		backgroundMap.bind();
			backgroundSphere.draw();
		backgroundMap.unbind();

	ofPopMatrix();

	cam.end();

	glDisable(GL_DEPTH_TEST);
	ofDisableAlphaBlending();
	ofDisableBlendMode();
	ofDisableLighting();


	//UI
	interface.draw();
}

void ofApp::updateSunPosition(){
    Poco::DateTime now; //UTC
	ofLogVerbose() << "updating sun position :";
	ofLogVerbose() << now.day() << " " << now.month() << " " << now.year() << " " << now.hour() << " " << now.minute();

	cTime ctime;
	ctime.iYear = now.year();
	ctime.iMonth = now.month();
	ctime.iDay = now.day();
	ctime.dHours = now.hour(); //UTC
	ctime.dMinutes = now.minute();
	ctime.dSeconds = now.second();

	cLocation location;
	location.dLatitude = Manager.myCoordinates.getLatitude();
	location.dLongitude = Manager.myCoordinates.getLongitude();
	ofVec2f data = sunpos(ctime, location, &sunCalc);

    double angle = 15 * (ctime.dHours + ctime.dMinutes/60);  //convert time to angle
    sunCoordinates.x = ofMap(angle, 0, 360, 180, -180);      //map angle to longitude
	sunCoordinates.y =  -data.y *180/PI;	                 //inclination of earth - convert from rad to degrees and invert

	ofLogVerbose() << "sun coordinates: longitude " << sunCoordinates.x << " / latitude:" << sunCoordinates.y;
}

void ofApp::scale(ofxAndroidScaleEventArgs& _distance){
    float distance = _distance.getCurrentSpan();
    if(previousDistance != distance) {
        if (distance > previousDistance) zoomDistance = zoomDistance - 30;
        else zoomDistance = zoomDistance + 30;
    }
    previousDistance = distance;
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

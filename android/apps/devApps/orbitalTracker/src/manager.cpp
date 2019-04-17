#include "manager.h"


void manager::setup(){
    TLE = new TLEController();
    load();

    //gps
    ofLog() << "initializing GPS...";
    if (gps.startLocation()) ofLog() << "GPS initializesd!"; else ofLogError() << "GPS initialization failure";
    ofAddListener(ofxGPS::newLocationDataEvent, this, &manager::onGPSUpdate);
    myPosition = ofVec3f(0,0,0);
    gpsCalcTimer = ofGetElapsedTimeMillis();

    //update timer
    updateTimer = ofGetElapsedTimeMillis() + 5000;
    TLE->startThread();
}

void manager::update(){
    for(int i = 0; i<objects.size(); i++) objects[i].update();

    //update every 40s except when not connected
    if (!TLE->isThreadRunning() && ofGetElapsedTimeMillis() > updateTimer) {
        bool online = TLE->bServersOnline && ofxAndroidIsOnline();
        int wait = 40000;
        switch (online) {
            case false :
                if(!TLE->bLoaded) ofxAndroidAlertBox("cannot reach TLE servers");
                wait = 10000;
            default :
                TLE->startThread();
                updateTimer = ofGetElapsedTimeMillis() + wait;
                break;
        }
    }
}

void manager::draw(){
    for(int i = 0; i<objects.size(); i++) objects[i].draw();

    //draw my position
    ofSetColor(0,255,0);
    if(ofToString(myCoordinates) != "0,0,0") ofDrawSphere(getMyPosition(), 80);

}

ofVec3f manager::getMyPosition(){
    while(ofGetElapsedTimeMillis() > gpsCalcTimer){
        ofQuaternion latRotation;
        ofQuaternion longRotation;
        latRotation.makeRotate(-float(myCoordinates.getLatitude()), 1, 0, 0);
        longRotation.makeRotate(float(myCoordinates.getLongitude()), 0, 1, 0);
        ofVec3f center = ofVec3f(0,0, float(myCoordinates.getElevation() / 1000 + ofx::Geo::GeoUtils::EARTH_RADIUS_KM));
        myPosition = latRotation * longRotation * center;
        gpsCalcTimer = ofGetElapsedTimeMillis() + 500;
    }
    return myPosition;
}

//register GPS event
void manager::onGPSUpdate(const ofxGPS::LocationData& data) {
    myCoordinates = ofx::Geo::ElevatedCoordinate(data.latitude, data.longitude, data.altitude);
}

void manager::load() {
    ofxXmlSettings xml;
    xml.load("satellites.xml");
    xml.pushTag("root");
    int total = xml.getNumTags("satellite");
    ofLog() << "loading saved satellites : total: " << total;
    for (int i = 0; i < total; i++) {
        int active = xml.getAttribute("satellite", "active", active, i);
        string id = xml.getAttribute("satellite", "id", id, i);
        if(active == 1) {
            add(id);
        }
    }
    xml.popTag();
}

void manager::add(string id){
    if(ofContains(globals::satellitesIds, id)){
        ofLog() << "satellite already loaded: " + id;
    }
    else{
        ofxXmlSettings xml;
        xml.load("satellites.xml");
        xml.pushTag("root");
            int total = xml.getNumTags("satellite");
            bool foundAtXml = false;
            for (int i = 0; i < total; i++) {
                string loadedSat = xml.getAttribute("satellite", "id", id, i);
                if(loadedSat == id) {
                    xml.setAttribute("satellite", "active", "1", i);
                    foundAtXml = true;
                }
            }
            if(!foundAtXml) {
                xml.addTag("satellite");
                xml.setAttribute("satellite", "id", id, total);
                xml.setAttribute("satellite", "active", "1", total);
            }
            globals::satellitesIds.push_back(id);
            object temp;
            temp.setup(id, TLE);
            objects.push_back(temp);
            ofLog() << "satellite loaded: " << id;
        xml.popTag();

        if (xml.save("satellites.xml")) ofLog() << "success writing to XML  " + id; else ofLogError() << "cannot write to XML";
    }
}

void manager::remove(string id){
    ofxXmlSettings xml;
    xml.load("satellites.xml");
    xml.pushTag("root");
    int total = xml.getNumTags("satellite");
    for (int i = 0; i < total; i++) {
        string loadedSatId = xml.getAttribute("satellite", "id", id, i);
        if(loadedSatId == id) {
            xml.setAttribute("satellite", "active", 0, i);
        }
        if(objects[i].id == id) objects.erase(objects.begin() + i);
    }
    globals::satellitesIds.erase(std::remove(globals::satellitesIds.begin(), globals::satellitesIds.end(), id), globals::satellitesIds.end());

    ofLog() << "satellite removed: " << id;
    xml.popTag();

    if (xml.save("satellites.xml")) ofLog() << "success writing to XML  " + id; else ofLogError() << "cannot write to XML";
}
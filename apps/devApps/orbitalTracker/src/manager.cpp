#include "ofMain.h"
#include "manager.h"


void manager::setup(){
    //gps
    ofLog() << "initializing GPS...";
    if (gps.startLocation()) ofLog() << "GPS initialized!"; else ofLogError() << "GPS initialization failure";
    ofAddListener(ofxGPS::newLocationDataEvent, this, &manager::onGPSUpdate);

    //load saved satellites on XML
    loadSaved();

    //update timer
    updateNextTime = ofGetElapsedTimeMillis();
}

void manager::update(){
    //update satellite internet data every 5 minutes - but if not connected try every 1 s
    if (!satellites.isThreadRunning() && ofGetElapsedTimeMillis() > updateNextTime) {
        int total = satellites.TLE.size();
        switch (total){
            case 0:
                updateNextTime = ofGetElapsedTimeMillis() + 1000;
                break;
            default:
                updateNextTime = ofGetElapsedTimeMillis() + 300000;
        }
        satellites.startThread();
    }
}

void manager::draw(){
    ofFill();
    if(!satellites.isThreadRunning()) {
        for (int i = 0; i < globals::satellitesIds.size(); i++) {
            ofVec3f position = satellites.updatePosition()[globals::satellitesIds[i]];
            if (position != ofVec3f(0, 0, 0)) {
                ofSetColor(255, 0, 0);
                ofDrawSphere(position, 80);

                ofSetColor(255);
                ofDrawBitmapString(globals::satellitesIds[i], position);
            }
        }
    }

    //draw my position
    ofSetColor(0,255,0);
    if(ofToString(myCoordenates) != "0,0,0") ofDrawSphere(getMyPosition(), 80);
}


ofVec3f manager::getMyPosition(){
    ofVec3f myPosition;
    ofQuaternion latRotation;
    ofQuaternion longRotation;

    ofVec3f center = ofVec3f(0,0, float(myCoordenates.getElevation() / 1000 + ofx::Geo::GeoUtils::EARTH_RADIUS_KM));

    latRotation.makeRotate(-float(myCoordenates.getLatitude()), 1, 0, 0);
    longRotation.makeRotate(float(myCoordenates.getLongitude()), 0, 1, 0);
    myPosition = latRotation * longRotation * center;

    return myPosition;
}

//register GPS event
void manager::onGPSUpdate(const ofxGPS::LocationData& data) {
    myCoordenates = ofx::Geo::ElevatedCoordinate(data.latitude, data.longitude, data.altitude);
}

void manager::loadSaved() {
    ofxXmlSettings xml;
    xml.load("satellites.xml");
    xml.pushTag("root");
    int total = xml.getNumTags("satellite");
    ofLog() << "loading saved satellites : total: " << total;
    for (int i = 0; i < total; i++) {
        int active = xml.getAttribute("satellite", "active", active, i);
        string id = xml.getAttribute("satellite", "id", id, i);
        if(active == 1) {
            ofLog() << "found active: " << active << " / id: " << id;
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
                    ofLog() << "satellite activated " + id;
                }
            }
            if(!foundAtXml) {
                xml.addTag("satellite");
                xml.setAttribute("satellite", "id", id, total);
                xml.setAttribute("satellite", "active", "1", total);
                ofLog() << "satellite tag created and activated " + id;
            }
            globals::satellitesIds.push_back(id);
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
            ofLog() << "satellite deactivated " + id;
        }
    }
    globals::satellitesIds.erase(std::remove(globals::satellitesIds.begin(), globals::satellitesIds.end(), id), globals::satellitesIds.end());
    ofLog() << "satellite removed: " << id;
    xml.popTag();

    if (xml.save("satellites.xml")) ofLog() << "success writing to XML  " + id; else ofLogError() << "cannot write to XML";
}
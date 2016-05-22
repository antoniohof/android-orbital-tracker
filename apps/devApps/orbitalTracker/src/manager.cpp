#include "manager.h"


void manager::setup(){
    TLE = new TLEController();
    load();

    //gps
    ofLog() << "initializing GPS...";
    if (gps.startLocation()) ofLog() << "GPS initializesd!"; else ofLogError() << "GPS initialization failure";
    ofAddListener(ofxGPS::newLocationDataEvent, this, &manager::onGPSUpdate);

    //update timer
    updateTimer = ofGetElapsedTimeMillis();
}

void manager::update(){

    for(int i = 0; i<objects.size(); i++) objects[i].update();

    //update satellite internet data every 5 minutes - but if not connected try every 1 s
    if (!TLE->isThreadRunning() && ofGetElapsedTimeMillis() > updateTimer) {
        switch (TLE->bLoaded){
            case false:
                updateTimer = ofGetElapsedTimeMillis() + 1000;
                break;
            default:
                updateTimer = ofGetElapsedTimeMillis() + 300000;
        }
        TLE->startThread();
    }
}

void manager::draw(){
    for(int i = 0; i<objects.size(); i++) objects[i].draw();

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
            ofLog() << "satellite deactivated " + id;
        }
        if(objects[i].id == id) objects.erase(objects.begin() + i);
    }
    globals::satellitesIds.erase(std::remove(globals::satellitesIds.begin(), globals::satellitesIds.end(), id), globals::satellitesIds.end());

    ofLog() << "satellite removed: " << id;
    xml.popTag();

    if (xml.save("satellites.xml")) ofLog() << "success writing to XML  " + id; else ofLogError() << "cannot write to XML";
}
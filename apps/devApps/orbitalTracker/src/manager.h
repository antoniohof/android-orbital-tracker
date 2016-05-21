#pragma once
                                      
#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxGPS.h"
#include "TLEController.h"

class manager
{
    public:
        void setup();
        void update();
        void draw();

        //gps
        ofxGPS gps;
        void onGPSUpdate(const ofxGPS::LocationData& data);
        ofx::Geo::ElevatedCoordinate myCoordenates;
        ofVec3f getMyPosition();

        //update internet TLEs timer
        unsigned long long updateNextTime;

        //satellite
        TLEController satellites;
        void loadSaved();
        void add(string id);
        void remove(string id);
};
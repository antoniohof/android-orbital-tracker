#pragma once
                                      
#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxGPS.h"
#include "object.h"

class manager
{
    public:
        void setup();
        void update();
        void draw();

        //gps
        ofxGPS gps;
        void onGPSUpdate(const ofxGPS::LocationData& data);
        ofx::Geo::ElevatedCoordinate myCoordinates;
        ofVec3f getMyPosition();
        ofVec3f myPosition;
        unsigned long long gpsCalcTimer;

        //update TLE from internet
        TLEController * TLE;
        unsigned long long updateTimer;
        int alertCounter;

        //objects
        vector<object> objects;
        void load();
        void add(string id);
        void remove(string id);
};
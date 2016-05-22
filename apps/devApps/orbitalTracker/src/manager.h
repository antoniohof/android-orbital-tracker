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
        ofx::Geo::ElevatedCoordinate myCoordenates;
        ofVec3f getMyPosition();

        //update TLE from internet
        TLEController * TLE;
        unsigned long long updateTimer;

        //objects
        vector<object> objects;
        void load();
        void add(string id);
        void remove(string id);
};
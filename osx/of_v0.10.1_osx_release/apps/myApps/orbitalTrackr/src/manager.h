#pragma once
                                      
#include "ofMain.h"
#ifdef TARGET_ANDROID
#include "ofxAndroid.h"
#include "ofxGPS.h"
#endif
#include "object.h"

class manager
{
    public:
        void setup();
        void update();
        void draw();

#ifdef TARGET_ANDROID
        //gps
        // ofxGPS gps;
        // void onGPSUpdate(const ofxGPS::LocationData& data);
#endif
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

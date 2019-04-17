#include "ofMain.h"
#include "ofxSatellite.h"
#include "globals.h"

class TLEController : public ofThread{

public:

    TLEController() {
        ofLog() << "TLE controller initialized!";
    }

    vector<ofx::Satellite::Satellite> data;
    vector<ofx::Satellite::Satellite> stations;
    vector<ofx::Satellite::Satellite> visuals;
    vector<ofx::Satellite::Satellite> gps;

    //map container of position on globe and coordinate
    map<string, pair<ofVec3f, ofVec3f>> info;
    bool bLoaded = false;
    bool bServersOnline = false;
    unsigned long long updateTimer = ofGetElapsedTimeMillis();

    map<string, pair<ofVec3f, ofVec3f>> update() {
        while(!isThreadRunning() && ofGetElapsedTimeMillis() > updateTimer) {
            for (int i = 0; i < data.size(); i++) {
                if(ofContains(globals::satellitesIds, data[i].Name())){
                    //calculation to resolve satellite location on globe
                    ofVec3f position;
                    double longitude;
                    double latitude;
                    double elevation;
                    ofQuaternion latitudeRotation;
                    ofQuaternion longitudeRotation;
                    ofx::Geo::ElevatedCoordinate coordinates;
                    Poco::DateTime timeNow;

                    coordinates = ofx::Satellite::Utils::toElevatedCoordinate(data[i].find(timeNow).ToGeodetic());

                    latitude = coordinates.getLatitude();
                    longitude = coordinates.getLongitude();
                    elevation = coordinates.getElevation();

                    latitudeRotation.makeRotate(float(-latitude), 1, 0, 0);
                    longitudeRotation.makeRotate(float(longitude), 0, 1, 0);

                    ofVec3f center = ofVec3f(0, 0, float(elevation / 1000 + ofx::Geo::GeoUtils::EARTH_RADIUS_KM));

                    position = latitudeRotation * longitudeRotation * center;
                    ofVec3f coord = ofVec3f(longitude, latitude, elevation);
                    info[data[i].Name()] = make_pair(position, coord);
                }
            }
            updateTimer = ofGetElapsedTimeMillis() + 500;
        }
        return info;
    }


   void threadedFunction() {
       while(isThreadRunning()) {
           ofLog() << "starting TLE thread...";
           lock();

           //visuals
           ofHttpResponse responseVisuals;
           responseVisuals = ofLoadURL("http://www.celestrak.com/NORAD/elements/visual.txt");
           if(200 == responseVisuals.status) visuals = ofx::Satellite::Utils::loadTLEFromBuffer(responseVisuals.data);

           //stations
           ofHttpResponse responseStations;
           responseStations = ofLoadURL("http://www.celestrak.com/NORAD/elements/stations.txt");
           if(200 == responseStations.status) stations = ofx::Satellite::Utils::loadTLEFromBuffer(responseStations.data);

           //gps
           ofHttpResponse responseGps;
           responseGps = ofLoadURL("http://www.celestrak.com/NORAD/elements/gps-ops.txt");
           if(200 == responseGps.status) gps =  ofx::Satellite::Utils::loadTLEFromBuffer(responseGps.data);

           //ofHttpResponse planetLabs;
           //planetLabs = ofLoadURL("http://ephemerides.planet-labs.com/planet_mc.tle");

           //TLE
           ofBuffer TLElist;
           TLElist = responseStations.data;
           TLElist.append(responseGps.data);
           TLElist.append(responseVisuals.data);
           //TLEData.append(planetLabs.data);

           if (TLElist.size()>0 && 200 == responseGps.status && 200 == responseStations.status && 200 == responseVisuals.status) {
               ofLog() << "TLE servers online and up to date!";
               bLoaded = true;
               bServersOnline = true;
               data = ofx::Satellite::Utils::loadTLEFromBuffer(TLElist);
           }
           else {
               ofLogError() << "TLE servers offline";
               bServersOnline = false;
           }
           unlock();
           stopThread();
       }
    }
};
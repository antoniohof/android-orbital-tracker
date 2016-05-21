#include "ofMain.h"
#include "ofxSatellite.h"
#include "globals.h"

class TLEController : public ofThread{

public:

    vector<ofx::Satellite::Satellite> TLE;
    vector<ofx::Satellite::Satellite> stations;
    vector<ofx::Satellite::Satellite> visuals;
    vector<ofx::Satellite::Satellite> gps;
    map<string, ofVec3f> sat;

    TLEController(){
        ofLog() << "TLE controller initialized!";
    }

    map<string, ofVec3f> updatePosition() {
        sat.clear();
        while(!isThreadRunning()) {
            for (int i = 0; i < TLE.size(); i++) {
                if(ofContains(globals::satellitesIds, TLE[i].Name())){
                    //calculation to resolve satellite location on globe
                    ofVec3f position;

                    ofQuaternion latRot;
                    ofQuaternion longRot;
                    ofx::Geo::ElevatedCoordinate coordenates;
                    Poco::DateTime timeNow;

                    coordenates = ofx::Satellite::Utils::toElevatedCoordinate(
                            TLE[i].find(timeNow).ToGeodetic());

                    latRot.makeRotate(float(-coordenates.getLatitude()), 1, 0, 0);
                    longRot.makeRotate(float(coordenates.getLongitude()), 0, 1, 0);

                    ofVec3f center = ofVec3f(0, 0, float(coordenates.getElevation() / 1000 +
                                                         ofx::Geo::GeoUtils::EARTH_RADIUS_KM));

                    position = latRot * longRot * center;

                    sat[TLE[i].Name()] = position;
                }
            }
            return sat;
        }
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
           ofBuffer allTLE;
           allTLE = responseStations.data;
           allTLE.append(responseGps.data);
           allTLE.append(responseVisuals.data);
           //TLEData.append(planetLabs.data);

           if (allTLE.size()>1) {
               ofLog() << "TLE servers online and up to date!";
               TLE = ofx::Satellite::Utils::loadTLEFromBuffer(allTLE);
           }
           else {
               ofLogError() << "TLE servers error";
           }
           unlock();
           stopThread();
       }
    }
};
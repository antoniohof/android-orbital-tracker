//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#include "ofApp.h"


void ofApp::setup()
{
    london = ofxGeo::Coordinate(51.5085300, -0.1257400);
    tokyo = ofxGeo::Coordinate(35.6148800, 139.5813000);

    distanceSpherical = ofxGeo::GeoUtils::distanceSpherical(london, tokyo);
    distanceHaversine = ofxGeo::GeoUtils::distanceHaversine(london, tokyo);
    bearingHaversine = ofxGeo::GeoUtils::bearingHaversine(london, tokyo);
    midpoint = ofxGeo::GeoUtils::midpoint(london, tokyo);
}


void ofApp::draw()
{
    ofBackground(0);

    std::stringstream ss;

    ss << "From: London (" << london << "), UTM [" << ofxGeo::GeoUtils::toUTM(london) << "]" << std::endl;
    ss << "  To: Tokyo  (" << tokyo  << "), UTM [" << ofxGeo::GeoUtils::toUTM(tokyo)  << "]" << std::endl;
    ss << "    Distance Spherical: " << distanceSpherical << " km" << std::endl;
    ss << "    Distance Haversine: " << distanceHaversine << " km" << std::endl;
    ss << "     Bearing Haversine: " << bearingHaversine << " degrees" << std::endl;
    ss << "              Midpoint: (" << midpoint << ")" << std::endl;

    ofDrawBitmapString(ss.str(), ofVec2f(8, 14));
}

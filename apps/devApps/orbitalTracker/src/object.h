#pragma once
                                      
#include "ofMain.h"
#include "ofxAndroid.h"
#include "TLEController.h"
#include "ofxTweenzor.h"

class object
{

public:
    void setup(string id, TLEController * TLE);
    void update();
    void draw();

    ofVec3f position;
    ofVec3f coordinates;
    string id;

private:
    TLEController * TLE;

    float size;
    float nameAlpha;
    bool animationStarted;
    void onCompleteAnimation(float* arg);
};
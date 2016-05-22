#include "object.h"

void object::setup(string id, TLEController * TLE){
    this->id = id;
    this->TLE = TLE;

    position = ofVec3f(0,0,0);
    coordinates = ofVec3f(0,0,0);

    size = 0;
    nameAlpha = 0;
    animationStarted = false;
}
void object::update(){
    if(!TLE->isThreadRunning() && TLE->bLoaded) {
        position = TLE->update()[id].first;
        coordinates = TLE->update()[id].second;
    }

    if(!animationStarted && position != ofVec3f(0,0,0)) {
        Tweenzor::add(&size, 0.f, 140.f, 0.3, 1.5f, EASE_OUT_ELASTIC);
        Tweenzor::addCompleteListener(Tweenzor::getTween(&size), this, &object::onCompleteAnimation);
        animationStarted = true;
    }
}

void object::draw(){
    ofFill();
    if (position != ofVec3f(0, 0, 0)) {
        ofSetColor(255, 0, 0);
        ofDrawSphere(position, size);
        ofDisableLighting();
        ofSetColor(255, nameAlpha);
        //ofDrawBitmapString(id, position);
        ofDrawBitmapString(coordinates, position);

        ofEnableLighting();
    }
}

void object::onCompleteAnimation(float* arg){
    Tweenzor::add(&nameAlpha, 0.f, 255.f, 0.3, 1.5f, EASE_OUT_QUAD);
}

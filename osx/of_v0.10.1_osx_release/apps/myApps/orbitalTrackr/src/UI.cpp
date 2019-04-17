#include "UI.h"

void UI::setup(manager * Manager){
    this->Manager = Manager;

    //interface
    bHeaderLoaded = false;
    bMenuLoaded = false;
    loadHeader();

    #ifdef TARGET_ANDROID
    //warning
    ofxAndroidProgressBox("loading objects...");
    #endif
}
void UI::update() {
    if(bHeaderLoaded) {
        if (ofToString(Manager->myCoordinates) == "0,0,0") {
            header->getTextInput("Lat/Long/Alt:")->setText("GPS DISABLED");
        } else header->getTextInput("Lat/Long/Alt:")->setText(ofToString(Manager->myCoordinates));
    }

    if (bHeaderLoaded)header->update();

    if (bMenuLoaded) {
        stationsList->update();
        scrollUp->update();
        scrollDown->update();
    }else loadMenus();
}

void UI::draw(){
    if(bHeaderLoaded) header->draw();

    if(bMenuLoaded){
        stationsList->draw();
        scrollUp->draw();
        scrollDown->draw();
    }
}

void UI::loadHeader() {
    ofLog() << "loading header...";

    header = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);
    header->setTheme(new ofxDatGuiThemeMidnight());
    header->setPosition(2, 2);
    header->setWidth(ofGetWidth());
    header->addHeader("Orbital Tracker", false);
    header->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    header->addTextInput("Lat/Long/Alt:", "INITIALIZING...");

    bHeaderLoaded = true;
    ofLog() << "done!";
}

void UI::loadMenus(){
    if(Manager->TLE->bLoaded) {
        ofLog() << "loading menus...";

        //buttons
        scrollUp = new ofxDatGuiButton("up");
        scrollUp->setTheme(new ofxDatGuiThemeMidnight());
        scrollUp->setWidth(ofGetWidth() / 6);
        scrollUp->setPosition(2, 170);

        scrollDown = new ofxDatGuiButton("down");
        scrollDown->setTheme(new ofxDatGuiThemeMidnight());
        scrollDown->setWidth(ofGetWidth() / 6);
        scrollDown->setPosition(150, 170);
        scrollUp->onButtonEvent(this, &UI::onButtonEvent);
        scrollDown->onButtonEvent(this, &UI::onButtonEvent);

        stationsList = new ofxDatGuiScrollView("stations", 10);
        stationsList->setTheme(new ofxDatGuiThemeMidnight());
        stationsList->setWidth(ofGetWidth() / 3);
        stationsList->setPosition(2, 300);

        for (int i = 1; i < Manager->TLE->stations.size(); i++) {
            string station = Manager->TLE->stations[i].Name();
            stationsList->add(station);
            if (ofContains(globals::satellitesIds, station)) {
                stationsList->getItemByName(station)->setStripeColor(ofColor::blue);
            } else stationsList->getItemByName(station)->setStripeColor(ofColor::grey);
        }
        stationsList->onScrollViewEvent(this, &UI::onScrollListEvent);
        ofLog() << "done!";

        bMenuLoaded = true;
#ifdef TARGET_ANDROID
        ofxAndroidDismissProgressBox(0);
#endif
    }
}


void UI::onButtonEvent(ofxDatGuiButtonEvent e){
    if(e.target->getName() == "up"){
        stationsList->move(-10, 0);
    }else{
        stationsList->move(0, 10);
    }
}

void UI::onScrollListEvent(ofxDatGuiScrollViewEvent e){
    string id = e.target->getName();
    if (ofContains(globals::satellitesIds, id)) {
        stationsList->getItemByName(id)->setStripeColor(ofColor::grey);
        Manager->remove(id);
    } else {
        stationsList->getItemByName(id)->setStripeColor(ofColor::blue);
        Manager->add(id);
    }
}


void UI::onComplete(float* arg){
    //Tweenzor::add(&nameAlpha, 0.f, 255.f, 0.3, 1.5f, EASE_OUT_QUAD);
}

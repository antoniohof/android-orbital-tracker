#pragma once
                                      
#include "ofMain.h"
// #include "ofxAndroid.h"
#include "ofxDatGui.h"
#include "manager.h"

class UI
{

public:
    void setup(manager * Manager);
    void update();
    void draw();


private:
    manager * Manager;


    //interface header
    void loadHeader();
    bool bHeaderLoaded;
    ofxDatGui * header;

    //menus
    void loadMenus();
    bool bMenuLoaded;

    //scroll lists
    ofxDatGuiScrollView* stationsList;

    //scroll buttons
    ofxDatGuiButton* scrollUp;
    ofxDatGuiButton *scrollDown;

    void onDropdownEvent(ofxDatGuiDropdownEvent e);
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void onScrollListEvent(ofxDatGuiScrollViewEvent e);


    void onComplete(float* arg);
};

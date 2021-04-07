#pragma once

#include "ofMain.h"

#include "ofxMicroUI.h"
#include "ofxMicroUIRemote.h"

#include "ofxiOS.h"

class ofApp : public ofxiOSApp{
	public:
		void setup();
		void draw();
    
	ofxMicroUI u;
    ofxMicroUISoftware soft = ofxMicroUISoftware(&u, "OscRemote");
	ofxMicroUIRemote uiRemote = ofxMicroUIRemote(&u, "remote", "_osc.txt");

    void exit();
    void touchDown(ofTouchEventArgs & touch);
    void touchMoved(ofTouchEventArgs & touch);
    void touchUp(ofTouchEventArgs & touch);
    void touchDoubleTap(ofTouchEventArgs & touch);
    void touchCancelled(ofTouchEventArgs & touch);

    void lostFocus();
    void gotFocus();
    void gotMemoryWarning();
    void deviceOrientationChanged(int newOrientation);
};

#include "ofApp.h"

void ofApp::setup(){
    ofSetOrientation(OF_ORIENTATION_90_RIGHT);
}

void ofApp::draw(){
	u.drawAlerts();
}

void ofApp::exit(){}

void ofApp::touchDown(ofTouchEventArgs & touch){}

void ofApp::touchMoved(ofTouchEventArgs & touch){}

void ofApp::touchUp(ofTouchEventArgs & touch){}

void ofApp::touchDoubleTap(ofTouchEventArgs & touch){}

void ofApp::touchCancelled(ofTouchEventArgs & touch){}

void ofApp::lostFocus(){}

void ofApp::gotFocus(){}

void ofApp::gotMemoryWarning(){}

void ofApp::deviceOrientationChanged(int newOrientation){
//    if(newOrientation==OF_ORIENTATION_90_LEFT){
//        ofSetOrientation(OF_ORIENTATION_90_RIGHT);
//    }
//    else if(newOrientation==OF_ORIENTATION_90_RIGHT){
//        ofSetOrientation(OF_ORIENTATION_90_LEFT);
//    }
}

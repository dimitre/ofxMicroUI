#include "ofApp.h"

void ofApp::setup(){
	ofSetCircleResolution(48);

//	uiRemote.sendOnLoadPreset = false;
//	uiRemote.loadConfig("_osc.txt");
	uiRemote.oscInfo = ((ofxMicroUI::inspector*)u.getElement("oscInfo"));//()
}

void ofApp::update(){
}

void ofApp::draw(){

}

void ofApp::keyPressed(int key){
}

void ofApp::keyReleased(int key){
}

#include "ofApp.h"

void ofApp::setup(){
    // if needed to invoke a function on beat
//    beat.bpm.invokeBeat = std::bind(&ofApp::beating, this);
}

void ofApp::update() {
    beat.update();
}

void ofApp::draw(){
	fbo->begin();
//	ofClear(0,255);
    ofSetColor(0,25);
    ofDrawRectangle(0,0,fbo->getWidth(), fbo->getHeight());
    ofSetColor(255);
    float x = beat.getShape() * fbo->getWidth();
    ofDrawRectangle(x, 0, 30, fbo->getHeight());
    if (beat.isBeat()) {
        cout << "BEAT:" << beat.seconds << endl;
        ofSetColor(255, 0, 80);
        ofDrawRectangle(0, 0, fbo->getWidth(), fbo->getHeight());
    }
	fbo->end();
	soft.drawFbo();
}

void ofApp::keyPressed(int key){
    if (key == ' ') {
        beat.tap();
    }
}

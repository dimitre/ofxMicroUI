#include "ofApp.h"

void ofApp::setup(){
	ofSetCircleResolution(48);
}

void ofApp::update(){}

void ofApp::draw(){
	ofBackground(40);
	fbo->begin();
	ofClear(0,255);
	if (scene == "circles") {
		for (int a=1; a<4; a++) {
			string id = ofToString(a);
			if (uiC->pBool["circle" + id]) {
				ofSetColor(uiC->pColor["cor" + id]);
				ofDrawCircle(
					uiC->pFloat["x" + id],
					uiC->pFloat["y" + id],
					uiC->pFloat["radius" + id]);
			}
		}
	}
	
	else if (scene == "lines") {
		for (int a=0; a<100; a++) {
			float x = ofRandom(0,fbo->getWidth());
			float y = ofRandom(0, fbo->getHeight());
			ofDrawLine(0,0,x,y);
		}
	}
	fbo->end();
	soft.drawFbo();
}

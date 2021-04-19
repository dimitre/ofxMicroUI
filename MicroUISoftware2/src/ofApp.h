#pragma once

#include "ofMain.h"
#include "ofxMicroUI.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
	
	ofxMicroUI u = ofxMicroUI("u.txt");
    ofxMicroUISoftware soft = ofxMicroUISoftware(&u, "test");

	ofxMicroUI * ui = &u.uis["ui"];
	ofxMicroUI * uiC = &u.uis["scene"];
	string & scene = ui->pString["scene"];

	void uiEvents(ofxMicroUI::element & e);
	ofFbo * fbo = &soft.fbo;
};

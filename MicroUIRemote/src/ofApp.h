#pragma once

#include "ofMain.h"
#include "ofxMicroUI.h"
#include "ofxMicroUIRemote.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
	
//	ofxMicroUI u = ofxMicroUI("u.txt");
	ofxMicroUI u;
	ofxMicroUI * ui = &u.uis["ui"];
	ofxMicroUI * uiC = &u.uis["scene"];
	string & scene = ui->pString["scene"];

//	ofxMicroUISoftware soft = ofxMicroUISoftware(&u);
	ofxMicroUISoftware soft = ofxMicroUISoftware(&u, "Remote");

	ofFbo * fbo = &soft.fbo;
	ofxMicroUIRemote uiRemote = ofxMicroUIRemote(&u, "Remote", "_osc.txt");

};

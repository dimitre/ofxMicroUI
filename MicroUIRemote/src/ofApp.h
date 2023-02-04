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
	
	ofxMicroUI u { "u.txt" };
	ofxMicroUI * ui = &u.uis["ui"];
	ofxMicroUI * uiC = &u.uis["scene"];
	string & scene = ui->pString["scene"];
	ofFbo * fbo = &soft.fbo;

	ofxMicroUISoftware soft  { &u, "Remote" };
	ofxMicroUIRemote uiRemote { &u, "Remote", "_osc.txt" };
};

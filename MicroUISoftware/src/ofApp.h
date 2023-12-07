#pragma once

#include "ofMain.h"
#include "ofxMicroUI.h"
#include "ofxMicroUISoftware.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
	
	ofxMicroUI u;
	ofxMicroUI * ui { &u.uis["ui"] };
	ofxMicroUI * uiC { &u.uis["scene"] };
	string & scene { ui->pString["scene"] };

	ofxMicroUISoftware soft { &u, "test" };
	ofFbo * fbo = &soft.fbos[0];
};

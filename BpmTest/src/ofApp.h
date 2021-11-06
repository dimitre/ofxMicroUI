#pragma once

#include "ofMain.h"
#include "ofxMicroUI.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		void keyPressed(int key);

	ofxMicroUI u = ofxMicroUI("u.txt");
	ofxMicroUISoftware soft = ofxMicroUISoftware(&u, "Empty");

	ofxMicroUI * ui = &u.uis["ui"];
	ofxMicroUI * uiC = &u.uis["scene"];
	string & scene = ui->pString["scene"];
	ofFbo * fbo = &soft.fbo;

	#include "feature.h"
	#include "featureBpm.h"

	featureBpm beat = featureBpm(&soft, &u.uis["bpm"]);
	
//    void beating() {
//    }
};

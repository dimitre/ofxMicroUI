#pragma once

#include "ofMain.h"
#include "ofxMicroUI.h"
#include "ofxMicroUISoftware.h"

class ofApp : public ofBaseApp{
public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyPressed(int key) override;



	ofxMicroUI u { "u.txt" };
	ofxMicroUI * ui = &u.uis["ui"];
	ofxMicroUI * uiC = &u.uis["scene"];
	ofxMicroUI * uiColors = &u.uis["colors"];
	string & scene = ui->pString["scene"];

	void uiEvents(ofxMicroUI::element & e);
	ofxMicroUISoftware soft { &u, "name" };
	ofFbo * fbo = &soft.fbos[0];

};

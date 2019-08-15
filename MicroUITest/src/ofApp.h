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
	
	
	float map(float in, float inMin, float inMax, float outMin, float outMax, float shaper){
		// (1) convert to pct (0-1)
		float pct = ofMap (in, inMin, inMax, 0, 1, true);
		// raise this number to a power
		pct = powf(pct, shaper);
		float out = ofMap(pct, 0,1, outMin, outMax, true);
		return out;
	}

	
	ofFbo fbo;
	ofVideoGrabber cam;

	ofxMicroUI m;
	
	//void uiEvents(ofxMicroUI::element & e);
	
	//--------------------------------------------------------------
	void uiEvents(ofxMicroUI::microUIEventObject & e) {
		cout << e.name << endl;
	}

	

};

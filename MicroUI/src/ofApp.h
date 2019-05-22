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
	
	ofxMicroUI m;
};

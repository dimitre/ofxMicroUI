#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1200, 800, OF_WINDOW);			// <-------- setup the GL context
	ofRunApp(new ofApp());
}

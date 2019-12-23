/* 

placeholder 

*/


#pragma once

#if defined( TARGET_OF_IPHONE ) || defined( TARGET_OF_IOS ) || defined( TARGET_ANDROID )
#define MICROUI_TARGET_TOUCH
#endif

#include "ofEvents.h"
#include "ofxMicroUI.h"
#include "ofxOsc.h"

#ifdef MICROUI_TARGET_TOUCH
#include "ofxAccelerometer.h"
#endif

class ofxMicroUIRemote : public ofBaseApp {
public:
	ofxOscSender 	send;
	ofxOscReceiver 	receive;

	string 	serverHostname = "";
	int 	serverPort = 8000;
	string 	remoteHostname = "";
	int 	remotePort = 9000;

	
	ofxMicroUIRemote() {
		//init();
		ofAddListener(ofEvents().draw, this, &ofxMicroUIRemote::onDraw);
		ofAddListener(ofEvents().update, this, &ofxMicroUIRemote::onUpdate);

	}
	
	//--------------------------------------------------------------
	void onDraw(ofEventArgs &data) { draw(); }

	//--------------------------------------------------------------
	void onUpdate(ofEventArgs &data) { update(); }

		
	~ofxMicroUIRemote() {}
};

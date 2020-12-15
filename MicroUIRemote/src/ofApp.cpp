#include "ofApp.h"



void ofApp::setup(){
	u.createFromText("u.txt");
	ofAddListener(u.uiEvent, this, &ofApp::uiEvents);
	for (auto & u : u.uis) {
		ofAddListener(u.second.uiEvent, this, &ofApp::uiEvents);
	}
	soft.setUI(&u);
	ofSetCircleResolution(48);
	
	uiRemote.sendOnLoadPreset = false;
	uiRemote.loadConfig("_osc.txt");
	uiRemote.oscInfo = ((ofxMicroUI::inspector*)u.getElement("oscInfo"));//()
	
	// mudar presets e presetsFolder vindo do OSC.
//		ofAddListener(uiRemote.eventMessage, this, &ofApp::remoteMessage);
	uiRemote.setupServer();
	uiRemote.addAllUIs(&u);
}

void ofApp::update(){
}

void ofApp::draw(){
	ofBackground(40);
	fbo->begin();
	ofClear(0,255);
//	ofClear(255,0,0,255);
	if (scene == "circles") {
		for (int a=1; a<4; a++) {
			string id = ofToString(a);
			if (uiC->pBool["circle" + id]) {
				ofSetColor(uiC->pColor["cor" + id]);
//				cout << id << uiC->pColor["cor" + id] << endl;
//				cout << uiC->pFloat["x" + id] << endl;
//				cout << uiC->pFloat["y" + id] << endl;
//				cout << uiC->pFloat["radius" + id] << endl;
				
				ofDrawCircle(
					uiC->pFloat["x" + id],
					uiC->pFloat["y" + id],
					uiC->pFloat["radius" + id]);
			}
		}
	}
	
	else if (scene == "lines") {
		for (int a=0; a<100; a++) {
			float x = ofRandom(0,fbo->getWidth());
			float y = ofRandom(0, fbo->getHeight());
			ofDrawLine(0,0,x,y);
		}
	}
	fbo->end();
	soft.drawFbo();
}

void ofApp::keyPressed(int key){
//	if (key == 'q') {
//		for (auto & n : uiRemote._nameUIs) {
//			cout << n.first << endl;
//		}
//	}
}

void ofApp::keyReleased(int key){
}


void ofApp::uiEvents(ofxMicroUI::element & e) {
	if (e.name == "presets") {
	}
}

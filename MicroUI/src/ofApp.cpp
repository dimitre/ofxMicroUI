#include "ofApp.h"

void ofApp::setup(){
	m.setupUI();
	m.load("preset.xml");
}

void ofApp::update(){
}

void ofApp::draw(){
	ofBackground(40);
	for (int a=1; a<4; a++) {
		ofSetColor(m.pFloat["r" + ofToString(a)],
				   m.pFloat["g" + ofToString(a)],
				   m.pFloat["b" + ofToString(a)]);
		ofDrawCircle(
					 m.pFloat["x" + ofToString(a)],
					 m.pFloat["y" + ofToString(a)],
					 m.pFloat["radius" + ofToString(a)]);
	}
	
//	ofSetColor(m.pFloat["r"], m.pFloat["g"], m.pFloat["b"]);
//	ofDrawCircle(m.pFloat["x1"],m.pFloat["y1"],m.pFloat["radius1"]);
//
//	ofSetColor(m.pFloat["r2"], m.pFloat["g2"], m.pFloat["b2"]);
//	ofDrawCircle(m.pFloat["x2"],m.pFloat["y2"],m.pFloat["radius2"]);
}

void ofApp::keyPressed(int key){
	if (key == 's') {
		m.save("preset.xml");
	}
	else if (key == 'a') {
		m.load("preset.xml");
	}
}

void ofApp::keyReleased(int key){
}

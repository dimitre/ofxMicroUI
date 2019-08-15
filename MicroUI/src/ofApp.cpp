#include "ofApp.h"

void ofApp::setup(){
	//m.setup();
	m.createFromText("m.txt");
	m.load("preset.xml");
}

void ofApp::update(){
}

void ofApp::draw(){
	ofBackground(40);
	for (int a=1; a<4; a++) {
		string id = ofToString(a);
		if (m.pBool["circle" + id]) {
			ofSetColor(
				m.pFloat["r" + id],
				m.pFloat["g" + id],
				m.pFloat["b" + id]);
			ofDrawCircle(
				m.pFloat["x" + id],
				m.pFloat["y" + id],
				m.pFloat["radius" + id]);
		}
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

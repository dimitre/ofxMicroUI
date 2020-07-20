#include "ofApp.h"

void ofApp::setup(){
	m.createFromText("m.txt");
	m.load("preset.xml");
}

void ofApp::update(){ }

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
}

void ofApp::keyPressed(int key){
	if (key == 's') {
		m.save("preset.xml");
	}
	else if (key == 'a') {
		m.load("preset.xml");
	}
}

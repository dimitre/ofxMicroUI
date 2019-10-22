#include "ofApp.h"

void ofApp::setup(){
	u.createFromText("u.txt");
	ofAddListener(ui->uiEvent, this, &ofApp::uiEvents);
	for (auto & u : u.uis) {
		ofAddListener(u.second.uiEvent, this, &ofApp::uiEvents);
	}
	soft.setUI(&u);
}


void ofApp::update(){
	
}

void ofApp::draw(){
	ofBackground(40);
	
	fbo->begin();
	ofClear(0,255);
	for (int a=1; a<4; a++) {
		string id = ofToString(a);
		if (ui->pBool["circle" + id]) {
			ofSetColor(
				ui->pFloat["r" + id],
				ui->pFloat["g" + id],
				ui->pFloat["b" + id]);
			ofDrawCircle(
				ui->pFloat["x" + id],
				ui->pFloat["y" + id],
				ui->pFloat["radius" + id]);
		}
	}
	fbo->end();
	soft.drawFbo();
}

void ofApp::keyPressed(int key){
}

void ofApp::keyReleased(int key){
}


void ofApp::uiEvents(ofxMicroUI::element & e) {
	if (e.name == "presets") {
	}
}

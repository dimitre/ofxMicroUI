#include "ofApp.h"

ofImage imageCam;
//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowPosition(60, 60);
	m.createFromText("u.txt");
	
	fbo.allocate(640, 480, GL_RGBA);
	fbo.begin();
	ofClear(0,255);
	fbo.end();
	
	
//	cam.setDeviceID(0);
//	cam.setDesiredFrameRate(20);
//	cam.initGrabber(640, 480);
	
	imageCam.allocate(640, 480, OF_IMAGE_COLOR);
	m.load("def.xml");

}

//--------------------------------------------------------------
void ofApp::update(){
	cam.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(50);

	fbo.begin();
	ofClear(0,255);
	ofSetColor(255);
	ofDrawBitmapString(m.pString["radioOptions"], 20, 20);

	if (cam.isFrameNew()) {
		float contraste = m.pFloat["contraste"];
		float piso = m.pFloat["piso"];
		float rand = m.pFloat["rand"];
		float rand2 = m.pFloat["rand2"];
		cam.getTexture().readToPixels(imageCam.getPixels());
		for (int a=0; a<imageCam.getWidth(); a++) {
			for (int b=0; b<imageCam.getHeight(); b++) {
				float cor = imageCam.getColor(a, b).getLightness();
				cor *= contraste;
				cor += piso;
				cor += ofRandom(-rand,rand);
				cor += ofRandom(-rand2,rand2);
				cor = ofClamp(cor, 0, 255);
				imageCam.setColor(a, b, ofColor(cor));
			}
		}
		imageCam.update();
		imageCam.draw(0,0);
	}
	
	
	// I personally think ofColor could have a glm::vec3 initializer
	ofSetColor(m.pVec3["color"].x * 255.0, m.pVec3["color"].y * 255.0, m.pVec3["color"].z * 255.0);
	
	if (m.pBool["rect"]) {
		ofDrawRectangle(m.pFloat["rectX"],m.pFloat["rectY"],
					m.pFloat["rectW"],m.pFloat["rectH"]);
	}

	//glm::vec3 cor = m.pVec3["color"] * 255.0;
	//ofSetColor((ofColor)cor);
//	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
//ac	ofEnableBlendMode(OF_BLENDMODE_MULTIPLY); //ADD nao, screen tvz
	
//	OF_BLENDMODE_DISABLED = 0,
//	OF_BLENDMODE_ALPHA = 1,
//	OF_BLENDMODE_ADD = 2,
//	OF_BLENDMODE_SUBTRACT = 3,
//	OF_BLENDMODE_MULTIPLY = 4,
//	OF_BLENDMODE_SCREEN = 5
	
	ofSetColor(m.pVec3["color2"].x * 255.0, m.pVec3["color2"].y * 255.0, m.pVec3["color2"].z * 255.0);

	
	float mult = 5;
	for (int a=0; a<100; a++) {
		float x = a * mult;
		float y = map(a,0,100,0,100,m.pFloat["shaper"]) * mult;
		ofDrawRectangle(x,y,5,5);
	}
	
	if (m.pBool["circle"]) {
		ofDrawCircle(m.pFloat["x"], m.pFloat["y"], m.pFloat["radius"]);
	}
	
	
	fbo.end();
	fbo.draw(600,10);
	

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 's') {
		m.save("def.xml");
	}
	
	if (key == 'a') {
		m.load("def.xml");
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

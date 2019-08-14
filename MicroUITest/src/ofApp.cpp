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
	m.load("_presets/1.xml");
	ofEnableAlphaBlending();
}

//--------------------------------------------------------------
void ofApp::update(){
	cam.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	m.settings.colorVal 	= m.pFloat["colorVal"];
	m.settings.colorBg 		= m.pFloat["colorBg"];
	m.settings.colorLabel 	= m.pFloat["colorLabel"];
	

	m.settings.colorLabel 	= ofFloatColor(m.pVec3["colorLabel"].x, m.pVec3["colorLabel"].y, m.pVec3["colorLabel"].z, m.pFloat["colorLabelAlpha"]);


	ofBackground(m.pFloat["appBg"]);
	string s = ofToString("Mouse Position: " + ofToString(mouseX)+ "x" + ofToString(mouseY));
	((ofxMicroUI::inspector *) m.getElement("mouse"))->set(s);

	//if (2==3)
	{
		fbo.begin();
		ofClear(0,255);
		ofSetColor(255);
		ofDrawBitmapString(m.pString["text"], 20, 20);

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

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == '1' || key == '2' || key == '3' || key == '4' ||
		key == '5' || key == '6' || key == '7' || key == '8'
		) {
		string n = "_presets/" + ofToString(char(key)) + ".xml";
		//cout << n << endl;
		if (ofGetKeyPressed(OF_KEY_COMMAND)) {
			m.save(n);
		} else {
			m.load(n);
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

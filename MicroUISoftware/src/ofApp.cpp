#include "ofApp.h"

ofImage imageCam;
ofFbo * _fboUI = NULL;
//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowPosition(60, 60);
	m.createFromText("u.txt");
	
	soft.ui = &m;

//	cam.setDeviceID(0);
//	cam.setDesiredFrameRate(20);
//	cam.initGrabber(640, 480);

	imageCam.allocate(640, 480, OF_IMAGE_COLOR);
	m.load("_presets/1.xml");
	ofEnableAlphaBlending();	
	ofAddListener(m.uiEvent, this, &ofApp::uiEvents);
	
	_fboUI = &((ofxMicroUI::fboElement *)m.getElement("fbo"))->fbo;
	
	
	ofAddListener(m.uiEvent, this, &ofApp::uiEvents);
	for (auto & u : m.uis) {
		ofAddListener(u.second.uiEvent, this, &ofApp::uiEvents);
	}

}

//--------------------------------------------------------------
void ofApp::update(){
	cam.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	m._settings->colorVal 	= m.pFloat["colorVal"];
	m._settings->colorBg 		= m.pFloat["colorBg"];
	m._settings->colorLabel 	= m.pFloat["colorLabel"];
	
	m._settings->colorLabel 	= ofFloatColor(m.pVec3["colorLabel"].x, m.pVec3["colorLabel"].y, m.pVec3["colorLabel"].z, m.pFloat["colorLabelAlpha"]);

	ofBackground(m.pFloat["appBg"]);
//	m.getInspector("mouse")->set("Mouse Position: " + ofToString(mouseX)+ "x" + ofToString(mouseY));


	_fboUI->begin();
	ofClear(0,255);
	ofSetColor(255);
	for (int i = 0; i <40; i++) {
		ofDrawLine(0,0,ofRandom(0,_fboUI->getWidth()), ofRandom(0,_fboUI->getHeight()));
	}
	_fboUI->end();
	

	fbo->begin();
	ofClear(0,255);
	ofSetColor(255);
	ofDrawBitmapString(m.pString["text"], 20, 20);
	//if (2==3)
	{

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
		
		fbo->end();
		
		soft.drawFbo();
		//fbo.draw(545,230);
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

////--------------------------------------------------------------
//void ofApp::uiEvents(ofxMicroUI::element & e) {
//	cout << e.name << endl;
//	//cout << ((ofxMicroUI::slider(e))).getVal() << endl;
//}

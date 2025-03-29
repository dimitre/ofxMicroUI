#include "ofApp.h"

void ofApp::setup(){
	ofAddListener(u.uiEvent, this, &ofApp::uiEvents);
	for (auto & u : u.uis) {
		ofAddListener(u.second.uiEvent, this, &ofApp::uiEvents);
	}
	soft.dragFbo = true;
	soft.setUI(&u);
}

void ofApp::update(){
}

void ofApp::draw(){
	ofBackground(uiColors->pColor["appBg"]);

	//ofBackground(40);
	fbo->begin();
	ofClear(0,255);

	if (scene == "circles") {
		for (int a=1; a<4; a++) {
			string id = ofToString(a);
			if (uiC->pBool["circle" + id]) {
				ofSetColor(
					uiC->pFloat["r" + id],
					uiC->pFloat["g" + id],
					uiC->pFloat["b" + id]);
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


void ofApp::uiEvents(ofxMicroUI::element & e) {
	u._settings->uiOpacity		= uiColors->pFloat["uiOpacity"];
	u._settings->colorVal 		= uiColors->pColor["colorVal"];
	u._settings->colorBg 		= uiColors->pColor["colorBg"];
	u._settings->colorLabel		= uiColors->pColor["colorLabel"];
	u._settings->colorShadowLabel		= uiColors->pColor["colorShadowLabel"];
	u._settings->uiColorBg		= uiColors->pColor["uiColorBg"];

	for (auto & u : u.allUIs) {
		u->uiColorBg			= uiColors->pColor["uiColorBg"];
	}
	u.redraw();
	

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'e') {
        std::string saida;
        // colorBg	64
        // colorVal	0
        // uiMargin	5
        // uiPadding	5
        // uiColorBg	44 44 44 170
        // colorLabel	#3ef88b
        // colorLabel	#57fc7c
        // colorLabel	#ffffff
        // colorShadowLabel	0 0
        // #uiColorBg	0 255 100 170
        // uiOpacity	255
        // useFixedLabel	1

        auto c = u._settings->colorBg;
        saida += "colorBg	" + ofToString(int(c.r)) + " " + ofToString(int(c.g)) + " " + ofToString(int(c.b)) + " " + ofToString(int(c.a)) + "\n";
        c = u._settings->uiColorBg;
        saida += "uiColorBg	" + ofToString(int(c.r)) + " " + ofToString(int(c.g)) + " " + ofToString(int(c.b)) + " " + ofToString(int(c.a)) + "\n";
        c = u._settings->colorVal;
        saida += "colorVal	" + ofToString(int(c.r)) + " " + ofToString(int(c.g)) + " " + ofToString(int(c.b)) + " " + ofToString(int(c.a)) + "\n";
        c = u._settings->colorLabel;
        saida += "colorLabel	" + ofToString(int(c.r)) + " " + ofToString(int(c.g)) + " " + ofToString(int(c.b)) + " " + ofToString(int(c.a)) + "\n";
        c = u._settings->colorShadowLabel;
        saida += "colorShadowLabel	" + ofToString(int(c.r)) + " " + ofToString(int(c.g)) + " " + ofToString(int(c.b)) + " " + ofToString(int(c.a)) + "\n";
        cout << saida << endl;
    }
}

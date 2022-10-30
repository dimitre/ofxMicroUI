#include "ofxMicroUI.h"

void ofxMicroUI::addListeners() {
	if (!hasListeners) {
		ofAddListener(ofEvents().setup, this, &ofxMicroUI::onSetup);
		ofAddListener(ofEvents().update, this, &ofxMicroUI::onUpdate);
		ofAddListener(ofEvents().draw, this, &ofxMicroUI::onDraw);
		//ofAddListener(ofEvents().mouseMoved, this, &ofxMicroUI::onMouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &ofxMicroUI::onMousePressed);
		ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUI::onMouseDragged);
		ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUI::onMouseReleased);
		ofAddListener(ofEvents().exit, this, &ofxMicroUI::onExit);
		hasListeners = true;
//        allUIs.emplace_back(this);
	}
}

void ofxMicroUI::removeListeners() {
	if (hasListeners) {
		ofRemoveListener(ofEvents().setup, this, &ofxMicroUI::onSetup);
		ofRemoveListener(ofEvents().draw, this, &ofxMicroUI::onDraw);
		//ofRemoveListener(ofEvents().mouseMoved, this, &ofxMicroUI::onMouseMoved);
		ofRemoveListener(ofEvents().mousePressed, this, &ofxMicroUI::onMousePressed);
		ofRemoveListener(ofEvents().mouseDragged, this, &ofxMicroUI::onMouseDragged);
		ofRemoveListener(ofEvents().mouseReleased, this, &ofxMicroUI::onMouseReleased);
		ofRemoveListener(ofEvents().update, this, &ofxMicroUI::onUpdate);
		ofRemoveListener(ofEvents().exit, this, &ofxMicroUI::onExit);

		hasListeners = false;
//        allUIs.emplace_back(this);
	}
}


void ofxMicroUI::draw() {
	if (redrawUI) {
		fbo.begin();
		ofClear(0,0);
		ofSetColor(uiColorBg);
		ofDrawRectangle(rect);
		
		if (uiColorTop != ofColor(0)) {
			ofSetColor(uiColorTop);
			ofDrawRectangle(0,0,rect.width, 5);
		}
		
		ofSetColor(255);
		for (auto & e : elements) {
			if (!e->alwaysRedraw) {
				e->draw();
			}
		}
		fbo.end();
		redrawUI = false;
	}
	
	if (visible && _settings->visible) {
		fbo.begin();
		for (auto & e : elements) {
			if (e->haveToRedraw) {
				e->redrawElement();
				//e->draw();
			}
		}
		fbo.end();
		
		ofSetColor(255, _settings->uiOpacity);
//			ofSetColor(255, uiOpacity);
		
		//  se este desenhar depois dos always redraw precisamos fazer buracos pra nao sobrepor a opacidade
		fbo.draw(rectPos.getPosition() + _settings->offset);

		// melhorar com lookup isso aqui
		ofPushMatrix();
		ofTranslate(rectPos.getPosition() + _settings->offset);
		for (auto & e : elements) {
			if (e->alwaysRedraw) {
//                cout << "alwaysredraw " << e->name << endl;
				e->draw();
			}
		}
		ofPopMatrix();
	}
}

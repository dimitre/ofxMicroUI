#include "ofxMicroUI.h"

void ofxMicroUI::addListeners() {
    if (!hasListeners) {
        ofAddListener(ofEvents().setup, this, &ofxMicroUI::onSetup);
        ofAddListener(ofEvents().draw, this, &ofxMicroUI::onDraw);
        //ofAddListener(ofEvents().mouseMoved, this, &ofxMicroUI::onMouseMoved);
        ofAddListener(ofEvents().mousePressed, this, &ofxMicroUI::onMousePressed);
        ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUI::onMouseDragged);
        ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUI::onMouseReleased);
        ofAddListener(ofEvents().update, this, &ofxMicroUI::onUpdate);
        hasListeners = true;
        
//        allUIs.emplace_back(this);
    }
}

void ofxMicroUI::draw() {
    if (redrawUI) {
        fbo.begin();
        ofClear(0,0);
        ofSetColor(uiColorBg);
        ofDrawRectangle(rect);
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
        fbo.draw(rectPos.getPosition() + _settings->offset);
        
        
        // melhorar com lookup isso aqui
        ofPushMatrix();
        ofTranslate(rectPos.getPosition() + _settings->offset);
        for (auto & e : elements) {
            if (e->alwaysRedraw) {
                e->draw();
            }
        }
        ofPopMatrix();
    }
}

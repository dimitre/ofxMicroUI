/*
 This is the software class, to extend the funcionality of ofxMicroUI
 If needed
 
 TODO
 change fbo fbo2, fbo3 to other kind of fbo.
 
 */
#pragma once

#include "ofBaseApp.h"

#include "ofConstants.h"
// #include "ofParameter.h"
#include "ofTrueTypeFont.h"
#include "ofBitmapFont.h"
#include "ofFbo.h"

#include "ofxMicroUI.h"

using namespace std;

class ofxMicroUISoftware : public ofBaseApp {
public:
	
	ofFbo fbo, fbo2, fbo3;
	ofFbo fbo4;
	ofPixels fboPixels;
	ofFbo * fboFinal = &fbo;
	ofRectangle fboRect;
	ofRectangle fboRectFull;
	ofxMicroUI * _ui = NULL;
	
	map <string, ofFbo> mapFbos;

	ofxMicroUISoftware();
	void setUI(ofxMicroUI * u);
	void updateFboRect();

//	~ofxMicroUISoftware() {}
	
	void drawFbo() {
		if (_ui != NULL) {
//			ofRectangle & r = ui->visible ? fboRect : fboRectFull;
//			ofRectangle & r = fboRect;
			if (_ui->visible) {
				ofSetColor(0);
				ofDrawRectangle(fboRect);
			}
			ofSetColor(255);
			fboFinal->draw(fboRect);
		}
	}

	void allocateFbos(int w, int h, int multiSampling = 0) {
		if (multiSampling) {
// Raspberry
//			fbo.allocate(w, h, GL_RGBA32F_ARB, multiSampling);
			fbo.allocate(w, h, GL_RGBA, multiSampling);
			fbo2.allocate(w, h, GL_RGBA, multiSampling);
			fbo3.allocate(w, h, GL_RGBA, multiSampling);
		} else {
			//fbo.allocate(w, h, GL_RGBA32F_ARB);
			fbo.allocate(w, h, GL_RGBA);
			fbo2.allocate(w, h, GL_RGBA);
			fbo3.allocate(w, h, GL_RGBA);
		}
		fboPixels.allocate(w, h, OF_IMAGE_COLOR); //OF_IMAGE_COLOR_ALPHA

		fbo.begin();
		ofClear(0,255);
		fbo.end();
		
		fbo2.begin();
		ofClear(0,255);
		fbo2.end();
		
		fbo3.begin();
		ofClear(0,255);
		fbo3.end();
	}
	
	map <char, int> keyPreset = {
		{ 'a', 0 },
		{ 's', 1 },
		{ 'd', 2 },
		{ 'f', 3 },
		{ 'g', 4 },
		{ 'h', 5 },
		{ 'j', 6 },
		{ 'k', 7 },
		{ 'l', 8 },
		{ 'z', 9 },
		{ 'x', 10 },
		{ 'c', 11 },
		{ 'v', 12 },
		{ 'b', 13 },
		{ 'n', 14 },
		{ 'm', 15 },
		{ ',', 16 },
		{ '.', 17 }
	};
	
	void keyPressed(int key){
//		if (ofGetKeyPressed(OF_KEY_SUPER)) {
//			cout << "OF_KEY_SUPER" << endl;
//		}
//		
//		else if (ofGetKeyPressed(OF_KEY_ALT)) {
//			cout << "OF_KEY_ALT" << endl;
//		}
//		
//		else if (ofGetKeyPressed(OF_KEY_CONTROL)) {
//			cout << "OF_KEY_CONTROL" << endl;
//		}
//
//		else if (ofGetKeyPressed(OF_KEY_COMMAND)) {
//			cout << "OF_KEY_COMMAND" << endl;
//		}
//
//		else if (ofGetKeyPressed(OF_KEY_SHIFT)) {
//			cout << "OF_KEY_SHIFT" << endl;
//		}


		if (ofGetKeyPressed(OF_KEY_COMMAND)) {
			if (key == 'f' || key == 'F') {
				ofToggleFullscreen();
			}
			else if (key == 's' || key == 'S') {
				string name = _ui->pString["presets"];
				//cout << "saving actual preset " << name << endl;
				_ui->savePreset(name);
			}
			else if (key == 'o') {
				string n = _ui->pString["presets"];
				string presetFolder = ofToDataPath(_ui->getPresetPath(true) + "/" + n);
				string comando = "open " + presetFolder;
				cout << comando << endl;
				ofSystem(comando);
			}
		} else {
			if (key == '=') {
				_ui->toggleVisible();
				if (!_ui->visible) {
					ofHideCursor();
				} else {
					ofShowCursor();
				}
			}
			else if (key == '-') {
				ofToggleFullscreen();
			}
			
			if (key < 255) {
				if ( keyPreset.find(key) != keyPreset.end() ) {
					ofxMicroUI::element * e;
					e = _ui->getElement("presets");
					if (e != NULL && e->name != "") {
						((ofxMicroUI::presets*)e)->set(keyPreset[key]);
					} else {
		//				cout << "e not found! ):" << endl;
					}
				}
			}
		}
	}
	
	void onKeyPressed(ofKeyEventArgs& data) {
		keyPressed(data.key);
	}

	void onDraw(ofEventArgs &data) { }

	bool dragging = false;
	bool dragFbo = false;
	glm::vec2 firstXY;

	struct drag {
		ofxMicroUI::element *ex = NULL;
		ofxMicroUI::element *ey = NULL;
		glm::vec2 dragPos;
		void update(glm::vec2 xy) {
			dragPos = xy;
			if (ex != NULL) {
				ex->set(xy.x);
			}
			if (ey != NULL) {
				ey->set(xy.y);
			}
		}
	};
	
	//void onMouseMoved(ofMouseEventArgs &data) {}
	void onMousePressed(ofMouseEventArgs &data) {
		if (dragFbo) {
			glm::vec2 xy = glm::vec2(data.x, data.y);
			if (fboRect.inside(xy)) {
				firstXY = xy;
				dragging = true;
			}
		}
	}
	
	void onMouseDragged(ofMouseEventArgs &data) {
		if (dragFbo && dragging) {
			glm::vec2 xy = glm::vec2(data.x, data.y);
			fboRect.x += data.x - firstXY.x;
			fboRect.y += data.y - firstXY.y;
			
			firstXY = xy;
			
//			cout << "mousexy: " << data.x << " x " << data.y << endl;
//			cout << "fborect: " << fboRect.x << " x " << fboRect.y << endl;
//			cout << fboRect.y << endl;
			float x = fboRect.x;
			float y = fboRect.y;
			_ui->getSlider("fboY")->set(y);
			_ui->getSlider("fboX")->set(x);
		}
	}
	
	void onMouseReleased(ofMouseEventArgs &data) {
		dragging = false;
	}
	
	void uiEventsAll(ofxMicroUI::element & e) {
		// shortcutUIEvent(e);
		if (ofIsStringInString(e.name, "_shortcut")) {
			if (!e._settings->presetIsLoading && *e.s != "") {
				vector <string> explode = ofSplitString(e.name, "_shortcut");
				float val = ofToFloat(*e.s);
				e._ui->getSlider(explode[0])->set(val);
			}
		}
        
        if (e.name == "resetAll") {
            for (auto & ee : e._ui->elements) {
                // evita loop infinito
                if (ee->name != "resetAll") {
                    ee->resetDefault();
                }
            }
        }
	}
	
	// Master only
	void uiEvents(ofxMicroUI::element & e) {
		if (e.name == "easing") {
			_ui->_settings->easing = *e.f;
		}
		
		else if (e.name == "presetsFolder") {
			_ui->setPresetsFolder(*e.s);
		}

		else if (e.name == "fps") {
			// se o tipo for string.
			if (e.s != NULL) {
				ofSetFrameRate(ofToInt(*e.s));
			}
			if (e.i != NULL) {
				cout << "ofxMicroUISoftware :: " << e.name << " :: " << *e.i << endl;
				ofSetFrameRate(*e.i);
			}
		}
		
		else if (e.name == "fboX" || e.name == "fboY" || e.name == "fboScale") {
			updateFboRect();
		}
		
		else if (e.name == "opacityUI") {
			_ui->_settings->uiOpacity = *e.f;
			_ui->uiOpacity = *e.f;
		}
		
		else if (e.name == "verticalSync") {
			cout << "ofxMicroUISoftware :: " << e.name << " :: " << *e.b << endl;
			ofSetVerticalSync(*e.b);
		}
		
		else if (e.name == "dragFbo") {
			dragFbo = *e.b;
		}
		
		if (e.tag == "showUIByName") {
			for (auto & u : _ui->uis) {
				if (ofIsStringInString(u.first, e.name)) {
					u.second.setVisible (*e.b);
				}
			}
			_ui->reflowUIs();
		}
		
		else if (e.tag == "showUI") {
			for (auto & u : _ui->allUIs) {
				if (u->uiTag == e.name) {
					u->setVisible (*e.b);
				}
			}
			_ui->reflowUIs();
		}
		
	}
	

	
	void onExit(ofEventArgs &data) {
		if (_ui != NULL) {
			cout << "ofxMicroUISoftware exit, saving preset" << endl;
			//cout << _ui->presetsRootFolder << endl;
			_ui->save(_ui->presetsRootFolder + "/master.xml");
			for (auto & u : _ui->uis) {
				if (u.second.saveMode == ofxMicroUI::MASTER) {
					string f = _ui->presetsRootFolder + "/" + u.first + ".xml";
					u.second.save(f);
				}
			}
		}
		else {
			cout << "ofxMicroUISoftware need to set ui pointer" << endl;
		}
	}
	


	ofPixels pixelsExport;
	ofFbo fboExport;

	void fboToPixels() {
		if (!pixelsExport.isAllocated()) {
			cout << "allocating pixelsExport" << endl;
			pixelsExport.allocate(fboFinal->getWidth(), fboFinal->getHeight(), OF_IMAGE_COLOR);
		}
		if (!fboExport.isAllocated()) {
			cout << "allocating fboExport" << endl;
			fboExport.allocate(fboFinal->getWidth(), fboFinal->getHeight(), GL_RGB);
		}
		fboExport.begin();
		ofClear(0,255);
		ofSetColor(255);
		fboFinal->draw(0,0);
		fboExport.end();
		fboExport.readToPixels(pixelsExport);
	}
	
	void fboToPng() {
		fboToPixels();
		
		string p = ofToString(_ui->pString["presets"]);
		string folder = "_output";
		if (!ofFile::doesFileExist(folder)) {
			ofDirectory::createDirectory(folder);
		}
		// create directory if doesnt exist
		string fullFileName = folder + "/" + p + "_" +ofGetTimestampString() + ".png";
		
		ofSaveImage(pixelsExport, fullFileName);
		string resultado = ofSystem("open " + ofToDataPath(fullFileName));
	}
	
#include "tiffFastWriter.h"
	
	void fboToTiff() {
		fboToPixels();
		
		string p = ofToString(_ui->pString["presets"]);
		string folder = "_output";
		if (!ofFile::doesFileExist(folder)) {
			ofDirectory::createDirectory(folder);
		}
		// create directory if doesnt exist
		string fullFileName = folder + "/" + p + "_" +ofGetTimestampString() + ".tif";
		
		tiffFastWriter rec;
		rec.recordTiff(&pixelsExport, fullFileName);
		
//		ofSaveImage(pixelsExport, fullFileName);
		string resultado = ofSystem("open " + ofToDataPath(fullFileName));
	}
	
	

};

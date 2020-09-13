/*
 This is the software class, to extend the funcionality of ofxMicroUI
 If needed
 
 TODO
 change fbo fbo2, fbo3 to other kind of fbo.
 
 */

#pragma once


class ofxMicroUISoftware : public ofBaseApp {
public:
	ofFbo fbo, fbo2, fbo3;
	ofPixels fboPixels;
	ofFbo * fboFinal = &fbo;
	ofRectangle fboRect;
	ofRectangle fboRectFull;
	ofxMicroUI * _ui = NULL;
	
	
	// 31 october 2019 test
	map <string, ofFbo> mapFbos;

	ofxMicroUISoftware() {
		init();
	}
	
	void setUI(ofxMicroUI * u) {
		_ui = u;
		
		string f = "_ui/style.txt";
		if (ofFile::doesFileExist(f)) {
			_ui->_settings->styleLines = ofBufferFromFile(f).getText();
		}
		
		// set the fbo pointer to save presets
		if (_ui->presetElement != NULL) {
			_ui->presetElement->_fbo = fboFinal;
		}
		fboRectFull = ofRectangle(0,0,fboFinal->getWidth(), fboFinal->getHeight());
		ofAddListener(_ui->uiEvent, this, &ofxMicroUISoftware::uiEvents);

		// now to handle all events from all uis (shortcut)
		ofAddListener(_ui->uiEvent, this, &ofxMicroUISoftware::uiEventsAll);
		
		
		for (auto & e : _ui->elements) {
			if (ofIsStringInString(e->name, "_shortcut")) {
//				cout << "XAXAXAAAA" << e->name << endl;
				e->saveXml = false;
			}
		}
		for (auto & u : _ui->allUIs) {
			ofAddListener(u->uiEvent, this, &ofxMicroUISoftware::uiEventsAll);
			// medida provisoria
//			cout << "------" << endl;
//			cout << u->uiName << endl;
			
			for (auto & e : u->elements) {
				if (ofIsStringInString(e->name, "_shortcut")) {
//					cout << "XAXAXAAAA" << e->name << endl;
					e->saveXml = false;
				}
			}
		}


		_ui->load(_ui->presetsRootFolder + "/master.xml");
		
		if (_ui->pString["presetsFolder"] == "") {
			((ofxMicroUI::radio*)_ui->getElement("presetsFolder"))->set("1");
		}
		
		for (auto & u : _ui->uis) {
			if (u.second.loadMode == ofxMicroUI::MASTER) {
				string f = _ui->presetsRootFolder + "/" + u.first + ".xml";
				u.second.load(f);
			}
		}
		
		updateFboRect();
	}
	
	void updateFboRect() {
		fboRect = ofRectangle(_ui->pInt["fboX"],
				  _ui->pInt["fboY"],
				  fboFinal->getWidth() * _ui->pFloat["fboScale"],
				  fboFinal->getHeight() * _ui->pFloat["fboScale"]
		);
	}
	
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
	
	void init() {
		cout << "ofxMicroUISoftware Init" << endl;
		ofAddListener(ofEvents().keyPressed, this, &ofxMicroUISoftware::onKeyPressed);

		int w, h, multiSampling = 0;
		if (ofFile::doesFileExist("_output.txt")) {
			vector <string> output = ofxMicroUI::textToVector("_output.txt");
			vector <string> dimensoes = ofSplitString(output[0], " ");
			w = ofToInt(dimensoes[0]);
			h = ofToInt(dimensoes[1]);
			if (dimensoes.size() > 2) {
				multiSampling = ofToInt(dimensoes[2]);
			}
		} else {
//			cout << "missing output.txt file" << endl;
			w = 1280;
			h = 720;
		}

		allocateFbos(w, h, multiSampling);

		
		//ofxMicroUI::alert("microUISoftware setup");
		//ofAddListener(ofEvents().draw, this, &ofxMicroUI::onDraw);
		//ofAddListener(ofEvents().mouseMoved, this, &ofxMicroUI::onMouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &ofxMicroUISoftware::onMousePressed);
		ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUISoftware::onMouseDragged);
		ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUISoftware::onMouseReleased);
		ofAddListener(ofEvents().exit, this, &ofxMicroUISoftware::onExit);
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
//		if (ofGetKeyPressed(OF_KEY_ALT)) {
//			cout << "OF_KEY_ALT" << endl;
//		}
//		if (ofGetKeyPressed(OF_KEY_CONTROL)) {
//			cout << "OF_KEY_CONTROL" << endl;
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
//					cout << key << endl;
//					cout << keyPreset[key] << endl;
//					cout << "-----" << endl;

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
		shortcutUIEvent(e);
	}
	
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
//			cout << e.name << endl;
//			cout << *e.b << endl;
			ofSetVerticalSync(*e.b);
		}
	}
	
	void shortcutUIEvent(ofxMicroUI::element & e) {
		if (ofIsStringInString(e.name, "_shortcut")) {
			if (!e._settings->presetIsLoading && *e.s != "") {
				cout << "shortcutUIEvent " << e.name << endl;
				vector <string> explode = ofSplitString(e.name, "_shortcut");
				float val = ofToFloat(*e.s);
				e._ui->getSlider(explode[0])->set(val);
			}
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
};

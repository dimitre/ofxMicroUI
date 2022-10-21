#include "ofxMicroUISoftware.h"

void ofxMicroUISoftware::afterSetUI() {
//		 cout << "****************************************************************" << endl;
//		 cout << "afterSetUI agora" << endl;
	string f = "_ui/_style.txt";
	if (ofFile::doesFileExist(f)) {
		_ui->_settings->styleLines = ofBufferFromFile(f).getText();
	}
	

	fboRectFull = ofRectangle(0,0,fboFinal->getWidth(), fboFinal->getHeight());
	ofAddListener(_ui->uiEvent, this, &ofxMicroUISoftware::uiEvents);

	// now to handle all events from all uis (shortcut, etc)
	ofAddListener(_ui->uiEvent, this, &ofxMicroUISoftware::uiEventsAll);
	for (auto & u : _ui->allUIs) {
		ofAddListener(u->uiEvent, this, &ofxMicroUISoftware::uiEventsAll);
	}

	_ui->load(_ui->presetsRootFolder + "/master.xml");
	
	for (auto & u : _ui->uis) {
		if (u.second.loadMode == ofxMicroUI::MASTER) {
			string f = _ui->presetsRootFolder + "/" + u.first + ".xml";
			u.second.load(f);
		}
	}
	updateFboRect();
	if (_ui->pString["presetsFolder"] == "") {
		ofxMicroUI::radio * r = _ui->getRadio("presetsFolder");
		if (r != NULL) {
			r->set("1");
		}
//            ((ofxMicroUI::radio*)_ui->getElement("presetsFolder"))->set("1");
	}
}




void ofxMicroUISoftware::loadPreset(const string & s) {
	if (_ui->presetElement != NULL) {
		_ui->presetElement->set(s);
	}
}

void ofxMicroUISoftware::keyPressed(int key){
	if (key == OF_KEY_LEFT) {
		if (_ui->presetElement != NULL) {
			_ui->presetElement->cycle(-1);
		}
	}
	
	else if (key == OF_KEY_RIGHT) {
		if (_ui->presetElement != NULL) {
			_ui->presetElement->cycle(1);
		}
	}
	
	else if (key == OF_KEY_UP) {
		if (_ui->presetElement != NULL) {
			_ui->presetElement->cycle(-_ui->_settings->presetCols);
		}
	}
	
	else if (key == OF_KEY_DOWN) {
		if (_ui->presetElement != NULL) {
			_ui->presetElement->cycle(_ui->_settings->presetCols);
		}
	}

	// scrolling diogo
	else if (key == '[') {
		softScroll ++ ;
		if (softScroll > 0) {
			softScroll = 0;
		}
		_ui->_settings->offset.x = softScroll*softW;
	}
	else if (key == ']') {
		softScroll -- ;
		_ui->_settings->offset.x = softScroll*softW;
	}
	
	else if (key == '\\') {
		usingSoftScroll ^= 1;
		_ui->_settings->offset.x = usingSoftScroll ? softScroll*softW : 0;
	}
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
		else if (key == 'o' || key == 'O') {
			string n = _ui->pString["presets"];
			string presetFolder = ofToDataPath(_ui->getPresetPath(true) + "/" + n);
			string comando = "open " + presetFolder;
			cout << comando << endl;
			ofSystem(comando);
		}
	} else {
		// no key command pressed
		
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
		else if (key == '0') {
			_ui->toggleVisible();
			showFbo = _ui->_settings->visible;
//                cout << showFbo << endl;
			if (showFbo) {
				ofSetWindowShape(windowSize.x, windowSize.y);
			} else {
				ofSetWindowShape(100, 100);
				ofSetWindowPosition(30,30);
			}
		}
		
		if (usePresetShortcut && key < 255) {
			if ( keyPreset.find(key) != keyPreset.end() ) {
				if (_ui != NULL && _ui->presetElement != NULL) {
					ofxMicroUI::element * e = _ui->presetElement;
					if (e != NULL && e->name != "") {
						((ofxMicroUI::presets*)e)->set(keyPreset[key]);
					} else {
		//				cout << "e not found! ):" << endl;
					}
				}
			}
		}
	}
}


void ofxMicroUISoftware::uiEventsAll(ofxMicroUI::element & e) {
	// shortcutUIEvent(e);
	if (e.name == "resetAll") {
		if (!e._settings->presetIsLoading) {
			cout << e.name << "::" << e._ui->uiName << endl;
			for (auto & ee : e._ui->elements) {
				// evita loop infinito
				if (ee->name != "resetAll") {
					ee->resetDefault();
				}
			}
		}
	}
	
	if (ofIsStringInString(e.name, "_shortcutPlus")) {
		if (!e._settings->presetIsLoading && *e.s != "") {
			vector <string> explode = ofSplitString(e.name, "_shortcutPlus");
			float val = ofToFloat(*e.s);
			e._ui->getSlider(explode[0])->add(val);
		}
	}
	
	else if (ofIsStringInString(e.name, "_shortcut")) {
		if (!e._settings->presetIsLoading && *e.s != "") {
			vector <string> explode = ofSplitString(e.name, "_shortcut");
			float val = ofToFloat(*e.s);
			e._ui->getSlider(explode[0])->set(val);
		}
	}
}




void ofxMicroUISoftware::uiEvents(ofxMicroUI::element & e) {
	if (e.name == "easing") {
		_ui->_settings->easing = *e.f;
	}
	
	else if (e.name == "presetsFolder") {
		_ui->setPresetsFolder(*e.s);
	}

	else if (e.name == "fps") {
		
		// se o tipo for string.
		if (e.s != NULL) {
			
			cout << "FPS STRING " << *e.s << endl;
			ofSetFrameRate(ofToInt(*e.s));
		}
		if (e.i != NULL) {
//                cout << "FPS INT " << *e.i << endl;
//				cout << "ofxMicroUISoftware :: " << e.name << " :: " << *e.i << endl;
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

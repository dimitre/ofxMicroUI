#include "ofxMicroUISoftware.h"

ofxMicroUISoftware::ofxMicroUISoftware() {
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

	ofAddListener(ofEvents().mousePressed, this, &ofxMicroUISoftware::onMousePressed);
	ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUISoftware::onMouseDragged);
	ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUISoftware::onMouseReleased);
	ofAddListener(ofEvents().exit, this, &ofxMicroUISoftware::onExit);
}

void ofxMicroUISoftware::setUI(ofxMicroUI * u) {
	_ui = u;
	
	string f = "_ui/_style.txt";
	if (ofFile::doesFileExist(f)) {
		_ui->_settings->styleLines = ofBufferFromFile(f).getText();
	}
	
	// set the fbo pointer to save presets
	if (_ui->presetElement != NULL) {
		_ui->presetElement->_fbo = fboFinal;
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
	}
}


void ofxMicroUISoftware::updateFboRect() {
	fboRect = ofRectangle(_ui->pInt["fboX"],
				_ui->pInt["fboY"],
				fboFinal->getWidth() * _ui->pFloat["fboScale"],
				fboFinal->getHeight() * _ui->pFloat["fboScale"]
	);
}
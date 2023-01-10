#include "ofxMicroUI.h"
#include "ofxMicroUIElements.h"
#include "ofVideoPlayer.h"
#include "ofSoundPlayer.h"

void ofxMicroUI::element::notify() {
//		cout << "void notify" << endl;
//		if (_settings->presetIsLoading) {
//			_ui->loadingEvents.push_back(this);
//		} else
		
		if (useNotify && _ui->uiIsCreated) // && _settings->useNotify
		{
//			cout << "void notify inside" << endl;
//			cout << "microUI notify! " << _ui->uiName << "/" << name << endl;
//			ofNotifyEvent(_ui->uiEvent, *this);
			ofNotifyEvent(_ui->uiEvent, *this);
		}
//		ofNotifyEvent(_ui->uiEvent2, **this);
//		ofNotifyEvent(_ui->uiEvent3, this);

}

ofxMicroUI::videoList::videoList(string & n, ofxMicroUI & ui, vector<string> items, string & v, ofVideoPlayer & vid)
: dirList(n, ui, items, v) {
	_video = &vid;
}

void ofxMicroUI::videoList::updateVal()  {
	string f = getFileName();
	if (_video != NULL && *s != "") {
		if (loadedFile != f) {
			_video->load(f);
			// 25 jan 2020 - novas fronteiras
			_video->play();
			loadedFile = f;
			cout << "LOAD videoList: " << name << " : " << f << endl;
		}
	}
}

ofxMicroUI::audioList::audioList(string & n, ofxMicroUI & ui, vector<string> items, string & v, ofSoundPlayer & sound)
: dirList(n, ui, items, v) {
	_sound = &sound;
}

void ofxMicroUI::audioList::updateVal() {
	string f = getFileName();
	if (_sound != NULL && *s != "") {
		if (loadedFile != f) {
			_sound->load(f);
			// 25 jan 2020 - novas fronteiras
			_sound->play();
			loadedFile = f;
			cout << "LOAD audioList: " << name << " : " << f << endl;
		}
	}
}

void ofxMicroUI::group::copyValFrom(element & e)  {
	ofxMicroUI::group* grupo = (ofxMicroUI::group*)(&e);
	for (auto & el : elements) {
		element * elementoInterno = grupo->getElement(el->name);
		if (elementoInterno != NULL) {
			el->copyValFrom(*elementoInterno);
		}
	}
	updateVal();
	redraw();
}


ofxMicroUI::flipflop::flipflop(string & n, ofxMicroUI & ui, int & v) { //: group::group(n, ui)
	_val = &v;
	setupElement(n, ui, false);
	bool saveState = _ui->useLabelOnNewElement;
	_ui->useLabelOnNewElement = false;
	_ui->setFlowVert(false);
	for (int a=0; a<8; a++) {
		string name = "b" + ofToString(a);
		elements.push_back(new toggle (name, ui, vals[a], vals[a], true));
	}
	string name = "val";
	elements.push_back(new inspector(name, ui));
	elements.back()->rect.width = 40;
	_ui->useLabelOnNewElement = saveState;
	_ui->setFlowVert(true);
}


ofxMicroUI::colorHsv::colorHsv(string & n, ofxMicroUI & ui, ofColor defaultColor, ofColor & c, int kind) {
	setupElement(n, ui, false);
	
	useAlpha = (kind == 1);
	useRange = (kind == 2);
	_val = &c;
	// 27 june 2020 novas fronteiras.
	*_val = defaultColor;
	string sName = "hueBrightness";
	elements.push_back(new label(name, ui));
	elements.push_back(new slider2d(sName, ui, xy));
	elements.back()->useNotify = false;
	
	ofFbo * _fbo = &((slider2d*)elements.back())->fbo;
	_fbo->begin();
	ofClear(0);
	ofColor cor;
	int w = _fbo->getWidth();
	int h = _fbo->getHeight();
	for (int b=0; b<h; b++) {
		for (int a=0; a<w; a++) {
			float hue = (255 * a / (float) w);
			cor = ofColor::fromHsb(hue, 255, b*255/h, 255);
			ofFill();
			ofSetColor(cor);
			ofDrawRectangle(a,b,1,1);
		}
	}
	_fbo->end();

	((ofxMicroUI::slider2d*)elements.back())->drawVal();
	elements.back()->useNotify = false;

	{
		glm::vec3 vals = glm::vec3(0,255,127);
		elements.push_back(new slider(nameSat, ui, vals, sat));
		elements.back()->useNotify = false;
	}
	
	if (useAlpha) {
		glm::vec3 vals = glm::vec3(0,255,255);
		string sName = "alpha";
		elements.push_back(new slider(sName, ui, vals, alpha));
		elements.back()->useNotify = false;
	} else {
		alpha = 255;
	}

	if (useRange) {
		glm::vec3 vals = glm::vec3(0,1,.3);
		string sName = "range";
		elements.push_back(new slider(sName, ui, vals, range));
		elements.back()->useNotify = false;
	}

	groupResize();
}



ofxMicroUI::vec3::vec3(string & n, ofxMicroUI & ui, glm::vec3 & v) {
	_val = &v;
	setupElement(n, ui, false);
	glm::vec3 vals = glm::vec3(0,1,.5);
	//friend class?
	//string name = "GROUP";
	string x = "x";
	string y = "y";
	string z = "z";
	elements.push_back(new label(name, ui));
	elements.push_back(new slider(x, ui, vals, _val->x));
	elements.push_back(new slider(y, ui, vals, _val->y));
	elements.push_back(new slider(z, ui, vals, _val->z));
	groupResize();
}

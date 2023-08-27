#include "ofxMicroUI.h"
#include "ofxMicroUIElements.h"
#include "ofVideoPlayer.h"
#include "ofSoundPlayer.h"
#include "ofSystemUtils.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

ofxMicroUI::element::element(string & n, ofxMicroUI & ui) {
	setupElement(n, ui);
}

void ofxMicroUI::element::draw() {
	drawElement();
	if (useLabel) {
		drawLabel();
	}
}

void ofxMicroUI::element::checkMouse(int x, int y, bool first) {
	if (_ui->freeFlow) {
		
		if (rect.inside(x, y)) {
			wasPressed = true;
			setValFromMouse(x,y);
		} else {
			if (wasPressed) {
				wasPressed = false;
				setValFromMouse(x,y);
			}
		}
	} else {
		setValFromMouse(x, y);
	}
}

void ofxMicroUI::element::mouseRelease(int x, int y) {
	if (rect.inside(x, y)) {
		wasPressed = false;
	}
}


void ofxMicroUI::element::notify() {
//		cout << "notify " << name << endl;
//		cout << "useNotify " << useNotify << endl;
//		cout << "_ui->uiIsCreated " << _ui->uiIsCreated << endl;
	
	// && _settings->useNotify
	if (useNotify && _ui->uiIsCreated) {
		ofNotifyEvent(_ui->uiEvent, *this);
	}
}

void ofxMicroUI::element::drawLabel() {
	if (labelText != "") {
		ofSetColor(_settings->colorShadowLabel);
//			ofSetColor(0,120);
		
		if (_settings->useCustomFont) {
			_settings->font.drawString(labelText, rect.x + labelPos.x + 1, rect.y + labelPos.y + 1);
		} else {
			ofDrawBitmapString(labelText, rect.x + labelPos.x + 1, rect.y + labelPos.y + 1);
		}

		ofSetColor(getColorLabel());
//			ofSetColor(_settings->getColorLabel(glm::vec2(rect.x, rect.y)));
		if (_settings->useCustomFont) {
			_settings->font.drawString(labelText, rect.x + labelPos.x, rect.y + labelPos.y);
		} else {
			ofDrawBitmapString(labelText, rect.x + labelPos.x, rect.y + labelPos.y);
		}
	}
}

void ofxMicroUI::element::redrawElement() {
	haveToRedraw = false;
	ofDisableAlphaBlending();
	ofSetColor(0, 0);
	ofDrawRectangle(rect);

	ofEnableAlphaBlending();
//		ofSetColor(_settings->uiColorBg);
	ofSetColor(_ui->uiColorBg);
	ofDrawRectangle(rect);
	ofSetColor(255);
	draw();
}


void ofxMicroUI::element::setupElement(string & n, ofxMicroUI & ui, bool advance) {
	//cout << "setupElement :: " << n << endl;
	name = n;

	_ui = &ui;
	_settings = _ui->_settings;
	
	tag = _ui->tagOnNewElement;
	saveXml = _ui->saveXmlOnNewElement;
	
	if (rect.width < 1) {
		rect.width = _settings->elementRect.width;
	}
	if (rect.height < 1) {
		rect.height = _settings->elementRect.height;
	}

	// rect.position = glm::vec2(_ui->flowXY);
//        rect.setPosition(_ui->flowXY);
	rect.position.x = _ui->flowXY.x;
	rect.position.y = _ui->flowXY.y;
	
	if (_ui->useLabelOnNewElement) {
		labelText = n;
		
		// todo : settings getLabelPos, considering opentypefont.
		getLabelPos();
	}

	// this way settings knows the last element dimensions
	_ui->flowRect = rect;
	
	// not if element type is a group.
	if (advance) {
		// now it needs to check twice when flowing horizontal, like a radio.
		if (!_ui->advanceLayout()) {
			// unfortunate. rectangle uses ofPoint.
			// rect.position = glm::vec2(_ui->flowXY);
//                rect.setPosition(_ui->flowXY);
			rect.position.x = _ui->flowXY.x;
			rect.position.y = _ui->flowXY.y;
			// rect.position = ofPoint(_ui->flowXY);
			_ui->advanceLayout();
		}
	}
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

	((ofxMicroUI::slider2d*)elements.back())->redrawVal();
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






//
//ofxMicroUI::flipflop::flipflop(string & n, ofxMicroUI & ui, int & v) { //: group::group(n, ui)
//	_val = &v;
//	setupElement(n, ui, false);
//	bool saveState = _ui->useLabelOnNewElement;
//	_ui->useLabelOnNewElement = false;
//	_ui->setFlowVert(false);
//	for (int a=0; a<8; a++) {
//		string name = "b" + ofToString(a);
//		elements.push_back(new toggle (name, ui, vals[a], vals[a], true));
//	}
//	string name = "val";
//	elements.push_back(new inspector(name, ui));
//	elements.back()->rect.width = 40;
//	_ui->useLabelOnNewElement = saveState;
//	_ui->setFlowVert(true);
//}
//
//
//class flipflop : public group {
//public:
//	bool vals[8] = { false };
//	int *_val = NULL;
//	
//	flipflop() {};
//	flipflop(string & n, ofxMicroUI & ui, int & v);
//	
//
//	
//	void updateVal() override {
//		*_val = 0;
//		
//		std::bitset<8> bits;
//		for (int a=0; a<8; a++) {
//			bits[7-a] = vals[a];
//		}
//		*_val = bits.to_ulong();
//		((inspector*)elements.back())->set(ofToString(*_val));
////		cout << "updateVal from flipflop " << name << ":" << *_val << endl; //
//		redraw();
//		notify();
//	}
//};




/*
class vec3 : public group {
public:
	glm::vec3 * _val = NULL;

	vec3(string & n, ofxMicroUI & ui, glm::vec3 & v);
	
	// can't override, not existent in base class
	glm::vec3 getVal() {
		return *_val;
	}
	
	void set(glm::vec3 v) override {
		*_val = v;
		for (auto & e : elements) {
			if (e->name == "x") {
				e->set(_val->x);
			}
			if (e->name == "y") {
				e->set(_val->y);
			}
			if (e->name == "z") {
				e->set(_val->z);
			}
		}
	}
	
	void set(string s) override {
		vector<string> vals = ofSplitString(s, ", ");
		set(glm::vec3(ofToFloat(vals[0]), ofToFloat(vals[1]), ofToFloat(vals[2])));
	}
};
 
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

*/


void ofxMicroUI::input::checkMouse(int x, int y, bool first) {
	if (rect.inside(x, y)) {
		//https://openframeworks.cc/documentation/utils/ofSystemUtils/#show_ofSystemTextBoxDialog
		std::cout << "ofSystemTextBoxDialog " << *_val << std::endl;
		set(ofSystemTextBoxDialog("value", *_val));
	}
}


void ofxMicroUI::slider::setValFromMouse(int x, int y)  {
	if (ofGetKeyPressed(OF_KEY_COMMAND)) {
		std::string val { ofSystemTextBoxDialog(name, ofToString(getVal())) };
		if (val != "") {
			set(ofToFloat(val));
		}
	} else {
		int xx = ofClamp(x, rect.x, rect.x + rect.width);
		int yy = ofClamp(y, rect.y, rect.y + rect.height);
		glm::vec2 xy = glm::vec2 (xx,yy) - glm::vec2(rect.x, rect.y);
		glm::vec2 wh = glm::vec2 (rect.width, rect.height);
		glm::vec2 val = min + (max-min)*(xy/wh);
		if (isInt) {
			val = min + (max+1-min)*(xy/wh);
			val.x = ofClamp(val.x, min, max);
		}
		set(val.x);
	}
}

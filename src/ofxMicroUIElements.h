
// default element skeleton
class element {
public:
	
	element() {}
	~element() {}
	
	// new, 05 november 2019 - make fps work
	bool alwaysRedraw = false;
	
	
	bool * b = NULL;
	string * s = NULL;
	int * i = NULL;
	float * f = NULL;
//	float & ff;
	
	bool saveXml = true;
	
	ofxMicroUI * _ui = NULL;
	microUISettings * _settings = NULL;
	//bool alwaysRedraw = false;
	
	string name = "";
	string labelText = "";
	string tag = "";
	
	bool wasPressed = false;
	
	virtual void set(float v) {}
	virtual void set(int v) {}
	// only for radio by index
	virtual void set(unsigned int v) {}
	virtual void set(bool v) {}
	virtual void set(string v) {}
	//virtual void set(glm::vec2 v) {}
	virtual void set(glm::vec3 v) {}

	// this variables can be only set once per element kind. it can be a pointer.
	glm::vec2 labelPos = glm::vec2(0,0);
	
	// invisible rectangle, handles the mouse click
	ofRectangle rect;
	
	// visible and static
	ofRectangle rectBg = rect;
	
	// visible and changing according to the value
	ofRectangle rectVal = rect;

	ofColor getColorRainbow() {
		float hueStart = 120;
		float  h = rect.x / 9.0 + rect.y / 6.0 + hueStart;
		h = fmod(h, 255);
		return ofColor::fromHsb(h, 200, 200);
	}
	
	ofColor getColorBg() {
		return _settings->useBgRainbow ? getColorRainbow() : _settings->colorBg;
	}

	ofColor getColorLabel() {
		return _settings->useLabelRainbow ? getColorRainbow() : _settings->colorLabel;
//		return _settings->colorLabel;
	}
	
	bool haveToRedraw = false;
	
	virtual void redrawElement() {
		haveToRedraw = false;
		ofDisableAlphaBlending();
		ofSetColor(0, 0);
		ofDrawRectangle(rect);

		ofEnableAlphaBlending();
		ofSetColor(_settings->colorUIBg);
		ofDrawRectangle(rect);
		ofSetColor(255);
		draw();
	}
	
	virtual void redraw() {
		haveToRedraw = true;
		//_ui->somethingToRedraw = true;
		//_ui->redrawUI = true;
//		cout << "redraw " << name << endl;
	}
	
	virtual void drawLabel() {
//		if (!alwaysRedraw) {
//			cout << "drawLabel " << name << endl;
//		}
		if (labelText != "") {
			ofSetColor(getColorLabel());
			if (_settings->useCustomFont) {
				_settings->font.drawString(labelText, rect.x + labelPos.x, rect.y + labelPos.y);
			} else {
				ofDrawBitmapString(labelText, rect.x + labelPos.x, rect.y + labelPos.y);
			}
		}
	}
	
	virtual void drawElement() {}
	
	virtual void draw() {
//		cout << "primitive draw " << name << endl;
		drawElement();
		drawLabel();
	}
	
	virtual void setValFromMouse(int x, int y) {}
	
	//virtual void checkMouse(int x, int y, bool first = false) {
	virtual void checkMouse(int x, int y, bool first = false) {
		//cout << "this is event from element. not override" << endl;
		if (rect.inside(x, y)) {
			wasPressed = true;
			setValFromMouse(x,y);
		} else {
			if (wasPressed) {
				wasPressed = false;
				setValFromMouse(x,y);
			}
		}
	}
	
	virtual void mouseRelease(int x, int y) {
		if (rect.inside(x, y)) {
			wasPressed = false;
		}
	}
	
	virtual void notify() {
		ofNotifyEvent(_ui->uiEvent, *this);
	}
	

	
	void getLabelPos(bool isToggle = false) {
		if (labelPos == glm::vec2(0,0)) {
			
//			labelPos = glm::vec2(_settings->elementPadding, _settings->elementRect.height - 3);
			labelPos = glm::vec2(_settings->elementPadding,
								 _settings->elementRect.height - _settings->labelPosBaseline);

			if (isToggle) {
				labelPos.x = _settings->elementRect.height + _settings->elementPadding;
			}
		}
	}

	void setupElement(string & n, ofxMicroUI & ui, bool advance = true) {
		//cout << "setupElement :: " << n << endl;
		
		
		_ui = &ui;
		_settings = _ui->_settings;
		
		tag = _ui->tagOnNewElement;
		saveXml = _ui->saveXmlOnNewElement;
		
		//_settings = &s;
		if (rect.width < 1) {
			rect.width = _settings->elementRect.width;
		}
		if (rect.height < 1) {
			rect.height = _settings->elementRect.height;
		}

		rect.position = ofPoint(_ui->flowXY);
		
		name = n;
		if (_ui->useLabelOnNewElement) {
			labelText = n;
			
			// todo : settings getLabelPos, considering opentypefont.
			getLabelPos();
		}

		// this way settings knows the last element dimensions
		
		//_settings->flowRect = rect;
		_ui->flowRect = rect;
//		if (name == "play_0") {
//			cout << rect.width << endl;
//			cout << _ui->flowRect << endl;
//		}
		
		// not if element type is a group.
		if (advance) {
			// now it needs to check twice when flowing horizontal, like a radio.
			if (!_ui->advanceLayout()) {
				rect.position = ofPoint(_ui->flowXY);
				_ui->advanceLayout();
			}
		}
	}
};

class label : public element {
public:
	label(string & n, ofxMicroUI & ui) {
		setupElement(n, ui);
	}
};

class fps : public element {
public:
//	using label::label;
	fps(string & n, ofxMicroUI & ui) {
		alwaysRedraw = true;
		setupElement(n, ui);
	}
	void draw() override {
		labelText = ofToString(ofGetFrameRate());
		drawLabel();
		//redraw();
	}
};


class inspector : public label {
public:

	using label::label;

	void set(string s) override {
		//cout << "inspector set " << name << " :: " << s << " :: " << labelText << endl;
		if (labelText != s)
		{
			labelText = s;
			redraw();
		}
	}
};



// new prototype
/*
 This element is the base class for everything multiple. Radio, vec3, presets, future group of elements
 */

class group : public element {
public:
	vector <element *> elements;
	map <string, element *> elementsLookup;

	group() {};
	group(string & n, ofxMicroUI & ui, glm::vec3 & v) {
		setupElement(n, ui, false);
	}
	
	virtual void updateVal() {
//		cout << "updateVal in group :: " << name << endl;
	}

	void checkMouse(int x, int y, bool first = false) override {
//		cout << "group checkmouse" << endl;
		bool stuffChanged = false;
		
		if (rect.inside(x, y)) {
			wasPressed = true;
			stuffChanged = true;
			for (auto & e : elements) {
				//cout << "checkmouse inside elements group " << e->name << endl;
				e->checkMouse(x, y, first);
			}
		} else {
			if (wasPressed) {
				//cout << "mouse was inside and is not anymore" << endl;
				wasPressed = false;
				setValFromMouse(x,y);
				stuffChanged = true;
				
				for (auto & e : elements) {
					//cout << "checkmouse inside elements group " << e->name << endl;
					e->checkMouse(x, y, first);
				}
			}
		}
		// it works here but it can be more subtle in another place. less calls
		if (stuffChanged) {
			redraw();
			updateVal();
		}
	}
	
	void mouseRelease(int x, int y) override {
		if (rect.inside(x, y)) {
			for (auto & e : elements) {
				e->mouseRelease(x,y);
			}
			wasPressed = false;
		}
	}

	void draw() override {
//		ofSetColor(_settings->alertColor);
//		ofDrawRectangle(rect);

		for (auto & e : elements) {
			e->draw();
		}
	}
	
	// naming? this function updates the rect of the group to fit all elements and handle mouse events
	void groupResize() {
		if (elements.size()) {
			rect = elements[0]->rect;
		}
		for (auto & e : elements) {
			rect.growToInclude(e->rect);
			elementsLookup[e->name] = e;
		}
	}
	
	element * getElement(string n) {
		return elementsLookup.find(n) != elementsLookup.end() ? elementsLookup[n] : NULL;
	}
	
	void addElement(element * e) {
		elements.push_back(e);
		// elements lookup?
	}
};




class radio : public group {
public:
	
	std::function<void(string)> invokeString = NULL;

	string * _val = NULL;
	// to store the state variables of the children elements.
	map <string, bool>	pBool;

	bool useLabel = false;
	
//	using group::group;
	radio() {};
	radio(string & n, ofxMicroUI & ui, vector<string> items, string & v) { // : name(n)
		setupElement(n, ui, false);
		_val = &v;
		s = &v;
		set(*_val);

		if (ui.useLabelOnNewElement) {
			useLabel = true;
			addElement(new label(name, ui));
		}
		ui.useLabelOnNewElement = true;
		_ui->setFlowVert(false);
		for (auto & i : items) {
			bool val = false;
			addElement(new itemRadio(i, ui, val, pBool[i], false));
		}
		_ui->setFlowVert(true);
		groupResize();
		_ui->advanceLayout();
	}
	
	string getVal() {
		return *_val;
	}
	
	string getValByIndex(unsigned int index) {
		int i = useLabel ? index+1 : index;
		i = ofClamp(i, 0, elements.size()-1);
		return elements[i]->name;
	}
	
	void set(unsigned int index) override {
		int i = useLabel ? index+1 : index;
//		cout << "radio set by index :: " << name << " :: " << i << endl;
		i = ofClamp(i, 0, elements.size()-1);
		string s = elements[i]->name;
		set(s);
	}
	
	void set(string s) override {
		//cout << "radio set by string :: " << name << endl;
		//cout << "set radio: " << name << " : " << s << endl;
		if (*_val != s) {
			if (*_val != "") {
				// xaxa
				//(static_cast<booleano*>(elementsLookup[*_val]))->set(false);
				if (elementsLookup.find(*_val) != elementsLookup.end()) {
					((booleano*) elementsLookup[*_val])->set(false);
				}
			}
			if (s != "") {
				if (elementsLookup.find(s) != elementsLookup.end()) {
					((booleano*) elementsLookup[s])->set(true);
				}
			}
			if (elementsLookup.find(s) != elementsLookup.end()) {
				*_val = s;
			}
		} else {
			// same value as before, only notify
		}
		notify();
		
		if (!_settings->presetIsLoading) {
			if (invokeString != NULL) {
				invokeString(*_val);
			}
		}
		redraw();
	}
	
	//void checkMouse(int x, int y, bool first = false) override {
	void checkMouse(int x, int y, bool first = false) override {
		//cout << "checkMouse :: " << name << endl;
		if (rect.inside(x, y)) {
			for (auto & e : elements) {
				if (!dynamic_cast<label*>(e)) {
					if (e->rect.inside(x,y)) {
						set(e->name);
						break; // break the element loop too.
					}
				}
			}
		}
	}
};


// INCOMPLETE
class colorHsv : public group {
public:
	ofColor * _val = NULL;
	//float h, s, v;
	float sat;
	glm::vec2 xy;
	
	colorHsv(string & n, ofxMicroUI & ui, ofColor & c) {
		_val = &c;
		setupElement(n, ui, false);
		elements.push_back(new label(name, ui));
		string hName = "h";
		string sName = "s";
		string vName = "v";
		//elements.push_back(new slider(sName, ui, vals, s));
		//elements.push_back(new slider(vName, ui, vals, v));
//		elements.push_back(new fboElement(name, ui));
//		ofFbo * _f = &((fboElement*)elements.back())->fbo;
		elements.push_back(new slider2d(name, ui, xy));
		ofFbo * _f = &((slider2d*)elements.back())->fbo;
		
		_f->begin();
		ofClear(0);
		ofColor cor;
		int w = _f->getWidth();
		int h = _f->getHeight();
		for (int b=0; b<h; b++) {
			for (int a=0; a<w; a++) {
				int este = b*w + a;
				float hue = (255 * a / (float) w);
				cor = ofColor::fromHsb(hue, 255, b*255/h, 255);
				ofFill();
				ofSetColor(cor);
				ofDrawRectangle(a,b,1,1);
			}
		}
		_f->end();
		
		glm::vec3 vals = glm::vec3(0,255,127);
		elements.push_back(new slider(hName, ui, vals, sat));
		groupResize();
		redraw();
	}
	
	void updateVal() override {
		*_val = ofColor::fromHsb(xy.x * 255, sat, xy.y * 255);
		//cout << "OVERRIDE! " << endl;
	}

	ofColor getVal() {
		//*_val = ofColor::fromHsb(h, s, v);
		return *_val;
	}
	
	void set(glm::vec3 v) override {
		xy.x = v.x;
		xy.y = v.z;
		sat = v.y;
		updateVal();
	}
};


class vec3 : public group {
public:
	glm::vec3 * _val = NULL;

	vec3(string & n, ofxMicroUI & ui, glm::vec3 & v) {
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


class slider : public element {
public:
	float * _val = NULL;
	int * _valInt = NULL;
	
	float min = 0;
	float max = 1;
	bool isInt = false;

	slider(string & n, ofxMicroUI & ui, glm::vec3 val, float & v) { // : name(n)
		f = &v;
		//ff = v;
		setupElement(n, ui);
		_val = &v;
		rectVal = rectBg = rect;
		min = val.x;
		max = val.y;
		set(val.z);
	}
	
	slider(string & n, ofxMicroUI & ui, glm::vec3 val, int & v) { // : name(n)
		i = &v;
		isInt = true;
		setupElement(n, ui);
		_valInt = &v;
		rectVal = rectBg = rect;
		min = val.x;
		max = val.y;
		set(val.z);
	}
	
	float getVal() {
		if (_val != NULL) {
			return *_val;
		}
		if (_valInt != NULL) {
			return *_valInt;
		} 
		else {
			return 0.0f;
		}
	}
	
	void drawElement() override {
		ofSetColor(getColorBg());
		ofDrawRectangle(rectBg);
		//ofSetColor(80);
		ofSetColor(_settings->colorVal);
		ofDrawRectangle(rectVal);
	}
	
	void set(float v) override {
		//val = v;
		if (_val != NULL) {
			*_val = v;
			rectVal.width = ofMap(v, min, max, 0, rect.width);
			labelText = name + " " + ofToString(*_val);
		}
		if (_valInt != NULL) {
			*_valInt = v;
			rectVal.width = ofMap(*_valInt, min, max, 0, rect.width);
			labelText = name + " " + ofToString(*_valInt);
		}
		
		notify();
		redraw();
		// EVENT TEST
		
		//_settings->microUIEvent.e = *this;
		//element* e = this;
		//ofNotifyEvent(_settings->microUIEvent, e);
		//_settings->event
	}
	
	void setValFromMouse(int x, int y) override {
		int xx = ofClamp(x, rect.x, rect.x + rect.width);
		int yy = ofClamp(y, rect.y, rect.y + rect.height);
		ofPoint xy = ofPoint (xx,yy) - ofPoint(rect.x, rect.y);
		ofPoint wh = ofPoint (rect.width, rect.height);
		ofPoint val = min + (max-min)*(xy/wh);
		if (isInt) {
			val = min + (max+1-min)*(xy/wh);
			val.x = ofClamp(val.x, min, max);
		}
		set(val.x);
	}

};

// cannot cast later
//class booleano : virtual public element {
class booleano : public element {
public:
	bool * _val = NULL;
	
	// temporary until implementation of the elementKind.
	bool isToggle = false;
	
	booleano(){};
	booleano(string & n, ofxMicroUI & ui, bool val, bool & v, bool elementIsToggle = true) { //, bool useLabel = true
		// temporary
		isToggle = elementIsToggle;

		// I needed to declare this because it is called before setupElement, so the pointers are set.
		_ui = &ui;
		_settings = _ui->_settings;

		if (ui.useLabelOnNewElement) {
			// this is the size of the element according to the text size. it is called before setupElement so the rectangle can be forwarded to _settings to calculate the flow of the next element.
			int letterWidth = 0;
			if (_settings->useCustomFont) {
				letterWidth = _settings->font.getStringBoundingBox(n, 0, 0).width;
			} else {
				// each letter = 8 pixels
				letterWidth = ofUTF8Length(n) * 8;
			}
			rect.width = letterWidth + ui._settings->elementPadding * 2; // mais margem 5*2
		} else {
			rect.width = ui._settings->elementRect.height;
			labelText = "";
		}

		int rectValMargin = ui._settings->elementRect.height/4;
		
		if (isToggle) {
			// it needs more space for the checkbox
			if (ui.useLabelOnNewElement) {
				getLabelPos(true);
				rect.width += labelPos.x;
			}
			rectBg.width = rectBg.height = ui._settings->elementRect.height;
			rectVal.width = rectVal.height = ui._settings->elementRect.height - rectValMargin*2;
		}
		
		setupElement(n, ui);
		
		if (isToggle) {
			rectBg.position = rect.position;
			rectVal.position = rect.position + ofPoint(rectValMargin, rectValMargin);
		} else {
			rectVal = rect;
			rectBg = rect;
		}

		_val = &v;
		b = &v;
		set(val);
	}
	
	bool getVal() {
		return *_val;
	}
	
	void toggle() {
		//cout << "toggle " << name << endl;
		set(*_val ^ 1);
	}
	
	void set(bool v) override {
		//cout << "set booleano: " << name << " : " << v << endl;
		*_val = v;
		redraw();
		notify();
	}
	
	void setValFromMouse(int x, int y) override {
		if (wasPressed) {
			toggle();
		}
	}
	
	void checkMouse(int x, int y, bool first = false) override {
		if (rect.inside(x, y)) {
			if (!wasPressed) {
				toggle();
			}
			wasPressed = true;
		} else {
			wasPressed = false;
		}
	}
	
	void drawElement() override {
//		ofSetColor(_settings->alertColor);
//		ofDrawRectangle(rect);
		ofSetColor(getColorBg());
		ofDrawRectangle(rectBg);
		
		if (*_val) {
			ofSetColor(isToggle ? getColorLabel() : _settings->colorVal);
			ofDrawRectangle(rectVal);
		}
	}
};

class toggle : public booleano {
public:
	// inherit constructor from booleano
	using booleano::booleano;
};

class itemRadio : public booleano {
public:
	using booleano::booleano;
};


class image : virtual public element {
public:
	ofImage img;
	//image();
	image(string & n, ofxMicroUI & ui, string fileName) {
		img.load(fileName);
		rect.height = img.getHeight();
		setupElement(n, ui);
	}
	
	void draw() override {
		ofSetColor(255);
		img.draw(rect.x, rect.y);
	}
};

// naming? fboElement for now, not to confuse with internal fbo.
class fboElement : public element {
public:
	ofFbo fbo;
	// third parameter?
	fboElement(string & n, ofxMicroUI & ui) {
		rect.height = ui._settings->elementRect.height * 2 + ui._settings->elementSpacing;
		setupElement(n, ui);
		fbo.allocate(rect.width, rect.height, GL_RGBA);
		fbo.begin();
		ofClear(0,255);
		fbo.end();
	}

	void draw() override {
		ofSetColor(255);
		fbo.draw(rect.x, rect.y);
	}
};

class slider2d : public fboElement {
public:
	glm::vec2 * _val = NULL;

	// remove in near future
	glm::vec2 min = glm::vec2(0,0);
	glm::vec2 max = glm::vec2(1,1);
	
	using fboElement::fboElement;
	slider2d(string & n, ofxMicroUI & ui, glm::vec2 & v) : fboElement(n, ui) {
		_val = &v;
		//set(val);
	}
	
	void draw() override {
		if (fbo.isAllocated()) {
			fbo.draw(rect.x, rect.y);
		}
		float x = rect.x + _val->x * rect.width;
		float y = rect.y + _val->y * rect.height;
		ofDrawLine(x, rect.y, x, rect.y + rect.height);
		ofDrawLine(rect.x, y, rect.x + rect.width, y);
		ofDrawRectangle(x-3, y-3, 6, 6);
	}
	
	void set(glm::vec2 v) {
		if (_val != NULL) {
			*_val = v;
			labelText = name + " " + ofToString(*_val);
		}
		notify();
		redraw();
	}
	
	glm::vec2 getVal() {
		return *_val;
	}
	
	void setValFromMouse(int x, int y) override {
		int xx = ofClamp(x, rect.x, rect.x + rect.width);
		int yy = ofClamp(y, rect.y, rect.y + rect.height);
		glm::vec2 xy = glm::vec2 (xx,yy) - glm::vec2(rect.x, rect.y);
		glm::vec2 wh = glm::vec2 (rect.width, rect.height);
		glm::vec2 val = min + (max-min)*(xy/wh);
		set(val);
	}
};

class presetItem : public booleano { //tentei itemRadio aqui também.
public:
	ofImage img;
	ofFbo fbo;
	bool hasPreset = false;

	presetItem(string & n, ofxMicroUI & ui, bool val, bool & v) : booleano() {
		_ui = &ui;
		_settings = _ui->_settings;
		isToggle = false;
		setupPresetItem();
		setupElement(n, ui);
		rectVal = rect;
		rectVal.height = 10;
		rectBg = rect;
		_val = &v;
		set(val);
	}
	
	void setupPresetItem() {
		isToggle = false;
		rect.height = _settings->elementRect.height * 2 + _settings->elementSpacing;
		rect.width  = (_settings->elementRect.width - _settings->elementSpacing * 2) / 3 ;
		fbo.allocate(rect.width, rect.height, GL_RGBA);
		fbo.begin();
		ofClear(0,0);
		fbo.end();
	}


	void drawElement() override {
		ofSetColor(getColorBg());
		ofDrawRectangle(rectBg);
		ofSetColor(255);
		fbo.draw(rect.x, rect.y);
		if (hasPreset) {
			ofSetColor(_settings->alertColor);
			ofDrawRectangle(rect.x + 10, rect.y + 10,10,10);
		}
		if (*_val) {
			ofSetColor(isToggle ? getColorLabel() : _settings->colorVal);
			ofDrawRectangle(rectVal);
		}
		drawLabel();
	}
	
	void hasXmlCheck() {
		string path = _ui->getPresetPath();
		string dir = path + "/" + name;
		fbo.begin();
		ofClear(0,0);
		if (ofFile::doesFileExist(dir)) {
			string imageFile = dir+"/0.tif";
			if (ofFile::doesFileExist(imageFile)) {
				img.load(imageFile);
				ofSetColor(255);
				img.draw(0,0);
			}
			hasPreset = true;
		}
		else {
			hasPreset = false;
		}
		fbo.end();
		redraw();
	}
};

class presets : public radio {
public:
	ofFbo * _fbo = NULL;
	
	presets() {}
	presets(string & n, ofxMicroUI & ui, vector<string> items, string & v) { // : name(n)
		setupElement(n, ui, false);
		_val = &v;
		set(*_val);

		if (_ui->useLabelOnNewElement) {
			useLabel = true;
			addElement(new label(name, ui));
		}
		
		_ui->useLabelOnNewElement = true;
		_ui->setFlowVert(false);
		
		// setar no label aqui tb.
		for (auto & i : items) {
			bool val = false;
			addElement(new presetItem(i, ui, val, pBool[i]));
		}
		_ui->setFlowVert(true);
		groupResize();
		hasXmlCheck();
		_ui->advanceLayout();
	}
	
	void hasXmlCheck() {
		for (auto & e : elements) {
			// avoid label element. maybe change in the future.
			if (e->name != name) {
				((presetItem*)e)->hasXmlCheck();
			}
		}
	}
};


class presetRadio : public radio {
public:
	presetRadio(string & n, ofxMicroUI & ui, int number, string & v) {
		setupElement(n, ui, false);
		_val = &v;
		set(*_val);
		
		addElement(new label(name, ui));
		_ui->setFlowVert(false);
		_ui->useLabelOnNewElement = false;
		for (int a=0; a<number; a++) {
		//for (auto & i : items) {
			bool val = false;
			
			// XAXA do I need val and pbool at the same time? I think not.
			string i = ofToString(a);
			addElement(new toggle(i, ui, val, pBool[i], false));
		}
		_ui->useLabelOnNewElement = true;
		_ui->setFlowVert(true);
		groupResize();
		_ui->advanceLayout();
	}
	
	void set(string s) override {
		if (*_val != s) {
			// new mode. best performance.
			if (*_val != "") {
				((booleano*) elementsLookup[*_val])->set(false);
			}
			if (s != "") {
				((booleano*) elementsLookup[s])->set(true);
			}
			*_val = s;
			
		} else {
			// same value as before, only notify
		}
		
		if (!_settings->presetIsLoading) {
			if (invokeString != NULL) {
				string n = "_presets/" + s + ".xml";
				invokeString(n);
			}
		} else {
//			cout << "preset is loading "  <<  endl;
		}
	}
};


// 22 aug 2019 same as radio, only able to store the full path to file
class dirList : public radio {
public:
	string filePath = "";

	using radio::radio;
	
	string getFileName() {
		if (*_val != "") {
			return filePath + "/" + *_val;
		}
		else return "";
	}
	
	ofxMicroUI * _ui = NULL;
	
	// igual ao radio, rever com carinho depois
	void set(string s) override {
//		cout << "DIRLIST SET" << endl;
		if (*_val != s) {
			if (*_val != "") {
				if (elementsLookup.find(*_val) != elementsLookup.end()) {
					((booleano*) elementsLookup[*_val])->set(false);
				}
			}
			if (s != "") {
				if (elementsLookup.find(s) != elementsLookup.end()) {
					((booleano*) elementsLookup[s])->set(true);
				}
			}
			if (elementsLookup.find(s) != elementsLookup.end()) {
				*_val = s;
				

				if (_ui != NULL) {
					_ui->clear();
					// cout << getFileName() << endl;
					_ui->createFromText(getFileName() + ".txt");
				}
			}
		} else {
			// same value as before, only notify
		}
		// rever aqui onde melhor colocar
		updateVal();

		notify();
		redraw();
	}
};


class imageList : public dirList {
public:
	ofImage * _image = NULL;
	string loadedFile = "";
	
	//using dirList::dirList;
	imageList(string & n, ofxMicroUI & ui, vector<string> items, string & v, ofImage & i) : dirList(n, ui, items, v) {
		_image = &i;
	}

	void updateVal() override {
		string f = getFileName();
		if (_image != NULL && *s != "") {
			if (loadedFile != f) {
				_image->load(f);
				loadedFile = f;
				cout << "LOAD: " << f << endl;
			}
		}
	}
};

class videoList : public dirList {
public:
	ofVideoPlayer * _video = NULL;
	string loadedFile = "";
	
	using dirList::dirList;
	videoList(string & n, ofxMicroUI & ui, vector<string> items, string & v,
			  ofVideoPlayer & vid
			  )
	: dirList(n, ui, items, v) {
		_video = &vid;
	}

	void updateVal() override {
		string f = getFileName();
		if (_video != NULL && *s != "") {
			if (loadedFile != f) {
				_video->load(f);
				loadedFile = f;
				cout << "LOAD: " << f << endl;
			}
		}
	}
};

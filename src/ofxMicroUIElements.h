
// default element skeleton
class element {
public:
	
	ofxMicroUI * _ui = NULL;
	microUISettings * _settings = NULL;
	//bool alwaysRedraw = false;
	
	string name = "";
	string labelText = "";
	bool wasPressed = false;
	
	virtual void set(float v) {}
	virtual void set(int v) {}
	virtual void set(bool v) {}
	virtual void set(string v) {}
	//virtual void set(glm::vec2 v) {}
	virtual void set(glm::vec3 v) {}

	// this variables can be only set once per element kind. it can be a pointer.
	glm::vec2 labelPos;
	
	// invisible rectangle, handles the mouse click
	ofRectangle rect;
	
	// visible and static
	ofRectangle rectBg = rect;
	
	// visible and changing according to the value
	ofRectangle rectVal = rect;

	ofColor getColorRainbow() {
		float  h = rect.x / 7.0 + rect.y / 7.0;
		return ofColor::fromHsb(h, 200, 200);
	}
	
	ofColor getColorBg() {
		return _settings->useBgRainbow ? getColorRainbow() : _settings->colorBg;
	}

	ofColor getColorLabel() {
		return _settings->useLabelRainbow ? getColorRainbow() : _settings->colorLabel;
//		return _settings->colorLabel;
	}
	
	virtual void drawLabel() {
		if (labelText != "") {
			ofSetColor(getColorLabel());
			ofDrawBitmapString(labelText, rect.x + labelPos.x, rect.y + labelPos.y);
		}
	}
	
	virtual void drawElement() {}
	
	virtual void draw() {
		drawElement();
		drawLabel();
//		ofSetColor(255,0,70, 100);
//		ofDrawRectangle(rect);
	}
	
	virtual void setValFromMouse(int x, int y) {}
	
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
	
	element() {}
	~element() {}

	void setupElement(string & n, ofxMicroUI & ui, bool advance = true) {
		_ui = &ui;
		_settings = _ui->_settings;
		//_settings = &s;
		if (rect.width < 1) {
			rect.width = _settings->elementRect.width;
		}
		if (rect.height < 1) {
			rect.height = _settings->elementRect.height;
		}
		//rect = _settings->elementRect;
//		rect.position = ofPoint(_ui->flowXY);
		rect.position = ofPoint(_ui->flowXY);
		
		name = n;
		if (_ui->useLabelOnNewElement) {
			labelText = n;
		}

		// this way settings knows the last element dimensions
		
		//_settings->flowRect = rect;
		_ui->flowRect = rect;
		
		// todo : settings getLabelPos, considering opentypefont.
		labelPos = glm::vec2(_settings->elementPadding, _settings->elementRect.height - 3);

		// not if element type is a group.
		if (advance) {
			//_settings->advanceLayout();
			
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


class inspector : public label {
public:
	using label::label;

	void set(string s) override {
		//cout << "inspector set " << name << " :: " << s << " :: " << labelText << endl;
		if (labelText != s)
		{
			labelText = s;
			_ui->redrawUI = true;
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

	group() {}
	group(string & n, ofxMicroUI & ui, glm::vec3 & v) {
		setupElement(n, ui);
	}

	void checkMouse(int x, int y, bool first = false) override {
		if (rect.inside(x, y)) {
			wasPressed = true;
			for (auto & e : elements) {
				//cout << "checkmouse inside elements group " << e->name << endl;
				e->checkMouse(x, y, first);
			}
		} else {
			if (wasPressed) {
				//cout << "mouse was inside and is not anymore" << endl;
				wasPressed = false;
				setValFromMouse(x,y);
				
				for (auto & e : elements) {
					//cout << "checkmouse inside elements group " << e->name << endl;
					e->checkMouse(x, y, first);
				}
			}
		}
	}
	
	void mouseRelease(int x, int y) override {
		if (rect.inside(x, y))
		{
			for (auto & e : elements) {
				e->mouseRelease(x,y);
			}
			wasPressed = false;
		}
	}

	void draw() override {
//		ofSetColor(0,0,180);
//		ofDrawRectangle(rect);
		for (auto & e : elements) {
			e->draw();
		}
	}
	
	// naming? this function updates the rect of the group to fit all elements and handle mouse events
	void groupResize() {
		rect = elements[0]->rect;
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
	string * _val = NULL;
	// to store the state variables of the children elements.
	map <string, bool>	pBool;

	radio() {}
	radio(string & n, ofxMicroUI & ui, vector<string> items, string & v) { // : name(n)
		setupElement(n, ui, false);
		_val = &v;
		set(*_val);

		addElement(new label(name, ui));
		_ui->setFlowVert(false);
		for (auto & i : items) {
			bool val = false;
			addElement(new itemradio(i, ui, val, pBool[i], false));
		}
		_ui->setFlowVert(true);
		groupResize();
		_ui->advanceLayout();
	}
	
	string getVal() {
		return *_val;
	}
	
//	element * getElement(string n) {
//		return elementsLookup.find(n) != elementsLookup.end() ? elementsLookup[n] : NULL;
//	}
	
	void set(string s) override {
		//cout << "set: " << name << " : " << s << endl;
		if (*_val != s) {
			// new mode. best performance.
			if (*_val != "") {
				((toggle*) elementsLookup[*_val])->set(false);
			}
			if (s != "") {
				if (elementsLookup.find(s) != elementsLookup.end()) {
					((toggle*) elementsLookup[s])->set(true);
				}
			}
			if (elementsLookup.find(s) != elementsLookup.end()) {
				*_val = s;
			}
		} else {
			// same value as before, only notify
		}
		
		notify();
	}
	
	void checkMouse(int x, int y, bool first = false) override {
		if (rect.inside(x, y)) {
			for (auto & e : elements) {
				if (e->rect.inside(x,y)) {
					// fixed the name issue.
					set(e->name);
					break; // break the element loop too.
				}
			}
		}
	}
};


class vec3 : public group {
public:
	glm::vec3 * _val = NULL;

	vec3(string & n, ofxMicroUI & ui, glm::vec3 & v) {
		_val = &v;
		setupElement(n, ui);
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
		setupElement(n, ui);
		_val = &v;
		rectVal = rectBg = rect;
		min = val.x;
		max = val.y;
		set(val.z);
	}
	
	slider(string & n, ofxMicroUI & ui, glm::vec3 val, int & v) { // : name(n)
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

class booleano : public element {
public:
	bool * _val = NULL;
	
	// temporary until implementation of the elementKind.
	bool isToggle;
	
	booleano(string & n, ofxMicroUI & ui, bool val, bool & v, bool elementIsToggle = true, bool useLabel = true) {
		// temporary
		isToggle = elementIsToggle;
		// this is the size of the element according to the text size. it is called before setupElement so the rectangle can be forwarded to _settings to calculate the flow of the next element.
		int contaLetras = 0;
		
		ui.useLabelOnNewElement = useLabel;
		
		if (ui.useLabelOnNewElement) {
			contaLetras = ofUTF8Length(n);
			rect.width = contaLetras * 8 + ui._settings->elementPadding * 2; // mais margem 5*2
		} else {
//			cout << "!!! dont use label on " << n << endl;
			rect.width = ui._settings->elementRect.height;
			labelText = "";
		}

		setupElement(n, ui);

		
		rectVal = rect;

		if (isToggle) {
//			cout << "isToggle" << endl;
			// it needs more space for the checkbox
			labelPos.x = ui._settings->elementRect.height + ui._settings->elementPadding;
			//labelPos = glm::vec2(25, 16);
			if (_ui->useLabelOnNewElement) {
				rect.width += labelPos.x;
//				cout << rect.width << endl;
//				cout << "useLabel " << name << " :: " << labelPos.x << endl;
//				cout << "labelText:" << labelText << endl;
			}
			
			rectBg.position = rect.position;
			rectBg.width = rectBg.height = ui._settings->elementRect.height;
			int rectValMargin = 4; // distance difference between the big and small square (checkbox)
			rectVal.width = rectVal.height = ui._settings->elementRect.height - rectValMargin*2;
			rectVal.position = rect.position + ofPoint(rectValMargin, rectValMargin);

		} else {
			rectBg = rect;
		}

		_val = &v;
		set(val);
	}
	
	bool getVal() {
		return *_val;
	}
	
	void toggle() {
		set(*_val ^ 1);
	}
	
	void set(bool v) override {
		*_val = v;
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


class itemradio : public booleano {
public:
	using booleano::booleano;
};


class image : public element {
public:
	ofImage img;
	image(string & n, ofxMicroUI & ui, string fileName) {
		img.load(fileName);
		rect.height = img.getHeight();
		setupElement(n, ui);
		
	}
	
	void draw() override {
//		ofSetColor(255,0,0);
//		ofDrawRectangle(rect);
		ofSetColor(255);
		img.draw(rect.x, rect.y);
	}
};


//class itempreset : public toggle {
//public:
//	using toggle::toggle;
//};


class preset : public radio {
public:
	
	std::function<void(string)> invokeString = NULL;

	preset(string & n, ofxMicroUI & ui, int number, string & v) {
		setupElement(n, ui, false);
		_val = &v;
		set(*_val);
		
		addElement(new label(name, ui));
		_ui->setFlowVert(false);
		for (int a=0; a<number; a++) {
		//for (auto & i : items) {
			bool val = false;
			
			// XAXA do I need val and pbool at the same time? I think not.
			string i = ofToString(a);
			addElement(new toggle(i, ui, val, pBool[i], false, false));
		}
		_ui->setFlowVert(true);
		groupResize();
		_ui->advanceLayout();
	}
	
	void set(string s) override {
		if (*_val != s) {
			// new mode. best performance.
			if (*_val != "") {
				((toggle*) elementsLookup[*_val])->set(false);
			}
			if (s != "") {
				((toggle*) elementsLookup[s])->set(true);
			}
			*_val = s;
			
		} else {
			// same value as before, only notify
		}
		
		if (!_ui->presetIsLoading) {
//		if (!_settings->presetIsLoading) {
			if (invokeString != NULL) {
				string n = "_presets/" + s + ".xml";
				invokeString(n);
			}
		} else {
			cout << "preset is loading "  <<  endl;
		}
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


// 22 aug 2019 - testing. equal to the radio, only able to store the full path to file
class dirList : public radio {
public:
	string filePath = "";
	using radio::radio;
	
	string getFileName() {
		return filePath + "/" + *_val;
	}
};


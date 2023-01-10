// default element skeleton
class element {
public:
	
	element() {}
	virtual ~element() {}
	
	// new thing. lets test.
	
	// todo: transformar em operator
	virtual void copyValFrom(element & e) {
		cout << "primitive copyvalfrom " << e.name << " :: i am " << name << endl;
	}
	
//	virtual element operator = (element & e) {
//		cout << "operator! " << e.name << endl;
//		copyValFrom(e);
//	}
	
	// test 2021
	element(string & n, ofxMicroUI & ui) {
//        cout << "PRIMITIVE CONSTRUCTOR " << n << endl;
		setupElement(n, ui);
		afterSetup();
	}
	
	virtual void afterSetup() {
//        cout << "primitive feature afterSetup " << name << endl;
	}
	
	// compatibility with Player Led Prisma only
	virtual void resetDefault() {}
	
	// new, 05 november 2019 - make fps work
	bool alwaysRedraw = false;
	bool saveXml = true;

	bool wasPressed = false;
	bool haveToRedraw = false;
	
	bool * b = NULL;
	string * s = NULL;
	int * i = NULL;
	float * f = NULL;
	
	// 16 sep 2022
	bool useLabel = true;
//	float * ff = NULL;
//	float & ff;
//	auto * v = NULL;
//	float & ff;
	
	ofxMicroUI * _ui = NULL;
	microUISettings * _settings = NULL;
	//bool alwaysRedraw = false;
	
	string name = "";
	string labelText = "";
	string tag = "";
	
	virtual void set(float v) {}
	virtual void set(int v) {}
	// only for radio by index
	virtual void set(unsigned int v) {}
	virtual void set(bool v) {}
	virtual void set(string v) {}
	virtual void set(glm::vec2 v) {}
	virtual void set(glm::vec3 v) {}
	virtual void set(glm::vec4 v) {}

	
//	virtual void setValFrom(element & e) {}

	// this variables can be only set once per element kind. it can be a pointer.
	glm::vec2 labelPos = glm::vec2(0,0);
	
	// invisible rectangle, handles the mouse click
	ofRectangle rect;
	
	// visible and static
	ofRectangle rectBg = rect;
	
	// visible and changing according to the value
	ofRectangle rectVal = rect;

	// this can be static outside, maybe in config. next ones too.
	ofColor getColorRainbow() {
		float hueStart = 120;
		float  h = rect.x / 9.0 + rect.y / 6.0 + hueStart;
		h = fmod(h, 255);
		return ofColor::fromHsb(h, 200, 200);
	}

	ofColor getColorBg() {
		return _settings->useBgRainbow ? getColorRainbow() : _settings->colorBg;
	}
//
//	 deveria estar no settings mas tem o colorrainbow q eh de cada element
	ofColor getColorLabel() {
		return _settings->useLabelRainbow ? getColorRainbow() : _settings->colorLabel;
	}

	virtual void redrawElement() {
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
	
	virtual void redraw() {
		haveToRedraw = true;
	}
	
	virtual void drawLabel() {
//		if (!alwaysRedraw) {
//			cout << "drawLabel " << name << endl;
//		}
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
	
	virtual void drawElement() {}
	
	virtual void draw() {
//		cout << "primitive draw " << name << endl;
		drawElement();
		if (useLabel) {
			drawLabel();
		}
	}
	
	virtual void setValFromMouse(int x, int y) {}
	
	virtual void checkMouse(int x, int y, bool first = false) {
		//cout << "this is event from element. not override" << endl;
#ifdef FLOWFREE
		if (rect.inside(x, y)) {
			wasPressed = true;
			setValFromMouse(x,y);
		} else {
			if (wasPressed) {
				wasPressed = false;
				setValFromMouse(x,y);
			}
		}
#else
		setValFromMouse(x, y);
#endif

	}
	
	virtual void mouseRelease(int x, int y) {
		if (rect.inside(x, y)) {
			wasPressed = false;
		}
	}
	
	
	bool useNotify = true;
	virtual void notify() {
//		cout << "void notify" << endl;
//		if (_settings->presetIsLoading) {
//			_ui->loadingEvents.push_back(this);
//		} else
		
		if (useNotify && _ui->uiIsCreated) // && _settings->useNotify
		{
//			cout << "void notify inside" << endl;
//			cout << "microUI notify! " << _ui->uiName << "/" << name << endl;
			ofNotifyEvent(_ui->uiEvent, *this);
		}
//		ofNotifyEvent(_ui->uiEvent2, **this);
//		ofNotifyEvent(_ui->uiEvent3, this);

	}
	
	void getLabelPos(bool isToggle = false) {
		if (labelPos == glm::vec2(0,0)) {
			labelPos = glm::vec2(_settings->elementPadding,
								 _settings->elementRect.height - _settings->labelPosBaseline);

			if (isToggle) {
				labelPos.x = _settings->elementRect.height + _settings->elementPadding;
			}
		}
	}

	void setupElement(string & n, ofxMicroUI & ui, bool advance = true) {
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
};




class label : public element {
public:
	
	void copyValFrom(element & e) override {}
	
	label(string & n, ofxMicroUI & ui) : element::element(n, ui) {
		saveXml = false;
		s = &labelText;
	}
};

class color : public element {
public:
	ofColor cor;
	color(string & n, ofxMicroUI & ui, ofColor c) : element::element(n, ui), cor(c) {
		saveXml = false;
	}
	
	void draw() override {
		ofSetColor(cor);
		ofDrawRectangle(rect);
	}
};


class fps : virtual public element {
public:
	
//    void afterSetup() override {
//        saveXml = false;
//        alwaysRedraw = true;
//        s = &labelText;
//    }
//    using element::element;

	fps(string & n, ofxMicroUI & ui) : element::element(n, ui) {
		saveXml = false;
		alwaysRedraw = true;
		s = &labelText;
	}

	void draw() override {
		labelText = ofToString(ofGetFrameRate());
		drawLabel();
	}
};





class inspector : public label {
public:
	using label::label;
	
	void set(string s) override {
		if (labelText != s) {
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
	
	using element::element;

	void copyValFrom(element & e) override {
		group* grupo = (ofxMicroUI::group*)(&e);
		for (auto & el : elements) {
			element * elementoInterno = grupo->getElement(el->name);
			if (elementoInterno != NULL) {
				el->copyValFrom(*elementoInterno);
			}
		}
		updateVal();
		redraw();
	}

	virtual void updateVal() {
//		cout << "updateVal in group :: " << name << endl;
	}

	element * _mouseElement = NULL;
	
	// group
	void checkMouse(int x, int y, bool first = false) override {
#ifdef FLOWFREE
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
			notify();
		}
#else
		if (first) {
			_mouseElement = NULL;
			for (auto & e : elements) {
				if (e->rect.inside(x, y)) {
					_mouseElement = e;
					e->checkMouse(x, y, first);
					break;
				}
			}
		} else {
			if (_mouseElement != NULL) {
				_mouseElement->checkMouse(x, y, first);
			}
		}
//		cout << "checkMouse " << first << endl;

		redraw();
		updateVal();
//		notify();
#endif
	}
	
	void mouseRelease(int x, int y) override {
		if (rect.inside(x, y)) {
			for (auto & e : elements) {
				e->mouseRelease(x,y);
			}
			wasPressed = false;
			
			// novidade 22 de dezembro de 2019
//			updateVal();
		}
	}

	void draw() override {
//		ofSetColor(_settings->alertColor);
//		ofDrawRectangle(rect);

		ofSetColor(255);
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


class flipflop : public group {
public:
	bool vals[8] = { false };
	int *_val = NULL;
	
	flipflop() {};
	flipflop(string & n, ofxMicroUI & ui, int & v) { //: group::group(n, ui)
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
	
	void updateVal() override {
		*_val = 0;
		
		std::bitset<8> bits;
		for (int a=0; a<8; a++) {
			bits[7-a] = vals[a];
		}
		*_val = bits.to_ulong();
		((inspector*)elements.back())->set(ofToString(*_val));
//		cout << "updateVal from flipflop " << name << ":" << *_val << endl; //
		redraw();
		notify();
	}
};


// cannot cast later
//class booleano : virtual public element {
class booleano : public element {
public:
	bool * _val = NULL;
	bool defaultVal;
	
	// temporary until implementation of the elementKind.
	bool isToggle = false;
	bool isBang = false;
	
	void copyValFrom(element & e) override {
		set( dynamic_cast<ofxMicroUI::booleano*>(&e)->getVal() );
	}
	
	booleano(){};
	booleano(string & n, ofxMicroUI & ui, bool val, bool & v, bool elementIsToggle = true) { //, bool useLabel = true
		// temporary
		isToggle = elementIsToggle;

		// I needed to declare this because it is called before setupElement, so the pointers are set.
		_ui = &ui;
		_settings = _ui->_settings;

		if (ui.useLabelOnNewElement) {
			// this is the size of the element according to the text size. it is called before setupElement so the rectangle can be forwarded to _settings to calculate the flow of the next element.
			unsigned int letterWidth = 0;
			if (_settings->useCustomFont) {
				letterWidth = _settings->font.getStringBoundingBox(n, 0, 0).width;
			} else {
				// each letter = 8 pixels
				letterWidth = (unsigned int)ofUTF8Length(n) * 8;
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
			rectVal.position = rect.position + glm::vec2(rectValMargin, rectValMargin);
		} else {
			rectVal = rect;
			rectBg = rect;
		}

		_val = &v;
		b = &v;
		defaultVal = val;
		set(val);
	}
	
	bool getVal() {
		return *_val;
	}
	
	// remover
	void toggle() {
		flip();
	}

	void flip() {
		//cout << "toggle " << name << endl;
		set(*_val ^ 1);
	}
	
	void set(bool v) override {
//        cout << "set booleano: " << name << " : " << v << endl;
		if (!isBang) {
			*_val = v;
			redraw();
		}
		notify();
	}
	
	void setValFromMouse(int x, int y) override {
		if (wasPressed) {
			toggle();
		}
	}
	
	// booleano
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
//        ofSetColor(_settings->alertColor);
//        ofDrawRectangle(rect);
		ofSetColor(getColorBg());
		ofDrawRectangle(rectBg);
		
		if (*_val) {
			ofSetColor(isToggle ? getColorLabel() : _settings->colorVal);
			ofDrawRectangle(rectVal);
		}
	}
	
	void resetDefault() override {
		set(defaultVal);
	}
};

class toggle : public booleano {
public:
	using booleano::booleano;
};

class hold : public booleano {
public:
	using booleano::booleano;
	
	void mouseRelease(int x, int y) override {
		if (rect.inside(x, y)) {
			set(false);
			wasPressed = false;
		}
	}
	
	// 3 nov 2021 - set without notify. sera?
	void set(bool v) override {
		*_val = v;
		redraw();
	}
};

class itemRadio : public booleano {
public:
	using booleano::booleano;
};

// xaxa novidade teste
class varKindString {
public:
	std::function<void(string)> invokeString = NULL;
	string * _val = NULL;
	
	string getVal() {
		return *_val;
	}
};

//xaxa
class input : public element, public varKindString {
public:
	input(string & n, ofxMicroUI & ui, string & v) : element::element(n,ui) {
		_val = &v;
	}

	void checkMouse(int x, int y, bool first = false) override {
		if (rect.inside(x, y)) {
			//https://openframeworks.cc/documentation/utils/ofSystemUtils/#show_ofSystemTextBoxDialog
			cout << "ofSystemTextBoxDialog " << *_val << endl;
			set(ofSystemTextBoxDialog("value", *_val));
		}
	}
		
	void drawElement() override {
		ofPushStyle();
		ofNoFill();
		ofDrawRectangle(rect);
		ofPopStyle();
	}
	
	void set(string s) override {
		*_val = s;
		if (invokeString != NULL && !_settings->presetIsLoading) {
//			invokeString(s);
			invokeString(s);
		}
		labelText = s;
		notify();
		redraw();
	}
};


class radio : public group, public varKindString {
public:
//	std::function<void(string)> invokeString = NULL;
//	string * _val = NULL;
	
	void copyValFrom(element & e) override {
		set( dynamic_cast<ofxMicroUI::radio*>(&e)->getVal() );
	}
	
	// to store the state variables of the children elements.
	map <string, bool>	pBool;
	bool useLabel = false;
	
	using group::group;
//    using element::element;
	radio() {};
	radio(string & n, ofxMicroUI & ui, vector<string> _items, string & v) { // : name(n)
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
		for (auto & i : _items) {
			bool val = false;
			addElement(new itemRadio(i, ui, val, pBool[i], false));
			
			// teste 2020 nf
			elements.back()->useNotify = false;
		}
		_ui->setFlowVert(true);
		groupResize();
		_ui->advanceLayout();
	}
	
	string getValByIndex(unsigned int index) {
		int i = useLabel ? index+1 : index;
		i = ofClamp(i, 0, elements.size()-1);
		return elements[i]->name;
	}
	
//    void set(unsigned int index) override {
	void set(int index) override {
//        cout << "radio void set " << name << endl;
		int i = useLabel ? index+1 : index;
//		cout << "radio set by index :: " << name << " :: " << i << endl;
		
		// precisa fazer um caso diferente se tem label ou nao?
		i = ofClamp(i, 0, elements.size()-1);
		string s = elements[i]->name;
		set(s);
	}
	

	
	void set(string s) override {
//		cout << "radio set by string :: " << name << " :: " << s << endl;
		if (*_val != s) {
			// limpa o elemento selecionado.
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
			else {
				*_val = "";
			}
			
			if (elementsLookup.find(s) != elementsLookup.end()) {
				*_val = s;
			}
		} else {
			// same value as before, only notify
		}
		
		if (invokeString != NULL && !_settings->presetIsLoading) {
			invokeString(*_val);
		}
		
		// inserted 29 sep 2020 to work with new camList
		updateVal();
		notify();
		redraw();
	}
	
	// radio
#ifdef FLOWFREE
	void checkMouse(int x, int y, bool first = false) override {
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
#else
		void checkMouse(int x, int y, bool first = false) override {
			for (auto & e : elements) {
				if (!dynamic_cast<label*>(e)) {
					if (e->rect.inside(x,y)) {
						if (*_val != e->name || first)
						{
//							cout << "radio set " << name << " :: " << e->name << endl;
							set(e->name);
						}
						break; // break the element loop too.
					}
				}
			}
		}
#endif
};


// 22 aug 2019 same as radio, only able to store the full path to file
class dirList : public radio {
public:
	string filePath = "";
	ofxMicroUI * _uiScene = NULL;

	using radio::radio;
	
	string getFileName() {
		if (_val != NULL && *_val != "") {
			return filePath + "/" + *_val;
		}
		else return "";
	}
	
	void updateVal() override {
//		cout << "DIRLIST UPDATEVAL" << endl;
		if (_uiScene != NULL) {
			string newTextFile = getFileName() + ".txt";
			if (_uiScene->loadedTextFile != newTextFile) {
				_uiScene->clear();
				_uiScene->createFromText(getFileName() + ".txt");
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
	float alpha = 255;
	glm::vec2 xy = glm::vec2(0,1);
	bool useAlpha = false;
	bool useRange = false;
//	string labelName, slider2dName = "";
	string nameSat = "sat";
	float range = 0.0;

	// colorHsv(string & n, ofxMicroUI & ui, ofColor defaultColor, ofColor & c, bool _useAlpha = false) {
	colorHsv(string & n, ofxMicroUI & ui, ofColor defaultColor, ofColor & c, int kind = 0) {		
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

		((slider2d*)elements.back())->drawVal();
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

	ofColor getColor(float n) {
		return ofColor::fromHsb(fmod((xy.x + n*range) * 255.0, 255.0) , sat, xy.y * 255.0, useAlpha ? alpha : 255);
	}
	
	void updateVal() override {
		*_val = ofColor::fromHsb(xy.x * 255.0, sat, xy.y * 255.0, useAlpha ? alpha : 255);
//		cout << "updateVal from colorHsv " << name << ":" << *_val << endl;
		notify();
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
		// new test
		// redraw();
	}
	
	void set(glm::vec4 v) override {
		xy.x = v.x;
		xy.y = v.z;
		sat = v.y;
		alpha = v.w;
		updateVal();
		// new test
		redraw();
	}
	
	// teste
	void setFromColor(ofFloatColor c) {
//		cout << "setFromColor " << c << endl;
		xy.x = c.getHue();
		xy.y = c.getBrightness();
		sat = c.getSaturation();
		elements[2]->set(sat*255);
		updateVal();
		// new test
		redraw();
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
	float def = 0;
	bool isInt = false;

	void copyValFrom(element & e) override {
		set( dynamic_cast<ofxMicroUI::slider*>(&e)->getVal() );
	}
	
	slider(string & n, ofxMicroUI & ui, glm::vec3 val, float & v) : element::element(n, ui) { // : name(n)
		f = &v;
		//ff = v;
//		setupElement(n, ui);
		_val = &v;
//		ff = _val;
		rectVal = rectBg = rect;
		min = val.x;
		max = val.y;
		def = val.z;
		set(val.z);
		
		useLabel = _ui->useLabelOnNewElement;
	}
	
	slider(string & n, ofxMicroUI & ui, glm::vec3 val, int & v) : element::element(n, ui) { // : name(n)
		i = &v;
		isInt = true;
//		setupElement(n, ui);
		_valInt = &v;
		rectVal = rectBg = rect;
		min = val.x;
		max = val.y;
		def = val.z;
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
		// cout << "slider set!" << name << " :: " << v << endl;
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
	}
	
	void add(float v) {
		if (_valInt != NULL) {
			set(*_valInt + v);
		}
		if (_val != NULL) {
			set(*_val + v);
		}
	}
	
	void setValFromMouse(int x, int y) override {
		if (ofGetKeyPressed(OF_KEY_COMMAND)) {
			string val = ofSystemTextBoxDialog(name, ofToString(getVal()));
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
	
	void resetDefault() override {
		set(def);
	}

};



class image : virtual public element {
public:
	ofImage img;
	//image();
	image(string & n, ofxMicroUI & ui, string fileName) {
		saveXml = false;
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
	int rows = 2;
	fboElement(string & n, ofxMicroUI & ui, int r = 2) : rows(r) {
		// cout << "fboElement setup " << name << endl;
//        int rows = 3;
		rect.height = ui._settings->elementRect.height * rows + (ui._settings->elementSpacing)*(rows-1);
		setupElement(n, ui);

		// cout << "setup fbo element " << name << endl;
		// cout << rect << endl;
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


class adsr : public fboElement {
public:
	using fboElement::fboElement;
	ofFbo fboData;
	
	vector <glm::vec2> points;
	
	adsr(string & n, ofxMicroUI & ui, glm::vec2 & v) : fboElement(n, ui) {
		fboData.allocate(fbo.getWidth(), fbo.getHeight(), GL_RGBA);
		fboData.begin();
		ofClear(0,0);
		fboData.end();
	}
	
   void draw() override {
	   ofSetColor(255,0,0);
	   ofDrawRectangle(rect);
	   ofSetColor(255);
	   fboData.draw(rect.x, rect.y);
   }
   
   // test 3 sep 2020 miaw colorPalette
   virtual void afterSet() {}
   
   void drawVal() {
	   fboData.begin();
	   
	   ofClear(0,255);
	   if (fbo.isAllocated()) {
		   fbo.draw(0,0);
	   }
	   
	   ofSetColor(255);
	   for (auto & p : points) {
		   ofDrawRectangle(p.x, p.y,10,10);
	   }

	   fboData.end();
   }
	
	void setValFromMouse(int x, int y) override {
		int xx = ofClamp(x, rect.x, rect.x + rect.width);
		int yy = ofClamp(y, rect.y, rect.y + rect.height);
		glm::vec2 xy = glm::vec2 (xx,yy) - glm::vec2(rect.x, rect.y);
//		glm::vec2 wh = glm::vec2 (rect.width, rect.height);
		points.push_back(xy);
		drawVal();
		notify();
		redraw();
	}

};


class slider2d : public fboElement {
public:
	glm::vec2 * _val = NULL;
// estes dois, teste.
//	glm::vec2 valFloat = glm::vec2(0.5, 0.5);
//	glm::vec2 ranges = glm::vec2(1.0, 1.0);

	// remove in near future
	glm::vec2 min = glm::vec2(0,0);
	glm::vec2 max = glm::vec2(1,1);
	ofFbo fboData;
	
	void copyValFrom(element & e) override {
		set( dynamic_cast<ofxMicroUI::slider2d*>(&e)->getVal() );
	}

	using fboElement::fboElement;
	
	slider2d(string & n, ofxMicroUI & ui, glm::vec2 & v, int rows = 2) : fboElement(n, ui, rows) {
		// novidade
//		setupElement(n, ui);
//		cout << "new slider2d name = " << name << endl;
		_val = &v;

		fboData.allocate(fbo.getWidth(), fbo.getHeight(), GL_RGBA);
		fboData.begin();
		ofClear(0,0);
		fboData.end();
		//set(val);
	}
	

	
	void draw() override {
		ofSetColor(255);
		drawVal();
		fboData.draw(rect.x, rect.y);
	}
	
	// test 3 sep 2020 miaw colorPalette
	virtual void afterSet() {}
	
	void drawVal() {
		fboData.begin();
		ofClear(0,255);
		if (fbo.isAllocated()) {
		 	fbo.draw(0,0);
		}
 //		float x = _val->x * rect.width;
 //		float y = _val->y * rect.height;
		
 		float x = ofMap(_val->x, min.x, max.x, 0, rect.width);
 		float y = ofMap(_val->y, min.y, max.y, 0, rect.height);
 		ofDrawLine(x, 0, x,  rect.height);
 		ofDrawLine(0, y, rect.width, y);
 		ofDrawRectangle(x-3, y-3, 6, 6);
		fboData.end();
	}

	void set(glm::vec2 v) override {
		if (_val != NULL) {
			*_val = v;
			labelText = name + " " + ofToString(*_val);
		}
		afterSet();

		drawVal();

		notify();
		redraw();
	}
	
	void set(string v) override {
		vector <string> cols = ofSplitString(v, " ");
		set(glm::vec2(ofToFloat(cols[0]), ofToFloat(cols[1])));
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



class colorPalette : public slider2d {
public:
	ofColor * _colorVal = NULL;
//	ofColor * _val = NULL;
	vector<vector<ofColor> > paletas;
	using slider2d::slider2d;

	void afterSet() {
		updateColor();
//		getColor(0);
	}
	
	void updateColor(float q = 0) {
		if (paletas.size()) {
			float x = _val->x;
			float y = fmod(_val->y+q, 1.0);
//			int qualPaleta = MIN(paletas.size()-1, x * paletas.size());
			int qualPaleta = fmod(x, paletas.size());
			int qualCor = int(y * paletas[qualPaleta].size()) % paletas[qualPaleta].size();
			*_colorVal = paletas[qualPaleta][qualCor];
		}
	}

	ofColor getColor(float q = 0) {
		if (paletas.size()) {
			updateColor(q);
			return *_colorVal;
		} else {
			// only to satisfy compiler.
			return ofColor(0);
		}
	}

	ofColor getColorByIndex(int i) {
		float x = _val->x;
		int qualPaleta = fmod(x, paletas.size());
		int qualCor = i % paletas[qualPaleta].size();
		return paletas[qualPaleta][qualCor];
	}

	unsigned int getPaletteSize(int p) {
		return (unsigned int)paletas[p].size();
	}

	unsigned int getPaletteSize() {
		float x = _val->x;
		int qualPaleta = fmod(x, paletas.size());
		return (unsigned int)paletas[qualPaleta].size();
	}
	
	void loadPalettes(string file) {
		if (ofFile::doesFileExist(file)) {
			paletas.clear();
			vector <string> allLines = ofxMicroUI::textToVector(file);
			for (auto & line : allLines) {
				if (line != "") {
					vector<ofColor> paletaTemporaria;
					for (auto & corString : ofSplitString(line, " ")) {
						if (corString.size() > 1) {
							string corhex = corString.size() == 6 ? corString : corString.substr(1);
							ofColor corzinha = ofColor::fromHex(ofHexToInt(corhex));
							paletaTemporaria.push_back(corzinha);
						}
					}
					paletas.push_back(move(paletaTemporaria));
				}
			}
			ofColor cor;
			int w = fbo.getWidth();
			int h = fbo.getHeight();
			fbo.begin();

			if (paletas.size() > 0) {
				for (int a=0; a<w; a++) {
					for (int b=0; b<h; b++) {
						unsigned int qualPaleta = (unsigned int)paletas.size() * a / w;
						unsigned int qualCor = (unsigned int)paletas[qualPaleta].size() * b / h;
						cor = paletas[qualPaleta][qualCor];
						ofFill();
						ofSetColor(cor);
						ofDrawRectangle(a,b,1,1);
					}
				}
			} else {
				for (int a=0; a<w; a++) {
					for (int b=0; b<h; b++) {
						cor = ofColor(0);
						ofFill();
						ofSetColor(cor);
						ofDrawRectangle(a,b,1,1);
					}
				}
			}
			
			fbo.end();
			
			max.x = paletas.size();
		}
		redraw();
	}
	
};


class presetItem : public booleano { //tentei itemRadio aqui também.
public:
	ofImage img;
	ofFbo fbo;
	bool hasPreset = false;
	float border = 1;

	presetItem(string & n, ofxMicroUI & ui, bool val, bool & v) : booleano() {
		_ui = &ui;
		_settings = _ui->_settings;
		isToggle = false;
		setupPresetItem();
		setupElement(n, ui);
		rectVal = rect;
//		rectVal.height = 10;
		rectVal.x += border * .5;
		rectVal.y += border * .5;
		rectVal.width -= border;
		rectVal.height -= border;
		rectBg = rect;
		
		labelPos.x = 8;
		
		_val = &v;
		set(val);
	}
	
	void setupPresetItem() {
		isToggle = false;
		
		// presetSize.
		rect.height = _settings->elementRect.height * 2 + _settings->elementSpacing;
		rect.width  = (_settings->elementRect.width - _settings->elementSpacing * 2) / 3 ;

		rect.height = _settings->elementRect.height * _settings->presetHeight + _settings->elementSpacing;
		int cols = _settings->presetCols;
		rect.width  = (_settings->elementRect.width - _settings->elementSpacing * (cols-1)) / cols ;


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
//			ofSetColor(_settings->alertColor);
			ofSetColor(_settings->colorVal);
			
			ofNoFill();
			for (auto & y : { 10, 13, 16 }) {
				ofDrawLine(rect.x + rect.width - 20, rect.y + y, rect.x + rect.width - 10, rect.y + y);
			}
			ofFill();
//			ofDrawRectangle(rect.x + 10, rect.y + 10,10,10);
		}
		if (*_val) {
//			ofSetColor(isToggle ? getColorLabel() : _settings->colorVal);
			ofSetColor(isToggle ? getColorLabel() : _settings->alertColor);
			ofPushStyle();
			ofNoFill();
			ofSetLineWidth(border);
			ofDrawRectangle(rectVal);
			ofPopStyle();
		}
		drawLabel();
	}
	
	void hasXmlCheck() {
		//if (_ui != NULL)
		{
			
			string path = _ui->getPresetPath();
			if (path != "") {
				string dir = path + "/" + name;
				fbo.begin();
				ofClear(0,0);
				if (ofFile::doesFileExist(dir)) {
					string imageFile = dir+"/0.tif";
					string imageFile2 = dir+"/0.png";
					if (ofFile::doesFileExist(imageFile)) {
						img.load(imageFile);
						ofSetColor(255);
						img.draw(0,0);
					}
					
					else if (ofFile::doesFileExist(imageFile2)) {
						img.load(imageFile2);
						ofSetColor(255);
						img.draw(0,0);
					}

					string textFile = dir+"/0.txt";
					if (ofFile::doesFileExist(textFile)) {
						string texto = ofxMicroUI::textToString(textFile);
						glm::vec2 pos = glm::vec2(labelPos.x, labelPos.y + 16);
						_settings->drawLabel(texto, pos);
					}
					
					hasPreset = true;
				}
				else {
					hasPreset = false;
				}
				fbo.end();
				redraw();
			}
		}
	}
};


// no need anymore. using plain radio now
//class presetsLoad : public radio {
//    using radio::radio;
//};
//class presetsSave : public radio {
//    using radio::radio;
//};


class presets : public radio {
public:
	// removi 30 oct 2021, coloquei global, assim tenho como setar antes da ui carregar
//	ofFbo * _fbo = NULL;
	
	// 12 oct 2020 - Switcher
	vector<string> items;
	map <string, int> itemPosition;
	
	presets() {}
	presets(string & n, ofxMicroUI & ui, vector<string> _items, string & v) { // : name(n)
		
		items = _items;
		int index = 0;
		for (auto & i : _items) {
			itemPosition[i] = index;
			index++;
		}
		
		setupElement(n, ui, false);
		_val = &v;
		s = &v;

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
			elements.back()->useNotify = false;
		}
		_ui->setFlowVert(true);
		groupResize();
		hasXmlCheck();
		_ui->advanceLayout();
	}
	
	// maybe implement in radio too?
	void cycle(unsigned int offset, bool clamp = false) {
		int index = itemPosition[*_val];
		index += offset;
		if (clamp) {
			index = ofClamp(index, 0, items.size()-1);
		} else {
			if (index < 0) {
				index += items.size();
			}
			else if (index >= int(items.size())) {
				index -= items.size();
			}
		}
		string name = items[index];
		set(name);
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


//class presetRadio : public radio {
//public:
//	presetRadio(string & n, ofxMicroUI & ui, int number, string & v) {
//		setupElement(n, ui, false);
//		_val = &v;
//		set(*_val);
//
//		addElement(new label(name, ui));
//		_ui->setFlowVert(false);
//		_ui->useLabelOnNewElement = false;
//		for (int a=0; a<number; a++) {
//		//for (auto & i : items) {
//			bool val = false;
//
//			// XAXA do I need val and pbool at the same time? I think not.
//			string i = ofToString(a);
//			addElement(new toggle(i, ui, val, pBool[i], false));
//		}
//		_ui->useLabelOnNewElement = true;
//		_ui->setFlowVert(true);
//		groupResize();
//		_ui->advanceLayout();
//	}
//
//	void set(string s) override {
//		if (*_val != s) {
//			// new mode. best performance.
//			if (*_val != "") {
//				((booleano*) elementsLookup[*_val])->set(false);
//			}
//			if (s != "") {
//				((booleano*) elementsLookup[s])->set(true);
//			}
//			*_val = s;
//
//		} else {
//			// same value as before, only notify
//		}
//
//		if (!_settings->presetIsLoading) {
//			if (invokeString != NULL) {
//				string n = "_presets/" + s + ".xml";
////				cout << n << endl;
//				invokeString(n);
//			}
//		} else {
////			cout << "preset is loading "  <<  endl;
//		}
//	}
//};



class bar : public element {
public:
	float val;
	
	bar(string & n, ofxMicroUI & ui) : element::element(n, ui){
		saveXml = false;
//		setupElement(n, ui);
		rectBg = rect;
		labelText = "";
		s = &labelText;
	}

	void set(float v) override {
		val = ofClamp(v, 0, 1);
		rectVal = ofRectangle(rect.x, rect.y, rect.width * val, rect.height);
		redraw();
	}
	
	void set(string s) override {
		if (labelText != s) {
			labelText = s;
			redraw();
		}
	}
	
	void drawElement() override {
		ofSetColor(getColorBg());
		ofDrawRectangle(rectBg);
		ofSetColor(_settings->colorVal);
		ofDrawRectangle(rectVal);
	}
};




class imageList : public dirList {
public:
	ofImage i;
	ofImage * _image = NULL;
	string loadedFile = "";

	bool disableArb = false;
	
	// using dirList::dirList;
	imageList(string & n, ofxMicroUI & ui, vector<string> items, string & v, ofImage & i) :
	dirList(n, ui, items, v) {
		_image = &i;
	}

	void updateVal() override {
		string f = getFileName();
		if (*s == "_" || *s == "") {
			if (_image->isAllocated()) {
				cout << "unload img" << endl;
				_image->clear();
				loadedFile = "";
			}
//			_image->unbind();
		}
		else if (_image != NULL && *s != "") {
			if (loadedFile != f) {
				if (disableArb) {
					ofDisableArbTex();
				}
				_image->load(f);
				loadedFile = f;
				
				if (disableArb) {
					ofEnableArbTex();
				}
//				cout << "LOAD imageList: " << name << " : " << f << endl;
			}
		}
	}
};

class videoList : public dirList {
public:
	ofVideoPlayer * _video = NULL;
	string loadedFile = "";
	
	// using dirList::dirList;
	videoList(string & n, ofxMicroUI & ui, vector<string> items, string & v, ofVideoPlayer & vid)
	: dirList(n, ui, items, v) {
		_video = &vid;
	}

	void updateVal() override {
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
};


class audioList : public dirList {
public:
	ofSoundPlayer * _sound = NULL;
	string loadedFile = "";
	
	// using dirList::dirList;
	audioList(string & n, ofxMicroUI & ui, vector<string> items, string & v, ofSoundPlayer & sound)
	: dirList(n, ui, items, v) {
		_sound = &sound;
	}

	void updateVal() override {
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
};


class textList : public dirList {
public:
//	string text = "";
	string * _text = NULL;
	string loadedFile = "";
	
//	using dirList::dirList;
	textList(string & n, ofxMicroUI & ui, vector<string> items, string & v, string & t) :
	dirList(n, ui, items, v) {
		_text = &t;
	}

	void updateVal() override {
		string f = getFileName();
		if (*s != "" && loadedFile != f) {
			*_text = ofBufferFromFile(f).getText();
			loadedFile = f;
			cout << "LOAD textList: " << name << " : " << f << endl;
		}
	}
};

class fontList : public dirList {
public:
//	string text = "";
	ofTrueTypeFont * _font = NULL;
	string loadedFile = "";
	int size = 40;
	int * _size = NULL;
	
//	using dirList::dirList;
	fontList(string & n, ofxMicroUI & ui, vector<string> items, string & v, ofTrueTypeFont & f) :
	dirList(n, ui, items, v) {
		_font = &f;
	}

	void updateVal() override {
		string f = getFileName();
		if (*s != "" && loadedFile != f) {
//			_font->load(f, size);
			if (_size != NULL) {
				size = *_size;
			}
			_font->load(f, size, true, true, true);
			loadedFile = f;
			cout << "LOAD fontList: " << name << " : " << f << endl;
		}
	}
	
	void reload() {
		cout << "font reload()" << endl;
		cout << getFileName() << endl;
		cout << loadedFile << endl;
//		_font->load(loadedFile, size);
		if (loadedFile != "") {
			_font->load(loadedFile, size, true, true, true);
		}
	}
};






class camList : public radio {
public:
	ofVideoGrabber * _cam = NULL;
	map <string, int> camIDs;
	vector <string> getCams(ofVideoGrabber & c) {
		_cam = &c;
		vector <string> opcoes;
		opcoes.push_back("_");
		if (_cam != NULL) {
			for (auto & d : _cam->listDevices()) {
				opcoes.push_back(d.deviceName);
			}
		}
		return opcoes;
	}

	camList(string & n, ofxMicroUI & ui, string & v, ofVideoGrabber & c) :
	radio(n, ui, getCams(c), v), _cam(&c) {
		_cam = &c;
		for (auto & d : _cam->listDevices()) {
			camIDs[d.deviceName] = d.id;
		}
	}
	
	int width = 1280;
	int height = 720;
	int frameRate = 30;

	void updateVal() override {
		if (*s != "") {
			if (*s == "_") {
				_cam->close();
			} else {
				int id = camIDs[*s];
				cout << "CAMLIST updateVal(): " << name << " : " << *s << " : " << id << endl;
				cout << " width: " << width << " height: " << height << " fps: " << frameRate << endl;
				if (_cam->isInitialized()) {
					_cam->close();
				}
//				_cam->setPixelFormat(OF_PIXELS_BGR);
				_cam->setDeviceID(id);
				if (frameRate > 0) {
					_cam->setDesiredFrameRate(frameRate);
				}
				_cam->initGrabber(width, height);
			}
		}
	}
};

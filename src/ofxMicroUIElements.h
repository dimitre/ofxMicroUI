
class element {
public:
	virtual ~element() {}
	element() {}
	element( std::string & n, ofxMicroUI & ui);

	// todo: transformar em operator
	//cout << "primitive element copyValFrom " << e.name << " :: i am " << name << endl;
//	virtual void copyValFrom(element & e) {}
	virtual void copyValFrom(element & ) {}

	bool alwaysRedraw { false };
	bool saveXml { true };
	bool useLabel { true };

	bool * b { nullptr };
	std::string * s { nullptr };
	int * i { nullptr };
	float * f { nullptr };

	ofxMicroUI * _ui { nullptr };
	microUISettings * _settings { nullptr };

	std::string name { "" };
	std::string labelText { "" };
	std::string tag { "" };

	bool wasPressed { false };
	bool haveToRedraw { false };

	// compatibility with Player Led Prisma only
	virtual void resetDefault() {}

//	virtual void set(std::any v) {
//		cout << typeid(v).name() << endl;
//	}
	// virtual void set(float v, bool normalized = false) {}
	virtual void set(float, bool = false) {}
	virtual void set(int) {}
	// only for radio by index
	virtual void set(unsigned int) {}
	virtual void set(bool) {}
	virtual void set(const std::string &) {}
	virtual void set(glm::vec2) {}
	virtual void set(glm::vec3) {}
	virtual void set(glm::vec4) {}

//	virtual void setValFrom(element & e) {}

	// this variables can be only set once per element kind. it can be a pointer.
	glm::vec2 labelPos { 0, 0 };

	ofRectangle rect { 0, 0, 0, 0 }; // invisible rectangle, handles the mouse click
	ofRectangle rectBg; // visible and static
	ofRectangle rectVal; // visible and changing according to the value

	ofColor getColorBg() {
		return _settings->getColorBg({rect.x, rect.y});
	}

	ofColor getColorLabel() {
		return _settings->getColorLabel({rect.x, rect.y});
	}

	void redraw() {
		haveToRedraw = true;
	}

	// mover pro settings
	void getLabelPos(bool isToggle = false) {
		if (labelPos == glm::vec2{ 0, 0 }) {
			labelPos = { _settings->elementPadding, _settings->elementRect.height - _settings->labelPosBaseline };

			if (isToggle) {
				labelPos.x = _settings->elementRect.height + _settings->elementPadding;
			}
		}
	}

	bool useNotify { true };
	void notify();
	void redrawElement();
	void drawLabel();
	void setupElement(std::string & n, ofxMicroUI & ui, bool advance = true);

	virtual void drawElement() {}
	virtual void draw();
	virtual void setValFromMouse(int, int) {}
	virtual void checkMouse(int x, int y, bool first = false);

	virtual void mouseRelease(int x, int y);
};


class label : public element {
public:
	label(std::string & n, ofxMicroUI & ui) : element::element(n, ui) {
		saveXml = false;
		s = &labelText;
	}
};


class inspector : public label {
public:
	using label::label;

	void set(const std::string & s) override {
		if (labelText != s) {
			labelText = s;
			redraw();
		}
	}
};


class ellapsed : virtual public label {
public:
	using label::label;

	ellapsed(std::string & n, ofxMicroUI & ui) : label::label(n, ui) {
//		alwaysRedraw = true;
	}

	int lastSec = 0;

	void draw() override {
		if (int(ofGetElapsedTimef()) != lastSec) {
			lastSec = int(ofGetElapsedTimef());
			labelText = ofToString(lastSec);
			redraw();
		}
		drawLabel();
	}
};




class fps : virtual public label {
public:
	using label::label;
	fps(std::string & n, ofxMicroUI & ui) : label::label(n, ui) {
		alwaysRedraw = true;
	}

	void draw() override {
		labelText = ofToString(ofGetFrameRate());
		drawLabel();
	}
};


class color : public element {
public:
	ofColor cor;
	color(std::string & n, ofxMicroUI & ui, ofColor c) : element::element(n, ui), cor(c) {
		saveXml = false;
	}

	void draw() override {
		ofSetColor(cor);
		ofDrawRectangle(rect);
	}
};


// everything that is a group of elements, like radio derives from this
class group : public element {
public:
	std::vector <element *> elements;
	std::unordered_map <std::string, element *> elementsLookup;
	element * _mouseElement = nullptr;

	using element::element;

	void copyValFrom(element & e) override;

	virtual void updateVal() {
//		cout << "updateVal in group :: " << name << endl;
	}

	void checkMouse(int x, int y, bool first = false) override {
		bool stuffChanged = false;

		if (_ui->freeFlow) {
			if (rect.inside(x, y)) {
				wasPressed = true;
				stuffChanged = true;
				for (auto & e : elements) {
					e->checkMouse(x, y, first);
				}
			} else {
				if (wasPressed) { //mouse was inside and is not anymore
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
		} else {
			if (first) {
				_mouseElement = nullptr;
				for (auto & e : elements) {
					if (e->rect.inside(x, y)) {
						_mouseElement = e;
						e->checkMouse(x, y, first);
						stuffChanged = true;
						break;
					}
				}
			} else {
				if (_mouseElement != nullptr) {
					_mouseElement->checkMouse(x, y, first);
					stuffChanged = true;
				}
			}
			//		cout << "checkMouse " << first << endl;
			if (stuffChanged) {
				redraw();
				updateVal();
				notify();
			}

//			redraw();
//			updateVal();
			//		notify();
		}
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
//		ofSetColor(255, 0, 70);
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

	element * getElement(const std::string & n) {
		return elementsLookup.find(n) != elementsLookup.end() ? elementsLookup[n] : nullptr;
	}

	void addElement(element * e) {
		elements.push_back(e);
		// elements lookup?
	}
};


class groupSave : public group {
//	using group::group;
};

// cannot cast later
//class booleano : virtual public element {
class booleano : public element {
public:
	bool * _val = nullptr;
	bool defaultVal;

	// temporary until implementation of the elementKind.
	bool isToggle = false;
	bool isBang = false;

	void setBang() {
		isBang = true;
		saveXml = false;
	}

	void copyValFrom(element & e) override {
		set( dynamic_cast<ofxMicroUI::booleano*>(&e)->getVal() );
	}

	booleano(){};
	booleano(std::string & n, ofxMicroUI & ui, bool val, bool & v, bool elementIsToggle = true) { //, bool useLabel = true
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

				if (labelPos == glm::vec2(0,0)) {
					labelPos = { _settings->elementPadding,
						_settings->elementRect.height - _settings->labelPosBaseline };

					if (isToggle) {
						labelPos.x = _settings->elementRect.height + _settings->elementPadding;
					}
				}
				rect.width += labelPos.x;
			}
			rectBg.width = rectBg.height = ui._settings->elementRect.height;
			rectVal.width = rectVal.height = ui._settings->elementRect.height - rectValMargin*2;
		}

		setupElement(n, ui);

		if (isToggle) {
			rectBg.position = rect.position;
			rectVal.position = rect.position + glm::vec3{ rectValMargin, rectValMargin, 0.0f };
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

	void setValFromMouse(int, int) override {
		if (wasPressed) {
			toggle();
		}
	}

	// booleano
	void checkMouse(int x, int y, bool = false) override {
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

	void set(bool v) override {
		*_val = v;
		redraw();
	}
};


class itemRadio : public booleano {
public:
	using booleano::booleano;
};



class varKindString {
public:
	std::function<void(std::string)> invokeString = nullptr;
	std::string * _val = nullptr;

	std::string getVal() {
		return *_val;
	}
};


class input : public element, public varKindString {
public:
	input(std::string & n, ofxMicroUI & ui, std::string & v) : element::element(n,ui) {
		_val = &v;
	}


	void checkMouse(int x, int y, bool first = false) override;
//	void checkMouse(int x, int y, bool first = false) override {
//		if (rect.inside(x, y)) {
//			//https://openframeworks.cc/documentation/utils/ofSystemUtils/#show_ofSystemTextBoxDialog
//			std::cout << "ofSystemTextBoxDialog " << *_val << std::endl;
//			set(ofSystemTextBoxDialog("value", *_val));
//		}
//	}

	void drawElement() override {
		ofPushStyle();
		ofNoFill();
		ofDrawRectangle(rect);
		ofPopStyle();
	}

	void set(const std::string & s) override {
		*_val = s;
		if (invokeString != nullptr && !_settings->presetIsLoading) {
			invokeString(s);
		}
		labelText = s;
		notify();
		redraw();
	}
};



class radio : public group, public varKindString {
public:

	void copyValFrom(element & e) override {
		set( dynamic_cast<ofxMicroUI::radio*>(&e)->getVal() );
	}

	// to store the state variables of the children elements.
	std::unordered_map <std::string, bool>	pBool;

	// fixme : review this
	bool useLabel = false;

	using group::group;

	radio() {};
	radio(std::string & n, ofxMicroUI & ui, std::vector<std::string> _items, std::string & v) { // : name(n)
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

	std::string getValByIndex(unsigned int index) {
		int i = useLabel ? index+1 : index;
		i = ofClamp(i, 0, elements.size()-1);
		return elements[i]->name;
	}

	void set(int index) override {
		int i = useLabel ? index+1 : index;
		i = ofClamp(i, 0, elements.size()-1);
		std::string s { elements[i]->name };
		set(s);
	}



	void set(const std::string & s) override {
//	void set(std::string s) override {
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

		if (invokeString != nullptr && !_settings->presetIsLoading) {
			invokeString(*_val);
		}

		// inserted 29 sep 2020 to work with new camList
		updateVal();
		notify();
		redraw();
	}

	// radio
	void checkMouse(int x, int y, bool first = false) override {
		if (rect.inside(x, y)) {
			for (auto & e : elements) {
				if (!dynamic_cast<label*>(e)) {
					if (e->rect.inside(x,y)) {
						if (_ui->freeFlow) {
							set(e->name);
						} else {
							if (*_val != e->name || first) {
								set(e->name);
							}
						}
						break; // break the element loop too.
					}
				}
			}
		}
	}
};


class dirList : public radio {
public:
	of::filesystem::path filePath { "" };
	of::filesystem::path loadedFile { "" };

	ofxMicroUI * _uiScene = nullptr;

	using radio::radio;

	of::filesystem::path getFileName() {
		if (_val != nullptr && !empty(*_val)) {
			return filePath / *_val;
		}
		else return {};
	}

	void updateVal() override {
		if (_uiScene != nullptr) {
			if (_val != nullptr && !empty(*_val)) {
				of::filesystem::path newTextFile { filePath / (*_val + ".txt") };
				_uiScene->sceneToLoad = newTextFile;
//				if (_uiScene->loadedTextFile != newTextFile) {
//					_uiScene->clear();
//					_uiScene->createFromText(newTextFile);
//				}
			}

//			of::filesystem::path newTextFile =
//			getFileName() + std::string(".txt");
		}
	}
};


class colorBase {
public:
	virtual ~colorBase() = default;

	virtual ofFloatColor getColor(float = 0) {
		std::cout << "getColor on colorBase primitive" << std::endl;
		return 0;
	}
};


// INCOMPLETE
class colorHsv : public groupSave, colorBase {
//class colorHsv : public group {
public:
	ofFloatColor * _val = nullptr;
	//float h, s, v;
	float sat = 0.5f;
	float alpha = 1.0f;
	glm::vec2 xy { 0, 1 };
	bool useAlpha = false;
	bool useRange = false;
//	string labelName, slider2dName = "";
	std::string nameSat { "sat" };
	float range = 0.0;

	ofFloatColor getColor(float n = 0) override {

//		return ofFloatColor::fromHsb(
//			std::fmod((xy.x + n*range), 1.0f),
//			sat,
//			xy.y,
//			useAlpha ? alpha : 1.0f
//		);

		// DONE: FIXME: avoid ofColor -> ofFloatColor conversion
//		return ofColor::fromHsb(
//			std::fmod((xy.x + n*range) * 255.0f, 255.0f),
//			sat,
//			xy.y * 255.0f,
//			useAlpha ? alpha : 255.0f
//		);
		
		
		return ofFloatColor::fromHsb(
			std::fmod((xy.x + n*range) * 1.0f, 1.0f),
			sat,
			xy.y * 1.0f,
			useAlpha ? alpha : 1.0f
		);
	}


	// colorHsv(string & n, ofxMicroUI & ui, ofColor defaultColor, ofColor & c, bool _useAlpha = false) {
	colorHsv(std::string & n, ofxMicroUI & ui, ofFloatColor defaultColor, ofFloatColor & c, int kind = 0);

	void updateVal() override {
//		*_val = ofFloatColor::fromHsb(xy.x, sat / 255.0f, xy.y, useAlpha ? alpha : 1.0f);
		*_val = ofFloatColor::fromHsb(xy.x, sat, xy.y, useAlpha ? alpha : 1.0f);
//		using std::cout;
//		using std::endl;
//		cout << "xy " << xy << endl;
//		cout << "sat " << sat << endl;
//		cout << "alpha " << alpha << endl;
//		cout << "range " << range << endl;
//		cout << "updateVal from colorHsv " << name << ":" << *_val << endl;
		redraw();
		notify();
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
		redraw();
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
//		elements[2]->set(sat*255);
		elements[2]->set(sat);
		updateVal();
		// new test
		redraw();
	}


//	std::string colorToHex(const ofColor& color) {
//		std::stringstream ss;
//		ss << "#"
//		   << std::hex << std::setw(2) << std::setfill('0') << (int)color.r
//		   << std::setw(2) << std::setfill('0') << (int)color.g
//		   << std::setw(2) << std::setfill('0') << (int)color.b;
//		return ss.str();
//	}
	ofFloatColor hexToColor(const std::string& hexString) {
		std::string hex = hexString;

		// Remove '#' if present
		if (hex[0] == '#') {
			hex = hex.substr(1);
		}

		// Parse hex values
		int r = 0, g = 0, b = 0, a = 255;

		if (hex.length() >= 6) {
			r = std::stoi(hex.substr(0, 2), nullptr, 16);
			g = std::stoi(hex.substr(2, 2), nullptr, 16);
			b = std::stoi(hex.substr(4, 2), nullptr, 16);

			// Optional alpha channel
			if (hex.length() >= 8) {
				a = std::stoi(hex.substr(6, 2), nullptr, 16);
			}
		}

		// Convert to float (0.0 - 1.0)
		return ofFloatColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}

	std::string colorToHex(const ofFloatColor& color) {
		std::stringstream ss;
		ss << "#"
		   << std::hex << std::setw(2) << std::setfill('0') << (int)(color.r * 255)
		   << std::setw(2) << std::setfill('0') << (int)(color.g * 255)
		   << std::setw(2) << std::setfill('0') << (int)(color.b * 255);
		return ss.str();
	}

	void setFromHex(std::string hex) {
		cout << "setFromHex " << hex << endl;
		setFromColor(hexToColor(hex));
	}

	std::string getHexColor() {
//		return "xyz";
		return colorToHex(*_val);
	}

	void checkMouse(int x, int y, bool first = false) override;
//	void setValFromMouse(int x, int y) override;
};



class slider : public element {
public:
	float * _val = nullptr;
	int * _valInt = nullptr;

	float min = 0;
	float max = 1;
	float def = 0;
	bool isInt = false;

	void copyValFrom(element & e) override {
//		cout << e.name << endl;
		auto val = dynamic_cast<ofxMicroUI::slider*>(&e)->getVal();
		val = ofClamp(val, min, max);
		set( val );
	}

	slider(std::string & n, ofxMicroUI & ui, glm::vec3 val, float & v) : element::element(n, ui) { // : name(n)
		f = &v;
		_val = &v;
		rectVal = rectBg = rect;
		min = val.x;
		max = val.y;
		def = val.z;
		set(val.z);
		useLabel = _ui->useLabelOnNewElement;
	}

	slider(std::string & n, ofxMicroUI & ui, glm::vec3 val, int & v) : element::element(n, ui) { // : name(n)
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
		if (_val != nullptr) {
			return *_val;
		}
		if (_valInt != nullptr) {
			return *_valInt;
		}
		else {
			return 0.0f;
		}
	}

	float getValNorm() {
		if (_val != nullptr) {
			return ofMap(*_val, min, max, 0.0f, 1.0f);
		}
		if (_valInt != nullptr) {
			return ofMap(*_valInt, min, max, 0.0f, 1.0f);
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

	void set(float v, bool normalized = false) override {
		// cout << "slider set!" << name << " :: " << v << endl;
		//val = v;
		if (normalized) {
			v = ofMap(v, 0.0f, 1.0f, min, max);
		}

		if (_val != nullptr) {
			*_val = v;
			rectVal.width = ofMap(v, min, max, 0, rect.width);
			labelText = name + " " + ofToString(*_val);
		}
		if (_valInt != nullptr) {
			*_valInt = v;
			rectVal.width = ofMap(*_valInt, min, max, 0, rect.width);
			labelText = name + " " + ofToString(*_valInt);
		}

		notify();
		redraw();
	}

	void add(float v) {
		if (_valInt != nullptr) {
			set(*_valInt + v);
		}
		if (_val != nullptr) {
			set(*_val + v);
		}
	}

	void setValFromMouse(int x, int y) override;


	void resetDefault() override {
		set(def);
	}

};



class image2 : virtual public element {
public:
	ofImage img;
	//image();
	image2(std::string & n, ofxMicroUI & ui, std::string fileName) {
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

class image : virtual public element {
public:
	ofImage img;
	//image();
	image(std::string & n, ofxMicroUI & ui, std::string fileName) {
		saveXml = false;
		img.load(fileName);
//		rect.height = img.getHeight();
		setupElement(n, ui);
		img.resize(rect.width, rect.height);
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
	fboElement(std::string & n, ofxMicroUI & ui, int r = 2) : rows(r) {
		// cout << "fboElement setup " << name << endl;
//        int rows = 3;
		rect.height = ui._settings->elementRect.height * rows + (ui._settings->elementSpacing)*(rows-1);
		setupElement(n, ui);

		// cout << "setup fbo element " << name << endl;
		// cout << rect << endl;
//		fbo.allocate(rect.width, rect.height, GL_RGBA);
		fbo.allocate(rect.width, rect.height, GL_RGB);
		fbo.begin();
		ofClear(0,255);
		fbo.end();
//		fbo.getTexture().getTextureData().bFlipTexture = false;
	}

	void draw() override {
		ofPushStyle();
		ofSetColor(255);
		fbo.getTexture().draw(rect.x, rect.y);
//		fbo.draw(rect.x, rect.y);
		ofPopStyle();
	}
};


class adsr : public fboElement {
public:
	using fboElement::fboElement;
	ofFbo fboData;

	std::vector <glm::vec2> points;

	adsr(std::string & n, ofxMicroUI & ui, glm::vec2 & ) : fboElement(n, ui) {
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
//		glm::vec2 xy = glm::vec2 (xx,yy) - glm::vec2(rect.x, rect.y);
		glm::vec2 xy { xx - rect.x, yy - rect.y };
//		glm::vec2 wh = glm::vec2 (rect.width, rect.height);
		points.push_back(xy);
		drawVal();
		notify();
		redraw();
	}

};


class slider2d : public fboElement {
public:
	glm::vec2 * _val = nullptr;
	// estes dois, teste.
	//	glm::vec2 valFloat = glm::vec2(0.5, 0.5);
	//	glm::vec2 ranges = glm::vec2(1.0, 1.0);

	// remove in near future
	glm::vec2 min { 0, 0 };
	glm::vec2 max { 1, 1 };

	void copyValFrom(element & e) override {
		set( dynamic_cast<ofxMicroUI::slider2d*>(&e)->getVal() );
	}

	using fboElement::fboElement;

	slider2d(std::string & n, ofxMicroUI & ui, glm::vec2 & v, int rows = 2) : fboElement(n, ui, rows) {
		// novidade
//		setupElement(n, ui);
//		cout << "new slider2d name = " << name << endl;
		_val = &v;
		//set(val);
	}



	void draw() override {
		fboElement::draw();
		ofSetColor(255);
//		drawVal();
//		fboData.getTexture().draw(rect.x, rect.y);

		ofPushMatrix();
		ofTranslate(rect.x, rect.y);
		float x = ofMap(_val->x, min.x, max.x, 0, rect.width);
		float y = ofMap(_val->y, min.y, max.y, 0, rect.height);
		ofDrawLine(x, 0, x,  rect.height);
		ofDrawLine(0, y, rect.width, y);
		ofDrawRectangle(x-3, y-3, 6, 6);
		ofPopMatrix();
//		fboData.draw(rect.x, rect.y);
	}

	// test 3 sep 2020 miaw colorPalette
	virtual void afterSet() {}

	void set(glm::vec2 v) override {
//		std::cout << "OWW slider2d set " << name << " : "  << v << std::endl;
		if (_val != nullptr) {
			*_val = v;
			labelText = name + " " + ofToString(*_val);
		}
		afterSet();
		redraw();
		notify();
	}

	void set(const std::string & v) override {
		std::vector <std::string> cols { ofSplitString(v, " ") };
		if (cols.size() > 1) {
			set( glm::vec2{ ofToFloat(cols[0]), ofToFloat(cols[1]) });
		}
	}

	glm::vec2 getVal() {
		return *_val;
	}

	void setValFromMouse(int x, int y) override {
		int xx = ofClamp(x, rect.x, rect.x + rect.width);
		int yy = ofClamp(y, rect.y, rect.y + rect.height);
//		glm::vec2 xy = glm::vec2 (xx,yy) - glm::vec2(rect.x, rect.y);
		glm::vec2 xy { xx-rect.x, yy-rect.y };
		glm::vec2 wh { rect.width, rect.height };
		glm::vec2 val { min + (max-min)*(xy/wh) };

 		set(val);
	}
};



class colorPalette : public slider2d, colorBase {
public:
	ofFloatColor * _colorVal = nullptr;
	std::vector < std::vector<ofFloatColor> > paletas;
	using slider2d::slider2d;

	ofFloatColor getColor(float q = 0) override {
		if (paletas.size()) {
			updateColor(q);
			return *_colorVal;
		} else {
			return ofFloatColor(0);
		}
	}

	void afterSet() override {
		updateColor();
	}

	void updateColor(float q = 0) {
		if (paletas.size()) {
			float x = _val->x;
			float y = std::fmod(_val->y+q, 1.0);
			int qualPaleta = std::fmod(x, paletas.size());
			int qualCor = int(y * paletas[qualPaleta].size()) % paletas[qualPaleta].size();
			*_colorVal = paletas[qualPaleta][qualCor];
		}
	}

	ofFloatColor getColorByIndex(int i) {
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

	void loadPalettes(std::string file) {
		if (ofFile::doesFileExist(file)) {
			paletas.clear();
			std::vector <std::string> allLines { ofxMicroUI::textToVector(file) };
			for (auto & line : allLines) {
				if (line != "") {
					std::vector<ofFloatColor> paletaTemporaria;
					for (auto & corString : ofSplitString(line, " ")) {
						if (corString.size() > 1) {
							std::string corhex { corString.size() == 6 ? corString : corString.substr(1) };
							ofFloatColor corzinha = ofColor::fromHex(ofHexToInt(corhex));
							paletaTemporaria.emplace_back(corzinha);
						}
					}
					paletas.emplace_back(paletaTemporaria);
				}
			}
			ofFloatColor cor;
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
						cor = ofFloatColor(0);
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

	presetItem(std::string & n, ofxMicroUI & ui, bool val, bool & v) : booleano() {
		_ui = &ui;
		_settings = _ui->_settings;
		isToggle = false;
		setupPresetItem();
		setupElement(n, ui);
		rectVal = rect;
//		rectVal.height = 10;
		rectVal.x += border * .5f;
		rectVal.y += border * .5f;
		rectVal.width -= border;
		rectVal.height -= border;
		rectBg = rect;

		labelPos.x = 8;

		_val = &v;
		set(val);
	}

	void setupPresetItem() {
		isToggle = false;
		int cols = _settings->presetCols;

		// presetSize.
//		rect.height = _settings->elementRect.height * 2 + _settings->elementSpacing;
//		rect.width  = (_settings->elementRect.width - _settings->elementSpacing * 2) / cols ;

		rect.height = _settings->elementRect.height * _settings->presetHeight + _settings->elementSpacing;
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
		//if (_ui != nullptr)
		{

			of::filesystem::path path { _ui->getPresetPath() };
			if (!path.empty()) {
				auto dir { path / name };
				fbo.begin();
				ofClear(0,0);
				if (ofFile::doesFileExist(dir)) {
					auto imageFile { dir / "0.tif" };
					auto imageFile2 { dir / "0.png" };
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

					auto textFile { dir / "0.txt" };
					if (fs::exists(ofToDataPath(textFile))) {
						std::string texto { ofxMicroUI::textToString(textFile) };
						glm::vec2 pos { labelPos.x, labelPos.y + 16 };
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





class presets : public radio {
public:
	// removi 30 oct 2021, coloquei global, assim tenho como setar antes da ui carregar
//	ofFbo * _fbo = nullptr;

	// 12 oct 2020 - Switcher
	std::vector<std::string> items;
	std::unordered_map <std::string, int> itemPosition;

	presets() {}
	presets(std::string & n, ofxMicroUI & ui, std::vector<std::string> _items, std::string & v) { // : name(n)

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
		std::string name { items[index] };
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



class bar : public element {
public:
	float val;

	bar(std::string & n, ofxMicroUI & ui) : element::element(n, ui){
		saveXml = false;
		rectBg = rect;
		labelText = "";
		s = &labelText;
	}

	void set(float v, bool = false) override {
		val = ofClamp(v, 0, 1);
		rectVal = ofRectangle(rect.x, rect.y, rect.width * val, rect.height);
		redraw();
	}

	void set(const std::string & s) override {
		if (labelText != s) {
			labelText = s;
			redraw();
		}
	}

	void drawElement() override {
		ofSetColor(getColorBg());
		ofDrawRectangle(rectBg);
//		ofSetColor(_settings->colorVal);
		ofSetColor(_settings->alertColor2);
		ofDrawRectangle(rectVal);
	}
};




class imageList : public dirList {
public:
	ofImage i;
	ofImage * _image { nullptr };

	bool disableArb = false;

	imageList(std::string & n, ofxMicroUI & ui, std::vector<std::string> items, std::string & v, ofImage & i) :
	dirList(n, ui, items, v) {
		_image = &i;
	}

	void updateVal() override {
		auto f { getFileName() };
		if (*s == "_" || empty(*s)) {
			if (_image->isAllocated()) {
				std::cout << "ofxMicroUI::imageList unload img" << std::endl;
				_image->clear();
				loadedFile = "";
			}
//			_image->unbind();
		}
		else if (_image != nullptr && !empty(*s)) {
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

/*
  Already on CPP File
 */

class videoList : public dirList {
public:
	ofVideoPlayer * _video { nullptr };

	videoList(std::string & n, ofxMicroUI & ui, std::vector<std::string> items, std::string & v, ofVideoPlayer & vid);
	void updateVal() override;
};


class audioList : public dirList {
public:
	ofSoundPlayer * _sound { nullptr };

	audioList(std::string & n, ofxMicroUI & ui, std::vector<std::string> items, std::string & v, ofSoundPlayer & sound);
	void updateVal() override;
};


class textList : public dirList {
public:
	std::string * _text { nullptr };

	textList(std::string & n, ofxMicroUI & ui, std::vector<std::string> items, std::string & v, std::string & t) :
	dirList(n, ui, items, v) {
		_text = &t;
	}

	void updateVal() override {
		auto f = getFileName();
		if (*s != "" && loadedFile != f) {
			*_text = ofBufferFromFile(f).getText();
			loadedFile = f;
			std::cout << "LOAD textList: " << name << " : " << f << std::endl;
		}
	}
};

class fontList : public dirList {
public:
	ofTrueTypeFont * _font { nullptr };
	float size = 40;
	float * _size = nullptr;

	bool antiAlias = true;
	bool fullCharacterSet = true;
	bool makeContours = true;

//	using dirList::dirList;
	fontList(std::string & n, ofxMicroUI & ui, std::vector<std::string> items, std::string & v, ofTrueTypeFont & f) :
	dirList(n, ui, items, v) {
		_font = &f;
	}

	void updateVal() override {
		auto f { getFileName() };
		// FIXME: check if *s is nullptr

		if (!empty(*s) && loadedFile != f) {
			if (_size != nullptr) {
				size = *_size;
			}
			_font->load(f, size, antiAlias, fullCharacterSet, makeContours);
			loadedFile = f;
			std::cout << "LOAD fontList: " << name << " : " << f << std::endl;
		}
	}

	void reload() {
		std::cout << "font reload()" << std::endl;
//		cout << getFileName() << endl;
		std::cout << loadedFile << std::endl;
		if (loadedFile != "") {
			_font->load(loadedFile, size, antiAlias, fullCharacterSet, makeContours);
		}
	}
};



class camList : public radio {
public:
	ofVideoGrabber * _cam = nullptr;
	std::unordered_map <std::string, int> camIDs;
	std::vector <std::string> getCams(ofVideoGrabber & c) {
		_cam = &c;
		std::vector <std::string> opcoes;
		opcoes.emplace_back("_");
		if (_cam != nullptr) {
			for (auto & d : _cam->listDevices()) {
				opcoes.emplace_back(d.deviceName);
			}
		}
		return opcoes;
	}

	camList(std::string & n, ofxMicroUI & ui, std::string & v, ofVideoGrabber & c) :
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
				std::cout << "opening camera with id = " << *s << std::endl;
				int id = camIDs[*s];
				std::cout << "CAMLIST updateVal(): " << name << " : " << *s << " : " << id << std::endl;
				std::cout << " width: " << width << " height: " << height << " fps: " << frameRate << std::endl;
				if (_cam->isInitialized()) {
					_cam->close();
				}
//				_cam->setPixelFormat(OF_PIXELS_BGR);
				_cam->setDeviceID(id);
				if (frameRate > 0) {
					_cam->setDesiredFrameRate(frameRate);
				}

				_cam->setup(width, height);
			}
		}
	}
};



class colorPalImg : public colorBase, public groupSave {
public:
	ofFloatColor * _val = nullptr;

	std::string pal;
	float range;
	std::vector <ofImage> images;

	std::vector <std::vector <ofColor> > cores;

	colorPalImg(std::string & n, ofxMicroUI & ui, ofFloatColor & c, of::filesystem::path folder) {
		setupElement(n, ui, false);
		_val = &c;

		std::vector <of::filesystem::path> files;
		std::vector <std::string> names;
		std::vector <std::string> indices;

		auto folderData = ofToDataPath(folder);
//		cout << "folder = " << folder << endl;
		int index=0;
		for (const auto & f : of::filesystem::directory_iterator(folderData)) {
			std::string name = f.path().filename().string();
			if (name[0] != '.') {
				files.emplace_back(f.path());
				names.emplace_back(f.path().filename().string());

//				images.emplace_back();

				//			cout << f.path() << endl;
				auto img { folder / f.path().filename() };
				//			images.back().load(folder / f.filename());

				ofImage i;
				i.load(img);
				std::vector <ofColor> listaDeCores;
				for (int a=0; a<i.getWidth(); a++) {
					listaDeCores.emplace_back(i.getColor(a, 0));
				}
				cores.emplace_back(listaDeCores);


//				images.back().load(img);
				//			ofImage i;
				//			i.load(f.path());
				//			images.emplace_back(i);
				//			images.load(f.path());
				//			std::cout << f.path() << std::endl;
				indices.emplace_back(ofToString(index));
				index++;
			}
		}

		elements.emplace_back(new label(name, ui));

		std::string radioName { "pal" };
		elements.emplace_back(new radio(radioName, ui, indices, pal));
		elements.back()->useNotify = false;


		std::string rangeName { "range" };
		elements.emplace_back(new slider(rangeName, ui, glm::vec3(0.0f, 1.0f, 0.0f), range));
		elements.back()->useNotify = false;
		for (auto & f : files) {
			elements.emplace_back(new image(name, ui, f.string()));
		}
		groupResize();
	}

	ofFloatColor getColor(float n = 0) override {
		if (empty(pal)) {
			return ofColor(0);
		}
		int palIndex = ofToInt(pal);
		if (palIndex < 0) palIndex = 0;


//		if (images.size() > palIndex)
		{
//			float x = n * range * (float)images[palIndex].getWidth();
//			cout << x << endl;
//			return images[palIndex].getColor(x, 2);

			int i = n * range * cores[palIndex].size();
			i = ofClamp(i, 0, cores[palIndex].size() -1);
//			cout << i << endl;
			return cores[palIndex][i];
		}
//		else {
//			cout << "WOW deslize" << endl;
//			return ofColor(0);
//		}
	}


	void updateVal() override {
		*_val = getColor(0);
		redraw();
		notify();
	}

	ofColor getVal() {
		return *_val;
	}
};

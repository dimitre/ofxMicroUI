// default element skeleton
class element {
public:
	microUISettings * _settings = NULL;
	
	string name = "";
	string labelText = "";
	bool wasPressed = false;
	
	virtual void set(float v) {}
	virtual void set(int v) {}
	virtual void set(bool v) {}
	virtual void set(string v) {}
	virtual void set(glm::vec2 v) {}
	virtual void set(glm::vec3 v) {}

	// this variables can be only set once per element kind. it can be a pointer.
	glm::vec2 labelPos = glm::vec2(5, 16);
	
	// invisible rectangle, handles the mouse click
	ofRectangle rect = ofRectangle(0,0,240,20);
	
	// visible and static
	ofRectangle rectBg = rect;
	
	// visible and changing according to the value
	ofRectangle rectVal = rect;

	virtual void drawLabel() {
		ofSetColor(255);
		ofDrawBitmapString(labelText, rect.x + labelPos.x, rect.y + labelPos.y);
	}
	
	virtual void drawElement() {}
	
	virtual void draw() {
		drawElement();
		drawLabel();
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
	
	element() {}
	~element() {}

	void setupElement(string & n, microUISettings & s, bool advance = true) {
		_settings = &s;
		rect.position = ofPoint(_settings->xy);
		name = n;
		labelText = n;

		// this way settings knows the last element dimensions
		_settings->flowRect = rect;

		// not if element type is a group.
		if (advance) {
			//_settings->advanceLine();
			if (!_settings->advanceLine()) {
				rect.position = ofPoint(_settings->xy);
				_settings->advanceLine();
			}
		}
		
	}
};


class label : public element {
public:
	label(string & n, microUISettings & s) {
		setupElement(n, s);
	}
};






// new prototype
/*
 This element is the base class for everything multiple. Radio, vec3, presets, future group of elements
 */

class group : public element {
public:
	vector <element *> elements;
	
	group() {}
	group(string & n, microUISettings & s, glm::vec3 & v) {
		setupElement(n, s);
	}
	
	// naming? this function updates the rect of the group to fit all elements and handle mouse events
	void groupResize() {
		rect = elements[0]->rect;
		for (auto & e : elements) {
			rect.growToInclude(e->rect);
		}
	}
	
	void checkMouse(int x, int y, bool first = false) override {
		if (rect.inside(x, y)) {
			for (auto & e : elements) {
				cout << "checkmouse inside elements group " << e->name << endl;
				e->checkMouse(x, y, first);
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
};


class radio : public group {
public:
	string * _val = NULL;
	map <string, bool>	pBool;

	radio(string & n, microUISettings & s, vector<string> items, string & v) { // : name(n)
		setupElement(n, s, false);
		_val = &v;
		set(*_val);
		
		elements.push_back(new label(name, s));
		_settings->setFlowVert(false);
		for (auto & i : items) {
			bool val = false;
			elements.push_back(new itemradio(i, s, val, pBool[i], false));
		}
		_settings->setFlowVert(true);
		groupResize();
		_settings->advanceLine();
		//setupElement(n, s);
	}
	
	string getVal() {
		return *_val;
	}
	
	void set(string s) override {
		//cout << "radio set " << name << ":" << s << endl;
		if (*_val != s) {
			for (auto & e : elements) {
				if (e->name == *_val) {
					e->set(false);
				}
				if (e->name == s) {
					e->set(true);
				}
			}
			*_val = s;
		} else {
			// same value as before, only notify
		}
	}
	
	void checkMouse(int x, int y, bool first = false) override {
		if (rect.inside(x, y)) {
			for (auto & e : elements) {
				if (e->rect.inside(x,y)) {
					//cout << "inside rect " << e->name << endl;
					if (e->name != *_val) {
						for (auto & ee : elements) {
							if (ee->name == *_val) {
								ee->set(false);
								break; // break this loop
							}
						}
						e->set(true);
						*_val = e->name;
					} else {
						// same value as before, only notify when event handling
					}
					break; // break the element loop too.
				}
			}
		}
	}
};


class vec3 : public group {
public:
	glm::vec3 * _val = NULL;

	vec3(string & n, microUISettings & s, glm::vec3 & v) {
		_val = &v;
		setupElement(n, s);
		glm::vec3 vals = glm::vec3(0,1,.5);
		//friend class?
		//string name = "GROUP";
		string x = "x";
		string y = "y";
		string z = "z";
		elements.push_back(new label(name, s));
		elements.push_back(new slider(x, s, vals, _val->x));
		elements.push_back(new slider(y, s, vals, _val->y));
		elements.push_back(new slider(z, s, vals, _val->z));

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
	float min = 0;
	float max = 1;

	slider(string & n, microUISettings & s, glm::vec3 val, float & v) { // : name(n)
		setupElement(n, s);
		_val = &v;
		rectVal = rect;
		rectBg = rect;
		min = val.x;
		max = val.y;
		set(val.z);
	}
	
	float getVal() {
		return *_val;
	}
	
	void drawElement() override {
		ofSetColor(127);
		ofDrawRectangle(rectBg);
		ofSetColor(80);
		ofDrawRectangle(rectVal);
	}
	
	void set(float v) override {
		//val = v;
		if (_val != NULL) {
			*_val = v;
		}
		rectVal.width = ofMap(v, min, max, 0, rect.width);
		labelText = name + " " + ofToString(*_val);
	}
	
	void setValFromMouse(int x, int y) override {
		int xx = ofClamp(x, rect.x, rect.x + rect.width);
		int yy = ofClamp(y, rect.y, rect.y + rect.height);
		ofPoint xy = ofPoint (xx,yy) - ofPoint(rect.x, rect.y);
		ofPoint wh = ofPoint (rect.width, rect.height);
		ofPoint val = min + (max-min)*(xy/wh);
		set(val.x);
	}

};

class booleano : public element {
public:
	bool * _val = NULL;
	
	// experiment to transform booleano in radioitem
	
	booleano(string & n, microUISettings & s, bool val, bool & v, bool isToggle = true) {
		// this is the size of the element according to the text size. it is called before setupElement so the rectangle can be forwarded to _settings to calculate the flow of the next element.
		int contaletras = 0;
		for(auto c: ofUTF8Iterator(n)){
			contaletras++;
		}
		
		rect.width = contaletras * 8 + 5*2; // mais margem
		setupElement(n, s);
		
		rectVal = rect;

		if (isToggle) {
			// it needs more space for the checkbox
			labelPos = glm::vec2(25, 16);
			rectVal.position = rect.position + ofPoint(5,5);
			rectVal.width = rectVal.height = 10;
			rect.width += 25;
			rectBg.position = rect.position;
			rectBg.width = 20;
			rectBg.height = 20;
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
		ofSetColor(127);
		ofDrawRectangle(rectBg);
		
		if (*_val) {
			ofSetColor(40);
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
	// inherit constructor from booleano
	// doesnt work, in fact it includes the function of the primitive element.
	//	void postSetup() override {
	//		cout << "postsetup here" << endl;
	//		labelPos = glm::vec2(5, 16);
	//	}
	
	using booleano::booleano;
};

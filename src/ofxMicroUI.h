/*

Ideas: 
having an ofFbo for each element to redraw and store, only use texture when not changed.
three boolean controls, toggle, hold and bang.
bind event to elements.
event system.

make elementgroup to act as one. in xml, draw, etc.
*/




class ofxMicroUI : public ofBaseApp {
public:

	map <string, float>	pFloat;
	map <string, bool>	pBool;
	map <string, glm::vec3>	pVec3;

	//glm::vec2 xy = glm::vec2(10,10);

	/*
	 Settings control the flow (distribution on xy) of elements, settings of elements, margins, etc.
	 a pointer is added to each element so they all obey to the same settings.
	 */
	struct microUISettings {
		/*
		 offset x, offset y (padding inside column)
		 column rectangle
		 slider dimensions.
		 */
		glm::vec2 xy = glm::vec2(10,10);
		map <string, int> pInt;
		int margin = 10;
		int spacing = 4;
		ofRectangle elementRect = ofRectangle(0,0,240,20);

		void advanceLine() {
			xy.y += 20 + spacing;
		}
		
		void newCol() {
			xy.x += 240 + spacing;
			xy.y = margin;
		}
	} settings;
	
	
	// default element skeleton
	class element {
	public:
		microUISettings * _settings = NULL;
		
		string name = "";
		ofRectangle rect = ofRectangle(0,0,240,20);
		ofRectangle rectVal = ofRectangle(0,0,240,20);
		string labelText = "";

		bool wasPressed = false;
		
		virtual void set(float v) {}
		virtual void set(int v) {}
		virtual void set(bool v) {}
		virtual void set(string v) {}
		virtual void set(glm::vec2 v) {}
		virtual void set(glm::vec3 v) {}

		glm::vec2 labelPos = glm::vec2(5, 16);

		virtual void drawLabel() {
			ofSetColor(255);
			ofDrawBitmapString(labelText, rect.x + labelPos.x, rect.y + labelPos.y);
		}
		
		virtual void drawElement() {
		}
		
		virtual void draw() {
			drawElement();
			drawLabel();
		}
		
		virtual void setValFromMouse(int x, int y) {
		}
		
		virtual void checkMouse(int x, int y, bool first = false) {
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

		void setupElement(string & n, microUISettings & s) {
			_settings = &s;
			rect.position = ofPoint(_settings->xy);
			name = n;
			labelText = n;
			_settings->advanceLine();
		}
	};
	
	
	class label : public element {
	public:
		label(string & n, microUISettings & s) {
			setupElement(n, s);
		}
	};
	
	
	// new prototype
	class group : public element {
	public:
		vector <element *> elements;
		glm::vec3 * _val = NULL;
		
		group() {}
		group(string & n, microUISettings & s, glm::vec3 & v) {
			_val = &v;
			setupElement(n, s);
		}
		
		void checkMouse(int x, int y, bool first = false) {
			if (rect.inside(x, y)) {
				for (auto & e : elements) {
					e->checkMouse(x, y, first);
				}
			}
		}
		
		void mouseRelease(int x, int y) {
			if (rect.inside(x, y))
			{
				for (auto & e : elements) {
					e->mouseRelease(x,y);
				}
				wasPressed = false;
			}
		}

		void draw() {
			for (auto & e : elements) {
				e->draw();
			}
		}
	};
	
	class vec3 : public group {
	public:
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

			rect = elements[0]->rect;
			for (auto & e : elements) {
				rect.growToInclude(e->rect);
			}
		}
		
		void set(glm::vec3 v) {
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
		
		void set(string s) {
			vector<string> vals = ofSplitString(s, ", ");
			set(glm::vec3(ofToFloat(vals[0]), ofToFloat(vals[1]), ofToFloat(vals[2])));
		}

		glm::vec3 getVal() {
			return *_val;
		}
	};


	class slider : public element {
	public:
		float * _val = NULL;
		float min = 0;
		float max = 1;
		//		float val = 0.5;

//		void drawLabel() {
//			name + " " + ofToString(val);
//		}
		
		void drawElement() {
			ofSetColor(127);
			ofDrawRectangle(rect);
			ofSetColor(80);
			ofDrawRectangle(rectVal);
		}
		
		float getVal() {
			return *_val;
		}
		
		void set(float v) {
			//val = v;
			if (_val != NULL) {
				*_val = v;
			}
			rectVal.width = ofMap(v, min, max, 0, rect.width);
			labelText = name + " " + ofToString(*_val);
		}
		
		void setValFromMouse(int x, int y) {
			int xx = ofClamp(x, rect.x, rect.x + rect.width);
			int yy = ofClamp(y, rect.y, rect.y + rect.height);
			ofPoint xy = ofPoint (xx,yy) - ofPoint(rect.x, rect.y);
			ofPoint wh = ofPoint (rect.width, rect.height);
			ofPoint val = min + (max-min)*(xy/wh);
			set(val.x);
		}
		
		
		slider(string & n, microUISettings & s, glm::vec3 val, float & v) { // : name(n)
			setupElement(n, s);
			_val = &v;
			rectVal = rect;
			min = val.x;
			max = val.y;
			set(val.z);
		}		
	};
	
	class booleano : public element {
	public:
		bool * _val = NULL;
		
		bool getVal() {
			return *_val;
		}
		
		void set(bool v) {
			*_val = v;
		}
		
		void toggle() {
			set(*_val ^ 1);
		}
		
		void setValFromMouse(int x, int y) {
			if (wasPressed) {
				toggle();
			}
		}
		
		void checkMouse(int x, int y, bool first = false) {
			if (rect.inside(x, y)) {
				if (!wasPressed) {
					toggle();
				}
				wasPressed = true;
			} else {
				wasPressed = false;
			}
		}
		
		
		void drawElement() {
			ofSetColor(127);
			ofDrawRectangle(rect.x, rect.y, 20, 20);
			
			if (*_val) {
				ofSetColor(40);
				ofDrawRectangle(rectVal);
			}
		}
		
		booleano (string & n, microUISettings & s, bool val, bool & v) {
			setupElement(n, s);
			rectVal.position = rect.position + ofPoint(5,5);
			rectVal.width = rectVal.height = 10;
			_val = &v;
			set(val);
			labelPos = glm::vec2(25, 16);
		}

	};

	// TODO
	element * getElement(string n) {
	}
	
	vector <element*> elements;

	void setupUI() {
		createFromText("m.txt");
	}

	void mouseUI(int x, int y, bool pressed) {
		for (auto & e : elements) {
			e->checkMouse(x, y, pressed);
		}
	}

	void onDraw(ofEventArgs &data) {
		for (auto & e : elements) {
			e->draw();
		}
	}
	
	//void onMouseMoved(ofMouseEventArgs &data) {}
	void onMousePressed(ofMouseEventArgs &data) {
		mouseUI(data.x, data.y, true);
	}
	
	void onMouseDragged(ofMouseEventArgs &data) {
		mouseUI(data.x, data.y, false);
	}
	
	void onMouseReleased(ofMouseEventArgs &data) {
		// mouseRELEASE
		// XAXA - set false wasPressed in each.
		for (auto & e : elements) {
			e->mouseRelease(data.x, data.y);
		}
	}
	
	vector <string> textToVector(string file) {
		vector <string> saida;
		ofBuffer buff2 = ofBufferFromFile(file);
		for(auto & line: buff2.getLines()) {
			saida.push_back(line);
		}
		return saida;
	}
	
	void alert(string s) {
		cout << "= ofxMicroUI = " << s << endl;
	}

	ofxMicroUI() {
		alert("setup");
		ofAddListener(ofEvents().draw, this, &ofxMicroUI::onDraw);
		//ofAddListener(ofEvents().mouseMoved, this, &ofxMicroUI::onMouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &ofxMicroUI::onMousePressed);
		ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUI::onMouseDragged);
		ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUI::onMouseReleased);
	}
	
	~ofxMicroUI() {
		alert("destroy");
	}
	

	void createFromText(string fileName) {
		alert("createFromText " + fileName);
		vector <string> lines = textToVector(fileName);
		for (auto & l : lines) {

			vector <string> cols = ofSplitString(l, "\t");
			if (cols.size() == 1) {
				if (l == "") {
					settings.advanceLine();
				}
				else if (l == "newCol") {
					settings.newCol();
				}
			}
			if (cols.size() >= 2) {
				string name = cols[1];

				if (cols[0] == "label") {
					elements.push_back(new label(name, settings));
				}
				
				else if (cols[0] == "vec3") {
					elements.push_back(new vec3(name, settings, pVec3[name]));
				}

				else if (cols[0] == "float") {
					vector <string> values = ofSplitString(cols[2]," ");
					glm::vec3 vals = glm::vec3(ofToFloat(values[0]),ofToFloat(values[1]),ofToFloat(values[2]));
					elements.push_back(new slider(name, settings, vals, pFloat[name]));
				}

				else if (cols[0] == "bool") {
					bool val = ofToBool(cols[2]);
					pBool[name] = val;
					elements.push_back(new booleano (name, settings, val, pBool[name]));
				}
			}
		}
	}
	
	void load(string xml) {
		alert("load " + xml);
		
		if (ofFile::doesFileExist(xml)) {
			ofXml xmlSettings;
			xmlSettings.load(xml);
			int UIVersion = xmlSettings.getChild("ofxMicroUI").getIntValue();
			if (UIVersion == 1)
			{
				auto xmlElements = 	xmlSettings.getChild("element");
				auto floats = 		xmlElements.findFirst("float");
				auto bools = 		xmlElements.findFirst("boolean");
				auto vec3s = 		xmlElements.findFirst("group");

				for (auto & e : elements) {
					if (floats.getChild(e->name)) {
						auto valor = floats.getChild(e->name).getFloatValue();
						e->set(valor);
					}
					if (bools.getChild(e->name)) {
						auto valor = bools.getChild(e->name).getBoolValue();
						e->set(valor);
					}
					if (vec3s.getChild(e->name)) {
						auto valor = vec3s.getChild(e->name).getValue();
						//cout << valor << endl;
						e->set(valor);
					}
				}
			}
		}
	}
	
	void save(string xml) {
		alert("save " + xml);

		int version = 1;
		ofXml xmlSettings;
		xmlSettings.appendChild("ofxMicroUI").set(version);
		auto xmlElements = 	xmlSettings.appendChild("element");
		auto floats = xmlElements.appendChild("float");
		auto bools = xmlElements.appendChild("boolean");
		auto groups = xmlElements.appendChild("group");

		for (auto & e : elements) {
			slider * els = dynamic_cast<slider*>(e);
			booleano * elb = dynamic_cast<booleano*>(e);
			vec3 * el3 = dynamic_cast<vec3*>(e);
			
			if (els) {
				floats.appendChild(e->name).set(els->getVal());
			}
			if (elb) {
				bools.appendChild(e->name).set(elb->getVal());
			}
			if (el3) {
				groups.appendChild(e->name).set(el3->getVal());
			}
			//floats.appendChild(e->name).set(((slider*)e)->getVal());
		}
		xmlSettings.save(xml);
	}
};

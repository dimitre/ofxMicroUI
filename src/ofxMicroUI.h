class ofxMicroUI : public ofBaseApp {
public:

	map <string, float>	pFloat;
	map <string, bool>	pBool;

	glm::vec2 xy = glm::vec2(10,10);

	struct microUISettings {
		map <string, int> pInt;
		int margin = 10;
		int spacing = 4;
		ofRectangle sliderRect = ofRectangle(0,0,240,20);
		
		/*
		offset x, offset y (padding inside column)
		column rectangle
		slider dimensions.
		*/
		
	} settings;
	
	class element {
	public:
		// esqueleto padrao
		string name = "";
		ofRectangle rect = ofRectangle(0,0,240,20);
		ofRectangle rectVal = ofRectangle(0,0,240,20);
		float min = 0;
		float max = 1;
		float val = 0.5;
		
		bool wasPressed = false;
		
		virtual void set(float v) {}
		virtual void set(int v) {}
		virtual void set(bool v) {}
		virtual void set(string v) {}
		virtual void set(glm::vec2 v) {}

		// c++14
//		virtual auto getVal() {
//			return val;
//		}
		virtual void drawLabel() {
			ofSetColor(255);
			ofDrawBitmapString(name + " " + ofToString(val), rect.x + 5, rect.y + 16);
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

	};
	

	class slider : public element {
	public:
		float * _val = NULL;
		
		slider(string n, glm::vec3 val, glm::vec2 xy, float & v) { // : name(n)
			name = n;
			_val = &v;
			rect.x = xy.x;
			rect.y = xy.y;
			rectVal = rect;
			min = val.x;
			max = val.y;
			set(val.z);
		}
		
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
			val = v;
			if (_val != NULL) {
				*_val = v;
			}
			rectVal.width = ofMap(v, min, max, 0, rect.width);
		}
		
		void setValFromMouse(int x, int y) {
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
		
		bool getVal() {
			return *_val;
		}
		
		void set(bool v) {
			*_val = v;
		}
		
		void toggle() {
//			cout << "toggle" << endl;
//			cout << *_val << endl;
			set(*_val ^ 1);
		}
		
		void setValFromMouse(int x, int y) {
			cout << "setValFromMouse Bool" << endl;
			cout << wasPressed << endl;
			if (wasPressed) {
				toggle();
			}
		}
		
		booleano (string n, bool val, glm::vec2 xy, bool & v) {
			name = n;
			_val = &v;
			set(val);
			rect.x = xy.x;
			rect.y = xy.y;
		}
		
		void drawElement() {
			ofSetColor(127);
			ofDrawRectangle(rect.x, rect.y, 20, 20);
			
			if (*_val) {
				ofSetColor(40);
				ofDrawRectangle(rect.x + 5, rect.y + 5, 10, 10);
				//ofDrawRectangle(rect);
			}
//			ofSetColor(127);
//			ofDrawRectangle(rect);
//			ofSetColor(80);
//			ofDrawRectangle(rectVal);
		}
	};

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
	
	void advanceLine() {
		xy.y += 20 + settings.spacing;
	}
	
	void newCol() {
		xy.x += 240 + settings.spacing;
		xy.y = settings.margin;
	}
	
	void createFromText(string fileName) {
		alert("createFromText " + fileName);
		vector <string> lines = textToVector(fileName);
		for (auto & l : lines) {
			if (l == "") {
				advanceLine();
			}
			else if (l == "newCol") {
				newCol();
			}
			//cout << l << endl;
			vector <string> cols = ofSplitString(l, "\t");
			if (cols[0] == "float") {
				//cout << l << endl;
				vector <string> values = ofSplitString(cols[2]," ");
				glm::vec3 vals = glm::vec3(ofToFloat(values[0]),ofToFloat(values[1]),ofToFloat(values[2]));
				string name = cols[1];
				//elements.emplace_back( slider(name, vals, xy, pFloat[name]));
				elements.push_back(new slider(name, vals, xy, pFloat[name]));

				advanceLine();
			}
			
			else if (cols[0] == "bool") {
				//cout << l << endl;
				bool val = ofToBool(cols[2]);
				string name = cols[1];
				pBool[name] = val;
//				elements.emplace_back( booleano (name, val, xy, pBool[name]));
				elements.push_back(new booleano (name, val, xy, pBool[name]));
				advanceLine();
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

				for (auto & e : elements) {
					if (floats.getChild(e->name)) {
						auto valor = floats.getChild(e->name).getFloatValue();
						e->set(valor);
					}
					if (bools.getChild(e->name)) {
						auto valor = bools.getChild(e->name).getBoolValue();
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

		for (auto & e : elements) {
			slider * els = dynamic_cast<slider*>(e);
			booleano * elb = dynamic_cast<booleano*>(e);
			if (els) {
				floats.appendChild(e->name).set(els->getVal());
			}
			if (elb) {
				bools.appendChild(e->name).set(elb->getVal());
			}
			//floats.appendChild(e->name).set(((slider*)e)->getVal());
		}
		xmlSettings.save(xml);
	}
};

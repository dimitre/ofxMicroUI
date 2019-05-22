class ofxMicroUI : public ofBaseApp {
public:

	map <string, float>	pFloat;

	glm::vec2 xy = glm::vec2(10,10);

	class element {
	public:
		// slider padrao
		string name = "";
		ofRectangle rect = ofRectangle(0,0,240,20);
		ofRectangle rectVal = ofRectangle(0,0,240,20);
		float min = 0;
		float max = 1;
		float val = 0.5;
		float * _val = NULL;
		
		bool wasPressed = false;
		

		void set(float v) {
			val = v;
			if (_val != NULL) {
				*_val = v;
			}
			rectVal.width = ofMap(v, min, max, 0, rect.width);
		}
		
		virtual float getVal() {
			return val;
		}

		
		element(string n, glm::vec3 val, glm::vec2 xy, float & v) : name(n) {
			_val = &v;
			rect.x = xy.x;
			rect.y = xy.y;
			rectVal = rect;
			min = val.x;
			max = val.y;
			set(val.z);
		}

		virtual void draw() {
			ofSetColor(127);
			ofDrawRectangle(rect);
			ofSetColor(80);
			ofDrawRectangle(rectVal);
			ofSetColor(255);
			ofDrawBitmapString(name + " " + ofToString(val), rect.x + 5, rect.y + 16);
		}
		
		virtual void setValFromMouse(int x, int y) {
			int xx = ofClamp(x, rect.x, rect.x + rect.width);
			int yy = ofClamp(y, rect.y, rect.y + rect.height);
			ofPoint xy = ofPoint (xx,yy) - ofPoint(rect.x, rect.y);
			ofPoint wh = ofPoint (rect.width, rect.height);
			ofPoint val = min + (max-min)*(xy/wh);
			set(val.x);
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
	};

	
	vector <element> elements;

	void setupUI() {
		createFromText("m.txt");
	}

	void mouseUI(int x, int y, bool pressed) {
		for (auto & e : elements) {
			e.checkMouse(x, y, pressed);
		}
	}

	void onDraw(ofEventArgs &data) {
		for (auto & e : elements) {
			e.draw();
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
			e.mouseRelease(data.x, data.y);
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
		cout << "===== ofxMicroUI " << s << endl;
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
			if (l == "") {
				xy.y+= 20 + 4;
			}
			//cout << l << endl;
			vector <string> cols = ofSplitString(l, "\t");
			if (cols[0] == "float") {
				//cout << l << endl;
				vector <string> values = ofSplitString(cols[2]," ");
				glm::vec3 vals = glm::vec3(ofToFloat(values[0]),ofToFloat(values[1]),ofToFloat(values[2]));
				string name = cols[1];
				elements.emplace_back( element(name, vals, xy, pFloat[name]));
				xy.y+= 20 + 4;
			}
		}
	}
	
	void load(string xml) {
		alert("load " + xml);
		
		if (ofFile::doesFileExist(xml)) {
			ofXml xmlSettings;
			xmlSettings.load(xml);
			int UIVersion = xmlSettings.getChild("ofxDmtrUIVersion").getIntValue();
			if (UIVersion == 4 || UIVersion == 5)
			{
				auto xmlElements = 	xmlSettings.getChild("element");
				auto floats = 		xmlElements.findFirst("float");
				for (auto & e : elements) {
					if (floats.getChild(e.name)) {
						auto valor = floats.getChild(e.name).getFloatValue();
						e.set(valor);
					}
				}
			}
		}
	}
	
	void save(string xml) {
		alert("save " + xml);

		int version = 5;
		ofXml xmlSettings;
		xmlSettings.appendChild("ofxDmtrUIVersion").set(version);
		auto xmlElements = 	xmlSettings.appendChild("element");
		auto floats = xmlElements.appendChild("float");
		
		for (auto & e : elements) {
			floats.appendChild(e.name).set(e.getVal());
		}
		xmlSettings.save(xml);
	}
};

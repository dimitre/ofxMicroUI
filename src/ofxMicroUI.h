// forwared declaration
//class ofxMicroUISoftware;



class ofxMicroUI : public ofBaseApp {
public:
	friend class element;
#include "ofxMicroUISettings.h"
#include "ofxMicroUIElements.h"
#include "ofxMicroUIParseText.h"

	microUISettings * _settings = &settingsUI;
	
	ofxMicroUI() {
		alert("microUI setup " + textFile);
		ofAddListener(ofEvents().draw, this, &ofxMicroUI::onDraw);
		//ofAddListener(ofEvents().mouseMoved, this, &ofxMicroUI::onMouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &ofxMicroUI::onMousePressed);
		ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUI::onMouseDragged);
		ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUI::onMouseReleased);
		
		ofAddListener(ofEvents().update, this, &ofxMicroUI::onUpdate);
	}
	
	~ofxMicroUI() {
		alert("destroy " + textFile);
	}

	void onUpdate(ofEventArgs &data) {
		//update();
		float easing = 10.0;
		for (auto & p : pEasy) {
			if (easing > 0) {
				if (ABS(pEasy[p.first] - pFloat[p.first]) > 0.000007) {  //0.00007
					pEasy[p.first] += (pFloat[p.first] - pEasy[p.first])/easing;
				} else {
					pEasy[p.first] = pFloat[p.first];
				}
			}
			else {
				pEasy[p.first] = pFloat[p.first];
			}
		}
	}
	
	// EVENTS
	ofEvent<element> uiEvent;
	ofEvent<element*> microUIEvent;
	//	ofEvent<microUIEventObject> microUIEvent;

	
	
	// REWRITE EVERYTHING
	map <string, ofxMicroUI> uis;
	glm::vec2 xy = glm::vec2(0,0);
	int margem = 10;
	ofxMicroUI * _lastUI = this;
	void addUI(string t, bool down = false) {
		//cout << "lastui pos " << _lastUI->uiPos << endl;
		if (!_lastUI->updatedRect) {
			_lastUI->updateRect();
		}
		if (down) {
			xy += glm::vec2(0, _lastUI->rect.height + margem);
		} else {
			xy.y = 0;
			xy += glm::vec2(_lastUI->rect.width + margem, 0);
		}
		uis[t].uiPos = xy;
		uis[t]._settings = _settings;

		string file = t + ".txt";
		cout << "addUI :: " << file << endl;
		uis[t].createFromText(t + ".txt");
		
		_lastUI = &uis[t];
	}
	
	void microUIDraw() {
		if (redrawUI) {
			fbo.begin();
			ofClear(0,0);

			ofSetColor(0, 200);
			ofDrawRectangle(rect);

			ofSetColor(255);
			for (auto & e : elements) {
				e->draw();
			}
			fbo.end();
			redrawUI = false;
		}
		
		if (visible) {
			ofSetColor(255);
			fbo.draw(rect.getPosition() + ofPoint(uiPos));
		}
	}
	
	bool visible = true;
		
	enum microUIVarType {
		MICROUI_FLOAT,
		MICROUI_INT,
		MICROUI_STRING,
		MICROUI_BOOLEAN,
		MICROUI_POINT,
		MICROUI_COLOR,
		MICROUI_VEC3,
	};

	map <string, float>		pFloat;
	map <string, bool>		pBool;
	map <string, string>	pString;
	map <string, glm::vec3>	pVec3;
	
	// future usage
	map <string, glm::vec2>	pVec2;
	map <string, ofColor> 	pColor;
	map <string, ofColor> 	pColorEasy;
	map <string, int>		pInt;
	map <string, float>		pEasy;
	
	vector <element*> elements;
	

	// position to draw UI on screen (and handle mouse events)
	glm::vec2 uiPos = glm::vec2(0, 0);
	ofRectangle rect = ofRectangle(0,0,0,0);	
	ofFbo fbo;

	// EVERYTHING MOUSE
	void mouseUI(int x, int y, bool pressed) {
		x -= uiPos.x;
		y -= uiPos.y;
		for (auto & e : elements) {
			e->checkMouse(x, y, pressed);
		}
		redrawUI = true;
	}

	void onDraw(ofEventArgs &data) {
		microUIDraw();
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
			e->mouseRelease(data.x - uiPos.x, data.y - uiPos.y);
		}
		redrawUI = true;
	}
	
	
	// TOOLS
	
	vector <string> textToVector(string file) {
		vector <string> saida;
		ofBuffer buff2 = ofBufferFromFile(file);
		for(auto & line: buff2.getLines()) {
			saida.push_back(line);
		}
		return saida;
	}
	
	void alert(string s) {
		cout << ":: ofxMicroUI :: " << s << endl;
	}
	
	void messageBox(string s) {
		vector <string> linhas = ofSplitString(s, "\r");
		int size = 0;
		for (auto & l : linhas) {
			size = MAX(size, l.size());
		}
		// cout << "messagebox :: " << s << endl;
		// cout << "size = " << size << endl;
		for (int a=0; a<size+4; a++) {
			cout << "-" ;
		}
		cout << endl;
		
		for (auto & l : linhas) {
			string spaces = "";
			int difSize = (size - l.size());
			//cout << difSize << endl;
			if (difSize) {
				for (int a=0; a<difSize; a++) {
					spaces += " ";
				}
			}
			cout << "| " << l << spaces << " |" << endl;
		}
		for (int a=0; a<size+4; a++) {
			cout << "-" ;
		}
		cout << endl;
	}
	
	void expires(int dataInicial, int dias = 10) {
		time_t rawtime;
		struct tm * timeinfo;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		int segundosPorDia = 86400;
		int segundosExpira = segundosPorDia * dias;
		float diasExpira = (segundosExpira - (difftime(rawtime,dataInicial))) / (float)segundosPorDia;
		
		string notice = "Dmtr " + ofToString(rawtime) + " :: ";
		notice +=  "Expires in " + ofToString(diasExpira) + " days";
		messageBox(notice);
		//cout << "-------- Dmtr Expires: " ;
		//cout << rawtime;
		//cout << "expires in " + ofToString(diasExpira) + " days" << endl;
		if (diasExpira < 0 || diasExpira > dias) {
			ofSystemAlertDialog("Dmtr.org Software Expired ~ " + ofToString(dataInicial) + "\rhttp://dmtr.org/");
			std::exit(1);
		}
	}

	
	// move to another place
	bool presetIsLoading = false;
	
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
				auto strings = 		xmlElements.findFirst("string");
				auto vec3s = 		xmlElements.findFirst("group");

//				_settings->presetIsLoading = true;
				presetIsLoading = true;
				for (auto & e : elements) {
					if (floats.getChild(e->name)) {
						auto valor = floats.getChild(e->name).getFloatValue();
						e->set(valor);
					}
					if (bools.getChild(e->name)) {
						auto valor = bools.getChild(e->name).getBoolValue();
						e->set(valor);
					}
					if (strings.getChild(e->name)) {
						auto valor = strings.getChild(e->name).getValue();
						e->set(valor);
					}
					if (vec3s.getChild(e->name)) {
						auto valor = vec3s.getChild(e->name).getValue();
						//cout << valor << endl;
						e->set(valor);
					}
				}
//				_settings->presetIsLoading = false;
				presetIsLoading = false;

			}
		}
		redrawUI = true;
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
		auto strings = xmlElements.appendChild("string");

//		_settings->presetIsLoading = true;
		presetIsLoading = true;
		for (auto & e : elements) {
			// not the best way of differentiate elements.
			// I'll implement element kind or var kind
			// change to element kind.
			slider * els = dynamic_cast<slider*>(e);
			booleano * elb = dynamic_cast<booleano*>(e);
			radio * elr = dynamic_cast<radio*>(e);
			vec3 * el3 = dynamic_cast<vec3*>(e);
			
			if (els) {
				floats.appendChild(e->name).set(els->getVal());
			}
			if (elb) {
				bools.appendChild(e->name).set(elb->getVal());
			}
			if (elr) {
				strings.appendChild(e->name).set(elr->getVal());
			}
			if (el3) {
				groups.appendChild(e->name).set(el3->getVal());
			}
			//floats.appendChild(e->name).set(((slider*)e)->getVal());
		}
//		_settings->presetIsLoading = false;
		presetIsLoading = false;

		xmlSettings.save(xml);
	}
	
	void savePreset(int n) {
		save("_presets/" + ofToString(n) + "_master.xml");
		for (auto & u : uis) {
			u.second.save("_presets/" + ofToString(n) + "_" + u.first + ".xml");
		}
	}
	
	void loadPreset(int n) {
		load("_presets/" + ofToString(n) + "_master.xml");
		for (auto & u : uis) {
			u.second.load("_presets/" + ofToString(n) + "_" + u.first + ".xml");
		}
	}
	
	void saveOrLoad(string n) {
		//cout << "save or load " << n << endl;
		if (ofGetKeyPressed(OF_KEY_COMMAND)) {
			save(n);
		} else {
			load(n);
		}
	}
	
	void clear() {
		initFlow();
		elements.clear();

		pFloat.clear();
		pInt.clear();
		pBool.clear();
		pString.clear();
		pColor.clear();
	}
	

	
	// FLOW
	
	/*
	 It was recently moved from settings. variables to flow the element coordinates.
	 makes more sense to be part of the ui object.
	 */
	
	// NEW. try to implement it.
	bool useLabelOnNewElement = true;
	bool flowVert = true;
	bool redrawUI = true;
	glm::vec2 flowXY = glm::vec2(_settings->margin, _settings->margin);
	float xBak = 0;
	
	// this rectangle stores the last element size to flow the element coordinates
	ofRectangle flowRect;
	
	void initFlow() {
		flowXY = glm::vec2(_settings->margin, _settings->margin);
	}
	
	void setFlowVert(bool s) {
		// if flow was horizontal and we change to horizontal, save the x coordinate
		if (flowVert && !s) {
			xBak = flowXY.x;
		}
		// if flow was vertical and we change to vertical, bring back the backup x coordinate.
		if (!flowVert && s) {
			flowXY.x = xBak;
		}
		flowVert = s;
	}

	bool advanceLayout() {
		bool success = true;
		if (flowVert) {
			flowXY.y += flowRect.height + _settings->elementSpacing;
		} else {
			int newX = flowXY.x + flowRect.width + _settings->elementSpacing - xBak;
			//int newX = flowXY.x + flowRect.width + _settings->elementSpacing ;
			if (newX > _settings->elementRect.width ) {
				success = false;
				flowXY.y += flowRect.height + _settings->elementSpacing;
				flowXY.x = xBak;
			} else {
				flowXY.x += flowRect.width + _settings->elementSpacing;
			}
		}
		return success;
	}
	
	void newLine() {
		flowXY.y += flowRect.height + _settings->elementSpacing;
	}
	
	void newCol() {
		flowXY.x += _settings->elementRect.width + _settings->margin;
		flowXY.y = _settings->margin;
	}
	
	
	
	map <string, vector<string> > templateUI;
	string buildingTemplate = "";
	map <string, vector <string> > templateVectorString;
	
	
	vector <string> futureLines;

};


#include "ofxMicroUISoftware.h"

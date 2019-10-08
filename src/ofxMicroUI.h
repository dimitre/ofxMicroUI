
class ofxMicroUI : public ofBaseApp {
public:
	friend class element;
#include "ofxMicroUISettings.h"
#include "ofxMicroUIElements.h"
#include "ofxMicroUIParseText.h"

	
	// UI Basic Settings
	microUISettings * _settings = &settingsUI;
	string uiName = "master";
	// position to draw UI on screen (and handle mouse events)
	ofRectangle rectPos = ofRectangle(0,0,0,0);
	ofRectangle rect = ofRectangle(0,0,0,0); // this rectangle have the coordinate 0,0,w,h
	ofFbo fbo;
	bool visible = true;

	
	// UI VARIABLES
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
	
	// Not used yet
	enum microUIVarType {
		MICROUI_FLOAT,
		MICROUI_INT,
		MICROUI_STRING,
		MICROUI_BOOLEAN,
		MICROUI_POINT,
		MICROUI_COLOR,
		MICROUI_VEC3,
	};
	
	
	// UI EVENTS
	ofEvent<element> uiEvent;
	ofEvent<element*> microUIEvent;
	//	ofEvent<microUIEventObject> microUIEvent;
	
	
	// control saving and loading of presets
	enum loadSaveType {
		NONE, PRESETSFOLDER, MASTER
	};
	loadSaveType loadMode = PRESETSFOLDER;
	loadSaveType saveMode = PRESETSFOLDER;


	//
	// TODO : renaming -  variable to handle new elements if they save to preset or not
	bool saveXmlOnNewElement = true;
	// NEW. try to implement it.
	bool useLabelOnNewElement = true;
	string tagOnNewElement = "";
	

	
	
	ofxMicroUI() {
		//alert("microUI setup ");
		ofAddListener(ofEvents().draw, this, &ofxMicroUI::onDraw);
		//ofAddListener(ofEvents().mouseMoved, this, &ofxMicroUI::onMouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &ofxMicroUI::onMousePressed);
		ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUI::onMouseDragged);
		ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUI::onMouseReleased);
		
		ofAddListener(ofEvents().update, this, &ofxMicroUI::onUpdate);
	}
	
	~ofxMicroUI() {
		//alert("destroy " + textFile);
	}

	void onUpdate(ofEventArgs &data) {
		//update();
		//float easing = 10.0;
		float easing = _settings->easing;
		for (auto & p : pEasy) {
			if (easing > 0.4) {
				if (ABS(pEasy[p.first] - pFloat[p.first]) > 0.000001) {  //0.00007 //0.000007
					pEasy[p.first] += (pFloat[p.first] - pEasy[p.first]) / easing;
				} else {
					pEasy[p.first] = pFloat[p.first];
				}
			}
			else {
				pEasy[p.first] = pFloat[p.first];
			}
		}
	}
	
	void microUIDraw() {
		if (redrawUI) {
			fbo.begin();
			ofClear(0,0);

			ofSetColor(_settings->colorUIBg);
			//ofSetColor(0, 200);
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
			fbo.draw(rectPos.getPosition());
		}
	}
	


	// EVERYTHING MOUSE
	void mouseUI(int x, int y, bool pressed) {
		//cout << rect << endl;
		if (visible && rectPos.inside(x, y)) {
			x -= rectPos.x;
			y -= rectPos.y;
//			redrawUI = false;
			for (auto & e : elements) {
				e->checkMouse(x, y, pressed);
//				if (e->checkMouse(x, y, pressed)) {
//				}
			}
			redrawUI = true;
		}
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
			e->mouseRelease(data.x - rectPos.x, data.y - rectPos.y);
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
		unsigned int size = 0;
		for (auto & l : linhas) {
			size = MAX(size, l.size());
		}
		// cout << "messagebox :: " << s << endl;
		// cout << "size = " << size << endl;
		for (unsigned int a=0; a<size+4; a++) {
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
		for (unsigned int a=0; a<size+4; a++) {
			cout << "-" ;
		}
		cout << endl;
	}
	
	void expires(int dataInicial, int dias = 10) {
		time_t rawtime;
		time ( &rawtime );
		//struct tm * timeinfo = localtime ( &rawtime );
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
		if (ofFile::doesFileExist(xml)) {
			//alert("load " + xml);
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

//				presetIsLoading = true;
				for (auto & e : elements) {
					if (e->saveXml) {
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
				}
//				presetIsLoading = false;
			}
		} else {
			//alert("load :: not found: " + xml);
		}
		redrawUI = true;
	}
	

	void save(string xml) {
		//alert("save " + xml);
		int version = 1;
		ofXml xmlSettings;
		xmlSettings.appendChild("ofxMicroUI").set(version);
		auto xmlElements = 	xmlSettings.appendChild("element");
		auto floats = xmlElements.appendChild("float");
		auto bools = xmlElements.appendChild("boolean");
		auto groups = xmlElements.appendChild("group");
		auto strings = xmlElements.appendChild("string");

//		presetIsLoading = true;
		for (auto & e : elements) {
			if (e->saveXml) {
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
			}
		}
//		presetIsLoading = false;
		xmlSettings.save(xml);
	}
	
//	void saveOrLoad(string n) {
//		if (ofGetKeyPressed(OF_KEY_COMMAND)) {
//			save(n);
//		} else {
//			load(n);
//		}
//	}

	string presetsRootFolder = "_presets";
	string presetsFolder = "1";
	string getPresetPath(bool create = false) {
		if (create && !ofFile::doesFileExist(presetsRootFolder)) {
			ofDirectory::createDirectory(presetsRootFolder);
		}
		string fullPath = presetsRootFolder + "/" + presetsFolder;
		//cout << fullPath << endl;
		if (create && !ofFile::doesFileExist(fullPath)) {
			ofDirectory::createDirectory(fullPath);
		}
		return fullPath;
	}
	
	void savePreset(string n) {
		alert("savePreset " + n);
		presetIsLoading = true;

		string presetFolder = getPresetPath(true) + "/" + n;
		if (!ofFile::doesFileExist(presetFolder)) {
			ofDirectory::createDirectory(presetFolder);
		}
		for (auto & u : uis) {
			if (u.second.saveMode == PRESETSFOLDER) {
				u.second.save(presetFolder + "/" + u.first + ".xml");
			}
		}
		presetIsLoading = false;
		
		if (presetElement != NULL) {
			presetElement->hasXmlCheck();
		}
	}
	
	void loadPreset(string n) {
		alert("loadPreset " + n);
		presetIsLoading = true;
		string presetFolder = getPresetPath() + "/" + n;
		for (auto & u : uis) {
			if (u.second.loadMode == PRESETSFOLDER) {
				u.second.load(presetFolder + "/" + u.first + ".xml");
			}
		}
		presetIsLoading = false;
	}
	
	void saveOrLoadAll(string n) {
		if (ofGetKeyPressed(OF_KEY_COMMAND)) {
			savePreset(n);
		} else {
			loadPreset(n);
		}
	}

	presets * presetElement = NULL;
	void setPresetsFolder(string s) {
		presetsFolder = s;
		if (presetElement != NULL) {
			presetElement->hasXmlCheck();
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
	

	
	// FLOW ELEMENTS
	/*
	 It was recently moved from settings. variables to flow the element coordinates.
	 makes more sense to be part of the ui object.
	 */
		
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
			
			if ((newX - _settings->elementSpacing) > _settings->elementRect.width ) {
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
	
	
	// TEMPLATE
	
	map <string, vector<string> > templateUI;
	string buildingTemplate = "";
	map <string, vector <string> > templateVectorString;
	
	
	
	// Removing soon
	vector <string> futureLines;

	void toggleVisible() {
		visible ^= 1;
		for (auto & u : uis) {
			u.second.visible = visible;
		}
	}
	
	
	
	
	
	
	// LAYOUT UIS
	// REWRITE EVERYTHING
	map <string, ofxMicroUI> uis;
	glm::vec2 xy = glm::vec2(0,0);
	int margem = 10;
	ofxMicroUI * _lastUI = this;
	void addUI(string t, bool down = false) {
		if (!_lastUI->updatedRect) {
			_lastUI->updateRect();
		}
		if (down) {
			xy += glm::vec2(0, _lastUI->rect.height + margem);
		} else {
			xy.y = 0;
			xy += glm::vec2(_lastUI->rect.width + margem, 0);
		}
		uis[t].uiName = t;
		uis[t].rectPos.x = xy.x;
		uis[t].rectPos.y = xy.y;
		uis[t]._settings = _settings;

		string file = t + ".txt";
		//alert ("addUI :: " + file);
		uis[t].createFromText(t + ".txt");
		_lastUI = &uis[t];
	}
};


#include "ofxMicroUISoftware.h"

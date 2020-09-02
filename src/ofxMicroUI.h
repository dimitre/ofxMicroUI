#pragma once

class ofxMicroUI : public ofBaseApp {
public:
	friend class element;
	class element;
	
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
	
	map <string,ofImage>	pImage;
	map <string,ofVideoPlayer>	pVideo;

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
	ofEvent<string> uiEventMaster;
	
	// Try to use it.
	ofEvent<element*> uiEvent2;
	ofEvent<element&> uiEvent3;
	//	ofEvent<microUIEventObject> microUIEvent;
	
	
	// control saving and loading of presets
	enum loadSaveType {
		NONE, PRESETSFOLDER, MASTER
	};
	loadSaveType loadMode = PRESETSFOLDER;
	loadSaveType saveMode = PRESETSFOLDER;


	// MOVE TO SETTINGS?
	// TODO : renaming -  variable to handle new elements if they save to preset or not
	bool saveXmlOnNewElement = true;
	// NEW. try to implement it.
	bool useLabelOnNewElement = true;
	string tagOnNewElement = "";
	string tagOnNewUI = "";

	
	bool hasListeners = false;
	
	string uiTag = "";

	ofxMicroUI() {
		//alert("microUI setup ");
		
	}
	
	~ofxMicroUI() {
		//alert("destroy " + textFile);
	}

	
	void addListeners() {
		hasListeners = true;
		ofAddListener(ofEvents().draw, this, &ofxMicroUI::onDraw);
		//ofAddListener(ofEvents().mouseMoved, this, &ofxMicroUI::onMouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &ofxMicroUI::onMousePressed);
		ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUI::onMouseDragged);
		ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUI::onMouseReleased);
		ofAddListener(ofEvents().update, this, &ofxMicroUI::onUpdate);
	}

	void onUpdate(ofEventArgs &data) {
		if (willChangePreset != "") {
			presetElement->set(willChangePreset);
			willChangePreset = "";
		}
		//update();
		
		if (_settings->easing) {
			for (auto & c : pColorEasy) {
				c.second.lerp(pColor[c.first], _settings->easing);
			}
			
			//		float ofLerp(float start, float stop, float amt)
			for (auto & p : pEasy) {
				p.second = ofLerp(p.second, pFloat[p.first], _settings->easing);
			}
		}
		
		else {
			pColorEasy = pColor;
			pEasy = pFloat;
		}
		
		//float easing = 10.0;
//		float easing = _settings->easing;

//		for (auto & p : pEasy) {
//			//cout << p.first << endl;
//			if (easing > 0.4) {
//				if (ABS(pEasy[p.first] - pFloat[p.first]) > 0.000001) {  //0.00007 //0.000007
//					pEasy[p.first] += (pFloat[p.first] - pEasy[p.first]) / easing;
//				} else {
//					pEasy[p.first] = pFloat[p.first];
//				}
//			}
//			else {
//				pEasy[p.first] = pFloat[p.first];
//			}
//		}
	}
	
	void microUIDraw() {
		if (redrawUI) {
			fbo.begin();
			ofClear(0,0);
//			ofSetColor(_settings->uiColorBg);
			ofSetColor(uiColorBg);
			
//			cout << "microUI redrawUI " << uiName << " :: " << rect << "" << endl;
//			cout << rect << endl;
			ofDrawRectangle(rect);

			ofSetColor(255);
			for (auto & e : elements) {
				if (!e->alwaysRedraw) {
					e->draw();
				}
			}
			fbo.end();
			redrawUI = false;
		}
		

		if (visible && _settings->visible) {
			fbo.begin();
			for (auto & e : elements) {
				if (e->haveToRedraw) {
					e->redrawElement();
					//e->draw();
				}
			}
			fbo.end();
			
			ofSetColor(255, _settings->uiOpacity);
//			ofSetColor(255, uiOpacity);
			fbo.draw(rectPos.getPosition() + _settings->offset);
			
			
			// melhorar com lookup isso aqui
			ofPushMatrix();
			ofTranslate(rectPos.getPosition() + _settings->offset);
			for (auto & e : elements) {
				if (e->alwaysRedraw) {
					e->draw();
				}
			}
			ofPopMatrix();
		}
		
	}
	

	// EVERYTHING MOUSE
	void mouseUI(int x, int y, bool pressed) {
		// novidade, offset implementado
		
		int xx = x - _settings->offset.x;
		int yy = y - _settings->offset.y;
		if (_settings->visible && visible && rectPos.inside(xx, yy)) {
			xx -= rectPos.x;
			yy -= rectPos.y;
			// future : break if element is found. in the case no ui overlap.
			for (auto & e : elements) {
				e->checkMouse(xx, yy, pressed);
			}
		}
	}

	void onDraw(ofEventArgs &data) {
		microUIDraw();
	}
	
	void onMousePressed(ofMouseEventArgs &data) {
		mouseUI(data.x, data.y, true);
	}
	
	void onMouseDragged(ofMouseEventArgs &data) {
		mouseUI(data.x, data.y, false);
	}
	
	void onMouseReleased(ofMouseEventArgs &data) {
		// mouseRELEASE
		// XAXA - set false wasPressed in each.
		int xx = data.x - rectPos.x - _settings->offset.x;
		int yy = data.y - rectPos.y - _settings->offset.y;

		for (auto & e : elements) {
//			e->mouseRelease(data.x - rectPos.x, data.y - rectPos.y);
			e->mouseRelease(xx, yy);
		}
	}
	
	
	// TOOLS
	static vector <string> textToVector(string file) {
		vector <string> saida;
		ofBuffer buff2 = ofBufferFromFile(file);
		for(auto & line: buff2.getLines()) {
			saida.push_back(line);
		}
		return saida;
	}
	
	static void alert(string s) {
		cout << ":: ofxMicroUI :: " << s << endl;
	}
	
	static string messageBoxString(string s) {
		vector <string> linhas = ofSplitString(s, "\r");
		unsigned int size = 0;
		for (auto & l : linhas) {
			size = MAX(size, l.size());
		}

		string saida = "";
		saida += "+";
		for (unsigned int a=1; a<size+3; a++) {
			saida += "-" ;
		}
		saida += "+\r";
		
		for (auto & l : linhas) {
			string spaces = "";
			int difSize = (size - l.size());
			if (difSize) {
				for (int a=0; a<difSize; a++) {
					spaces += " ";
				}
			}
			saida += "| " + l + spaces + " |" + "\r";
		}
		
		saida += "+";
		for (unsigned int a=1; a<size+3; a++) {
			saida += "-" ;
		}
		saida += "+";
		saida += "\r";
//		cout << saida << endl;
		return saida;
	}
	
	static void messageBox(string s) {
		cout << messageBoxString(s) << endl;

	}
	
	static void expires(int dataInicial, int dias = 10) {
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

	
	// repeat here for master? maybe a better way of handling it?
	bool presetIsLoading = false;
	
	void load(string xml) {
		if (ofFile::doesFileExist(xml)) {
			presetIsLoading = true;

//			alert("load " + xml);
			ofXml xmlSettings;
			xmlSettings.load(xml);
			int UIVersion = xmlSettings.getChild("ofxMicroUI").getIntValue();
			if (UIVersion == 1)
			{
				auto xmlElements = 	xmlSettings.getChild("element");
				auto floats = 		xmlElements.findFirst("float");
				auto bools = 		xmlElements.findFirst("boolean");
				auto strings = 		xmlElements.findFirst("string");
				auto group = 		xmlElements.findFirst("group");
				auto vec2 = 		xmlElements.findFirst("vec2");

				for (auto & e : elements) {
					if (e->saveXml) {
						if (floats.getChild(e->name)) {
							auto valor = floats.getChild(e->name).getFloatValue();
							e->set(valor);
						}
						else if (bools.getChild(e->name)) {
							auto valor = bools.getChild(e->name).getBoolValue();
							e->set(valor);
						}
						else if (strings.getChild(e->name)) {
							auto valor = strings.getChild(e->name).getValue();
							e->set(valor);
						}
//						if (vec3s.getChild(e->name)) {
//							auto valor = vec3s.getChild(e->name).getValue();
//							//cout << valor << endl;
//							e->set(valor);
//						}
						else if (vec2.getChild(e->name)) {
							auto valor = vec2.getChild(e->name).getValue();
							e->set(valor);
//							cout << "loading slider2d " << e->name << " ::: " << valor << endl;
						}
						
						else if (group.getChild(e->name)) {
							auto x = 	group.getChild(e->name).getChild("x").getFloatValue();
							auto y = 	group.getChild(e->name).getChild("y").getFloatValue();
							auto sat = 	group.getChild(e->name).getChild("sat").getFloatValue();
							auto alpha = 	group.getChild(e->name).getChild("alpha").getFloatValue();
//							((colorHsv*)e)->set(glm::vec3(x, sat, y));
							((colorHsv*)e)->set(glm::vec4(x, sat, y, alpha));
						}
					}
				}
			}
			presetIsLoading = false;

		} else {
			//alert("load :: not found: " + xml);
		}
//		redraw();
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
		// falta vec3s, sera q esta em groups?
		auto vec2 = xmlElements.appendChild("vec2");

		for (auto & e : elements) {
			if (e->saveXml) {
				// not the best way of differentiate elements.
				// I'll implement element kind or var kind
				// change to element kind.
				slider * els = dynamic_cast<slider*>(e);
				booleano * elb = dynamic_cast<booleano*>(e);
				radio * elr = dynamic_cast<radio*>(e);
				vec3 * el3 = dynamic_cast<vec3*>(e);
				slider2d * el2 = dynamic_cast<slider2d*>(e);
				colorHsv * chsv = dynamic_cast<colorHsv*>(e);

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
				if (el2) {
//					cout << "saving slider 2d with the name of " << e->name << " and value " << el2->getVal() << endl;
					vec2.appendChild(e->name).set(el2->getVal());
				}
				if (chsv) {
					auto colorHsv = groups.appendChild(e->name);
					colorHsv.appendChild("x").set(chsv->xy.x);
					colorHsv.appendChild("y").set(chsv->xy.y);
					colorHsv.appendChild("sat").set(chsv->sat);
					colorHsv.appendChild("alpha").set(chsv->alpha);
				}
			}
		}
		xmlSettings.save(xml);
	}

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
	
	void loadPreset(string n) {
		
		alert("loadPreset " + n);
		_settings->presetIsLoading = true;
		string presetFolder = getPresetPath() + "/" + n;
		for (auto & u : allUIs) {
			if (u->loadMode == PRESETSFOLDER) {
				u->load(presetFolder + "/" + u->uiName + ".xml");
			}
		}
		_settings->presetIsLoading = false;
		
		string s = "loaded";
		ofNotifyEvent(uiEventMaster, s);
		
//		int contagem = 0;
//		for (auto & u : allUIs) {
//			if (u->loadingEvents.size()) {
//				cout << "|||| executando :  " << u->uiName << " :: " << u->loadingEvents.size() << endl;
//				contagem += u->loadingEvents.size();
//				for (auto & e : u->loadingEvents) {
//					ofNotifyEvent(uiEvent, *e);
//				}
//
//				u->loadingEvents.clear();
//			} else {
////				cout << "|||| loadingevents empty" << endl;
//			}
//		}
//		cout << "Total loadingEvents element items " << contagem << endl;
	}
	
	void loadPresetByIndex(int i) {
		string n = presetElement->getValByIndex(i);
		loadPreset(n);
		presetElement->redraw();
	}
	
	void savePreset(string n) {
		alert("savePreset " + n);
		_settings->presetIsLoading = true;

		string presetFolder = getPresetPath(true) + "/" + n;
		if (!ofFile::doesFileExist(presetFolder)) {
			ofDirectory::createDirectory(presetFolder);
		}
//		for (auto & u : uis) {
//			if (u.second.saveMode == PRESETSFOLDER) {
//				u.second.save(presetFolder + "/" + u.first + ".xml");
//			}
//		}
		for (auto & u : allUIs) {
			if (u->saveMode == PRESETSFOLDER) {
				u->save(presetFolder + "/" + u->uiName + ".xml");
			}
		}
		
		_settings->presetIsLoading = false;

		if (presetElement != NULL) {
			saveThumb(n);
			presetElement->hasXmlCheck();
		}
		presetElement->redraw();
	}
	
	
	void saveThumb(string n) {
		cout << "saveThumb :: " << n << endl;
		if (presetElement != NULL) {
			//presetItem * item = NULL;
			//presetElement->
			// mover isso pra dentro do objeto presets?
			presetItem * item = (presetItem *)presetElement->getElement(n);
			if (item != NULL) {
				ofFbo * _f = &item->fbo;
				if (presetElement->_fbo != NULL && _f != NULL) {
					_f->begin();
					ofClear(0,255);
//					presetElement->_fbo->draw(-_f->getWidth()*.5, -_f->getHeight()*0.5 ,_f->getWidth()*2, _f->getHeight()*2);
					ofSetColor(255);
					presetElement->_fbo->draw(
											  -_f->getWidth()*1,
											  -_f->getHeight()*1,
											  _f->getWidth()*3,
											  _f->getHeight()*3
											  );
					_f->end();

//					string file = getPresetPath(true) + "/" + n + "/0.tif";
					presetElement->redraw();
					string file = getPresetPath(true) + "/" + n + "/0.png";
					ofPixels pixels;
					_f->readToPixels(pixels);
					ofSaveImage(pixels, file);
					//bool ofSaveImage(const ofShortPixels &pix, ofBuffer &buffer, ofImageFormat format=OF_IMAGE_FORMAT_PNG, ofImageQualityType qualityLevel=OF_IMAGE_QUALITY_BEST)
				}
			}
		}
	}
	

	
//	void sceneChange(string n) {}
	
	void saveOrLoadAll(string n) {
		if (ofGetKeyPressed(OF_KEY_COMMAND)) {
			savePreset(n);
		} else {
			loadPreset(n);
		}
	}

	presets * presetElement = NULL;
	
	void setPresetsFolder(string s) {
		alert("setPresetsFolder :: " + s);
		presetsFolder = s;
		if (presetElement != NULL) {
			presetElement->hasXmlCheck();
			presetElement->redraw();

		}
	}
	
	void clear() {
		cout << "ofxMicroUI clear :: " << uiName << endl;
		rect.width = rect.height = 10;
		updatedRect = false;
		initFlow();
		elements.clear();

		pFloat.clear();
		pInt.clear();
		pBool.clear();
		pString.clear();
		pColor.clear();
		
		redrawUI = true;
	}
	

	
	// FLOW ELEMENTS
	/*
	 It was recently moved from settings. variables to flow the element coordinates.
	 makes more sense to be part of the ui object.
	 */
		
	bool flowVert = true;
	bool redrawUI = true;
//	glm::vec2 flowXY = glm::vec2(_settings->margin, _settings->margin);
//	glm::vec2 flowXY = glm::vec2(_settings->uiPadding, _settings->uiPadding);
	glm::vec2 flowXY;
	float xBak = 0;
	
	// this rectangle stores the last element size to flow the element coordinates
	ofRectangle flowRect;
	
	void initFlow() {
		
//		flowXY = glm::vec2(_settings->margin, _settings->margin);
//		cout << "initFlow " << uiName << " : " << _settings->uiPadding << endl;
		flowXY = glm::vec2(_settings->uiPadding, _settings->uiPadding);
//		cout << "initflow :: " << uiName << " :: " << flowXY << endl;
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
//		flowXY.y += flowRect.height + _settings->elementSpacing;
		flowXY.y += _settings->elementRect.height + _settings->elementSpacing;
	}
	
	void newCol() {
//		flowXY.x += _settings->elementRect.width + _settings->margin;
//		flowXY.y = _settings->margin;
		flowXY.x += _settings->elementRect.width + _settings->uiPadding;
		flowXY.y = _settings->uiPadding;
	}
	
	
	// TEMPLATE - melhorar essa porra
	string buildingTemplate = "";
	map <string, vector <string>> templateUI;
	map <string, vector <string>> templateVectorString;

	
//	void toggleVisible() {
//		visible ^= 1;
//		for (auto & u : uis) {
//			u.second.visible = visible;
//		}
//	}
//
	void toggleVisible() {
		_settings->visible ^= 1;
	}
	
	
	// Removing soon
	vector <string> futureLines;

	
	
	// LAYOUT UIS
	// REWRITE EVERYTHING
	map <string, ofxMicroUI> uis;
	glm::vec2 xy = glm::vec2(0,0);
	ofxMicroUI * _lastUI = this;
	ofxMicroUI * _masterUI = NULL;
	ofxMicroUI * _downUI = NULL;
	
	vector <ofxMicroUI *> allUIs;

	void addUI(string t, bool down = false, string loadText = "") {
//		cout << "addUI " << t << " isdown:" << (down ? "true" : "false") << endl;
		if (!_lastUI->updatedRect) {
			_lastUI->updateRect();
		}
		if (down) {
			xy += glm::vec2(0, _lastUI->rect.height + _settings->uiMargin);
		} else {
			xy.y = 0;
			xy += glm::vec2(_lastUI->rect.width + _settings->uiMargin, 0);
		}
		
		// pointer
		ofxMicroUI * u = &uis[t];
		
		// if I use uis map to load save, they are ordered alphabetically, this pointer fixes things up
		allUIs.push_back(u);
		
		u->uiName = t;
		u->_masterUI = this;
		u->rectPos.x = xy.x;
		u->rectPos.y = xy.y;
		u->_settings = _settings;
		u->uiColorBg = _settings->uiColorBg;
		u->uiOpacity = _settings->uiOpacity;
		u->rect.width = rect.width - u->_settings->uiMargin;
		u->uiTag = tagOnNewUI;

		if (down) {
			_lastUI->_downUI = u;
		}

		string file = t + ".txt";
		if (loadText != "") {
			file = loadText;
		}
//		u->initFlow();
		u->createFromText(file);
		_lastUI = u;
	}
	
	void redraw() {
		redrawUI = true;
		for (auto & u : uis) {
			u.second.redraw();
		}
	}
	

	// for quick ofxDmtrUI3 compatibility
	map <string, ofFbo> mapFbos;
	
	
	string willChangePreset = "";
	
	
	
	void set(string name, float v) {
		slider * e = getSlider(name);
		if (e != NULL) {
//			e->eventFromOsc = true;
			e->set(v);
		}
//		getSlider(name)->set(v);
	}

	void set(string name, int v) {
		slider * e = getSlider(name);
		if (e != NULL) {
			e->set(v);
		} else {
			cout << "set element is null : " << uiName << " :: " << name << endl;
		}
	}
	
	void set(string name, bool v) {
		toggle * e = getToggle(name);
		if (e != NULL) {
			e->set(v);
		}
	}

	void set(string name, string v) {
		radio * e = getRadio(name);
		if (e != NULL) {
			e->set(v);
		}
	}
	
	

//	void set(string name, ofPoint v) {
//		slider2d * e = getSlider2d(name);
//		if (e != NULL) {
//			e->set(v);
//		}
//	};
	
	
	vector <element*> loadingEvents;
	

	
	void setVisible (bool b) {
		visible = b;
		adjustUIDown();
	}

	void adjustUIDown() {
		if (_downUI != NULL) {
//			cout << "adjustUIDown :: " << uiName << endl;
			float posY = visible ? (rectPos.y + rect.height + _settings->uiMargin) : rectPos.y;
			_downUI->rectPos.y = posY;
			_downUI->adjustUIDown();
		}
	}

	// UI STYLE
	float uiOpacity = 230;
	ofColor uiColorBg = ofColor(0,0,0,230);
	
	
	ofColor stringHexToColor(string corString) {
		//int corInt = ofHexToInt(corString.substr(1));
		ofColor cor = ofColor::fromHex(ofHexToInt(corString.substr(1)));
		return cor;
	}
};

#include "ofxMicroUISoftware.h"

#pragma once

//#define FLOWFREE 1

// novidades aqui pra funcionar o software
// using namespace std;
// #include "ofBaseApp.h"

#include "ofMain.h"
// #include "ofEvents.h"

class ofxMicroUI { // : public ofBaseApp
public:
	friend class element;
	class element;
	
//	ofKey OF_KEY_SAVE = OF_KEY_SUPER;
	ofKey OF_KEY_SAVE = OF_KEY_ALT;

#include "ofxMicroUISettings.h"
#include "ofxMicroUIElements.h"
#include "ofxMicroUIParseText.h"
#include "ofxMicroUITools.h"

	bool verbose = false;
//	bool verbose = true;

	// UI Basic Settings
	microUISettings * _settings = &settingsUI;
	string uiName = "master";
	// position to draw UI on screen (and handle mouse events)
	ofRectangle rectPos = ofRectangle(0,0,0,0);
	ofRectangle rect = ofRectangle(0,0,0,0); // this rectangle have the coordinate 0,0,w,h
	ofFbo fbo;
	bool visible = true;
	
	presets * presetElement = NULL;
	
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
	map <string,ofSoundPlayer>	pAudio;

	map <string, string>	pText;
	map <string, ofTrueTypeFont>	pFont;

	map <string, ofVideoGrabber>	pCam;

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
	
	struct event {
		ofxMicroUI * _ui = NULL;
		string name = "";
		event(ofxMicroUI * u, string n) : _ui(u), name(n) {}
	};
	ofEvent <event> uiEventGeneral;

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
	
	// fazer struct com essas coisas. ou passar tudo apenas pro soft.
	string tagOnNewElement = "";
	string tagOnNewUI = "";
	ofColor uiColorTopOnNewUI = ofColor(0);

	
	bool hasListeners = false;
	
	string uiTag = "";

	ofxMicroUI() {
		addListeners();
	}
	
	ofxMicroUI(string s) {
		addListeners();
		createFromText(s);
	}

	~ofxMicroUI() {
	}

	void addListeners();

	void draw();

	void onUpdate(ofEventArgs &data) {
		if (willChangePreset != "") {
			presetElement->set(willChangePreset);
			willChangePreset = "";
		}
		//update();
		
		if (_settings->easing) {
			for (auto & p : pEasy) {
				p.second = ofLerp(p.second, pFloat[p.first], _settings->easing);
			}
			
			for (auto & c : pColorEasy) {
				c.second.lerp(pColor[c.first], _settings->easing);
			}
		}
		
		else {
			pColorEasy = pColor;
			pEasy = pFloat;
		}
	}
	
	element * _mouseElement = NULL;

	// EVERYTHING MOUSE
	void mouseUI(int x, int y, bool pressed) {
		// novidade, offset implementado
		
#ifdef FLOWFREE
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
#else
		if (_settings->visible && visible) { // && rectPos.inside(xx, yy)
			int xx = x - _settings->offset.x - rectPos.x;
			int yy = y - _settings->offset.y - rectPos.y;
			// future : break if element is found. in the case no ui overlap.
			if (pressed) {
				_mouseElement = NULL;
				for (auto & e : elements) {
					if (e->rect.inside(xx, yy)) {
						_mouseElement = e;
						e->checkMouse(xx, yy, pressed);
						break;
					}
				}
			} else {
				if (_mouseElement != NULL) {
					_mouseElement->checkMouse(xx, yy, pressed);
				}
			}
		}
#endif
	}


	void notify(string s) {
		ofNotifyEvent(uiEventMaster, s);
		event e = event(this, s);
		ofNotifyEvent(uiEventGeneral, e);
	}

	void onDraw(ofEventArgs &data) {
		draw();
	}
	void onSetup(ofEventArgs &data) {
		//cout << "||||| ofxMicroUI Setup" << endl;
		notify("setup");
	}

	void onMousePressed(ofMouseEventArgs &data) {
//		cout << "microui mouse pressed " << data.button << endl;
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
	
	void alert(string s) {
		cout << "ofxMicroUI " << uiName << " : " << s << endl;
	}

	// tools era aqui
	
	// repeat here for master? maybe a better way of handling it?
//	bool presetIsLoading = false;
	
	
	void load(string xml) {
		if (verbose) {
			alert("LOAD " + xml);
		}
		if (ofFile::doesFileExist(xml)) {
//			presetIsLoading = true;

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
						
						// not very good. I have to make an enum or a string to specify element types maybe.
						slider * els = dynamic_cast<slider*>(e);
						booleano * elb = dynamic_cast<booleano*>(e);
//						radio * elr = dynamic_cast<radio*>(e);
//						vec3 * el3 = dynamic_cast<vec3*>(e);
//						slider2d * el2 = dynamic_cast<slider2d*>(e);
//						colorHsv * chsv = dynamic_cast<colorHsv*>(e);
						
						if (els && floats.getChild(e->name)) {
							auto valor = floats.getChild(e->name).getFloatValue();
							slider * elf = dynamic_cast<slider*>(e);
//							cout << "float! set " << e->name << " : " << valor << endl;
							if (elf) {
								e->set(valor);
							}
//							e->set(valor);
						}
						else if (elb && bools.getChild(e->name)) {
							auto valor = bools.getChild(e->name).getBoolValue();
//							cout << "bool! set " << e->name << " : " << valor << endl;
							booleano * elb = dynamic_cast<booleano*>(e);
							if (elb && !elb->isBang) {
								e->set(valor);
							}
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
//			presetIsLoading = false;
			notify("load");
		} else {
			//alert("load :: not found: " + xml);
		}
//		redraw();
	}
	
	
	void elementsToXml() {
		
	}
	

	void save(string xml) {
		if (verbose) {
			alert("SAVE " + xml);
		}
		int version = 1;
		ofXml xmlSettings;
		xmlSettings.appendChild("ofxMicroUI").set(version);
		auto xmlElements = xmlSettings.appendChild("element");
		
		auto floats = xmlElements.appendChild("float");
		auto bools = xmlElements.appendChild("boolean");
		auto groups = xmlElements.appendChild("group");
		auto strings = xmlElements.appendChild("string");
		// falta vec3s, sera q esta em groups?
		auto vec2 = xmlElements.appendChild("vec2");

		
		// fazer uma função element to xml.
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
				group * elg = dynamic_cast<group*>(e);
				
				// temporary test
				if (elg && !elr) {
					cout << "element group with the name " << e->name << endl;
					for (auto & ee : ((group*)e)->elements) {
						cout << ee->name << endl;
					}
					cout << "-----" << endl;
				}

				if (els) {
					floats.appendChild(e->name).set(els->getVal());
				}
				if (elb && !elb->isBang) {
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
		cout << "ofxMicroUI::loadPreset " << n << endl;
		if (verbose) {
			alert("loadPreset " + n);
		}
//		cout << "PRESET IS LOADING BEGIN" << endl;
		_settings->presetIsLoading = true;
		string presetFolder = getPresetPath() + "/" + n;
		for (auto & u : allUIs) {
			if (u->loadMode == PRESETSFOLDER) {
				u->load(presetFolder + "/" + u->uiName + ".xml");
			}
		}
//		cout << "PRESET IS LOADING END" << endl;
		_settings->presetIsLoading = false;
		
		notify("loaded");
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

		for (auto & u : allUIs) {
			if (u->saveMode == PRESETSFOLDER) {
				u->save(presetFolder + "/" + u->uiName + ".xml");
			}
		}
		
		_settings->presetIsLoading = false;

		if (presetElement != NULL) {
			saveThumb(n);
			presetElement->hasXmlCheck();
            presetElement->redraw();
		}
	}
	
	void saveThumb(string n) {
		cout << "saveThumb :: " << n << endl;
		if (presetElement != NULL) {
			// mover isso pra dentro do objeto presets?
			presetItem * item = (presetItem *)presetElement->getElement(n);
			if (item != NULL) {
				ofFbo * _f = &item->fbo;
				if (presetElement->_fbo != NULL && _f != NULL) {
					_f->begin();
					ofClear(0,255);
					ofSetColor(255);
					
					float margin = 0.25;
					float margin2 = 1 + margin;
					
					presetElement->_fbo->draw(
							  -_f->getWidth()	* margin,
							  -_f->getHeight()	* margin,
							  _f->getWidth()	* margin2,
							  _f->getHeight()	* margin2
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
		if (ofGetKeyPressed(OF_KEY_SAVE)) {
			savePreset(n);
		} else {
			loadPreset(n);
		}
	}

	void setPresetsFolder(string s) {
//		alert("setPresetsFolder :: " + s);
		presetsFolder = s;
		if (presetElement != NULL) {
			presetElement->hasXmlCheck();
			presetElement->redraw();

		}
	}
	
	void clear() {
		createdLines = "";
//		alert(" clear!");
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
	glm::vec2 flowXY;
	float xBak = 0;
	
	// this rectangle stores the last element size to flow the element coordinates
	ofRectangle flowRect;
	
	void initFlow() {
		flowXY = glm::vec2(_settings->uiPadding, _settings->uiPadding);
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
		flowXY.y += _settings->elementRect.height + _settings->elementSpacing;
	}
	
	void newCol() {
		flowXY.x += _settings->elementRect.width + _settings->uiPadding;
		flowXY.y = _settings->uiPadding;
	}
	
	
	// TEMPLATE - melhorar essa porra
	string buildingTemplate = "";
	map <string, vector <string>> templateUI;
	map <string, vector <string>> templateVectorString;

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
	
	bool isDown = false;

	void addUI(string t, bool down = false, string loadText = "") {
//        cout << "ofxMicroUI addUI " << uiName << " : " << t << endl;
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
        
//        cout << xy << endl;
        
		// pointer
		ofxMicroUI * u = &uis[t];
		
		// if I use uis map to load save, they are ordered alphabetically, this pointer fixes things up
		allUIs.push_back(u);
		
		u->isDown = down;
		u->uiName = t;
		u->_masterUI = this;
		u->rectPos.x = xy.x;
		u->rectPos.y = xy.y;
		u->_settings = _settings;
		u->uiColorBg = _settings->uiColorBg;
		u->uiOpacity = _settings->uiOpacity;
		u->rect.width = rect.width - u->_settings->uiMargin;
		u->uiTag = tagOnNewUI;
		u->uiColorTop = uiColorTopOnNewUI;

		if (down) {
			_lastUI->_downUI = u;
		}

		string file = t + ".txt";
		if (loadText != "") {
			file = loadText;
		}
//		u->initFlow();
        if (ofFile::doesFileExist(file)) {
            u->createFromText(file);
        }
		_lastUI = u;
	}
	
	
	void reflowUIs() {
		xy = glm::vec2(0,0);
		rectPos.x = xy.x;
		rectPos.y = xy.y;
		_lastUI = this;
		
		for (auto & u : allUIs) {
//			cout << u.first << endl;
//			cout << u->uiName << endl;
			
			if (u->visible) {
				if (u->isDown) {
					xy += glm::vec2(0, _lastUI->rect.height + _settings->uiMargin);
				} else {
					xy.y = 0;
					xy += glm::vec2(_lastUI->rect.width + _settings->uiMargin, 0);
				}
				u->rectPos.x = xy.x;
				u->rectPos.y = xy.y;
				_lastUI = u;
			}
		}
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
		} else {
			cout << "set non existant element " << name << "::" << uiName << endl;
		}
//		getSlider(name)->set(v);
	}

	void set(string name, int v) {
		slider * e = getSlider(name);
		if (e != NULL) {
			e->set(v);
		} else {
//			cout << "set element is null : " << uiName << " :: " << name << endl;
		}
        
        element * el = getElement(name);
        if (el != NULL) {
            cout << "element " << name << " is not null " << name << endl;
            el->set(v);
        } else {
            cout << "element " << name << " is NULL " << name << endl;
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
	
	vector <element*> loadingEvents;
	
	void setVisible (bool b) {
		visible = b;
//		adjustUIDown();
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
	ofColor uiColorTop = ofColor(0);
	
	void forwardEventFrom(element & e) {
		ofxMicroUI::slider * els = dynamic_cast<ofxMicroUI::slider*>(&e);
		if (els) {
			if (els->isInt) {
				set(e.name, *e.i);
			} else {
				set(e.name, *e.f);
			}
		}
		
		else if (dynamic_cast<ofxMicroUI::toggle*>(&e)) {
			set(e.name, *e.b);
		}

		else if (dynamic_cast<ofxMicroUI::radio*>(&e)) {
			set(e.name, *e.s);
		}

		// nao vai funcionar, a nao ser que o colorHsv seja um objeto estilo group.
		else if (dynamic_cast<ofxMicroUI::colorHsv*>(&e)) {
			((ofxMicroUI::colorHsv*)getElement(e.name))->setFromColor(e._ui->pColor[e.name]);
		}
	}

	
	vector <ofxMicroUI *> shortcutUIs;
	void addShortcutUI(ofxMicroUI * _ui) {
		shortcutUIs.push_back(_ui);
		ofAddListener(uiEvent, this, &ofxMicroUI::uiEvents);
	}
	
	void uiEvents(ofxMicroUI::element & e) {
		if (!e._ui->_settings->presetIsLoading) {
			for (auto & s : shortcutUIs) {
				s->forwardEventFrom(e);
			}
		}
	}
};

#include "ofxMicroUISoftware.h"

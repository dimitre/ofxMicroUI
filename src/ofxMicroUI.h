// forwared declaration
//class ofxMicroUISoftware;



class ofxMicroUI : public ofBaseApp {
public:
	
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
				if (ABS(pEasy[p.first] - pFloat[p.first]) > 0.00007) {  //0.00007
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
	
	map <string, ofxMicroUI> uis;

	glm::vec2 xy = glm::vec2(0,0);
	int margem = 15;
	ofxMicroUI * _lastUI = this;
	void addUI(string t) {
		//cout << "lastui pos " << _lastUI->uiPos << endl;
		if (!_lastUI->updatedRect) {
			_lastUI->updateRect();
		}
		xy += glm::vec2(_lastUI->rect.width + margem, 0);
		uis[t].uiPos = xy;
		uis[t]._settings = _settings;
		uis[t]._settings->init();
		uis[t].createFromText(t + ".txt");
		_lastUI = &uis[t];
		
		_settings->redrawUI = true;
		//uis.push_back(move(m));
	}
	
	void microUIDraw() {
		if (_settings->redrawUI) {
			fbo.begin();
			ofClear(0,0);

			ofSetColor(0, 200);
			ofDrawRectangle(rect);

			ofSetColor(255);
			for (auto & e : elements) {
				e->draw();
			}
			fbo.end();
			_settings->redrawUI = false;
		}
		ofSetColor(255);
		fbo.draw(rect.getPosition() + ofPoint(uiPos));
	}
		
	enum microUIVarType {
		MICROUI_FLOAT,
		MICROUI_INT,
		MICROUI_STRING,
		MICROUI_BOOLEAN,
		MICROUI_POINT,
		MICROUI_COLOR,
		MICROUI_VEC3,
	};

	
	map <string, float>	pFloat;
	map <string, bool>	pBool;
	map <string, string>	pString;
	map <string, glm::vec3>	pVec3;
	
	// future usage
	map <string, glm::vec2>	pVec2;
	map <string, ofColor> 	pColor;
	map <string, ofColor> 	pColorEasy;
	map <string, int>		pInt;
	map <string, float>		pEasy;
	
	map <string, element *> elementsLookup;
	vector <element*> elements;
	

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
		_settings->redrawUI = true;
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
		_settings->redrawUI = true;
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
		cout << ":: ofxMicroUI :: " << s << endl;
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
				auto strings = 		xmlElements.findFirst("string");
				auto vec3s = 		xmlElements.findFirst("group");

				_settings->presetIsLoading = true;
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
				_settings->presetIsLoading = false;

			}
		}
		_settings->redrawUI = true;
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

		_settings->presetIsLoading = true;
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
		_settings->presetIsLoading = false;

		xmlSettings.save(xml);
	}
	
	void saveOrLoad(string n) {
		//cout << "save or load " << n << endl;
		if (ofGetKeyPressed(OF_KEY_COMMAND)) {
			save(n);
		} else {
			load(n);
		}
	}
	
	

	// TODO
	// lookup table to return element by name
	element * getElement(string n) {
		return elementsLookup.find(n) != elementsLookup.end() ? elementsLookup[n] : NULL;
	}
	
	// and other kinds
	slider * getSlider(string & n) {
		return (slider*)getElement(n);
	}
	
	void clear() {
		_settings->init();
		elements.clear();

		pFloat.clear();
		pInt.clear();
		pBool.clear();
		pString.clear();
		pColor.clear();
	}
};


#include "ofxMicroUISoftware.h"

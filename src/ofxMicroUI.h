class ofxMicroUI : public ofBaseApp {
public:
	
#include "ofxMicroUISettings.h"
#include "ofxMicroUIElements.h"
#include "ofxMicroUIParseText.h"

	ofxMicroUI() {
		alert("microUI setup " + textFile);
		ofAddListener(ofEvents().draw, this, &ofxMicroUI::onDraw);
		//ofAddListener(ofEvents().mouseMoved, this, &ofxMicroUI::onMouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &ofxMicroUI::onMousePressed);
		ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUI::onMouseDragged);
		ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUI::onMouseReleased);
	}
	
	~ofxMicroUI() {
		alert("destroy " + textFile);
	}
	
	void microUIDraw() {
		if (settings.redrawUI) {
			fbo.begin();
			ofClear(0,0);

			ofSetColor(0, 200);
			ofDrawRectangle(rect);

			for (auto & e : elements) {
				e->draw();
			}
			fbo.end();
			settings.redrawUI = false;
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
		settings.redrawUI = true;
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
		settings.redrawUI = true;
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

				settings.presetIsLoading = true;
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
				settings.presetIsLoading = false;

			}
		}
		settings.redrawUI = true;
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

		settings.presetIsLoading = true;
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
		settings.presetIsLoading = false;

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
		
	}
	
	void clear() {
		settings.init();
		elements.clear();

		pFloat.clear();
		pInt.clear();
		pBool.clear();
		pString.clear();
		pColor.clear();
	}
};



/*
 
 This is the software class, to extend the funcionality of ofxMicroUI
 If needed
 
 
 */
class ofxMicroUISoftware : public ofBaseApp {
public:
	
	ofFbo fbo;
	ofRectangle rect;
	ofxMicroUI * u = NULL;
	
	map <string, ofxMicroUI> uis;

//	struct fboRect {
//	public:
//		ofRectangle rect;
//		ofFbo fbo;
//		void draw() {
//			ofSetColor(0);
//			ofDrawRectangle(rect);
//			ofSetColor(255);
//			fbo.draw(rect.x, rect.y, rect.width, rect.height);
//		}
//	};
	
	
	void drawFbo() {
		if (u != NULL) {
			ofSetColor(0);
			//rect.setPosition(u->pInt["fboX"],u->pInt["fboY"]);
			rect.setWidth(fbo.getWidth() * u->pFloat["fboScale"]);
			rect.setHeight(fbo.getHeight() * u->pFloat["fboScale"]);
			ofSetColor(0);
			ofDrawRectangle(rect);
			ofSetColor(255);
			fbo.draw(rect);
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
	
	ofxMicroUISoftware() {
		
		int w, h, multiSampling;
		if (ofFile::doesFileExist("output.txt")) {
			vector <string> output = textToVector("output.txt");
			vector <string> dimensoes = ofSplitString(output[0], " ");
			w = ofToInt(dimensoes[0]);
			h = ofToInt(dimensoes[1]);
			multiSampling = 0;
			if (dimensoes.size() > 2) {
				multiSampling = ofToInt(dimensoes[2]);
			}
		} else {
			cout << "missing output.txt file" << endl;
			w = 1280;
			h = 720;
		}
		if (multiSampling) {
			fbo.allocate(w, h, GL_RGBA32F_ARB, multiSampling);
		} else {
			fbo.allocate(w, h, GL_RGBA32F_ARB);
		}
		cout << "allocate fbo " << w << "x" << h << endl;
		fbo.begin();
		ofClear(0,255);
		fbo.end();
		//ofxMicroUI::alert("microUISoftware setup");
		//ofAddListener(ofEvents().draw, this, &ofxMicroUI::onDraw);
		//ofAddListener(ofEvents().mouseMoved, this, &ofxMicroUI::onMouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &ofxMicroUISoftware::onMousePressed);
		ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUISoftware::onMouseDragged);
		ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUISoftware::onMouseReleased);
	}
	
	~ofxMicroUISoftware() {}
	
	void onDraw(ofEventArgs &data) {
	}
	
	
	class drag {
	public:
		ofxMicroUI::element *ex = NULL;
		ofxMicroUI::element *ey = NULL;
		glm::vec2 dragPos;
		void update(glm::vec2 xy) {
			dragPos = xy;
			if (ex != NULL) {
				ex->set(xy.x);
			}
			if (ey != NULL) {
				ey->set(xy.y);
			}
		}
	};
	
	
	
	
	
	bool dragging = false;
	glm::vec2 firstXY;
	//void onMouseMoved(ofMouseEventArgs &data) {}
	void onMousePressed(ofMouseEventArgs &data) {
		glm::vec2 xy = glm::vec2(data.x, data.y);
		if (rect.inside(xy)) {
			firstXY = xy;
			dragging = true;
		}
	}
	
	void onMouseDragged(ofMouseEventArgs &data) {
		if (dragging) {
			glm::vec2 xy = glm::vec2(data.x, data.y);

			rect.x += data.x - firstXY.x;
			rect.y += data.y - firstXY.y;
			firstXY = xy;
			
			((ofxMicroUI::slider*)u->getElement("fboX"))->set(rect.x);
			((ofxMicroUI::slider*)u->getElement("fboY"))->set(rect.y);
//			cout << rect.x << endl;
//			cout << rect.y << endl;
		}

	}
	
	void onMouseReleased(ofMouseEventArgs &data) {
		dragging = false;
	}
	
	void addUI(string t) {
		uis[t].settings = u->settings;
		uis[t].uiPos = glm::vec2(700,0);
		uis[t].settings.init();
		uis[t].createFromText(t + ".txt");
		//uis.push_back(move(m));
	}
	
};

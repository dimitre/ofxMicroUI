/*
Ideas:
having an ofFbo for each element to redraw and store, only use texture when not changed.
three boolean controls, toggle, hold and bang.
bind event to elements.
event system.
 
ofParameter?

make elementgroup to act as one. in xml, draw, etc.
*/

class ofxMicroUI : public ofBaseApp {
public:

	map <string, float>	pFloat;
	map <string, bool>	pBool;
	map <string, string>	pString;
	map <string, glm::vec3>	pVec3;

	struct microUISettings {
		/*
		 Settings control the flow (distribution on xy) of elements, settings of elements, margins, etc.
		 a pointer is added to each element so they all obey to the same settings.

		 offset x, offset y (padding inside column)
		 column rectangle
		 slider dimensions.
		 */
		glm::vec2 xy = glm::vec2(10,10);
		map <string, int> pInt;
		int margin = 10;
		int spacing = 4;
		ofRectangle elementRect = ofRectangle(0,0,240,20);
		
		bool flowVert = true;
		ofRectangle flowRect;

		void advanceLine() {
			if (flowVert) {
				xy.y += 20 + spacing;
			} else {
				//cout << flowRect.width << endl;
				xy.x += flowRect.width + spacing;
			}
		}
		
		void newCol() {
			xy.x += 240 + spacing;
			xy.y = margin;
		}
	} settings;
	
	

#include "ofxMicroUIElements.h"
	
	
	// TODO
	//
	element * getElement(string & n) {
	}
	
	// and other kinds
	slider * getSlider(string & n) {
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
					elements.push_back(new toggle (name, settings, val, pBool[name]));
				}
				
				else if (cols[0] == "radio") {
					elements.push_back(new radio(name, settings, ofSplitString(cols[2]," "), pString[name]));
				}
			}
		}
	}
};

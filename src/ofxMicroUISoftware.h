/*
 
 This is the software class, to extend the funcionality of ofxMicroUI
 If needed
 
 
 */
class ofxMicroUISoftware : public ofBaseApp {
public:
	
	ofFbo fbo;
	ofFbo fbo2;
	ofFbo * fboFinal = &fbo;
	ofRectangle fboRect;
	ofRectangle fboRectFull;
	ofxMicroUI * ui = NULL;
	
	
	// 31 october 2019 test
	map <string, ofFbo> mapFbos;

	
	//map <string, ofxMicroUI> uis;

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


		
	ofxMicroUISoftware() {
		cout << "ofxMicroUISoftware Init" << endl;
		ofAddListener(ofEvents().keyPressed, this, &ofxMicroUISoftware::onKeyPressed);

		int w, h, multiSampling = 0;
		if (ofFile::doesFileExist("output.txt")) {
			vector <string> output = ofxMicroUI::textToVector("output.txt");
			vector <string> dimensoes = ofSplitString(output[0], " ");
			w = ofToInt(dimensoes[0]);
			h = ofToInt(dimensoes[1]);
			if (dimensoes.size() > 2) {
				multiSampling = ofToInt(dimensoes[2]);
			}
		} else {
			cout << "missing output.txt file" << endl;
			w = 1280;
			h = 720;
		}

		if (multiSampling) {
// Raspberry
//			fbo.allocate(w, h, GL_RGBA32F_ARB, multiSampling);
			fbo.allocate(w, h, GL_RGBA, multiSampling);
			fbo2.allocate(w, h, GL_RGBA, multiSampling);
		} else {
			//fbo.allocate(w, h, GL_RGBA32F_ARB);
			fbo.allocate(w, h, GL_RGBA);
			fbo2.allocate(w, h, GL_RGBA);
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
		ofAddListener(ofEvents().exit, this, &ofxMicroUISoftware::onExit);
	}
	
	~ofxMicroUISoftware() {}
	
	void drawFbo() {
		if (ui != NULL) {
			ofRectangle & r = ui->visible ? fboRect : fboRectFull;
//			ofPushMatrix();
//			ofRotateZDeg(-90);
//			ofTranslate(-fboRectFull.width, 0);

			ofSetColor(0);
			ofDrawRectangle(r);
			ofSetColor(255);
			fboFinal->draw(r);
//			ofPopMatrix();
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
	
	map <char, int> keyPreset = {
		{ 'a', 0 },
		{ 's', 1 },
		{ 'd', 2 },
		{ 'f', 3 },
		{ 'g', 4 },
		{ 'h', 5 },
		{ 'j', 6 },
		{ 'k', 7 },
		{ 'l', 8 },
		{ 'z', 9 },
		{ 'x', 10 },
		{ 'c', 11 },
		{ 'v', 12 },
		{ 'b', 13 },
		{ 'n', 14 },
		{ 'm', 15 },
		{ ',', 16 },
		{ '.', 17 }
	};
	
	
	void keyPressed(int key){
		if (ofGetKeyPressed(OF_KEY_COMMAND)) {
			if (key == 'f' || key == 'F') {
				ofToggleFullscreen();
			}
			else if (key == 's' || key == 'S') {
				string name = ui->pString["presets"];
				//cout << "saving actual preset " << name << endl;
				ui->savePreset(name);
			}
			else if (key == 'o') {
				string n = ui->pString["presets"];
				string presetFolder = ofToDataPath(ui->getPresetPath(true) + "/" + n);
				string comando = "open " + presetFolder;
				cout << comando << endl;
				ofSystem(comando);
			}
		} else {
			if (key == '=') {
				ui->toggleVisible();
				if (!ui->visible) {
					ofHideCursor();
				} else {
					ofShowCursor();
				}
			}
			else if (key == '-') {
				ofToggleFullscreen();
			}
			
			if ( keyPreset.find(key) != keyPreset.end() ) {
				ofxMicroUI::element * e;
				e = ui->getElement("presets");
				if (e != NULL && e->name != "") {
					((ofxMicroUI::presets*)e)->set(keyPreset[key]);
				} else {
	//				cout << "e not found! ):" << endl;
				}
			}
		}
	}
	
	void onKeyPressed(ofKeyEventArgs& data) {
		keyPressed(data.key);
	}

	
	void onDraw(ofEventArgs &data) { }

	
	struct drag {
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
	bool dragFbo = false;
	
	glm::vec2 firstXY;
	//void onMouseMoved(ofMouseEventArgs &data) {}
	void onMousePressed(ofMouseEventArgs &data) {
		if (dragFbo) {
			glm::vec2 xy = glm::vec2(data.x, data.y);
			if (fboRect.inside(xy)) {
				firstXY = xy;
				dragging = true;
			}
		}
	}
	
	void onMouseDragged(ofMouseEventArgs &data) {
		if (dragFbo && dragging) {
			glm::vec2 xy = glm::vec2(data.x, data.y);

			fboRect.x += data.x - firstXY.x;
			fboRect.y += data.y - firstXY.y;
			firstXY = xy;
			ui->getSlider("fboX")->set(fboRect.x);
			ui->getSlider("fboY")->set(fboRect.y);
		}
	}
	
	void onMouseReleased(ofMouseEventArgs &data) {
		dragging = false;
	}
	

	void uiEvents(ofxMicroUI::element & e) {
		if (e.name == "easing") {
			ui->_settings->easing = *e.f;
		}
		else if (e.name == "presetsFolder") {
			ui->setPresetsFolder(*e.s);
		}

		else if (e.name == "fps") {
			ofSetFrameRate(*e.i);
		}
		
		else if (e.name == "fboX" || e.name == "fboY" || e.name == "fboScale") {
			fboRect = ofRectangle(ui->pInt["fboX"],
									  ui->pInt["fboY"],
									  fboFinal->getWidth() * ui->pFloat["fboScale"],
									  fboFinal->getHeight() * ui->pFloat["fboScale"]
							);
		}
	}
	
	void onExit(ofEventArgs &data) {
		cout << "ofxMicroUISoftware exit, saving preset" << endl;
		ui->save(ui->presetsRootFolder + "/master.xml");
		for (auto & u : ui->uis) {
			if (u.second.saveMode == ofxMicroUI::MASTER) {
				u.second.save(ui->presetsRootFolder + "/" + u.first + ".xml");
			}
		}
	}
	
	void setUI(ofxMicroUI * u) {
		ui = u;
		// set the fbo pointer to save presets
		if (ui->presetElement != NULL) {
			ui->presetElement->_fbo = fboFinal;
		}
		fboRectFull = ofRectangle(0,0,fboFinal->getWidth(), fboFinal->getHeight());
		
		ofAddListener(ui->uiEvent, this, &ofxMicroUISoftware::uiEvents);
		ui->load(ui->presetsRootFolder + "/master.xml");
		
		for (auto & u : ui->uis) {
			if (u.second.loadMode == ofxMicroUI::MASTER) {
				string f = ui->presetsRootFolder + "/" + u.first + ".xml";
				cout << f << endl;
				u.second.load(f);
			}
		}
	}
};

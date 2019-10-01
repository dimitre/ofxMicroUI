/*
 
 This is the software class, to extend the funcionality of ofxMicroUI
 If needed
 
 
 */
class ofxMicroUISoftware : public ofBaseApp {
public:
	
	ofFbo fbo;
	ofRectangle rect;
	ofxMicroUI * u = NULL;
	
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
	
	
	void drawFbo() {
		if (u != NULL) {
			ofSetColor(0);
			rect.x = u->pInt["fboX"];
			rect.y = u->pInt["fboY"];
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
	
	void keyPressed(int key){
		//cout << "software keypressed" << endl;
		//if (UINAME == "master")
		{
			if (key == '=') {
				//	settings.software->visible ^= 1;
			}
			else if (key == '-') {
				ofToggleFullscreen();
			}
			else if (key == 'f' || key == 'F') {
				if (ofGetKeyPressed(OF_KEY_COMMAND)) {
					ofToggleFullscreen();
				}
			}
		}
	}
	void onKeyPressed(ofKeyEventArgs& data) {
		keyPressed(data.key);
	}
	
	ofxMicroUISoftware() {
		ofAddListener(ofEvents().keyPressed, this, &ofxMicroUISoftware::onKeyPressed);

		int w, h, multiSampling = 0;
		if (ofFile::doesFileExist("output.txt")) {
			vector <string> output = textToVector("output.txt");
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
		} else {
			//fbo.allocate(w, h, GL_RGBA32F_ARB);
			fbo.allocate(w, h, GL_RGBA);
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
		}
	}
	
	void onMouseReleased(ofMouseEventArgs &data) {
		dragging = false;
	}
};

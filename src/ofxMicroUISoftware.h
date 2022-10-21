/*
 This is the software class, to extend the funcionality of ofxMicroUI
 If needed
 
 TODO
 change fbo fbo2, fbo3 to other kind of fbo.
 
 */
#pragma once

class ofxMicroUISoftware { // : public ofBaseApp
public:
//	ofKey OF_KEY_SAVE = OF_KEY_SUPER;
//	ofKey OF_KEY_SAVE = OF_KEY_ALT;
	// 2021 - software name
	ofxMicroUI * _ui = NULL;
	
	bool showFbo = true;
	glm::vec2 windowSize = glm::vec2(1280,720);

	string name = "";

	ofFbo fbo, fbo2, fbo3;
	//experimental
	ofFbo fbo4;
	ofPixels fboPixels;
	ofFbo * fboFinal = &fbo;
	ofRectangle fboRect;
	ofRectangle fboRectFull;
	
	// 31 october 2019 test
	map <string, ofFbo> mapFbos;
	
//    bool smallWindow = false;
	
	
	ofxMicroUISoftware() {
//		setup();
	}
	
	ofxMicroUISoftware(ofxMicroUI * u, string n, ofFbo * f) : _ui(u), name(n), fboFinal(f) {
		ofAddListener(_ui->uiEventMaster, this, &ofxMicroUISoftware::uiEventMaster);
		setup();
	}
	
	ofxMicroUISoftware(ofxMicroUI * u, string n, int nFbos = 1) : _ui(u), name(n) {
		ofAddListener(_ui->uiEventMaster, this, &ofxMicroUISoftware::uiEventMaster);
		if (nFbos == 1) {
			fboFinal = &fbo;
		}
		else if (nFbos == 2) {
			fboFinal = &fbo2;
		}
		else if (nFbos == 3) {
			fboFinal = &fbo3;
		}

		_ui->_fboPreset = fboFinal;
		
		setup();
	}
	
	void setup() {
		ofAddListener(ofEvents().keyPressed, this, &ofxMicroUISoftware::onKeyPressed);
		ofAddListener(ofEvents().mousePressed, this, &ofxMicroUISoftware::onMousePressed);
		ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUISoftware::onMouseDragged);
		ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUISoftware::onMouseReleased);
		ofAddListener(ofEvents().exit, this, &ofxMicroUISoftware::onExit);
		setupFromText("_output.txt");
		
		windowSize = glm::vec2(ofGetWindowWidth(), ofGetWindowHeight());
	}
	
	void setupFromText(string fileName, int line = 0) {
		int w = 0, h = 0, multiSampling = 0;
		if (ofFile::doesFileExist(fileName)) {
			vector <string> output = ofxMicroUI::textToVector(fileName);
			if (output.size()) {
				vector <string> dimensoes = ofSplitString(output[line], " ");
				w = ofToInt(dimensoes[0]);
				h = ofToInt(dimensoes[1]);
				if (dimensoes.size() > 2) {
					multiSampling = ofToInt(dimensoes[2]);
				}
			}
		} else {
			cout << "missing output.txt file : " << fileName << endl;
			w = 1280;
			h = 720;
		}
		allocateFbos(w, h, multiSampling);
		updateFboRect();
	}

	
	void afterSetUI();

	
	void addControlUI(ofxMicroUI * _ui) {
		ofAddListener(_ui->uiEvent, this, &ofxMicroUISoftware::uiEvents);
	}

	
	void setUI(ofxMicroUI * u) {
		cout << "ofxMicroUISoftware setUI! " << endl;
		_ui = u;
		afterSetUI();
		
		// podia checar se ja nao foi feito antes
		setup();
	}
	
	void updateFboRect() {
		fboRect = ofRectangle(_ui->pInt["fboX"],
			  _ui->pInt["fboY"],
			  fboFinal->getWidth() * _ui->pFloat["fboScale"],
			  fboFinal->getHeight() * _ui->pFloat["fboScale"]
		);
	}
	
//	~ofxMicroUISoftware() {}
	
	void drawFbo() {
		if (_ui != NULL) {
			if (showFbo) {
				ofSetColor(0);
				ofDrawRectangle(fboRect);
				ofSetColor(255);
				fboFinal->draw(fboRect);
			}
		}
	}
	
	
	// renomear esta variavel pra pixeldepth ou algo
	//        int depth = GL_RGBA32F; //GL_RGBA
#if defined(TARGET_RASPBERRY_PI)
	int depth = GL_RGBA; //GL_RGBA
#else
//	int depth = GL_RGBA16F; //GL_RGBA
	int depth = GL_RGBA; //GL_RGBA
#endif
	/*
	 GL_RGB16F or GL_RGBA16F
	 GL_RGB32F and GL_RGBA32F
	 
	 GL_RGBA32F
	 GL_RGBA32F_ARB
	 GL_RGBA64F
	 */

	void allocateFbos(int w, int h, int multiSampling = 0) {
		cout << "allocateFbos : " << w << ":" << h << endl;

		
		if (multiSampling) {
// Raspberry
			
//			fbo.allocate(w, h, GL_RGBA32F_ARB, multiSampling);
			fbo.allocate(w, h, depth, multiSampling);
			fbo2.allocate(w, h, depth, multiSampling);
			fbo3.allocate(w, h, depth, multiSampling);
		} else {
			//fbo.allocate(w, h, GL_RGBA32F_ARB);
			fbo.allocate(w, h, depth);
			fbo2.allocate(w, h, depth);
			fbo3.allocate(w, h, depth);
		}
		fboPixels.allocate(w, h, OF_IMAGE_COLOR); //OF_IMAGE_COLOR_ALPHA

		fbo.begin();
		ofClear(0,255);
		fbo.end();
		
		fbo2.begin();
		ofClear(0,255);
		fbo2.end();
		
		fbo3.begin();
		ofClear(0,255);
		fbo3.end();
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


	bool usingSoftScroll = true;
	int softW = 500;
	int softScroll = 0;

	
	bool usePresetShortcut = true;
	
	void loadPreset(const string & s);
	void keyPressed(int key);

	
	
	void onKeyPressed(ofKeyEventArgs& data) {
		keyPressed(data.key);
	}

	void onDraw(ofEventArgs &data) { }

	bool dragging = false;
	bool dragFbo = false;
	glm::vec2 firstXY;

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
			
//			cout << "mousexy: " << data.x << " x " << data.y << endl;
//			cout << "fborect: " << fboRect.x << " x " << fboRect.y << endl;
//			cout << fboRect.y << endl;
			float x = fboRect.x;
			float y = fboRect.y;
			_ui->getSlider("fboY")->set(y);
			_ui->getSlider("fboX")->set(x);
		}
	}
	
	void onMouseReleased(ofMouseEventArgs &data) {
		dragging = false;
	}
	
	void uiEventMaster(string & e) {
//        if (e == "createFromText") {
		if (e == "setup") {
			afterSetUI();
		}
	}
	
	void uiEventsAll(ofxMicroUI::element & e);

	// Master only
	void uiEvents(ofxMicroUI::element & e);
	

	
	void onExit(ofEventArgs &data) {
		if (_ui != NULL) {
			cout << "ofxMicroUISoftware exit, saving master.xml" << endl;
			//cout << _ui->presetsRootFolder << endl;
			_ui->save(_ui->presetsRootFolder + "/master.xml");
			for (auto & u : _ui->uis) {
				if (u.second.saveMode == ofxMicroUI::MASTER) {
					string f = _ui->presetsRootFolder + "/" + u.first + ".xml";
					u.second.save(f);
				}
			}
		}
		else {
			cout << "ofxMicroUISoftware need to set ui pointer" << endl;
		}
	}
	
	ofShortPixels shortPixelsExport;
	ofPixels pixelsExport;
	ofFbo fboExport;

	void fboToPixels(bool useShort = true) {
//		if (!pixelsExport.isAllocated()) {
//			cout << "allocating pixelsExport" << endl;
//			pixelsExport.allocate(fboFinal->getWidth(), fboFinal->getHeight(), OF_IMAGE_COLOR);
//		}
		if (!fboExport.isAllocated()) {
			cout << "allocating fboExport" << endl;
			fboExport.allocate(fboFinal->getWidth(), fboFinal->getHeight(), useShort ? depth : GL_RGB);
		}
		fboExport.begin();
		ofClear(0,255);
		ofSetColor(255);
		fboFinal->draw(0,0);
		fboExport.end();
		if (useShort) {
			fboExport.readToPixels(shortPixelsExport);
		} else {
			fboExport.readToPixels(pixelsExport);
		}
	}
	
	void fboToPng() {
		fboToPixels();
		string p = ofToString(_ui->pString["presets"]);
		string folder = "_output";
		if (!ofFile::doesFileExist(folder)) {
			ofDirectory::createDirectory(folder);
		}
		// create directory if doesnt exist
		string fullFileName = folder + "/" + p + "_" +ofGetTimestampString() + ".png";
		// ofSaveImage(pixelsExport, fullFileName);
		ofSaveImage(shortPixelsExport, fullFileName);
		string resultado = ofSystem("open " + ofxMicroUI::dataPath(fullFileName));
	}


#include "tiffFastWriter.h"
	
	void fboToTiff() {
		fboToPixels(false);
		string p = ofToString(_ui->pString["presets"]);
		string folder = "_output";
		if (!ofFile::doesFileExist(folder)) {
			ofDirectory::createDirectory(folder);
		}
		// create directory if doesnt exist
		string fullFileName = folder + "/" + p + "_" +ofGetTimestampString() + ".tif";
		tiffFastWriter rec;
		rec.recordTiff(&pixelsExport, fullFileName);
		string resultado = ofSystem("open " + ofxMicroUI::dataPath(fullFileName));
	}
	
	void fboToTiff(string n) {
		fboToPixels();
		tiffFastWriter rec;
	   rec.recordTiff(&pixelsExport, n);
	}
	
	
//    void drawSecondWindow1(ofEventArgs & args) {
//        ofClear(0,255);
//        ofSetColor(255);
//        fboFinal->draw(0,0);
//    }
};

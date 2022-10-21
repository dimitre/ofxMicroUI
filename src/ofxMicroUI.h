 #pragma once

//#define FLOWFREE 1

//#include <intrin.h> // for _movsd
#include "ofRectangle.h"
#include "ofColor.h"
#include "ofEvents.h"
#include "ofFbo.h"
#include "ofUtils.h"
#include "ofTrueTypeFont.h"
#include "ofImage.h"
#include "ofVideoPlayer.h"
#include "ofSoundPlayer.h"
#include "ofVideoGrabber.h"
#include "ofGraphics.h"
#include "ofXml.h"
#include "ofSystemUtils.h"
#include <glm/vec2.hpp>

using std::string;
using std::vector;
using std::map;
using std::cout;
using std::endl;

//class ofImage;

//#include "ofxMicroUISoftware.h"


class ofxMicroUI { // : public ofBaseApp
public:
	friend class element;
	
	// forward declaration
	class element;
	class presets;
	class slider;
	class toggle;
	class radio;
	class inspector;
	
	class fboElement;
	class colorPalette;
	class colorHsv;
	class bar;
	

	struct microUISettings;
	
#include "ofxMicroUIElements.h"
#include "ofxMicroUISettings.h"
#include "ofxMicroUIParseText.h"
#include "ofxMicroUITools.h"
	
	// Not used yet
	enum microUIVarType {
		MICROUI_FLOAT,
		MICROUI_INT,
		MICROUI_STRING,
		MICROUI_BOOLEAN,
		MICROUI_POINT,
		MICROUI_COLOR,
		MICROUI_VEC3,
		MICROUI_GROUP,
	};

	//	ofKey OF_KEY_SAVE = OF_KEY_SUPER;
	ofKey OF_KEY_SAVE = OF_KEY_ALT;


	bool verbose = false;
//	bool verbose = true;

	// UI Basic Settings
	microUISettings * _settings = &settingsUI;
	string uiName = "master";
	// position to draw UI on screen (and handle mouse events)
	ofRectangle rectPos;
	ofRectangle rect; // this rectangle have the coordinate 0,0,w,h
	ofFbo fbo;
	bool visible = true;
	
	presets * presetElement = NULL;
	ofFbo * _fboPreset = NULL;

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
	void removeListeners();
	void draw();
	void mouseUI(int x, int y, bool pressed);
	void load(const string & xml);
	void save(const string & xml);
	
	void loadPreset(const string & n);
	void savePreset(const string & n);
	void saveThumb(const string & n);


	void onUpdate(ofEventArgs &data);


	element * _mouseElement = NULL;



	void notify(string s) {
		ofNotifyEvent(uiEventMaster, s);
		event e = event(this, s);
		ofNotifyEvent(uiEventGeneral, e);
	}

	void onDraw(ofEventArgs &data) {
		draw();
	}
	void onSetup(ofEventArgs &data) {
		notify("setup");
	}

	void onMousePressed(ofMouseEventArgs &data) {
		mouseUI(data.x, data.y, true);
	}
	
	void onMouseDragged(ofMouseEventArgs &data) {
		mouseUI(data.x, data.y, false);
	}
	
	void onMouseReleased(ofMouseEventArgs &data);


	
	void alert(string s) {
		cout << "ofxMicroUI " << uiName << " : " << s << endl;
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

	void loadPresetByIndex(const int & i);
	
	void saveOrLoadAll(const string & n) {
		if (ofGetKeyPressed(OF_KEY_SAVE)) {
			savePreset(n);
		} else {
			loadPreset(n);
		}
	}

	void setPresetsFolder(const string & s);


	void clear();

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
	
	void rewind() {
		flowXY.y -= _settings->elementRect.height + _settings->elementSpacing;
	}
	
	void newCol() {
		flowXY.x += _settings->elementRect.width + _settings->uiPadding;
		flowXY.y = _settings->uiPadding;
	}
	
	
	// TEMPLATE - melhorar essa porra
	string buildingTemplate = "";
	map <string, vector <string>> templateUI;
	map <string, vector <string>> templateVectorString;

	// 04 02 2022
	map <string, string> replaces;
	
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
	
	// novidade 25 de janeiro de 2022
	void removeUI(string name) {
//		cout << "removeUI " << name << " : " << uis[name].isDown << endl;
		_lastUI = &uis[name];
		if (uis[name].isDown) {
			xy -= glm::vec2(0, _lastUI->rect.height + _settings->uiMargin);
		} else {
			xy -= glm::vec2(_lastUI->rect.width + _settings->uiMargin, 0);
			xy.y = _lastUI->_lastUI->rect.y + _lastUI->_lastUI->rect.height + _settings->uiMargin;
		}
		uis[name].removeListeners();

		for (auto it = allUIs.begin(); it != allUIs.end();)
		{
			if (*it == &uis[name]) {
				it = allUIs.erase(it);
			}
			else {
				++it;
			}
		}

		uis.erase(name);
		_lastUI = allUIs.back();
		// ver reflow aqui
	}

	void addUI(string t, bool down = false, string loadText = "") {
//        cout << "ofxMicroUI addUI " << uiName << " : " << t << " : " << xy << endl;
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
	
	void set(const string & name, const float & v);
	void set(const string & name, const int & v);
	void set(const string & name, const bool & v);
	void set(const string & name, const string & v);


	
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


	vector <ofxMicroUI *> shortcutUIs;
	bool shortcutUIsEvents = false;
	void addShortcutUI(ofxMicroUI * _ui) {
		shortcutUIs.push_back(_ui);
		if (!shortcutUIsEvents) {
			ofAddListener(uiEvent, this, &ofxMicroUI::uiEvents);
			shortcutUIsEvents = true;
		}
	}
	
	void uiEvents(ofxMicroUI::element & e);
	void forwardEventFrom(element & e);
	void savePresetLabel(const string & p);
	
	
	static string dataPath(const string & folder) {
//		return ofToDataPath(folder).string();
		return ofToDataPath(folder);
	}
};

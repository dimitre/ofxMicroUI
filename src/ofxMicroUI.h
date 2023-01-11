#pragma once

//#define FLOWFREE 1

// novidades aqui pra funcionar o software
// using namespace std;
// #include "ofBaseApp.h"
//#include <intrin.h> // for _movsd

//#include "ofMain.h"
#include "ofRectangle.h"
#include "ofColor.h"
#include "ofEvents.h"
#include "ofFbo.h"

#include "ofUtils.h"
#include "ofTrueTypeFont.h"
#include "ofImage.h"
#include "ofVideoPlayer.h"
#include "ofSoundPlayer.h"

//class ofVideoPlayer;
//class ofSoundPlayer;
#include "ofVideoGrabber.h"
#include "ofGraphics.h"
#include "ofXml.h"
#include "ofSystemUtils.h"
#include <glm/vec2.hpp>

using namespace std;

#include "ofxMicroUISettings.h"

class ofxMicroUI { // : public ofBaseApp
public:
//	friend class element;
	// forward declaration
//	class element;
//	struct microUISettings;
	
#include "ofxMicroUIElements.h"
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
	microUISettings settingsUI;
	microUISettings * _settings = &settingsUI;
	string uiName = "master";
	// position to draw UI on screen (and handle mouse events)
	ofRectangle rectPos = ofRectangle(0,0,0,0);
	ofRectangle rect = ofRectangle(0,0,0,0); // this rectangle have the coordinate 0,0,w,h
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
//	ofEvent<element> uiEvent;
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

	void addListeners();
	void removeListeners();
	void draw();
	void load(const string & xml);
	void save(const string & xml);
	void saveThumb(const string & n);
	// fixme : review if const & any parameter
	void addUI(string t, bool down = false, string loadText = "");


	ofxMicroUI() {
		addListeners();
	}
	
	// FIXME - try to const
	ofxMicroUI(string s) {
		addListeners();
		createFromText(s);
	}

	~ofxMicroUI() {
	}


	bool exiting = false;
	void onExit(ofEventArgs &data);
	void onUpdate(ofEventArgs &data);

	element * _mouseElement = NULL;
	void mouseUI(int x, int y, bool pressed);


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
		// data.button = 0, normal, =2 right click, =1 middle click
		// cout << "microui mouse pressed " << data.button << endl;
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
	
	void loadPresetByIndex(int i) {
		string n = presetElement->getValByIndex(i);
		loadPreset(n);
		presetElement->redraw();
	}
	
	// todo: move
	void loadPreset(const string & n);
	void savePreset(const string & n);
	void clear();

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
	void removeUI(const string & name);
	void reflowUIs();
	void redraw();
	

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
//			cout << "element " << name << " is not null " << name << endl;
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
			cout << "setting radio " << name << " val " << v << endl;
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
		element * e2 = getElement(e.name);
		if (e2 != NULL) {
			e2->copyValFrom(e);
		}
	}

	vector <ofxMicroUI *> shortcutUIs;
	bool shortcutUIsEvents = false;
	void addShortcutUI(ofxMicroUI * _ui) {
		shortcutUIs.push_back(_ui);
		if (!shortcutUIsEvents) {
			ofAddListener(uiEvent, this, &ofxMicroUI::uiEvents);
			shortcutUIsEvents = true;
		}
	}
	
	void uiEvents(ofxMicroUI::element & e) {
//		cout << "uiEvents :: " << uiName << " :: " << e.name << endl;
		if (!e._ui->_settings->presetIsLoading) {
			for (auto & s : shortcutUIs) {
				s->forwardEventFrom(e);
			}
		}
	}
	
	void savePresetLabel(string p) {
		if (_masterUI == NULL) {
			_masterUI = this;
		}
		// cout << "savePresetLabel " << _masterUI->pString["presets"] << endl;
		string filePath = getPresetPath() + "/" + _masterUI->pString["presets"] + "/0.txt";
		ofxMicroUI::stringToFile(p, filePath);
		
		presetItem * item = (presetItem *)_masterUI->presetElement->getElement(_masterUI->pString["presets"]);
		if (item != NULL) {
			item->hasXmlCheck();
			item->redraw();
			_masterUI->presetElement->redraw();
		}
	}
	
	static string dataPath(const string & folder) {
		return ofToDataPath(folder);
	}
};

#include "ofxMicroUISoftware.h"

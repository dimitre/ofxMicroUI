#pragma once

// novidades aqui pra funcionar o software
// using namespace std;
// #include "ofBaseApp.h"
//#include <intrin.h> // for _movsd

#include "ofXml.h"

#define GLM_FORCE_CTOR_INIT
#include <glm/vec2.hpp>

template<typename T>
class ofColor_;
typedef ofColor_<unsigned char> ofColor;
typedef ofColor_<float> ofFloatColor;
typedef ofColor_<unsigned short> ofShortColor;

//template<typename T>
//class ofImage_;
//typedef ofImage_<unsigned char> ofImage;
//typedef ofImage_<float> ofFloatImage;
//typedef ofImage_<unsigned short> ofShortImage;

//class ofImage;
#include "ofImage.h"

#include "ofEvents.h"
#include "ofFbo.h"

#include "ofUtils.h"
#include "ofTrueTypeFont.h"
#include "ofVideoPlayer.h"
#include "ofSoundPlayer.h"

#include "ofVideoGrabber.h"

// I can remove this one if I move some functionality (ofSetColor / ofDrawRectangle) to cpp in ofxMicroUIElements
#include "ofGraphics.h"

#include "ofRectangle.h"

#include <unordered_map>

//using std::string;
//using std::unordered_map;
//using std::vector;
//using std::cout;
//using std::endl;

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
	microUISettings * _settings { &settingsUI };
	std::string uiName { "master" };
	// position to draw UI on screen (and handle mouse events)
	ofRectangle rectPos { 0,0,0,0 };
	ofRectangle rect { 0,0,0,0 }; // this rectangle have the coordinate 0,0,w,h
	ofFbo fbo;
	bool visible = true;

	presets * presetElement = nullptr;
	ofFbo * _fboPreset = nullptr;

	// UI VARIABLES
	std::unordered_map <std::string, bool> pBool;
	std::unordered_map <std::string, int> pInt;
	std::unordered_map <std::string, float> pFloat;
	std::unordered_map <std::string, float> pEasy;
	std::unordered_map <std::string, std::string> pString;
	std::unordered_map <std::string, glm::vec2> pVec2;
	std::unordered_map <std::string, glm::vec3> pVec3;
	std::unordered_map <std::string, ofFloatColor> pColor;
	std::unordered_map <std::string, ofFloatColor> pColorEasy;

	std::unordered_map <std::string, ofImage> pImage;
	std::unordered_map <std::string, ofVideoPlayer>	pVideo;
	std::unordered_map <std::string, ofSoundPlayer>	pAudio;
	std::unordered_map <std::string, ofVideoGrabber> pCam;
	std::unordered_map <std::string, std::string> pText;
	std::unordered_map <std::string, ofTrueTypeFont> pFont;

	std::vector <element*> elements;

	struct event {
		ofxMicroUI * _ui { nullptr };
		std::string name { "" };
		event(ofxMicroUI * u, std::string n) : _ui(u), name(n) {}
	};

	ofEvent <event> uiEventGeneral;

	// UI EVENTS
//	ofEvent<element> uiEvent;
	ofEvent<element> uiEvent;
	ofEvent<std::string> uiEventMaster;

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
	bool saveXmlOnNewElement { true };
	// NEW. try to implement it.
	bool useLabelOnNewElement { true };

	// fazer struct com essas coisas. ou passar tudo apenas pro soft.
	std::string tagOnNewElement { "" };
	std::string tagOnNewUI { "" };
	std::string uiTag { "" };

	ofColor uiColorTopOnNewUI { 0 };

	bool hasListeners = false;
	bool freeFlow = false;


	void addListeners();
	void removeListeners();
	void draw();

//	void load(const std::string & xml);
//	void save(const std::string & xml);
//	void saveThumb(const std::string & n);

	// novidade 21 de agosto pra copy paste
	std::string getXml();

	void setXml(const std::string & data);
	void load(const of::filesystem::path & fileName);
	void save(const of::filesystem::path & fileName);
	void saveThumb(const std::string & fileName);

	// FIXME: review if const & any parameter
	void addUI(std::string t, bool down = false, std::string loadText = "");


	ofxMicroUI() {
		addListeners();
	}

	// FIXME - try to const
	ofxMicroUI(std::string s) {
		addListeners();
		createFromText(s);
	}

	~ofxMicroUI() {}


	bool exiting = false;
	void onExit(ofEventArgs &data);
	void onUpdate(ofEventArgs &data);

	element * _mouseElement = nullptr;
	void mouseUI(int x, int y, bool pressed);

	void notify(std::string s) {
		ofNotifyEvent(uiEventMaster, s);
		event e = event(this, s);
		ofNotifyEvent(uiEventGeneral, e);
	}

	void onDraw(ofEventArgs &data) {
		draw();
	}
	void onSetup(ofEventArgs &data) {
//		cout << "=-=-=-=- " << uiName << endl;
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

	void alert(std::string s) {
		std::cout << "ofxMicroUI " << uiName << " : " << s << std::endl;
	}


	of::filesystem::path presetsRootFolder { "_presets" };
	of::filesystem::path presetsFolder { "1" };

	of::filesystem::path getPresetPath(bool create = false) {
		if (create && !ofFile::doesFileExist(presetsRootFolder)) {
			ofDirectory::createDirectory(presetsRootFolder);
		}
		of::filesystem::path fullPath { presetsRootFolder / presetsFolder };
		//cout << fullPath << endl;
		// FIXME: Path can be created entirely if using fs.
		if (create && !ofFile::doesFileExist(fullPath)) {
			ofDirectory::createDirectory(fullPath);
		}
		return fullPath;
	}

	void loadPresetByIndex(int i) {
		std::string n { presetElement->getValByIndex(i) };
		loadPreset(n);
		presetElement->redraw();
	}

	// todo: move
	void loadPreset(const std::string & n);
	void savePreset(const std::string & n);
	void clear();

	void saveOrLoadAll(std::string n) {
//		std::cout << "saveOrLoadAll ofGetKeyPressed(OF_KEY_SAVE)" << ofGetKeyPressed(OF_KEY_SAVE) << std::endl;
		if (ofGetKeyPressed(OF_KEY_SAVE)) {
			savePreset(n);
		} else {
			loadPreset(n);
		}
	}

	void setPresetsFolder(std::string s) {
//		alert("setPresetsFolder :: " + s);
		presetsFolder = s;
		if (presetElement != nullptr) {
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
	glm::ivec2 flowXY { 0, 0 };
	float xBak = 0;

	// this rectangle stores the last element size to flow the element coordinates
	ofRectangle flowRect;

	void initFlow() {
		flowXY = { _settings->uiPadding, _settings->uiPadding };
	}

	void setFlowVert(bool s);

	bool advanceLayout();

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
	std::string buildingTemplate { "" };
	std::unordered_map <std::string, std::vector <std::string>> templateUI;
	std::unordered_map <std::string, std::vector <std::string>> templateVectorString;

	// 04 02 2022
	std::unordered_map <std::string, std::string> replaces;

	void toggleVisible() {
		_settings->visible ^= 1;
	}

	// Removing soon
	std::vector <std::string> futureLines;

	// LAYOUT UIS
	// REWRITE EVERYTHING
	std::unordered_map <std::string, ofxMicroUI> uis;
	std::vector <ofxMicroUI *> allUIs;

	glm::ivec2 xy { 0, 0 };
	ofxMicroUI * _lastUI = this;
	ofxMicroUI * _masterUI = nullptr;
	ofxMicroUI * _downUI = nullptr;

	bool isDown = false;

	// novidade 25 de janeiro de 2022
	void removeUI(const std::string & name);
	void reflowUIs();
	void redraw();


	// for quick ofxDmtrUI3 compatibility
	std::unordered_map <std::string, ofFbo> mapFbos;

	std::string willChangePreset { "" };

//	void set(const string & name, std::any v) {
//		auto * e = getElement(name);
//		if (e != nullptr) {
//			e->set(v);
//		} else {
//			cout << "set non existant element " << name << "::" << uiName << endl;
//		}
//	}
	void set(const std::string & name, float v, bool normalized = false);
	void set(const std::string & name, int v);
	void set(const std::string & name, bool v);
	void set(const std::string & name, std::string v);

	void set(const std::string & name, const std::string & n2, float v);
	void set(const std::string & name, const std::string & n2, int v);
	void set(const std::string & name, const std::string & n2, bool v);
	void set(const std::string & name, const std::string & n2, std::string v);

	
	//	void set(string name, string v);

	std::vector <element*> loadingEvents;

	void setVisible(bool b) {
		visible = b;
//		adjustUIDown();
	}

	void adjustUIDown();


	// UI STYLE
	float uiOpacity { 230 };
	ofColor uiColorBg { ofColor(0,0,0,230) };
	ofColor uiColorTop { ofColor(0) };

	void forwardEventFrom(element & e) {
		element * e2 = getElement(e.name);
		if (e2 != nullptr) {
			e2->copyValFrom(e);
		}
	}

	std::vector <ofxMicroUI *> shortcutUIs;
	bool shortcutUIsEvents = false;
	void addShortcutUI(ofxMicroUI * _ui) {
		shortcutUIs.push_back(_ui);
		if (!shortcutUIsEvents) {
			ofAddListener(uiEvent, this, &ofxMicroUI::uiEvents);
			shortcutUIsEvents = true;
		}
	}

	void uiEvents(ofxMicroUI::element & e) {
		cout << "uiEvents :: " << uiName << " :: " << e.name << endl;
		if (!e._ui->_settings->presetIsLoading && e._ui->uiIsCreated) {
			for (auto & s : shortcutUIs) {
				s->forwardEventFrom(e);
			}
		}
	}

	void savePresetLabel(std::string p) {
		if (_masterUI == nullptr) {
			_masterUI = this;
		}
		// cout << "savePresetLabel " << _masterUI->pString["presets"] << endl;
		auto filePath { getPresetPath() / of::filesystem::path(_masterUI->pString["presets"]) / of::filesystem::path("0.txt") };
		ofxMicroUI::stringToFile(p, filePath);

		presetItem * item = (presetItem *)_masterUI->presetElement->getElement(_masterUI->pString["presets"]);
		if (item != nullptr) {
			item->hasXmlCheck();
			item->redraw();
			_masterUI->presetElement->redraw();
		}
	}

	// Tools
	static ofColor stringToColor(const std::string & s);
	static ofColor stringHexToColor(const std::string & corString);

	std::shared_ptr<ofAppBaseWindow> currentWindow;

	std::string buildingTemplateName { "" };
	std::string templateName { "" };

	// Novidade 21 agosto 2024
	ofxMicroUI * _lastClickedUI = nullptr;

//	ofxMicroUI * _copyUI = nullptr;
//	void copyUI (ofxMicroUI * c) {
//		std::cout << "copyUI" << std::endl;
//		_copyUI = c;
//	}
//	void pasteUI (ofxMicroUI * c) {
//		std::cout << "pasteUI" << std::endl;
//		if (_copyUI != nullptr) {
//			for (auto & e : c->elements) {
//				e->copyValFrom(*_copyUI->getElement(e->name));
//			}
//		}
//	}


	std::string getXmlFromElements() {
		return {};
	}

//	std::string getXmlNeue();

	void appendXmlFromElement(ofXml & elementsList, element * e);
	void setElementFromXml(const ofXml & xml, element * e);

	int uiVersion = 2;

	void copyValsFrom(ofxMicroUI * u) {
		std::cout << "copyValsFrom " << u->uiName << " to: " << uiName << std::endl;
		pBool = u->pBool;
		pInt = u->pInt;
		pFloat = u->pFloat;
		pEasy = u->pEasy;
		pString = u->pString;
		pColor = u->pColor;
		pColorEasy = u->pColorEasy;
		pVec2 = u->pVec2;
		pVec3 = u->pVec3;
	}
};

//#include "ofxMicroUISoftware.h"

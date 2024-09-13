#include "ofxMicroUI.h"
#include "ofFbo.h"
#include "ofVideoPlayer.h"
#include "ofSoundPlayer.h"
#include "ofTrueTypeFont.h"
#include "ofColor.h"
#include "ofXml.h"

#include "ofImage.h"
#include "ofSystemUtils.h"
#include "ofAppBaseWindow.h"


namespace fs = of::filesystem;

using std::string;
using std::cout;
using std::endl;
using std::vector;


void ofxMicroUI::addListeners() {
	currentWindow = ofGetCurrentWindow();
	
	
	if (!hasListeners) {
		ofAddListener(ofEvents().setup, this, &ofxMicroUI::onSetup);
		ofAddListener(ofEvents().update, this, &ofxMicroUI::onUpdate);
		ofAddListener(ofEvents().draw, this, &ofxMicroUI::onDraw);
		//ofAddListener(ofEvents().mouseMoved, this, &ofxMicroUI::onMouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &ofxMicroUI::onMousePressed);
		ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUI::onMouseDragged);
		ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUI::onMouseReleased);
		ofAddListener(ofEvents().exit, this, &ofxMicroUI::onExit);
		hasListeners = true;
//        allUIs.emplace_back(this);
	}
}

void ofxMicroUI::removeListeners() {
	if (hasListeners) {
		ofRemoveListener(ofEvents().setup, this, &ofxMicroUI::onSetup);
		ofRemoveListener(ofEvents().draw, this, &ofxMicroUI::onDraw);
		//ofRemoveListener(ofEvents().mouseMoved, this, &ofxMicroUI::onMouseMoved);
		ofRemoveListener(ofEvents().mousePressed, this, &ofxMicroUI::onMousePressed);
		ofRemoveListener(ofEvents().mouseDragged, this, &ofxMicroUI::onMouseDragged);
		ofRemoveListener(ofEvents().mouseReleased, this, &ofxMicroUI::onMouseReleased);
		ofRemoveListener(ofEvents().update, this, &ofxMicroUI::onUpdate);
		ofRemoveListener(ofEvents().exit, this, &ofxMicroUI::onExit);

		hasListeners = false;
//        allUIs.emplace_back(this);
	}
}


void ofxMicroUI::draw() {
	if (redrawUI) {
		fbo.begin();
		ofClear(0,0);
		ofSetColor(uiColorBg);
		ofDrawRectangle(rect);
		
		if (uiColorTop != ofColor(0)) {
			ofSetColor(uiColorTop);
			ofDrawRectangle(0,0,rect.width, 5);
		}
		
		ofSetColor(255);
		for (auto & e : elements) {
			if (!e->alwaysRedraw) {
				e->draw();
			}
		}
		fbo.end();
		redrawUI = false;
	}
	
	if (visible && _settings->visible) {
		fbo.begin();
		for (auto & e : elements) {
			if (e->haveToRedraw) {
				e->redrawElement();
				//e->draw();
			}
		}
		fbo.end();
		
		ofSetColor(255, _settings->uiOpacity);
//			ofSetColor(255, uiOpacity);
		
		//  se este desenhar depois dos always redraw precisamos fazer buracos pra nao sobrepor a opacidade
		fbo.draw(rectPos.getPosition() + _settings->offset);

		// melhorar com lookup isso aqui
		ofPushMatrix();
		ofTranslate(rectPos.getPosition() + _settings->offset);
		
		for (auto & e : elements) {
			ofPushMatrix();
			if (e->alwaysRedraw) {
//                cout << "alwaysredraw " << e->name << endl;
				e->draw();
			}
			ofPopMatrix();
		}
		ofPopMatrix();
	}
}


void ofxMicroUI::load(const fs::path & fileName) {
	if (verbose) {
		alert("LOAD " + ofPathToString(fileName));
	}

	// FIXME: FS
	if (ofFile::doesFileExist(fileName)) {
		setXml(ofBufferFromFile(fileName).getText());
	} else {
//		alert ("load not found " + fileName.string());
	}
//		redraw();
}



// used in save.
//std::string ofxMicroUI::getXmlNeue() {
//	int version = 2;
//	ofXml xmlSettings;
//	xmlSettings.appendChild("ofxMicroUI").set(version);
//	auto xmlElements { xmlSettings.appendChild("element") };
//	for (auto & e : elements) {
//		if (e->saveXml) {
//
//		}
//	}
//}



void ofxMicroUI::save(const fs::path & fileName) {
	std::ofstream presetFile(ofToDataPath(fileName));
	presetFile << getXml();
	presetFile.close();
}


void ofxMicroUI::saveThumb(const string & n) {
//		alert ("saveThumb :: " + n);
	if (presetElement != nullptr) {
		// mover isso pra dentro do objeto presets?
		presetItem * item { (presetItem *)presetElement->getElement(n) };
		if (item != nullptr) {
			ofFbo * _f { &item->fbo };
			if (_fboPreset != nullptr && _f != nullptr) {
				_f->begin();
				ofClear(0,255);
				ofSetColor(255);
				
				float margin = 0.25;
				float margin2 = 1 + margin;
				
				_fboPreset->draw(
							-_f->getWidth()	* margin,
							-_f->getHeight()	* margin,
							_f->getWidth()	* margin2,
							_f->getHeight()	* margin2
					);
				
				_fboPreset->draw(0,0, _f->getWidth(), _f->getHeight());
//                    cout << _f->getWidth() << " x " <<  _f->getHeight() << endl;
//                    cout << _fboPreset->getWidth() << " x " <<  _fboPreset->getHeight() << endl;

				_f->end();

//					string file = getPresetPath(true) + "/" + n + "/0.tif";
				presetElement->redraw();
				string file { getPresetPath(true) / fs::path(n) / fs::path("0.png") };
				ofPixels pixels;
				_f->readToPixels(pixels);
				ofSaveImage(pixels, file);
				//bool ofSaveImage(const ofShortPixels &pix, ofBuffer &buffer, ofImageFormat format=OF_IMAGE_FORMAT_PNG, ofImageQualityType qualityLevel=OF_IMAGE_QUALITY_BEST)
			}
		}
	}
}

void ofxMicroUI::addUI(string t, bool down, string loadText) {
	ofGetMainLoop()->setCurrentWindow(currentWindow);
	
	if (!_lastUI->updatedRect) {
		_lastUI->updateRect();
	}
	if (down) {
		xy += glm::vec2(0, _lastUI->rect.height + _settings->uiMargin);
	} else {
		xy.y = 0;
		xy += glm::vec2(_lastUI->rect.width + _settings->uiMargin, 0);
	}
	
	ofxMicroUI * u { &uis[t] };
	
	// if I use uis map to load save, they are ordered alphabetically, this pointer fixes things up
	// FIXME: use unordered_map instead and eliminate allUIs pointer
	allUIs.push_back(u);
	
	if (!empty(buildingTemplateName)) {
		u->templateName = buildingTemplateName;
	}
	
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

	
	string file { t + ".txt" };
	if (loadText != "") {
		file = loadText;
	}

	if (ofFile::doesFileExist(file)) {
		u->createFromText(file);
	}
	_lastUI = u;
}

void ofxMicroUI::removeUI(const string & name) {
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
	
	// AllUIS are needed. unordered_map doesn't guarantee particular order.
}

void ofxMicroUI::reflowUIs() {
	xy = glm::vec2(0,0);
	rectPos.x = xy.x;
	rectPos.y = xy.y;
	_lastUI = this;
	
	for (auto & u : allUIs) {
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

void ofxMicroUI::redraw() {
	redrawUI = true;
	for (auto & u : uis) {
		u.second.redraw();
	}
//	for (auto & u : allUIs) {
//		u->redraw();
//	}
}

void ofxMicroUI::loadPreset(const string & n) {
	if (verbose) {
		alert("loadPreset " + n);
	}

	_settings->presetIsLoading = true;
	auto presetFolder { getPresetPath() / n };
	
	unsigned int s = allUIs.size();
	bool repeat = false;
	
	
	for (auto & u : allUIs) {
		if (u->loadMode == PRESETSFOLDER) {
//			cout << "will load " << u->uiName << endl;
			u->load(presetFolder / (u->uiName + ".xml"));
		}
		if (allUIs.size() != s) {
			if (allUIs.size() > s) {
				repeat = true;
			}
			cout << "break " << repeat << allUIs.size() << endl;
			break;
//				s = allUIs.size();
		}
	}
	
	if (repeat) {
//		cout << "OWWW repeat" << endl;
		for (auto u = allUIs.begin() + s ; u != allUIs.end(); ++u) {
//			for (auto & u : allUIs) {
			if ((*u)->loadMode == PRESETSFOLDER) {
				(*u)->load(presetFolder / ((*u)->uiName + ".xml"));
			}
		}
	}

	_settings->presetIsLoading = false;
	
	notify("loaded");
}

void ofxMicroUI::savePreset(const string & n) {
	alert("savePreset " + n);
	_settings->presetIsLoading = true;

	auto presetFolder { getPresetPath(true) / n };
	if (!ofFile::doesFileExist(presetFolder)) {
		ofDirectory::createDirectory(presetFolder);
	}

	for (auto & u : allUIs) {
		if (u->saveMode == PRESETSFOLDER) {
			u->save(presetFolder / (u->uiName + ".xml"));
		}
	}
	
	_settings->presetIsLoading = false;

	if (presetElement != nullptr) {
		saveThumb(n);
		presetElement->hasXmlCheck();
		presetElement->redraw();
	} else {
//            cout << "presetElement is nullptr" << endl;
	}
}

void ofxMicroUI::clear() {
	createdLines = "";

	rect.width = rect.height = 10;
	updatedRect = false;
	initFlow();
	
	// todo: shared_ptr? unique_ptr? 
	for (auto & e : elements) {
		delete e;
	}
	elements.clear();

	pFloat.clear();
	pInt.clear();
	pBool.clear();
	pString.clear();
	pColor.clear();
	
	redrawUI = true;
}




void ofxMicroUI::onExit(ofEventArgs &data) {
	exiting = true;
}

void ofxMicroUI::onUpdate(ofEventArgs &data) {
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

	float mouseReleaseTime = 0;
	// EVERYTHING MOUSE
	void ofxMicroUI::mouseUI(int x, int y, bool pressed) {
		// novidade, offset implementado

		if (freeFlow) {
			int xx = x - _settings->offset.x;
			int yy = y - _settings->offset.y;
			if (_settings->visible && visible && rectPos.inside(xx, yy)) {
				xx -= rectPos.x;
				yy -= rectPos.y;
				// future : break if element is found. in the case no element overlap.
				for (auto & e : elements) {
					e->checkMouse(xx, yy, pressed);
				}
			}
		} else {
			
			if (_settings->visible && visible) { // && rectPos.inside(xx, yy)

				int xx = x - _settings->offset.x - rectPos.x;
				int yy = y - _settings->offset.y - rectPos.y;
				
				if (rectPos.inside(x - _settings->offset.x, y - _settings->offset.y) ) {
					if (_masterUI != nullptr) {
						_masterUI->_lastClickedUI = this;
					}

					if (pressed)
					{

						if (ofGetElapsedTimef() - mouseReleaseTime < 0.2) {
						}
						mouseReleaseTime = ofGetElapsedTimef();
					}

//					if (ofGetKeyPressed(OF_KEY_F1)) {
//						ofGetCurrentWindow()->setClipboardString(getXml());
//					}
//					else if (ofGetKeyPressed(OF_KEY_F2)) {
//						auto s = ofGetCurrentWindow()->getClipboardString();
//						setXml(s);
//					}
//					else if (ofGetKeyPressed(OF_KEY_F3)) {
//						_masterUI->copyUI(this);
//					}
//					else if (ofGetKeyPressed(OF_KEY_F4)) {
//						_masterUI->pasteUI(this);
//					}
				}
				
				if (pressed) {
					_mouseElement = nullptr;
					for (auto & e : elements) {
						if (e->rect.inside(xx, yy)) {
							_mouseElement = e;
							break;
						}
					}
				}
				if (_mouseElement != nullptr) {
					_mouseElement->checkMouse(xx, yy, pressed);
				}
			}
		}
	}


void ofxMicroUI::onMouseReleased(ofMouseEventArgs &data) {
	int xx = data.x - rectPos.x - _settings->offset.x;
	int yy = data.y - rectPos.y - _settings->offset.y;

	for (auto & e : elements) {
		e->mouseRelease(xx, yy);
	}
}



bool ofxMicroUI::advanceLayout() {
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


void ofxMicroUI::setFlowVert(bool s) {
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


void ofxMicroUI::set(const string & name, float v) {
	slider * e = getSlider(name);
	if (e != nullptr) {
		e->set(v);
	} else {
//			cout << "set non existant element " << name << "::" << uiName << endl;
	}
}

void ofxMicroUI::set(const string & name, int v) {
	slider * e = getSlider(name);
	if (e != nullptr) {
		e->set(v);
	} else {
//			cout << "set element is null : " << uiName << " :: " << name << endl;
	}
	
	element * el = getElement(name);
	if (el != nullptr) {
//			cout << "element " << name << " is not null " << name << endl;
		el->set(v);
	} else {
		cout << "element " << name << " is nullptr " << name << endl;
	}
}

void ofxMicroUI::set(const string & name, string v) {
//void ofxMicroUI::set(const string & name, string v) {
	cout << "setting radio " << name << " val " << v << endl;

	radio * e = getRadio(name);
	if (e != nullptr) {
		cout << "ofxMicroUI::set setting radio " << name << " val " << v << endl;
		e->set(v);
	} else {
		cout << "setting radio nullptr POINTER" << endl;
	}
}


void ofxMicroUI::set(const string & name, bool v) {
	cout << "setting bool" << endl;
	toggle * e = getToggle(name);
	if (e != nullptr) {
		e->set(v);
	}
}

//void ofxMicroUI::set(string name, string v) {
//	cout << "setting radio " << name << " val " << v << endl;
//
//	radio * e = getRadio(name);
//	if (e != nullptr) {
//		cout << "setting radio " << name << " val " << v << endl;
//		e->set(v);
//	} else {
//		cout << "setting radio nullptr POINTER" << endl;
//	}
//}


void ofxMicroUI::adjustUIDown() {
	if (_downUI != nullptr) {
//			cout << "adjustUIDown :: " << uiName << endl;
		float posY = visible ? (rectPos.y + rect.height + _settings->uiMargin) : rectPos.y;
		_downUI->rectPos.y = posY;
		_downUI->adjustUIDown();
	}
}


ofColor ofxMicroUI::stringToColor(const string & s) {
	ofColor cor;
	if (ofUTF8Substring(s, 0, 1) == "#") {
		cor = stringHexToColor(s);
	}
	else if (ofUTF8Substring(s, 0, 3) == "hsv") {
		vector <string> vals = ofSplitString(s, " ");
		cor = ofColor::fromHsb(ofToInt(vals[1]), ofToInt(vals[2]), ofToInt(vals[3]));
	}
	else {
		vector <string> vals = ofSplitString(s, " ");
		if (vals.size() == 1) {
			cor = ofColor(ofToInt(vals[0]));
		}
		else if (vals.size() == 2) {
			cor = ofColor(ofToInt(vals[0]), ofToInt(vals[1]));
		}
		else if (vals.size() == 3) {
			cor = ofColor(ofToInt(vals[0]), ofToInt(vals[1]), ofToInt(vals[2]));
		}
		else if (vals.size() == 4) {
			cor = ofColor(ofToInt(vals[0]), ofToInt(vals[1]), ofToInt(vals[2]), ofToInt(vals[3]));
		}
	}
	return cor;
}

ofColor ofxMicroUI::stringHexToColor(const string & corString) {
	ofColor cor = ofColor::fromHex(ofHexToInt(corString.substr(1)));
	if (corString.size() == 9) {
		cor = ofColor::fromHex(ofHexToInt(corString.substr(1, 6)));
		cor.a = ofHexToInt(corString.substr(7,2));
	}
	return cor;
}





// ofxMicroUITools
void ofxMicroUI::block() {
	if (ofFile::doesFileExist(".block")) {
		ofSystemAlertDialog("x");
		std::cout << "x" << std::endl;
		std::exit(1);
	}
}

void ofxMicroUI::expires(int dataInicial, int dias) {
	time_t rawtime;
	time ( &rawtime );
	int segundosPorDia = 86400;
	int segundosExpira = segundosPorDia * dias;
	float diasExpira = (segundosExpira - (difftime(rawtime,dataInicial))) / (float)segundosPorDia;
	
	std::string notice {
		"Dmtr " + ofToString(rawtime) + " :: " +
		"Expires in " + ofToString(diasExpira) + " days"
	};
	messageBox(notice);
	if (diasExpira < 0 || diasExpira > dias) {
		ofSystemAlertDialog("Dmtr.org Software Expired ~ " + ofToString(dataInicial) + "\rhttp://dmtr.org/");
		std::exit(1);
	}
}




// used in save.
std::string ofxMicroUI::getXml() {
	int version = 2;
	ofXml xmlSettings;
	xmlSettings.appendChild("ofxMicroUI").set(version);
	
	// 2024 new style of saving elements to XML
	// more compact and more modular
	if (version == 2) {
		auto elementsList { xmlSettings.appendChild("elementsList") };
		for (auto & e : elements) {
			appendXmlFromElement(elementsList, e);
		}
	}
	
	else {
		auto xmlElements { xmlSettings.appendChild("element") };
		auto floats 	{ xmlElements.appendChild("float") };
		auto bools 		{ xmlElements.appendChild("boolean") };
		auto groups 	{ xmlElements.appendChild("group") };
		auto strings 	{ xmlElements.appendChild("string") };
		// falta vec3s, sera q esta em groups?
		auto vec2 		{ xmlElements.appendChild("vec2") };
		
		
		// fazer uma função element to xml.
		for (auto & e : elements) {
			if (e->saveXml) {
				slider * els 	{ dynamic_cast<slider*>(e) };
				booleano * elb 	{ dynamic_cast<booleano*>(e) };
				slider2d * el2  { dynamic_cast<slider2d*>(e) };
				colorHsv * chsv { dynamic_cast<colorHsv*>(e) };
				
				// this now replaces the radio.
				varKindString * elstrings { dynamic_cast<varKindString*>(e) };
				if (elstrings) {
					strings.appendChild(e->name).set(elstrings->getVal());
				}
				
				if (els) {
					floats.appendChild(e->name).set(els->getVal());
				}
				if (elb && !elb->isBang) {
					bools.appendChild(e->name).set(elb->getVal());
				}
				if (el2) {
					vec2.appendChild(e->name).set(el2->getVal());
				}
				if (chsv) {
					auto colorHsv { groups.appendChild(e->name) };
					colorHsv.appendChild("x").set(chsv->xy.x);
					colorHsv.appendChild("y").set(chsv->xy.y);
					colorHsv.appendChild("sat").set(chsv->sat);
					colorHsv.appendChild("alpha").set(chsv->alpha);
					
					// if useRange
					colorHsv.appendChild("range").set(chsv->range);
				}
			}
		}
	}
	return xmlSettings.toString();
}



void ofxMicroUI::setXml(const std::string & data) {
	ofXml xmlSettings;
	bool result = xmlSettings.parse(data);
	if (!result) {
		alert ("parse fail");
		std::cout << data << std::endl;
//		std::exit(0);
	}
	int UIVersion = xmlSettings.getChild("ofxMicroUI").getIntValue();
//	std::cout << "ofxMicroUI load UIVersion : " << UIVersion << std::endl;

	if (UIVersion == 2) {
		auto elementsList = xmlSettings.getChild("elementsList");
		for (auto & e : elements) {
			auto xml = elementsList.getChild(e->name);
			setElementFromXml(xml, e);
		}
	}
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
				slider * els = dynamic_cast<slider*>(e);
				booleano * elb = dynamic_cast<booleano*>(e);
				
				if (els && floats.getChild(e->name)) {
					auto valor = floats.getChild(e->name).getFloatValue();
					slider * elf = dynamic_cast<slider*>(e);
					
					if (elf) {
						e->set(valor);
					}
				}
				else if (elb && bools.getChild(e->name)) {
					auto valor = bools.getChild(e->name).getBoolValue();
					booleano * elb = dynamic_cast<booleano*>(e);
					if (elb && !elb->isBang) {
						e->set(valor);
					}
				}
				else if (strings.getChild(e->name)) {
					auto valor = strings.getChild(e->name).getValue();
					e->set(valor);
				}

				else if (vec2.getChild(e->name)) {
					auto valor = vec2.getChild(e->name).getValue();
					e->set(valor);
				}
				
				else if (group.getChild(e->name)) {
					auto x = 	group.getChild(e->name).getChild("x").getFloatValue();
					auto y = 	group.getChild(e->name).getChild("y").getFloatValue();
					auto sat = 	group.getChild(e->name).getChild("sat").getFloatValue();
					auto alpha = 	group.getChild(e->name).getChild("alpha").getFloatValue();
					auto range = 	group.getChild(e->name).getChild("range").getFloatValue();
//					((colorHsv*)e)->set(glm::vec3(x, sat, y));
					((colorHsv*)e)->set(glm::vec4(x, sat, y, alpha));
					((colorHsv*)e)->range = range;
				}
			}
		}
	}
//			presetIsLoading = false;
	notify("load");
}




void ofxMicroUI::appendXmlFromElement(ofXml & elementsList, element * e) {
	if (e->saveXml) {
		if (auto este = dynamic_cast<slider*>(e)) {
			elementsList.appendChild(e->name).set(este->getVal());
		}
		if (auto este = dynamic_cast<booleano*>(e)) {
			elementsList.appendChild(e->name).set(este->getVal());
		}
		if (auto este = dynamic_cast<slider2d*>(e)) {
			elementsList.appendChild(e->name).set(este->getVal());
		}
		if (auto este = dynamic_cast<varKindString*>(e)) {
			elementsList.appendChild(e->name).set(este->getVal());
		}
		if (auto este = dynamic_cast<groupSave*>(e)) {
			auto myGroup = elementsList.appendChild(e->name);
			for (auto & el : este->elements) {
				appendXmlFromElement(myGroup, el);
			}
		}
	}
}


void ofxMicroUI::setElementFromXml(const ofXml & xml, element * e) {
	if (e->saveXml) {
		if (auto este = dynamic_cast<slider*>(e)) {
			este->set(xml.getFloatValue());
		}
		if (auto este = dynamic_cast<booleano*>(e)) {
			if (!este->isBang) {
				este->set(xml.getBoolValue());
			}
		}
		if (auto este = dynamic_cast<slider2d*>(e)) {
			// because it is const &
			auto val = xml.getValue();
			if (!empty(val)) {
				este->set(val);
			} else {
				cout << "EMPTY " << e->name << endl;
			}
		}
		if (auto este = dynamic_cast<varKindString*>(e)) {
			// because it is const &
			auto val = xml.getValue();
			e->set(val);
		}
		if (auto este = dynamic_cast<groupSave*>(e)) {
			for (auto & el : este->elements) {
				if (el->saveXml) {
					auto elXml = xml.getChild(el->name);
					setElementFromXml(elXml, el);
				}
			}
//			cout << "REDRAW groupSave " << este->name <<  " : " << este->_ui->uiName << endl;
			este->updateVal();
			este->redraw();
		}
	}
}

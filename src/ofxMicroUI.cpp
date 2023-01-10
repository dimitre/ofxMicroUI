#include "ofxMicroUI.h"

void ofxMicroUI::addListeners() {
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
			if (e->alwaysRedraw) {
//                cout << "alwaysredraw " << e->name << endl;
				e->draw();
			}
		}
		ofPopMatrix();
	}
}




	
void ofxMicroUI::load(const string & xml) {
	if (verbose) {
		alert("LOAD " + xml);
	}
	if (ofFile::doesFileExist(xml)) {
//			presetIsLoading = true;

//			alert("load " + xml);
		ofXml xmlSettings;
		xmlSettings.load(xml);
		int UIVersion = xmlSettings.getChild("ofxMicroUI").getIntValue();
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
					// not very good. I have to make an enum or a string to specify element types maybe.
					slider * els = dynamic_cast<slider*>(e);
					booleano * elb = dynamic_cast<booleano*>(e);
//						radio * elr = dynamic_cast<radio*>(e);
//						vec3 * el3 = dynamic_cast<vec3*>(e);
//						slider2d * el2 = dynamic_cast<slider2d*>(e);
//						colorHsv * chsv = dynamic_cast<colorHsv*>(e);
					
					if (els && floats.getChild(e->name)) {
						auto valor = floats.getChild(e->name).getFloatValue();
						slider * elf = dynamic_cast<slider*>(e);
						
//							cout << "float! set " << e->_ui->uiName << "/" << e->name << " : " << valor << endl;
						
						if (elf) {
							e->set(valor);
						}
//							e->set(valor);
					}
					else if (elb && bools.getChild(e->name)) {
						auto valor = bools.getChild(e->name).getBoolValue();
//							cout << "bool! set " << e->name << " : " << valor << endl;
						booleano * elb = dynamic_cast<booleano*>(e);
						if (elb && !elb->isBang) {
							e->set(valor);
						}
					}
					else if (strings.getChild(e->name)) {
						auto valor = strings.getChild(e->name).getValue();
						e->set(valor);
					}
//						if (vec3s.getChild(e->name)) {
//							auto valor = vec3s.getChild(e->name).getValue();
//							//cout << valor << endl;
//							e->set(valor);
//						}
					else if (vec2.getChild(e->name)) {
						auto valor = vec2.getChild(e->name).getValue();
						e->set(valor);
//							cout << "loading slider2d " << e->name << " ::: " << valor << endl;
					}
					
					else if (group.getChild(e->name)) {
						auto x = 	group.getChild(e->name).getChild("x").getFloatValue();
						auto y = 	group.getChild(e->name).getChild("y").getFloatValue();
						auto sat = 	group.getChild(e->name).getChild("sat").getFloatValue();
						auto alpha = 	group.getChild(e->name).getChild("alpha").getFloatValue();
						auto range = 	group.getChild(e->name).getChild("range").getFloatValue();
//							((colorHsv*)e)->set(glm::vec3(x, sat, y));
						((colorHsv*)e)->set(glm::vec4(x, sat, y, alpha));
						((colorHsv*)e)->range = range;
					}
				}
			}
		}
//			presetIsLoading = false;
		notify("load");
	} else {
		alert("load :: not found: " + xml);
	}
//		redraw();
}

void ofxMicroUI::save(const string & xml) {
	if (verbose) {
		alert("SAVE " + xml);
	}
	int version = 1;
	ofXml xmlSettings;
	xmlSettings.appendChild("ofxMicroUI").set(version);
	auto xmlElements = xmlSettings.appendChild("element");
	
	auto floats = xmlElements.appendChild("float");
	auto bools = xmlElements.appendChild("boolean");
	auto groups = xmlElements.appendChild("group");
	auto strings = xmlElements.appendChild("string");
	// falta vec3s, sera q esta em groups?
	auto vec2 = xmlElements.appendChild("vec2");

	
	// fazer uma função element to xml.
	for (auto & e : elements) {
		if (e->saveXml) {
			// not the best way of differentiate elements.
			// I'll implement element kind or var kind
			// change to element kind.
			slider * els = dynamic_cast<slider*>(e);
			booleano * elb = dynamic_cast<booleano*>(e);
			vec3 * el3 = dynamic_cast<vec3*>(e);
			slider2d * el2 = dynamic_cast<slider2d*>(e);
			colorHsv * chsv = dynamic_cast<colorHsv*>(e);

			// todo
			// group * elg = dynamic_cast<group*>(e);
			//				radio * elr = dynamic_cast<radio*>(e);

			// this now replaces the radio.
			varKindString * elstrings = dynamic_cast<varKindString*>(e);
			if (elstrings) {
				strings.appendChild(e->name).set(elstrings->getVal());
			}
			
			if (els) {
				floats.appendChild(e->name).set(els->getVal());
			}
			if (elb && !elb->isBang) {
				bools.appendChild(e->name).set(elb->getVal());
			}
			if (el3) {
				groups.appendChild(e->name).set(el3->getVal());
			}
			if (el2) {
//					cout << "saving slider 2d with the name of " << e->name << " and value " << el2->getVal() << endl;
				vec2.appendChild(e->name).set(el2->getVal());
			}
			if (chsv) {
				auto colorHsv = groups.appendChild(e->name);
				colorHsv.appendChild("x").set(chsv->xy.x);
				colorHsv.appendChild("y").set(chsv->xy.y);
				colorHsv.appendChild("sat").set(chsv->sat);
				colorHsv.appendChild("alpha").set(chsv->alpha);
				
				// if useRange
				colorHsv.appendChild("range").set(chsv->range);
			}
		}
	}
	xmlSettings.save(xml);
}


void ofxMicroUI::saveThumb(const string & n) {
//		alert ("saveThumb :: " + n);
	if (presetElement != NULL) {
		// mover isso pra dentro do objeto presets?
		presetItem * item = (presetItem *)presetElement->getElement(n);
		if (item != NULL) {
			ofFbo * _f = &item->fbo;
			if (_fboPreset != NULL && _f != NULL) {
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
				string file = getPresetPath(true) + "/" + n + "/0.png";
				ofPixels pixels;
				_f->readToPixels(pixels);
				ofSaveImage(pixels, file);
				//bool ofSaveImage(const ofShortPixels &pix, ofBuffer &buffer, ofImageFormat format=OF_IMAGE_FORMAT_PNG, ofImageQualityType qualityLevel=OF_IMAGE_QUALITY_BEST)
			}
		}
	}
}

void ofxMicroUI::addUI(string t, bool down, string loadText) {
	if (!_lastUI->updatedRect) {
		_lastUI->updateRect();
	}
	if (down) {
		xy += glm::vec2(0, _lastUI->rect.height + _settings->uiMargin);
	} else {
		xy.y = 0;
		xy += glm::vec2(_lastUI->rect.width + _settings->uiMargin, 0);
	}
	
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

	if (ofFile::doesFileExist(file)) {
		u->createFromText(file);
	}
	_lastUI = u;
}


void ofxMicroUI::loadPreset(const string & n) {
	// cout << "ofxMicroUI::loadPreset " << n << endl;
	if (verbose) {
		alert("loadPreset " + n);
	}
//		cout << "PRESET IS LOADING BEGIN" << endl;
	_settings->presetIsLoading = true;
	string presetFolder = getPresetPath() + "/" + n;
	
	unsigned int s = allUIs.size();
	bool repeat = false;
	for (auto & u : allUIs) {
		if (u->loadMode == PRESETSFOLDER) {
			u->load(presetFolder + "/" + u->uiName + ".xml");
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
		for (auto u = allUIs.begin() + s ; u != allUIs.end(); ++u) {
//			for (auto & u : allUIs) {
			if ((*u)->loadMode == PRESETSFOLDER) {
				(*u)->load(presetFolder + "/" + (*u)->uiName + ".xml");
			}
		}
	}
//		cout << "PRESET IS LOADING END" << endl;
	_settings->presetIsLoading = false;
	
	notify("loaded");
}

void ofxMicroUI::savePreset(const string & n) {
	alert("savePreset " + n);
	_settings->presetIsLoading = true;

	string presetFolder = getPresetPath(true) + "/" + n;
	if (!ofFile::doesFileExist(presetFolder)) {
		ofDirectory::createDirectory(presetFolder);
	}

	for (auto & u : allUIs) {
		if (u->saveMode == PRESETSFOLDER) {
			u->save(presetFolder + "/" + u->uiName + ".xml");
		}
	}
	
	_settings->presetIsLoading = false;

	if (presetElement != NULL) {
		saveThumb(n);
		presetElement->hasXmlCheck();
		presetElement->redraw();
	} else {
//            cout << "presetElement is NULL" << endl;
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
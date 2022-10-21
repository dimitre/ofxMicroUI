#include "ofxMicroUI.h"
#include "ofxMicroUIElements.h"

void ofxMicroUI::addListeners() {
	if (!hasListeners) {
		ofAddListener(ofEvents().setup, this, &ofxMicroUI::onSetup);
		ofAddListener(ofEvents().update, this, &ofxMicroUI::onUpdate);
		ofAddListener(ofEvents().draw, this, &ofxMicroUI::onDraw);
		//ofAddListener(ofEvents().mouseMoved, this, &ofxMicroUI::onMouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &ofxMicroUI::onMousePressed);
		ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUI::onMouseDragged);
		ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUI::onMouseReleased);
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

void ofxMicroUI::mouseUI(int x, int y, bool pressed) {
	
#ifdef FLOWFREE
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
#else
	if (_settings->visible && visible) { // && rectPos.inside(xx, yy)
		int xx = x - _settings->offset.x - rectPos.x;
		int yy = y - _settings->offset.y - rectPos.y;
		if (pressed) {
			_mouseElement = NULL;
			for (auto & e : elements) {
				if (e->rect.inside(xx, yy)) {
					_mouseElement = e;
					break;
				}
			}
		}
		if (_mouseElement != NULL) {
			_mouseElement->checkMouse(xx, yy, pressed);
		}
	}
#endif
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
			
//				// temporary test
//				if (elg && !elr) {
////					cout << "element group with the name " << e->name << endl;
////					for (auto & ee : ((group*)e)->elements) {
////						cout << ee->name << endl;
////					}
////					cout << "-----" << endl;
//				}
			
//				if (elr) {
//					strings.appendChild(e->name).set(elr->getVal());
//				}

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


void ofxMicroUI::loadPreset(const string & n) {
	cout << "ofxMicroUI::loadPreset " << n << endl;
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
	}
}

void ofxMicroUI::saveThumb(const string & n) {
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
				_f->end();

				// string file = getPresetPath(true) + "/" + n + "/0.tif";
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



void ofxMicroUI::updateRect() {
	elementsLookup.clear();
	
	// todo: substitute all this lookups to one map elementKindLookup[KIND] = x
	slidersLookup.clear();
	togglesLookup.clear();
	radiosLookup.clear();
	inspectorsLookup.clear();
	// build the interface rectangle to buffer drawing into an FBO, and create ElementsLookup
	//rect = elements[0]->rect;
	//cout << elements.size() << endl;
	for (auto & e : elements) {
		//inspector * test = ;
		if (dynamic_cast<slider*>(e)) {
			slidersLookup[e->name] = (slider*)e;
		}
		else if (dynamic_cast<toggle*>(e)) {
			togglesLookup[e->name] = (toggle*)e;
		}
		else if (dynamic_cast<inspector*>(e)) {
			inspectorsLookup[e->name] = (inspector*)e;
		}
		if (dynamic_cast<radio*>(e) || dynamic_cast<dirList*>(e)) {
			radiosLookup[e->name] = (radio*)e;
		}

		rect.growToInclude(e->rect);
		
		// todo: avoid label in lookups.
		// novidade 16 dez 2021
//		elementsLookup[e->name] = e;
		if (!dynamic_cast<label*>(e)) {
			elementsLookup[e->name] = e;
		}
	}
	
	// 31aug2022 - CCRIR Muti, adjust minimum ui width
	rect.width = MAX(rect.width, _settings->uiPadding + _settings->elementRect.width);
	
	rect.width += _settings->uiPadding;
	rect.height += _settings->uiPadding;
	//rectPos.setDimensions(rect.getDimensions());
	rectPos.width = rect.width;
	rectPos.height = rect.height;

	// cout << "updatedrect: " << endl;
	// cout << rect << endl;

	fbo.allocate(rect.width, rect.height, GL_RGBA);
	fbo.begin();
	ofClear(0,255);
	fbo.end();
	updatedRect = true;
	
	// novidade 15 de outubro de 2019
	adjustUIDown();
//	cout << "updateRect! " << uiName << rect << " : " << elements.size() <<  endl;

}



void ofxMicroUI::createFromLine(const string & l) {
//	cout << l << endl;
	vector <string> cols = ofSplitString(l, "\t");
	if (cols.size() == 1) {
		if (l == "") {
			newLine();
		}
		else if (l == "rewind") {
			rewind();
		}
		else if (l == "newCol") {
			newCol();
		}
		else if (l == "clear") {
			clear();
		}
		else if (l == "flowVert" || l == "flowHoriz") {
			setFlowVert(l == "flowVert");
		}
		else if (l == "fps" || l == "fpsInfo") {
//			elements.push_back(new fps(l, *this));
			elements.push_back(new fps(l, *this));
		}
	}
	if (cols.size() >= 2) {
		string name = cols[1];
		
		// START SETTINGS
		if (cols[0] == "include") {
//			for (auto & l : textToVector(cols[1])) {
//				createFromLine(l);
//			}
			vector <string> linhas = textToVector(cols[1]);
			createFromLines(linhas, false);
//			updatedRect = false;
		}
//		else if (cols[0] == "style") {
//			_settings->styleLines = ofBufferFromFile(cols[1]).getText();
//		}
		else if (cols[0] == "uiName") {
//            cout << "AAAAA " << endl;
//            cout << l << endl;
			uiName = cols[1];
		}
		else if (cols[0] == "useFixedLabel") {
			_settings->useFixedLabel = ofToInt(cols[1]);
		}
		else if (cols[0] == "uiMargin") {
			_settings->uiMargin = ofToFloat(cols[1]);
		}
		else if (cols[0] == "alertColor") {
			_settings->alertColor = stringToColor(cols[1]);
//			redrawUI = true;
		}
		else if (cols[0] == "alertColor2") {
			_settings->alertColor2 = stringToColor(cols[1]);
//			redrawUI = true;
		}
		else if (cols[0] == "alertColor3") {
			_settings->alertColor3 = stringToColor(cols[1]);
//			redrawUI = true;
		}
		else if (cols[0] == "uiColorBg") {
			_settings->uiColorBg = stringToColor(cols[1]);
			uiColorBg = stringToColor(cols[1]);
		}


		else if (cols[0] == "uiOpacity") {
			_settings->uiOpacity = ofToFloat(cols[1]);
		}
		else if (cols[0] == "uiPadding") {
			_settings->uiPadding = ofToFloat(cols[1]);
			initFlow();
		}
		else if (cols[0] == "elementSpacing") {
			_settings->elementSpacing = ofToFloat(cols[1]);
		}
		else if (cols[0] == "elementPadding") {
			_settings->elementPadding = ofToFloat(cols[1]);
		}
		else if (cols[0] == "sliderHeight") {
			_settings->elementRect.height = ofToFloat(cols[1]);
		}
		else if (cols[0] == "sliderWidth") {
			_settings->elementRect.width = ofToFloat(cols[1]);
		}
		else if (cols[0] == "colorBg") {
			_settings->colorBg = stringToColor(cols[1]);
		}
		else if (cols[0] == "colorVal") {
			_settings->colorVal = stringToColor(cols[1]);
		}
		else if (cols[0] == "colorLabel") {
			_settings->colorLabel = stringToColor(cols[1]);
		}
		else if (cols[0] == "colorShadowLabel") {
			_settings->colorShadowLabel = stringToColor(cols[1]);
		}
		else if (cols[0] == "useLabelRainbow") {
			_settings->useLabelRainbow = ofToBool(cols[1]);
		}
		else if (cols[0] == "useBgRainbow") {
			_settings->useBgRainbow = ofToBool(cols[1]);
		}
		else if (cols[0] == "labelPosBaseline") {
			_settings->labelPosBaseline = ofToInt(cols[1]);
		}
		else if (cols[0] == "font") {
//			cout << l << endl;
			_settings->useCustomFont = _settings->font.load(cols[1], ofToInt(cols[2]));
		}


		else if (cols[0] == "presetCols") {
			_settings->presetCols = ofToInt(cols[1]);
		}
		else if (cols[0] == "presetHeight") {
			_settings->presetHeight = ofToInt(cols[1]);
		}
		
		else if (cols[0] == "saveMode") {
			if (cols[1] == "NONE") {
				saveMode = NONE;
			} else if (cols[1] == "PRESETSFOLDER") {
				saveMode = PRESETSFOLDER;
			} else if (cols[1] == "MASTER") {
				saveMode = MASTER;
			}
//            cout << uiName << " saveMode " << cols[1] << endl;
		}
		
		else if (cols[0] == "loadMode") {
			if (cols[1] == "NONE") {
				loadMode = NONE;
			} else if (cols[1] == "PRESETSFOLDER") {
				loadMode = PRESETSFOLDER;
			} else if (cols[1] == "MASTER") {
				loadMode = MASTER;
			}
//            cout << uiName << " loadMode " << cols[1] << endl;
		}


		// END SETTINGS

		// Behavior
		else if (cols[0] == "saveXml") {
			saveXmlOnNewElement = ofToBool(cols[1]);
		}
		else if (cols[0] == "tag") {
			tagOnNewElement = cols[1];
		}
		else if (cols[0] == "uiTag") {
			tagOnNewUI = cols[1];
		}
		else if (cols[0] == "uiColorTop") {
			uiColorTopOnNewUI = stringToColor(cols[1]);
//			_settings->uiColorBg = stringToColor(cols[1]);
		}

		// template
		else if (cols[0] == "beginTemplate") {
			buildingTemplate = cols[1];
//			cout << ">>> beginTemplate " << cols[1] << "\t\t" << uiName << endl;
			templateUI[buildingTemplate].clear();
		}
		
		else if (cols[0] == "endTemplate") {
//			cout << "endTemplate :: " << buildingTemplate << endl;
			buildingTemplate = "";
		}
		
		else if (cols[0] == "template") {
			string name = cols[1];
			for (auto s : templateUI[name]) {
				string str = ofJoinString(templateVectorString[name], " ");
				ofStringReplace(s, "{$vectorString}", str);
				if (s == "{$lineString}") {
					createFromLines(templateVectorString[name]);
				}
				if (cols.size() > 3) {
					ofStringReplace(s, "$2", cols[3]);
				}
				ofStringReplace(s, "$", cols[2]);
				createFromLine(s);
			}
		}
		
		else if (cols[0] == "intsList" || cols[0] == "floatsList") { //
			vector <string> nomes = ofSplitString(name, " ");
			for (auto & n : nomes) {
				string line = "int	" + n + "	" + cols[2];
				if ( cols[0] == "floatsList" ) {
					line = "float	" + n + "	" + cols[2];
				}
				createFromLine(line);
			}
		}
		
		if (cols[0] == "toggleMatrix" || cols[0] == "boolMatrix") {
			useLabelOnNewElement = false;

			string valores = cols[2];
			if (valores != "") {
				vector <string> lines;
				vector <string> vals = ofSplitString(valores, " ");
				int maxx = ofToInt(vals[0]);
				int maxy = ofToInt(vals[1]);
				for (int y=0; y<maxy; y++) {
					lines.push_back("flowHoriz");
					for (int x=0; x<maxx; x++) {
						string nomeElement = name + ofToString(x) + ofToString(y);
						string n = name + "_" + ofToString(x) + "_" +ofToString(y);
//						lines.push_back("toggleNoLabel	" + n + "	0");
						lines.push_back("toggleNoLabel	" + n + "	0");
					}
					lines.push_back("flowVert");
					lines.push_back("");
				}
				
				for (auto & l : lines) {
					createFromLine(l);
				}
				// isto nao funcionou e parou tudo que havia abaixo dali.
				//createFromLines(lines);
			}
			useLabelOnNewElement = true;
		}
		
		
//		else if (tipo == "ints" || tipo == "floats" || tipo == "bools" || tipo == "bangs" ||
//				 tipo == "holds" || tipo == "colors" || tipo == "slider2ds" ||
//				 tipo == "boolsNoLabel" || tipo == "sliderVerts") {
			
		else if (cols[0] == "ints" || cols[0] == "floats" || cols[0] == "bools") {
			string tipo = cols[0];
			string nome = cols[1];
			vector <string> nomes = ofSplitString(cols[1], "[");
			string n = nomes[0];
			string intervalo = ofSplitString(nomes[1], "]")[0];
			int start = ofToInt(ofSplitString(intervalo, "-")[0]);
			int end = ofToInt(ofSplitString(intervalo, "-")[1]);
			string newTipo = tipo.substr(0, tipo.size()-1);
			if (tipo == "boolsNoLabel") {
				createFromLine("flowHoriz");
				newTipo = "toggleNoLabel";
			}
			for (int a=start; a<=end; a++) {
				createFromLine(newTipo + "	"+n + "_" + ofToString(a) + "	" + cols[2]);
			}
			if (tipo == "boolsNoLabel") {
				createFromLine("flowVert");
			}
		}
		
		else if (cols[0] == "labelOnNewElement") {
			useLabelOnNewElement = cols[1] == "1" ? 1 : 0;
		}
		// 2 parameters
		else if (cols[0] == "addUI" || cols[0] == "addUIDown") {
			string loadText = "";
			if (cols.size() > 2) {
				loadText = cols[2];
			}
			addUI(cols[1], cols[0] == "addUIDown", loadText);
		}
		
		else if (cols[0] == "addUILabel") {
			addUI(cols[1]);
			
			uis[cols[1]].createFromLine( "label	"+cols[2] );
			uis[cols[1]].updateRect();

//			uis[cols[1]].autoFit();
			// FIXME
		}
		
		else if (cols[0] == "addShortcutUI") {
			if (cols.size() == 2) {
				vector <string> uis = ofSplitString(cols[1], " ");
				for (auto & u : uis) {
					addShortcutUI(&_masterUI->uis[u]);
					cout << "addShortcutUI " << uiName << " :: " << u << endl;
				}
			}
			else if (cols.size() == 3) {
				vector <string> uisString = ofSplitString(cols[2], " ");
				for (auto & u : uisString) {
					uis[cols[1]].addShortcutUI(&uis[u]);
//					addShortcutUI(&_masterUI->uis[u]);
					cout << "addShortcutUI " << uiName << " :: " << u << endl;
				}
			}
		}

		else if (cols[0] == "colorHsv" || cols[0] == "colorHsvA" || cols[0] == "colorHsvRange") {
//			ofColor c = ofColor(255,0,70);
			ofColor c = ofColor(255);
			if (cols.size() > 2) {
				if (cols[2] != "") {
					cout << cols[2] << endl;
//				cout << "colorHsv color : " << cols[2] << endl;
					c = stringHexToColor(cols[2]);
				}
			}
			int param = 0;
			if (cols[0] == "colorHsvA") {
				param = 1;
			}
			else if (cols[0] == "colorHsvRange") {
				param = 2;
			}
			elements.push_back(new colorHsv(name, *this, c, pColor[name], param));
			// bool useAlpha = cols[0] == "colorHsvA";
			// elements.push_back(new colorHsv(name, *this, c, pColor[name], useAlpha));
		}

		else if (cols[0] == "colorHsvTest") {
			elements.push_back(new slider2d(name, *this, pVec2[name]));
			ofFbo * _fbo = &((slider2d*)elements.back())->fbo;
			
			if (2==3) {
				_fbo->begin();
				ofClear(0);
				ofColor cor;
				cout << "colorHsvTest" << endl;
				cout << _fbo->getWidth() << endl;
				cout << _fbo->getHeight() << endl;
				cout << "----------" << endl;

				int w = _fbo->getWidth();
				int h = _fbo->getHeight();
				for (int b=0; b<h; b++) {
					for (int a=0; a<w; a++) {
		//				int este = b*w + a;
						float hue = (255 * a / (float) w);
						cor = ofColor::fromHsb(hue, 255, b*255/h, 255);
						ofFill();
						ofSetColor(cor);
						ofDrawRectangle(a,b,1,1);
					}
				}
				_fbo->end();
			}
		}
 		
		else if (cols[0] == "colorPalette") {
//			elements.push_back(new colorPalette(name, *this, c, pColor[name], cols[0] == "colorHsvA"));
			elements.push_back(new label(name, *this));
			elements.push_back(new colorPalette(name, *this, pVec2[name], 1));
			((colorPalette*)elements.back())->loadPalettes(cols[2]);
			((colorPalette*)elements.back())->_colorVal = &pColor[name];
		}

		else if (cols[0] == "slider2d") {
			elements.push_back(new slider2d(name, *this, pVec2[name]));
		}
		
		else if (cols[0] == "adsr") {
			elements.push_back(new adsr(name, *this, pVec2[name]));
		}

		else if (cols[0] == "label") {
			elements.push_back(new label(name, *this));
		}

		else if (cols[0] == "input") {
			elements.push_back(new input(name, *this, pString[name]));
		}
		
		else if (cols[0] == "inputPresetLabel") {
			elements.push_back(new input(name, *this, pString[name]));
			using namespace std::placeholders;
			((input*)elements.back())->invokeString = std::bind(&ofxMicroUI::savePresetLabel, this, _1);
		}

		else if (cols[0] == "bar") {
			elements.push_back(new bar(name, *this));
		}
		
		else if (cols[0] == "inspector") {
			elements.push_back(new inspector(name, *this));
		}

		else if (cols[0] == "color") {
			//xaxa
			elements.push_back(new color(name, *this, stringToColor(cols[1])));
		}


		else if (cols[0] == "presets" || cols[0] == "presetsNoLabel") {
			if (cols[0] == "presetsNoLabel") {
				useLabelOnNewElement = false;
			}
			elements.push_back(new presets(name, *this, ofSplitString(cols[2]," "), pString[name]));
			using namespace std::placeholders;
			((presets*)elements.back())->invokeString = std::bind(&ofxMicroUI::saveOrLoadAll, this, _1);
			presetElement = (presets*)elements.back();
			if (cols[0] == "presetsNoLabel") {
				useLabelOnNewElement = true;
			}
		}
		
		else if (cols[0] == "presetsLoad") {
			elements.push_back(new radio(name, *this, ofSplitString(cols[2]," "), pString[name]));
			using namespace std::placeholders;
			((radio*)elements.back())->invokeString = std::bind(&ofxMicroUI::loadPreset, this, _1);
		}
		
		else if (cols[0] == "presetsSave") {
			elements.push_back(new radio(name, *this, ofSplitString(cols[2]," "), pString[name]));
			using namespace std::placeholders;
			((radio*)elements.back())->invokeString = std::bind(&ofxMicroUI::savePreset, this, _1);
		}

		else if (cols[0] == "fbo") {
			if (cols.size() == 3) {
				elements.push_back(new fboElement(name, *this, ofToInt(cols[2])));
			} else {
				elements.push_back(new fboElement(name, *this));
			}
		}

		
		// 3 parameters
		else if (cols[0] == "image") {
			elements.push_back(new image(name, *this, cols[2]));
		}

		else if (cols[0] == "vec3") {
			elements.push_back(new vec3(name, *this, pVec3[name]));
		}
		
		else if (cols[0] == "flipflop") {
			elements.push_back(new flipflop(name, *this, pInt[name]));
		}
		
		else if (cols[0] == "float" || cols[0] == "int") {
			vector <string> values = ofSplitString(cols[2]," ");
			glm::vec3 vals = glm::vec3(ofToFloat(values[0]),ofToFloat(values[1]),ofToFloat(values[2]));
			if (cols[0] == "float") {
				elements.push_back(new slider(name, *this, vals, pFloat[name]));
			} else {
				elements.push_back(new slider(name, *this, vals, pInt[name]));
			}
			
			if (cols.size() == 4) {
				if (cols[0] == "float") {
					// value minimum able to be zero
					vals.x = 0;
					vals.z = 0;
					if (cols[3] == "audio") {
						string n = name + "Audio";
						elements.push_back(new slider(n, *this, vals, pFloat[n]));
					}
					else if (cols[3] == "beat") {
						string n = name + "Beat";
						elements.push_back(new slider(n, *this, vals, pFloat[n]));
					}
					else if (cols[3] == "audioBeat") {
						{
							string n = name + "Audio";
							elements.push_back(new slider(n, *this, vals, pFloat[n]));
						}
						{
							string n = name + "Beat";
							elements.push_back(new slider(n, *this, vals, pFloat[n]));
						}
					}
				}
			}
		}
		
		else if (cols[0]  == "_float") {
			vector<string> vals = ofSplitString(cols[2]," ");
			pFloat[name] = stof(vals[2]);
		}
		
		else if (cols[0]  == "_int") {
			vector<string> vals = ofSplitString(cols[2]," ");
			pInt[name] = stof(vals[2]);
		}
		
		else if (cols[0]  == "_bool") {
			// cout << "BOOL " << name << " xxx " << stoi(cols[2]) << endl;
			pBool[name] = stoi(cols[2]);
		}
		
		
		else if (cols[0] == "bool" ||
				 cols[0] == "boolNoLabel" ||
				 cols[0] == "toggle" ||
				 cols[0] == "toggleNoLabel" ||
				 cols[0] == "bang"
				 ) {
			bool val = false;
			if (cols.size() > 2) {
				val = ofToBool(cols[2]);
			}
			pBool[name] = val;
			if (cols[0] == "toggleNoLabel" || cols[0] == "boolNoLabel") {
				useLabelOnNewElement = false;
			}
			elements.push_back(new toggle (name, *this, val, pBool[name], true));
			
			if (cols[0] == "toggleNoLabel" || cols[0] == "boolNoLabel") {
				useLabelOnNewElement = true;
			}
			
			if (cols[0] == "bang") {
				((toggle*)elements.back())->isBang = true;
			}
		}
		
		else if (cols[0] == "hold") {
			elements.push_back(new hold (name, *this, false, pBool[name], true));
		}
		
		// bang improvisado aqui. fazer de verdade.
//		else if (cols[0] == "bang") {
//			bool val = false;
//			pBool[name] = val;
//			elements.push_back(new toggle (name, *this, val, pBool[name], true));
//		}
		
		else if (cols[0] == "radio" || cols[0] == "radioNoLabel") {
			useLabelOnNewElement = !(cols[0] == "radioNoLabel"); // false if radioNoLabel == true
			elements.push_back(new radio(name, *this, ofSplitString(cols[2]," "), pString[name]));
			useLabelOnNewElement = true;
			if (ofIsStringInString(name, "_shortcut")) {
				elements.back()->saveXml = false;
			}
		}
		else if (cols[0] == "radioPipeNoLabel" || cols[0] == "radioPipe") {
			// todo : eliminate label. maybe optionally some variable on flow or settings
			//useLabelOnNewElement = false;
			elements.push_back(new radio(name, *this, ofSplitString(cols[2],"|"), pString[name]));
			//useLabelOnNewElement = true;
			if (ofIsStringInString(name, "_shortcut")) {
//				cout << name << ":::" << "YESSS" << endl;
				elements.back()->saveXml = false;
			}
		}

		else if (cols[0] == "camList") {
			elements.push_back(new camList(name, *this, pString[name], pCam[name]));
		}
		
		// todo : regexp
		else if (cols[0] == "dirList"
				 || cols[0] == "scene"
				 || cols[0] == "sceneNoLabel"
				 || cols[0] == "imageList"
				 || cols[0] == "texList"
				 || cols[0] == "videoList"
				 || cols[0] == "audioList"
				 || cols[0] == "textList"
				 || cols[0] == "fontList"
				 ) {
			ofDirectory dir;
			if (cols[0] == "scene" || cols[0] == "sceneNoLabel") {
				dir.allowExt("txt");
			}
			
			dir.listDir(cols[2]);
			dir.sort();
			vector <string> opcoes;
			opcoes.push_back("_");
			for (auto & d : dir) {
				if (
					cols[0] == "dirListNoExt" ||
					cols[0] == "scene" ||
					cols[0] == "sceneNoLabel"
				) {
					// sem extensao
					opcoes.push_back(d.getBaseName());
				} else
				{
					// com extensao
					opcoes.push_back(d.getFileName());
				}
			}
			
			if (cols[0] == "sceneNoLabel") {
				useLabelOnNewElement = false;
			}
			
			if (cols[0] == "imageList" || cols[0] == "texList") {
				elements.push_back(new imageList(name, *this, opcoes, pString[name], pImage[name]));
				if (cols[0] == "texList") {
					((imageList*)elements.back())->disableArb = true;
				}

			}
			
			else if (cols[0] == "videoList") {
				elements.push_back(new videoList(name, *this, opcoes, pString[name], pVideo[name]));
			}

			else if (cols[0] == "audioList") {
				elements.push_back(new audioList(name, *this, opcoes, pString[name], pAudio[name]));
			}

			else if (cols[0] == "textList") {
				elements.push_back(new textList(name, *this, opcoes, pString[name], pText[name]));
			}
			else if (cols[0] == "fontList") {
				elements.push_back(new fontList(name, *this, opcoes, pString[name], pFont[name]));
			}

			else {
				elements.push_back(new dirList(name, *this, opcoes, pString[name]));
			}
			((dirList*)elements.back())->filePath = cols[2];
			
			if (cols[0] == "sceneNoLabel") {
				useLabelOnNewElement = true;
			}

			if (cols[0] == "scene" || cols[0] == "sceneNoLabel") {
				if (_masterUI != NULL) {
					//_masterUI->
//					((dirList*)elements.back())->_ui = &_masterUI->uis[name];
					((dirList*)elements.back())->_uiScene = &_masterUI->uis[name];
				}
			}
		}
	}
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

void ofxMicroUI::onMouseReleased(ofMouseEventArgs &data) {
	int xx = data.x - rectPos.x - _settings->offset.x;
	int yy = data.y - rectPos.y - _settings->offset.y;

	for (auto & e : elements) {
		e->mouseRelease(xx, yy);
	}
}

void ofxMicroUI::loadPresetByIndex(const int & i) {
	string n = presetElement->getValByIndex(i);
	loadPreset(n);
	presetElement->redraw();
}



void ofxMicroUI::setPresetsFolder(const string & s) {
	presetsFolder = s;
	if (presetElement != NULL) {
		presetElement->hasXmlCheck();
		presetElement->redraw();
	}
}

void ofxMicroUI::clear() {
	createdLines = "";
	rect.width = rect.height = 10;
	updatedRect = false;
	initFlow();
	
	for (auto & e : elements) {
		delete e;
	}
	elements.clear();

	pFloat.clear();
	pInt.clear();
	pBool.clear();
	pString.clear();
	pColor.clear();
	// FIXME: clear other stuff here.
	
	redrawUI = true;
}




void ofxMicroUI::set(const string & name, const float & v) {
	slider * e = getSlider(name);
	if (e != NULL) {
//			e->eventFromOsc = true;
		e->set(v);
	} else {
		cout << "set non existant element " << name << "::" << uiName << endl;
	}
//		getSlider(name)->set(v);
}

void ofxMicroUI::set(const string & name, const int & v) {
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

void ofxMicroUI::set(const string & name, const bool & v) {
	toggle * e = getToggle(name);
	if (e != NULL) {
		e->set(v);
	}
}

void ofxMicroUI::set(const string & name, const string & v) {
	radio * e = getRadio(name);
	if (e != NULL) {
		cout << "setting radio " << name << " val " << v << endl;
		e->set(v);
	}
}




void ofxMicroUI::uiEvents(ofxMicroUI::element & e) {
//		cout << "uiEvents :: " << uiName << " :: " << e.name << endl;
	if (!e._ui->_settings->presetIsLoading) {
		for (auto & s : shortcutUIs) {
			s->forwardEventFrom(e);
		}
	}
}


void ofxMicroUI::forwardEventFrom(element & e) {
	element * e2 = getElement(e.name);
	if (e2 != NULL) {
		e2->copyValFrom(e);
	}
}

void ofxMicroUI::savePresetLabel(const string & p) {
	
	if (_masterUI == NULL) {
		_masterUI = this;
	}
	cout << "savePresetLabel " << _masterUI->pString["presets"] << endl;
	string filePath = getPresetPath() + "/" + _masterUI->pString["presets"] + "/0.txt";
	ofxMicroUI::stringToFile(p, filePath);
	
	presetItem * item = (presetItem *)_masterUI->presetElement->getElement(_masterUI->pString["presets"]);
	if (item != NULL) {
		item->hasXmlCheck();
		item->redraw();
		_masterUI->presetElement->redraw();
	}
//		cout << "savePresetLabel" << p << endl;
//		cout << "preset : " << f << endl;
	
}

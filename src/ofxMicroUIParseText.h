map <string, element *> 	elementsLookup;
map <string, slider *> 		slidersLookup;
map <string, toggle *> 		togglesLookup;
map <string, radio *> 		radiosLookup;
map <string, inspector *> 	inspectorsLookup;

element * getElement(string n) {
	return elementsLookup.find(n) != elementsLookup.end() ? elementsLookup[n] : NULL;
}

// and other kinds
slider * getSlider(string n) {
	return slidersLookup.find(n) != slidersLookup.end() ? slidersLookup[n] : NULL;
}

toggle * getToggle(string n) {
	return togglesLookup.find(n) != togglesLookup.end() ? togglesLookup[n] : NULL;
}

radio * getRadio(string n) {
	return radiosLookup.find(n) != radiosLookup.end() ? radiosLookup[n] : NULL;
}

inspector * getInspector(string n) {
	return inspectorsLookup.find(n) != inspectorsLookup.end() ? inspectorsLookup[n] : NULL;
}

bool updatedRect = false;

void updateRect() {
	elementsLookup.clear();
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
		if (dynamic_cast<toggle*>(e)) {
			togglesLookup[e->name] = (toggle*)e;
		}
		if (dynamic_cast<radio*>(e)) {
			radiosLookup[e->name] = (radio*)e;
		}
		if (dynamic_cast<inspector*>(e)) {
			inspectorsLookup[e->name] = (inspector*)e;
		}

		rect.growToInclude(e->rect);
		elementsLookup[e->name] = e;
	}
	
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

void createFromLines(string & line) {
	vector <string> lines = ofSplitString(line, "\r");
	createFromLines(lines);
}

//void createFromLines(vector<string> & lines) {

void createFromLines(vector<string> & lines, bool complete = true) {

	_settings->presetIsLoading = true;

	if (_settings->useFixedLabel && complete) {
		createFromLine("label	" + ofToUpper(uiName));
	}
	//elements.back()->
	
	for (auto & l : lines) {
		if (buildingTemplate == "") {
			createFromLine(l);
		} else {
			if (ofIsStringInString(l, "endTemplate")) {
				buildingTemplate = "";
			} else {
//				cout << ">>> building template " << buildingTemplate << "\t\t" << "add line " << l << endl;
				templateUI[buildingTemplate].push_back(l);
			}
		}
	}
	if (!updatedRect && complete) {
		updateRect();
	}
	_settings->presetIsLoading = false;
}

ofColor stringToColor(string s) {
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


void createFromLine(string l) {
	vector <string> cols = ofSplitString(l, "\t");
	if (cols.size() == 1) {
		if (l == "") {
			newLine();
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
            cout << uiName << " saveMode " << cols[1] << endl;
		}
		
		else if (cols[0] == "loadMode") {
			if (cols[1] == "NONE") {
				loadMode = NONE;
			} else if (cols[1] == "PRESETSFOLDER") {
				loadMode = PRESETSFOLDER;
			} else if (cols[1] == "MASTER") {
				loadMode = MASTER;
			}
            cout << uiName << " loadMode " << cols[1] << endl;
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
//			cout << ">>> using template " << name << endl;
			for (auto s : templateUI[name]) {
				string str = ofJoinString(templateVectorString[name], " ");
				ofStringReplace(s, "{$vectorString}", str);
				if (s == "{$lineString}") {
//					cout << s << endl;
//					cout << name << endl;
//					for (auto & l : templateVectorString[name]) {
//						cout << l << endl;
//					}
//					cout << "----------" << endl;
					createFromLines(templateVectorString[name]);
				}
				ofStringReplace(s, "$", cols[2]);
//				cout << s << endl;
				createFromLine(s);
			}
		}
		
		else if (cols[0] == "intsList") { // || tipo == "floatsList"
//			cout << "intsList! " << endl;
			vector <string> nomes = ofSplitString(name, " ");
			for (auto & n : nomes) {
				string line = "int	" + n + "	" + cols[2];
//				cout << line << endl;
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
			
		else if (cols[0] == "ints" || cols[0] == "floats") {
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
		
		
		// 2 parameters
		else if (cols[0] == "addUI" || cols[0] == "addUIDown") {
			string loadText = "";
			if (cols.size() > 2) {
				loadText = cols[2];
			}
			addUI(cols[1], cols[0] == "addUIDown", loadText);
		}
		
		else if (cols[0] == "addShortcutUI") {
			vector <string> uis = ofSplitString(cols[1], " ");
			for (auto & u : uis) {
                //if (_masterUI->uis.count(u))
                {
                    addShortcutUI(&_masterUI->uis[u]);
                }
			}
		}

		else if (cols[0] == "colorHsv" || cols[0] == "colorHsvA" || cols[0] == "colorHsvRange") {
//			ofColor c = ofColor(255,0,70);
			ofColor c = ofColor(255);
			if (cols.size() > 1) {
				// change color here.
				// stringtocolor?
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
			elements.push_back(new colorPalette(name, *this, pVec2[name]));
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
			elements.push_back(new fboElement(name, *this));
		}

		
		// 3 parameters
		else if (cols[0] == "image") {
			elements.push_back(new image(name, *this, cols[2]));
		}

		else if (cols[0] == "vec3") {
			elements.push_back(new vec3(name, *this, pVec3[name]));
		}
		
		else if (cols[0] == "float" || cols[0] == "int") {
			vector <string> values = ofSplitString(cols[2]," ");
			glm::vec3 vals = glm::vec3(ofToFloat(values[0]),ofToFloat(values[1]),ofToFloat(values[2]));
			if (cols[0] == "float") {
				elements.push_back(new slider(name, *this, vals, pFloat[name]));
			} else {
				elements.push_back(new slider(name, *this, vals, pInt[name]));
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
				 cols[0] == "toggle" ||
				 cols[0] == "toggleNoLabel" ||
				 cols[0] == "bang"
				 ) {
			bool val = false;
			if (cols.size() > 2) {
				val = ofToBool(cols[2]);
			}
			pBool[name] = val;
			if (cols[0] == "toggleNoLabel") {
				useLabelOnNewElement = false;
			}
			elements.push_back(new toggle (name, *this, val, pBool[name], true));
			
			if (cols[0] == "toggleNoLabel") {
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
		
		else if (cols[0] == "radio") {
			elements.push_back(new radio(name, *this, ofSplitString(cols[2]," "), pString[name]));
			if (ofIsStringInString(name, "_shortcut")) {
//				cout << name << ":::" << "YESSS" << endl;
				elements.back()->saveXml = false;
			}
		}
		else if (cols[0] == "radioNoLabel") {
			useLabelOnNewElement = false;
			elements.push_back(new radio(name, *this, ofSplitString(cols[2]," "), pString[name]));
			useLabelOnNewElement = true;
			if (ofIsStringInString(name, "_shortcut")) {
//				cout << name << ":::" << "YESSS" << endl;
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

string loadedTextFile = "";

void reload() {
	cout << "ofxMicroUI Reload" << uiName << endl;
	clear();
	createFromText(loadedTextFile);
}

string createdLines = "";

// 16 02 2021 - testing to solve future ui element issues
bool uiIsCreated = false;

void createFromText(string fileName) {
	initFlow();
	//alert("createFromText " + fileName);

	// temporary, to debug
    loadedTextFile = fileName;
	
	if (futureLines.size()) {
		createFromLines(futureLines);
		createdLines = ofJoinString(futureLines, "\r");
	}
	
//	cout << "create from text " << _settings->styleLines << endl;
//	cout << "----- " << uiName <<  endl;
//	for (auto & l : ofSplitString(_settings->styleLines, "\r")) {
//		cout << l << endl;
//		createFromLine(l);
//	}
//	cout << "-----" << endl;
	
//	createFromLines(_settings->styleLines);
	
	vector <string> lines = textToVector(fileName);
	createFromLines(lines);
	createdLines += ofJoinString(lines, "\r");
	
	notify("createFromText");

	// NOVIDADE 14 jul 2020
	redrawUI = true;
	//cout << futureLines.size() << endl;
	// yes? no?
	//futureLines.clear();
	uiIsCreated = true;
}

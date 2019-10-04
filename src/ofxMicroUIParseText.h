map <string, element *> 	elementsLookup;
map <string, slider *> 		slidersLookup;
map <string, toggle *> 		togglesLookup;
map <string, radio *> 		radiosLookup;
map <string, inspector *> 	inspectorsLookup;

// TODO
// lookup table to return element by name
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
	//cout << "updateRect ! " << endl;
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
	
	rect.width += _settings->margin;
	rect.height += _settings->margin;
	//rectPos.setDimensions(rect.getDimensions());
	rectPos.width = rect.width;
	rectPos.height = rect.height;
//	cout << "updatedrect: " << endl;
//	cout << rect << endl;

	fbo.allocate(rect.width, rect.height, GL_RGBA);
	fbo.begin();
	ofClear(0,255);
	fbo.end();
	updatedRect = true;
}


void createFromLines(vector<string> & lines) {
	
	for (auto & l : lines) {
		if (buildingTemplate == "") {
			createFromLine(l);
		} else {
			if (ofIsStringInString(l, "endTemplate")) {
				buildingTemplate = "";
			} else {
				templateUI[buildingTemplate].push_back(l);
			}
		}
	}
	if (!updatedRect) {
		updateRect();
	}
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
		else if (l == "flowVert" || l == "flowHoriz") {
			setFlowVert(l == "flowVert");
		}
	}
	if (cols.size() >= 2) {
		string name = cols[1];
		
		// START SETTINGS
		if (cols[0] == "elementSpacing") {
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
			_settings->colorBg = ofColor(ofToFloat(cols[1]));
		}
		else if (cols[0] == "colorVal") {
			_settings->colorVal = ofColor(ofToFloat(cols[1]));
		}
		else if (cols[0] == "colorLabel") {
			_settings->colorLabel = ofColor(ofToFloat(cols[1]));
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
			_settings->useCustomFont = _settings->font.load(cols[1], ofToInt(cols[2]));
			//software.customFont = software.font.load(nome, ofToInt(valores));;

		}
		// END SETTINGS
		
		else if (cols[0] == "beginTemplate") {
			buildingTemplate = cols[1];
			templateUI[buildingTemplate].clear();
		}
		
		else if (cols[0] == "template") {
			string name = cols[1];
			for (auto s : templateUI[name]) {
				string str = ofJoinString(templateVectorString[name], " ");
				ofStringReplace(s, "{$vectorString}", str);
				if (s == "{$lineString}") {
					createFromLines(templateVectorString[name]);
				}
				ofStringReplace(s, "$", cols[2]);
				createFromLine(s);
			}
		}

		if (cols[0] == "toggleMatrix") {
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
		
		
		// 2 parameters
		else if (cols[0] == "addUI") {
			addUI(cols[1]);
		}
		else if (cols[0] == "addUIDown") {
			addUI(cols[1],true);
		}


		else if (cols[0] == "label") {
			elements.push_back(new label(name, *this));
		}
		else if (cols[0] == "inspector") {
			elements.push_back(new inspector(name, *this));
		}

		else if (cols[0] == "presets") {
			elements.push_back(new presets(name, *this, ofSplitString(cols[2]," "), pString[name]));
//			elements.push_back(new presets(name, *this, 10, pString[name]));
//			using namespace std::placeholders;
//			((presetRadio*)elements.back())->invokeString = std::bind(&ofxMicroUI::saveOrLoad, this, _1);
			using namespace std::placeholders;
			((presetRadio*)elements.back())->invokeString = std::bind(&ofxMicroUI::saveOrLoadAll, this, _1);
		}
		else if (cols[0] == "presetsRadio") {
			elements.push_back(new presetRadio(name, *this, 10, pString[name]));
			using namespace std::placeholders;
			((presetRadio*)elements.back())->invokeString = std::bind(&ofxMicroUI::saveOrLoad, this, _1);
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
		
		else if (cols[0] == "bool" || cols[0] == "toggleNoLabel") {
			bool val = ofToBool(cols[2]);
			pBool[name] = val;
			if (cols[0] == "toggleNoLabel") {
				useLabelOnNewElement = false;
			}
			elements.push_back(new toggle (name, *this, val, pBool[name], true));
			if (cols[0] == "toggleNoLabel") {
				useLabelOnNewElement = true;
			}
		}
		
		else if (cols[0] == "radio") {
			elements.push_back(new radio(name, *this, ofSplitString(cols[2]," "), pString[name]));
		}
		else if (cols[0] == "radioPipeNoLabel") {
			// todo : eliminate label. maybe optionally some variable on flow or settings
			//useLabelOnNewElement = false;
			elements.push_back(new radio(name, *this, ofSplitString(cols[2],"|"), pString[name]));
			//useLabelOnNewElement = true;
		}

		else if (cols[0] == "dirList") {
			ofDirectory dir;
			dir.listDir(cols[2]);
			dir.sort();
			vector <string> opcoes;
			for (auto & d : dir) {
//				if (tipo == "dirListNoExt" || tipo == "scene" || tipo == "sceneNoLabel") {
//					opcoes.push_back(d.getBaseName());
//				} else
				{
					opcoes.push_back(d.getFileName());
				}
			}
			
//			cout << "dirList" << endl;
//			cout << ofJoinString(opcoes, "-") << endl;
			elements.push_back(new dirList(name, *this, opcoes, pString[name]));
			((dirList*)elements.back())->filePath = cols[2];
		}
	}
}


string textFile = "";

void createFromText(string fileName) {
	alert("createFromText " + fileName);
	// temporary, to debug
	textFile = fileName;
	vector <string> lines = textToVector(fileName);
	
	if (futureLines.size()) {
		createFromLines(futureLines);
	}
	createFromLines(lines);
	//cout << futureLines.size() << endl;
	// yes? no?
	//futureLines.clear();
}

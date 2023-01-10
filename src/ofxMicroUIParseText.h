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

void createFromLines(const string & line) {
	vector <string> lines = ofSplitString(line, "\n");
	createFromLines(lines);
}

//void createFromLines(vector<string> & lines) {

void createFromLines(const vector<string> & lines, bool complete = true) {

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

void createFromLine(string l);

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

	// temporary, to debug
	loadedTextFile = fileName;
	
	if (futureLines.size()) {
		createFromLines(futureLines);
		createdLines = ofJoinString(futureLines, "\r");
	}

//	vector <string> lines = textToVector(fileName);
	string lines = ofBufferFromFile(fileName).getText();
	
	if (replaces.size()) {
		for (auto & r : replaces) {
//			cout << r.first << " : " << r.second << endl;
			string find = "{" + r.first +"}";
			ofStringReplace(lines, find, r.second);
		}
	}
	
//	cout << lines << endl;
	createFromLines(lines);

	//	createdLines += ofJoinString(lines, "\r");
	createdLines += lines;
	notify("createFromText");

	// NOVIDADE 14 jul 2020
	redrawUI = true;
	//cout << futureLines.size() << endl;
	// yes? no?
	//futureLines.clear();
	uiIsCreated = true;
}

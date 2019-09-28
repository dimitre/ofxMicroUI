map <string, element *> elementsLookup;
map <string, inspector *> inspectorsLookup;

// TODO
// lookup table to return element by name
element * getElement(string n) {
	return elementsLookup.find(n) != elementsLookup.end() ? elementsLookup[n] : NULL;
}

// and other kinds
slider * getSlider(string n) {
	return (slider*)getElement(n);
}
inspector * getInspector(string n) {
	return inspectorsLookup.find(n) != inspectorsLookup.end() ? inspectorsLookup[n] : NULL;
}

bool updatedRect = false;

void updateRect() {
	//cout << "updateRect ! " << endl;
	elementsLookup.clear();
	
	// build the interface rectangle to buffer drawing into an FBO, and create ElementsLookup
	//rect = elements[0]->rect;
	//cout << elements.size() << endl;
	for (auto & e : elements) {
		inspector * test = dynamic_cast<inspector*>(e);
		if (test) {
			inspectorsLookup[e->name] = (inspector*)e;
		}

		//cout << e->name << endl;
		rect.growToInclude(e->rect);
		elementsLookup[e->name] = e;
	}
	
	rect.width += _settings->margin;
	rect.height += _settings->margin;
	
	fbo.allocate(rect.width, rect.height, GL_RGBA);
	fbo.begin();
	ofClear(0,255);
	fbo.end();
	updatedRect = true;
}


void createFromLines(vector<string> & lines) {
	for (auto & l : lines) {
		createFromLine(l);
	}
	if (!updatedRect) {
		updateRect();
	}
}

void createFromLine(string l) {
	vector <string> cols = ofSplitString(l, "\t");
	if (cols.size() == 1) {
		if (l == "") {
			_settings->newLine();
		}
		else if (l == "newCol") {
			_settings->newCol();
		}
		else if (l == "flowVert" || l == "flowHoriz") {
			_settings->setFlowVert(l == "flowVert");
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
		// END SETTINGS

		
		if (cols[0] == "toggleMatrix") {
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
						lines.push_back("toggleNoLabel	" + n + "	0");
					}
					lines.push_back("flowVert");
					lines.push_back("");
				}
				createFromLines(lines);
			}
		}
		
		
		// 2 parameters
		else if (cols[0] == "addUI") {
			// if (soft != NULL) {
				// soft->addUI(cols[1]);
				addUI(cols[1]);
			// }
		}


		else if (cols[0] == "label") {
			elements.push_back(new label(name, *_settings));
		}
		else if (cols[0] == "inspector") {
			elements.push_back(new inspector(name, *_settings));
		}

		else if (cols[0] == "presets") {
			elements.push_back(new preset(name, *_settings, 10, pString[name]));
			
			using namespace std::placeholders;
			((preset*)elements.back())->invokeString = std::bind(&ofxMicroUI::saveOrLoad, this, _1);
		}

		else if (cols[0] == "fbo") {
			elements.push_back(new fboElement(name, *_settings));
		}

		
		// 3 parameters
		else if (cols[0] == "image") {
			elements.push_back(new image(name, *_settings, cols[2]));
		}



		else if (cols[0] == "vec3") {
			elements.push_back(new vec3(name, *_settings, pVec3[name]));
		}
		
		else if (cols[0] == "float" || cols[0] == "int") {
			vector <string> values = ofSplitString(cols[2]," ");
			glm::vec3 vals = glm::vec3(ofToFloat(values[0]),ofToFloat(values[1]),ofToFloat(values[2]));
			if (cols[0] == "float") {
				elements.push_back(new slider(name, *_settings, vals, pFloat[name]));
			} else {
				elements.push_back(new slider(name, *_settings, vals, pInt[name]));
			}
		}
		
		else if (cols[0] == "bool" || cols[0] == "toggleNoLabel") {
			bool val = ofToBool(cols[2]);
			pBool[name] = val;
			elements.push_back(new toggle (name, *_settings, val, pBool[name], 1, cols[0] == "bool"));
		}
		
		else if (cols[0] == "radio") {
			elements.push_back(new radio(name, *_settings, ofSplitString(cols[2]," "), pString[name]));
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
			elements.push_back(new dirList(name, *_settings, opcoes, pString[name]));
			((dirList*)elements.back())->filePath = cols[2];
		}
	}
}


string textFile = "";

void createFromText(string fileName) {
	// temporary, to debug
	textFile = fileName;
	alert("createFromText " + fileName);
	vector <string> lines = textToVector(fileName);
	createFromLines(lines);
}

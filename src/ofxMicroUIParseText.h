
void updateRect() {
	elementsLookup.clear();
	
	// build the interface rectangle to buffer drawing into an FBO, and create ElementsLookup
	for (auto & e : elements) {
		rect.growToInclude(e->rect);
		elementsLookup[e->name] = e;
	}
	rect.width += settings.margin;
	rect.height += settings.margin;
	
	fbo.allocate(rect.width, rect.height, GL_RGBA);
}


void createFromLines(vector<string> & lines) {
	for (auto & l : lines) {
		createFromLine(l);
	}
	updateRect();
}

void createFromLine(string & l) {
	vector <string> cols = ofSplitString(l, "\t");
	if (cols.size() == 1) {
		if (l == "") {
			settings.newLine();
		}
		else if (l == "newCol") {
			settings.newCol();
		}
		else if (l == "flowVert" || l == "flowHoriz") {
			settings.setFlowVert(l == "flowVert");
		}
	}
	if (cols.size() >= 2) {
		string name = cols[1];
		
		// SETTINGS
		if (cols[0] == "elementSpacing") {
			settings.elementSpacing = ofToFloat(cols[1]);
		}
		else if (cols[0] == "elementPadding") {
			settings.elementPadding = ofToFloat(cols[1]);
		}
		else if (cols[0] == "sliderHeight") {
			settings.elementRect.height = ofToFloat(cols[1]);
		}
		else if (cols[0] == "sliderWidth") {
			settings.elementRect.width = ofToFloat(cols[1]);
		}
		else if (cols[0] == "colorBg") {
			settings.colorBg = ofColor(ofToFloat(cols[1]));
		}
		else if (cols[0] == "colorVal") {
			settings.colorVal = ofColor(ofToFloat(cols[1]));
		}
		else if (cols[0] == "colorLabel") {
			settings.colorLabel = ofColor(ofToFloat(cols[1]));
		}
		
		

		if (cols[0] == "label") {
			elements.push_back(new label(name, settings));
		}
		if (cols[0] == "inspector") {
			elements.push_back(new inspector(name, settings));
		}

		else if (cols[0] == "vec3") {
			elements.push_back(new vec3(name, settings, pVec3[name]));
		}
		
		else if (cols[0] == "float" || cols[0] == "int") {
			vector <string> values = ofSplitString(cols[2]," ");
			glm::vec3 vals = glm::vec3(ofToFloat(values[0]),ofToFloat(values[1]),ofToFloat(values[2]));
			if (cols[0] == "float") {
				elements.push_back(new slider(name, settings, vals, pFloat[name]));
			} else {
				elements.push_back(new slider(name, settings, vals, pInt[name]));
			}
		}
		
		else if (cols[0] == "bool") {
			bool val = ofToBool(cols[2]);
			pBool[name] = val;
			elements.push_back(new toggle (name, settings, val, pBool[name]));
		}
		
		else if (cols[0] == "radio") {
			elements.push_back(new radio(name, settings, ofSplitString(cols[2]," "), pString[name]));
		}
	}
}


void createFromText(string fileName) {
	alert("createFromText " + fileName);
	vector <string> lines = textToVector(fileName);
	createFromLines(lines);
//	for (auto & l : lines) {
//		createFromLine(l);
//
//	}
	
}

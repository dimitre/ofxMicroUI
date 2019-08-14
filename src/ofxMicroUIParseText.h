void updateRect() {
	for (auto & e : elements) {
		rect.growToInclude(e->rect);
	}
	rect.width += settings.margin;
	rect.height += settings.margin;
	
	fbo.allocate(rect.width, rect.height, GL_RGBA);
}


void createFromText(string fileName) {
	alert("createFromText " + fileName);
	vector <string> lines = textToVector(fileName);
	for (auto & l : lines) {
		
		vector <string> cols = ofSplitString(l, "\t");
		if (cols.size() == 1) {
			if (l == "") {
				settings.newLine();
			}
			else if (l == "newCol") {
				settings.newCol();
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

			if (cols[0] == "label") {
				elements.push_back(new label(name, settings));
			}
			
			else if (cols[0] == "vec3") {
				elements.push_back(new vec3(name, settings, pVec3[name]));
			}
			
			else if (cols[0] == "float") {
				vector <string> values = ofSplitString(cols[2]," ");
				glm::vec3 vals = glm::vec3(ofToFloat(values[0]),ofToFloat(values[1]),ofToFloat(values[2]));
				elements.push_back(new slider(name, settings, vals, pFloat[name]));
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
	
	updateRect();
}

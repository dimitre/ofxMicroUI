struct microUISettings {
private:
	bool flowVert = true;
	
public:

	/*
	 Settings control the flow (distribution on xy) of elements, settings of elements, margins, etc.
	 a pointer is added to each element so they all obey to the same settings.

	 offset x, offset y (padding inside column)
	 column rectangle
	 slider dimensions.
	 */
	glm::vec2 xy = glm::vec2(10,10);
	float xBak = 0;
	map <string, int> pInt;
	int margin = 10;
	int spacing = 4;
	ofRectangle elementRect = ofRectangle(0,0,240,20);
	
	ofRectangle flowRect;
	
	void setFlowVert(bool s) {
		// if flow was horizontal and we change to horizontal, save the x coordinate
		if (flowVert && !s) {
			xBak = xy.x;
			//cout << "backup of the x coordinate " << xBak << endl;
		}
		// if flow was vertical and we change to vertical, bring back the backup x coordinate.
		if (!flowVert && s) {
			xy.x = xBak;
			//cout << "restore of the x coordinate " << xy.x <<  endl;
		}
		flowVert = s;
	}

	void advanceLine() {
		if (flowVert) {
			xy.y += 20 + spacing;
		} else {
			int newX = xy.x + flowRect.width - xBak;
			if (newX > 240 ) {
				xy.y += 20 + spacing;
				xy.x = xBak;
			} else {
				xy.x += flowRect.width + spacing;
			}
		}
	}
	
	void newCol() {
		xy.x += 240 + spacing;
		xy.y = margin;
	}
} settings;

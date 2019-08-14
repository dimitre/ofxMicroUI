/*
 Settings control the flow (distribution on xy) of elements, settings of elements, margins, etc.
 a pointer is added to each element so they all obey to the same settings.
 
 offset x, offset y (padding inside column)
 column rectangle
 slider dimensions.
 */

struct microUISettings {
private:
	bool flowVert = true;
	
public:
	
	// takes care of the flow of the elements.
	int margin = 15;

	glm::vec2 xy = glm::vec2(margin, margin);
	float xBak = 0;
	int elementSpacing = 4;
	int elementPadding = 4;

	ofRectangle elementRect = ofRectangle(0,0,240,18);

	ofColor colorBg = 127;
	ofColor colorVal = 70;
	ofColor colorLabel = 255;
	// pointer or copy?
	ofRectangle flowRect;
	
	void setFlowVert(bool s) {
		// if flow was horizontal and we change to horizontal, save the x coordinate
		if (flowVert && !s) {
			xBak = xy.x;
		}
		// if flow was vertical and we change to vertical, bring back the backup x coordinate.
		if (!flowVert && s) {
			xy.x = xBak;
		}
		flowVert = s;
	}

	// change to advanceLayout?
	bool advanceLine() {
		bool success = true;
		if (flowVert) {
			xy.y += elementRect.height + elementSpacing;
		} else {
			int newX = xy.x + flowRect.width + elementSpacing - xBak;
			if (newX > elementRect.width ) {
				success = false;
				xy.y += elementRect.height + elementSpacing;
				xy.x = xBak;
			} else {
				xy.x += flowRect.width + elementSpacing;
			}
		}
		return success;
	}
	
	void newLine() {
		xy.y += elementRect.height + elementSpacing;
	}
	
	void newCol() {
		xy.x += elementRect.width + margin;
		xy.y = margin;
	}
} settings;

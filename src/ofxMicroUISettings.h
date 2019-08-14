/*
 Settings control the flow (distribution on xy) of elements, settings of elements, margins, etc.
 a pointer is added to each element so they all obey to the same settings.
 
 offset x, offset y (padding inside column)
 column rectangle
 slider dimensions.
 */

struct microUIEvent {
public:
	//element * e;
	microUIEvent() {}
};

struct microUISettings {
private:
	bool flowVert = true;
public:
	bool redrawUI = true;
	
	int margin = 15;
	glm::vec2 xy;
	float xBak = 0;
	int elementSpacing = 4;
	int elementPadding = 4;
	
	// this rectangle store the settings dimensions for new elements
	ofRectangle elementRect = ofRectangle(0,0,240,18);

	// this rectangle stores the last element size to flow the element coordinates
	ofRectangle flowRect;

	ofColor colorBg = 127;
	ofColor colorVal = 70;
	ofColor colorLabel = 255;
	
	microUISettings() {
		init();
	}
	
	void init() {
		flowVert = true;
		xy = glm::vec2(margin, margin);
	}
	// takes care of the flow of the elements.

	
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

	bool advanceLayout() {
		bool success = true;
		if (flowVert) {
			xy.y += flowRect.height + elementSpacing;
		} else {
			int newX = xy.x + flowRect.width + elementSpacing - xBak;
			if (newX > elementRect.width ) {
				success = false;
				xy.y += flowRect.height + elementSpacing;
				xy.x = xBak;
			} else {
				xy.x += flowRect.width + elementSpacing;
			}
		}
		return success;
	}
	
	void newLine() {
		xy.y += flowRect.height + elementSpacing;
	}
	
	void newCol() {
		xy.x += elementRect.width + margin;
		xy.y = margin;
	}
} settings;

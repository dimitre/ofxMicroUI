/*
 Settings control the flow (distribution on xy) of elements, settings of elements, margins, etc.
 a pointer is added to each lement so they all obey to the same settings.
 */
struct microUISettings {
public:
	microUISettings() {
		//cout << "create MicroUISettings" << endl;
	}
	
	~microUISettings() {
//        cout << "microUISettings destroy" << endl;
	}

	// this rectangle store the settings dimensions for new elements
	ofRectangle elementRect = ofRectangle(0,0,240,18);

	bool eventFromOsc = false;
	bool visible = true;
	float easing = 0.0;

	// UI STYLE
	float uiOpacity = 230;
	ofColor uiColorBg = ofColor(0,0,0,230);

	// create the own color object to be a pointer inside element.
	ofColor colorBg = 127; // element color if it is not rainbow
	ofColor colorVal = 70;
	ofColor colorLabel = 255;
	ofColor colorShadowLabel = ofColor(0, 120);

	ofColor alertColor = ofColor(255, 0, 50);
	ofColor alertColor2 = ofColor(80,0,255);
	ofColor alertColor3 = ofColor(0, 255, 50);

	bool useLabelRainbow = false;
	bool useBgRainbow = false;

	
	// STYLE
	int uiMargin = 10;
	int uiPadding = 10;
	int elementSpacing = 4;
	int elementPadding = 4;
	glm::vec2 offset = glm::vec2(0,0);
	
	
	ofTrueTypeFont font;
	bool useCustomFont = false;
	int labelPosBaseline = 3;

	
	bool presetIsLoading = false;
//	bool presetIsLoading = true;
	
	bool useFixedLabel = false;
	
	string styleLines = "";
	
	int presetCols = 4;
	int presetHeight = 2;
	

	ofColor getColorRainbow(const glm::vec2 & pos) {
		float hueStart = 120;
		float  h = pos.x / 9.0 + pos.y / 6.0 + hueStart;
		h = fmod(h, 255);
		return ofColor::fromHsb(h, 200, 200);
	}
	
	ofColor getColorLabel(const glm::vec2 & pos) {
		return useLabelRainbow ? getColorRainbow(pos) : colorLabel;
	}
	
	ofColor getColorBg(const glm::vec2 & pos) {
		return useBgRainbow ? getColorRainbow(pos) : colorBg;
	}
	
	void drawLabel(string & labelText, glm::vec2 & labelPos) {
		if (labelText != "") {
			ofSetColor(colorShadowLabel);			
			if (useCustomFont) {
				font.drawString(labelText, labelPos.x + 1, labelPos.y + 1);
			} else {
				ofDrawBitmapString(labelText, labelPos.x + 1, labelPos.y + 1);
			}

			ofSetColor(getColorLabel(labelPos));
			if (useCustomFont) {
				font.drawString(labelText, labelPos.x, labelPos.y);
			} else {
				ofDrawBitmapString(labelText, labelPos.x, labelPos.y);
			}
		}
	}
};


/*
 Settings control the flow (distribution on xy) of elements, settings of elements, margins, etc.
 a pointer is added to each lement so they all obey to the same settings.
 */
struct microUISettings {
public:
	microUISettings() {
		//cout << "create MicroUISettings" << endl;
	}
	
	// this rectangle store the settings dimensions for new elements
	ofRectangle elementRect = ofRectangle(0,0,240,18);
	
	float easing = 0.0;

	// UI STYLE
	float uiOpacity = 230;
	int uiMargin = 10;


	// STYLE
	int uiPadding = 10;
	int elementSpacing = 4;
	int elementPadding = 4;
	glm::vec2 offset = glm::vec2(0,0);
	
	bool useLabelRainbow = false;
	bool useBgRainbow = false;
	
	ofTrueTypeFont font;
	bool useCustomFont = false;
	int labelPosBaseline = 3;

	// create the own color object to be a pointer inside element.
	ofColor colorBg = 127;
	ofColor colorVal = 70;
	ofColor colorLabel = 255;
	ofColor colorUIBg = ofColor(0,200);
	ofColor alertColor = ofColor(255, 0, 50);
	ofColor alertColor2 = ofColor(0, 255, 50);
	
	bool presetIsLoading = false;
	bool useFixedLabel = false;
} settingsUI;

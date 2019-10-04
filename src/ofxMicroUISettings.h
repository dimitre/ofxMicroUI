/*
 Settings control the flow (distribution on xy) of elements, settings of elements, margins, etc.
 a pointer is added to each lement so they all obey to the same settings.
 */

struct microUISettings {
public:
	microUISettings() {
		cout << "create MicroUISettings" << endl;
	}
	
	// this rectangle store the settings dimensions for new elements
	ofRectangle elementRect = ofRectangle(0,0,240,18);

	// STYLE
	int margin = 10;
	int elementSpacing = 4;
	int elementPadding = 4;
	
	bool useLabelRainbow = false;
	bool useBgRainbow = false;
	
	bool useCustomFont = false;
	ofTrueTypeFont font;
	int labelPosBaseline = 3;

	// create the own color object to be a pointer inside element.
	ofColor colorBg = 127;
	ofColor colorVal = 70;
	ofColor colorLabel = 255;
	ofColor colorUIBg = ofColor(0,200);
	
} settingsUI;

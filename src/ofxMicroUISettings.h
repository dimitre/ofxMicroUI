/*
 Settings control the flow (distribution on xy) of elements, settings of elements, margins, etc.
 a pointer is added to each lement so they all obey to the same settings.
 
 offset x, offset y (padding inside column)
 column rectangle
 slider dimensions.
 */


struct microUISettings {
public:
	microUISettings() {}

	bool useCustomFont = false;
	ofTrueTypeFont font;

	// STYLE
	int margin = 15;
	int elementSpacing = 4;
	int elementPadding = 4;
	
	bool useLabelRainbow = false;
	bool useBgRainbow = false;
	
	// this rectangle store the settings dimensions for new elements
	ofRectangle elementRect = ofRectangle(0,0,240,18);

	// create the own color object to be a pointer inside element.
	ofColor colorBg = 127;
	ofColor colorVal = 70;
	ofColor colorLabel = 255;
	int labelPosBaseline = 3;
} settingsUI;




// forward declaration
class element;

// still wondering how to handle events here. any ideas?
// unused object right now
class microUIEventObject {
public:
	// not sure if it will help
	element * e = NULL;
	bool * b = NULL;
	string * s = NULL;
	int * i = NULL;
	float * f = NULL;
	glm::vec2 * v2 = NULL;
	glm::vec3 * v3 = NULL;
	string * name = NULL;
	
	//	microUIEventObject(string & n, float * fl) {
	//		name = &n;
	//		f = fl;
	//	}
	
	//	microUIEventObject(element & el) {
	//		e = &el;
	//	}
};

map <string, element *> 	elementsLookup;
map <string, slider *> 		slidersLookup;
map <string, toggle *> 		togglesLookup;
map <string, radio *> 		radiosLookup;
map <string, inspector *> 	inspectorsLookup;

element * getElement(const string & n) {
	return elementsLookup.find(n) != elementsLookup.end() ? elementsLookup[n] : NULL;
}

// and other kinds
slider * getSlider(const string & n) {
	return slidersLookup.find(n) != slidersLookup.end() ? slidersLookup[n] : NULL;
}

toggle * getToggle(const string & n) {
	return togglesLookup.find(n) != togglesLookup.end() ? togglesLookup[n] : NULL;
}

radio * getRadio(const string & n) {
	return radiosLookup.find(n) != radiosLookup.end() ? radiosLookup[n] : NULL;
}

inspector * getInspector(const string & n) {
	return inspectorsLookup.find(n) != inspectorsLookup.end() ? inspectorsLookup[n] : NULL;
}

bool updatedRect = false;
string loadedTextFile = "";
string createdLines = "";
// 16 02 2021 - testing to solve future ui element issues
bool uiIsCreated = false;

void reload() {
	cout << "ofxMicroUI Reload" << uiName << endl;
	clear();
	createFromText(loadedTextFile);
}

void updateRect();

void createFromLine(string l);
void createFromText(const string & fileName);
void createFromLines(const string & line);
void createFromLines(const vector<string> & lines, bool complete = true);

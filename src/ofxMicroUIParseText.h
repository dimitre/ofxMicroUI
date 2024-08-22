std::unordered_map <std::string, element *> 		elementsLookup;
std::unordered_map <std::string, slider *> 		slidersLookup;
std::unordered_map <std::string, toggle *> 		togglesLookup;
std::unordered_map <std::string, radio *> 		radiosLookup;
std::unordered_map <std::string, inspector *> 	inspectorsLookup;

element * getElement(const std::string & n) {
	return elementsLookup.find(n) != elementsLookup.end() ? elementsLookup[n] : nullptr;
}

// and other kinds
slider * getSlider(const std::string & n) {
	return slidersLookup.find(n) != slidersLookup.end() ? slidersLookup[n] : nullptr;
}

toggle * getToggle(const std::string & n) {
	return togglesLookup.find(n) != togglesLookup.end() ? togglesLookup[n] : nullptr;
}

radio * getRadio(const std::string & n) {
	return radiosLookup.find(n) != radiosLookup.end() ? radiosLookup[n] : nullptr;
}

inspector * getInspector(const std::string & n) {
	return inspectorsLookup.find(n) != inspectorsLookup.end() ? inspectorsLookup[n] : nullptr;
}

bool updatedRect = false;
of::filesystem::path loadedTextFile { "" };
std::string createdLines { "" };
// 16 02 2021 - testing to solve future ui element issues
bool uiIsCreated = false;

void reload() {
	std::cout << "ofxMicroUI Reload" << uiName << std::endl;
	clear();
	createFromText(loadedTextFile);
}

void updateRect();

void createFromLine(std::string l);
void createFromText(const of::filesystem::path & fileName);
void createFromLines(const std::string & line);
void createFromLines(const std::vector<std::string> & lines, bool complete = true);


//#include <any>

class ofxMicroUIShortcut {
public:
	ofxMicroUI * ui = nullptr;
	
	ofxMicroUIShortcut(const string & fileName, ofxMicroUI * _ui) : ui (_ui) {
		setup(fileName, ui);
	}
	ofxMicroUIShortcut() {};
	
	struct address {
		string ui;
		string element;
		int value;
//		std::any value;
	};

	map <char, address > shortcuts;

	void onKeyPressed(ofKeyEventArgs & data) {
		keyPressed(data.key);
	}

	void keyPressed(int key) {
		if ( shortcuts.find(key) != shortcuts.end() ) {
			ui->uis[shortcuts[key].ui].set(shortcuts[key].element, shortcuts[key].value);
		}
	}
	
	// needed?
	~ofxMicroUIShortcut() {
//		ofRemoveListener(ofEvents().keyPressed, this, &ofxMicroUISoftware::onKeyPressed);
	}

	void load(const string & fileName) {
		for (auto & l : ofxMicroUI::textToVector(fileName)) {
			if (l != "") {
				vector <string> cols = ofSplitString(l, "\t");
				char key = cols[0].c_str()[0];
				shortcuts[key] = { cols[1], cols[2], ofToInt(cols[4]) };
			}
		}
	}
	
	void setup(const string & fileName, ofxMicroUI * _ui) {
		ofAddListener(ofEvents().keyPressed, this, &ofxMicroUIShortcut::onKeyPressed);
		ui = _ui;
		load(fileName);
	}


};

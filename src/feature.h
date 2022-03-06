struct microFeature {
public:
	ofxMicroUISoftware * soft = NULL;
	string name = "";
	ofxMicroUI * ui = NULL;
	ofxMicroUI * ui2 = NULL;
	bool * use = NULL;
	
	void internalSetup() {
		if (ui != NULL) {
//            cout << "internalSetup :: " << ui->uiName << endl;
			ofAddListener(ui->uiEvent, this, &microFeature::uiEvents);
		}
		if (soft != NULL) {
			ofAddListener(soft->_ui->uiEventMaster, this, &microFeature::uiEventMaster);
		}
		if (name == "" && ui != NULL) {
			name = ui->uiName;
		}
	}
	
	
	//implicitly deleted
	microFeature() {}
	
	// criei 28 de outubro de 2021 somente pra acomodar o featurecolor
	microFeature(ofxMicroUI * _ui) : ui(_ui) {
//        cout << "microFeature init ui only :" << ui->uiName << endl;
		internalSetup();
	}
	
	// fiz agora para Cairo.
	microFeature(ofxMicroUISoftware * _soft, ofxMicroUI * _ui, bool * u = NULL) :
	soft(_soft), ui(_ui), use(u) {
//        cout << "microFeature init ui & bool :" << ui->uiName << endl;
		internalSetup();
	}

	microFeature(string n, ofxMicroUI * _ui, bool * u = NULL) : name(n), ui(_ui), use(u) {
		internalSetup();
	}
	
	microFeature(ofxMicroUISoftware * _soft) : soft(_soft) {
//        cout << "microFeature init soft only :" << endl;
		internalSetup();
	}
	
	microFeature(ofxMicroUISoftware * _soft, string n, bool * u = NULL) : soft(_soft), name(n), use(u) {
		internalSetup();
	}
	


//    microFeature(ofxMicroUISoftware * _soft, ofxMicroUI * _ui) : soft(_soft), ui(_ui) {
//        internalSetup();
//    }

	microFeature(ofxMicroUISoftware * _soft, ofxMicroUI * _ui, ofxMicroUI * _ui2) : soft(_soft), ui(_ui), ui2(_ui2) {
		internalSetup();
	}
	
	microFeature(ofxMicroUISoftware * _soft, string n, ofxMicroUI * _ui, ofxMicroUI * _ui2 = NULL) :
	soft(_soft), name(n), ui(_ui), ui2(_ui2) {
		internalSetup();
	}
	
	
	
	virtual void begin() {
		cout << "begin in primitive feature" << endl;
	};
	virtual void end() {
		cout << "end in primitive feature" << endl;
	};
	virtual void setup() {
		cout << "setup in primitive feature " << name << endl;
	};
	virtual void update() {
		cout << "update in primitive feature" << endl;
	};
	virtual void draw() {
		cout << "draw in primitive feature" << endl;
	};
	virtual void draw(int x, int y) {
		cout << "draw with parameters in primitive feature" << endl;
	};
	
	virtual void uiEvents(ofxMicroUI::element & e) {
//		cout << "uiEvents in primitive feature" << endl;
	}

	virtual void uiEventMaster(string & s) {
		if (s == "setup") {
			setup();
		}
	}

	virtual bool isOk() {
		return use == NULL || *use;
	}
	
	bool isSetup = false;
	virtual void checkSetup() {
		if (!isSetup) {
			setup();
			isSetup = true;
		}
	}
};



struct featurePolar : virtual public microFeature {
	using microFeature::microFeature;
	static float r2x (float a, float m) { return m * cos(ofDegToRad(a)); }
	static float r2y (float a, float m) { return m * sin(ofDegToRad(a)); }

	static float c2a (float x, float y) { return ofRadToDeg(atan2(y,x)); }
	static float c2m (float x, float y) { return sqrt(pow(x,2)+pow(y,2)); }
};


struct microFeatureBase : public virtual microFeature {
public:
	using microFeature::microFeature;
};

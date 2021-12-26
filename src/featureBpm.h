/*
Room for improvement - high resolution timer. maybe not needed.
https://github.com/memo/ofxMSATimer/blob/master/src/MSATimer.h
*/

#define USEBPM 1
#define USEAUDIOOUT 1

struct featureBpm : public microFeature {
public:
	using microFeature::microFeature;

	ofFbo * fbo = NULL;
	ofRectangle rectBeat;
	double seconds;
	double lastPercent;
	
	struct beat {
	public:
		double interval, seconds;
		double bpm;
		double start = 0;
		double percent, lastPercent;
		double lastBeatTime;
		double multiplier = 1.0;
		
		std::function<void()> invokeBeat = NULL;

		// mudar isso pra fora. deixar somente no feature mesmo.
		int beatsPerBar = 4;
		int beatCount = 0;

		double dmod(double x, double y) {
			return x - (int)(x/y) * y;
		}

		double getPercent() {
			return percent;
		}
		
		beat(float m = 1.0) : multiplier(m) {
			setBpm(120);
		}

		void update() {
			lastPercent = percent;
			double sec = seconds - start;
			percent = dmod(sec, interval) / interval;
		}
		
		void updateDivision() {
			interval = (double)60.0 / ((double) bpm);
			stringstream buffer;
			buffer << "Dmtr.org featureBpm" << endl;
			buffer << "update division : bpm is " << bpm << endl;
			buffer << "interval is : " << interval << endl;
			ofxMicroUI::messageBox(buffer.str());
		}

		void setBpm(double b) {
			bpm = b * multiplier;
			updateDivision();
		}
		
		void head() {
			start = seconds;
		}

		void setSeconds(double d) {
			seconds = d;
			update();

			if (isBeat()) {
				if (invokeBeat != NULL) {
					invokeBeat();
				}
			}
		}

		bool isBeat() {
			if (lastPercent > percent) {
				lastBeatTime = seconds;
				beatCount++;
			}
			return lastPercent > percent;
		}
	} bpm = beat(), bpm2 = beat(.5), bpm3 = beat(.25);
	
	
	void tap() {
		tapper.tap();
	}

	bool isBeat() {
		bool beat = lastPercent > bpm.percent;
		lastPercent = bpm.percent;
		return beat;
	}


	class bpmTapper {
	public:
		double seconds;
		double lastTap;
		vector <double> tapTimes;
		int nTaps = 0;
		double bpm;
		ofEvent<double> bpmChange;
		
		beat * _bpm = NULL;
		beat * _bpm2 = NULL;
		beat * _bpm3 = NULL;
		
		bpmTapper() {
		}
		
		void setSeconds(double d) {
			seconds = d;
		}
		
		void tap() {
			if (_bpm != NULL) {
				seconds = _bpm->seconds;
			}
			if (seconds > (lastTap+2.0)) {
				tapTimes.clear();
				nTaps = 0;
			}
			nTaps ++;
			tapTimes.push_back(seconds);
			lastTap = seconds;
			
			if (nTaps > 3) {
				double interval = (tapTimes[tapTimes.size()-1] - tapTimes[0])/((double)nTaps-1);
				bpm = 60.0 / interval;
				if (_bpm != NULL) {
					_bpm->setBpm(bpm);
					_bpm->head();
				}
				if (_bpm2 != NULL) {
					_bpm2->setBpm(bpm);
					_bpm2->head();
				}
				if (_bpm3 != NULL) {
					_bpm3->setBpm(bpm);
					_bpm3->head();
				}
				ofNotifyEvent(bpmChange, bpm);
			}
		}
	} tapper;
	
	
#ifdef USEAUDIOOUT
	ofSoundStream soundStream;
	ofSoundStreamSettings settings;
	void audioOut(ofSoundBuffer & buffer) {
		seconds = buffer.getTickCount() * buffer.getNumFrames() / double(buffer.getSampleRate());
		bpm.setSeconds(seconds);
		bpm2.setSeconds(seconds);
		bpm3.setSeconds(seconds);
		tapper.setSeconds(seconds);
	}
#endif	

	double getPercent() {
		return bpm.getPercent();
	}

	// mudar de nome
	float getShape() {
		// linear
		float r = bpm.getPercent();
		if (ui->pString["wave"] == "half") {
			r = bpm2.getPercent();
		}
		else if (ui->pString["wave"] == "quarter") {
			r = bpm3.getPercent();
		}
		else if (ui->pString["wave"] == "sin") {
			r = ofMap(sin(bpm.getPercent()*PI*2.0), -1, 1, 0, 1);
		}
		else if (ui->pString["wave"] == "hsin") {
			r = ofMap(sin(bpm2.getPercent()*PI*2.0), -1, 1, 0, 1);
		}
		else if (ui->pString["wave"] == "qsin") {
			r = ofMap(sin(bpm3.getPercent()*PI*2.0), -1, 1, 0, 1);
		}
		else if (ui->pString["wave"] == "invert") {
			r = 1.0 - getPercent();
		}
		return r;
	}

	void setup() override {
		cout << "featureBpm setup, uiName = " << ui->uiName << endl;
//		bpm2.multiplier = 0.5;
//		bpm3.multiplier = 0.25;

#ifdef USEAUDIOOUT
		// cout << "bpmSetup with audioOut :: " << ui->uiName << endl;
		settings.setOutListener(this);
		settings.sampleRate = 44100;
		settings.numOutputChannels = 1;
		settings.numInputChannels = 0;
		settings.bufferSize = 64; //32
		soundStream.setup(settings);
#else
		// cout << "bpmSetup without audioOut :: " << ui->uiName << endl;
#endif
		tapper._bpm = &bpm;
		tapper._bpm2 = &bpm;
		tapper._bpm3 = &bpm;
		ofAddListener(tapper.bpmChange, this, &featureBpm::bpmEvent);

		ofxMicroUI::fboElement * fboEl = ((ofxMicroUI::fboElement *)ui->getElement("fbo"));
		if (fboEl != NULL) {
			fboEl->alwaysRedraw = true;
			fbo = &fboEl->fbo;
			fbo->begin();
			ofClear(0,255);
			fbo->end();

			rectBeat.height = fbo->getHeight();
		} else {
			cout << "fboElement is NULL" << endl;
		}
	}

	void update() override {
#ifndef USEAUDIOOUT
		bpm.setSeconds(ofGetElapsedTimef());
#endif
		draw();
	}

	void bpmEvent(double & b) {
		ui->getSlider("bpm")->set(round(b));
	}

	void uiEvents(ofxMicroUI::element & e) override {

		if (e.name == "tap") {
			tapper.tap();
		}
		else if (e.name == "head") {
			bpm.head();
		}
		if (e.name == "bpm") {
			cout << "Interface BPM set " << *e.i << endl;
			bpm.setBpm(ui->pInt["bpm"]);
			bpm2.setBpm(ui->pInt["bpm"]);
			bpm3.setBpm(ui->pInt["bpm"]);
		}
//		else if (e.name == "divisions") {
//			int val = ofToInt(*e.s);
//			bpm.setDivisions(val);
//		}
//		else if (e.name == "beatsPerBar") {
//			int val = ofToInt(*e.s);
//			bpm.setBeatsPerBar(val);
//			rectBeat.width = fbo->getWidth()*(1/(float)val);
//		}
	}

	void draw() override {
//		if (u->_settings->visible)
		{
			fbo->begin();
			ofSetColor(0, 22.0); // trails = 2
			ofSetColor(0, ui->pFloat["trails"]);
			ofFill();
			ofDrawRectangle(0,0,fbo->getWidth(), fbo->getHeight());

			//xaxa
//			ofSetColor(ui->_settings->alertColor);
//			rectBeat.width = fbo->getWidth() * (1/(float)bpm.beatsPerBar);
//			rectBeat.x = rectBeat.width * bpm.getBeatCount();
//			ofDrawRectangle(rectBeat);
			
			ofSetColor(ui->_settings->alertColor2);
			float w = getShape() * fbo->getWidth();
			ofDrawRectangle(w, 0, 12, fbo->getHeight());

			string info = ofToString(bpm.bpm);
			glm::vec2 pos = glm::vec2(10, 14);
			ui->_settings->drawLabel(info, pos);
			fbo->end();
		}
	}

};

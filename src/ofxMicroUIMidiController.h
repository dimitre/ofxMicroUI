/*
ofxMicroUIMidiController
 Created by Dimitre Lima on 09/10/2019.
 Basically ported to ofxDmtrUI3MidiController.
 
 //http://community.akaipro.com/akai_professional/topics/midi-information-for-apc-mini
 //127 = verde

*/

#pragma once
#include "ofMain.h"
#include "ofEvents.h"
#include "ofxMidi.h"

class ofxMicroUIMidiController : public ofBaseApp, public ofxMidiListener {
public:
	
//    void offbytes() {
//        for (int a=120; a<=127; a++) {
//            vector <unsigned char> bytes = { 0xB1, (unsigned char)a, 0x00 };
//            cout << int(bytes[0]) << endl;
//            cout << int(bytes[1]) << endl;
//            cout << int(bytes[2]) << endl;
//            cout << "--" << endl;
//            midiControllerOut.sendMidiBytes(bytes);
//        }
//    }
	/*
	 All Notes Off. When an All Notes Off is received, all oscillators will turn off.
	 c = 123, v = 0: All Notes Off (See text for description of actual mode commands.)
	 c = 124, v = 0: Omni Mode Off
	 c = 125, v = 0: Omni Mode On
	 c = 126, v = M: Mono Mode On (Poly Off) where M is the number of channels (Omni Off) or 0 (Omni On)
	 c = 127, v = 0: Poly Mode On (Mono Off) (Note: These four messages also cause All Notes Off)
	 */

	ofxMicroUIMidiController() {}
	ofxMicroUIMidiController(ofxMicroUISoftware * _soft, string device) {
		set(device);
		setUI(*_soft->_ui);
		ofAddListener(_u->uiEventMaster, this, &ofxMicroUIMidiController::uiEventMaster);
	}
	
	void uiEventMaster(string & s) {
//        cout << "ofxMicroUIMidiController uiEventMaster :: " << s << endl;
		if (s == "setup") {
			frameAction = ofGetFrameNum();
			for (auto & e : _u->elements) {
				checkElement(*e);
			}
			for (auto & u : _u->uis) {
				for (auto & e : u.second.elements) {
					checkElement(*e);
				}
			}
		}
	}

	
	unsigned int vals[64] = { 0 };

	unsigned int apcMiniLeds[64] = {
		56, 57, 58, 59, 60, 61, 62, 63,
		48, 49, 50, 51, 52, 53, 54, 55,
		40, 41, 42, 43, 44, 45, 46, 47,
		32, 33, 34, 35, 36, 37, 38, 39,
		24, 25, 26, 27, 28, 29, 30, 31,
		16, 17, 18, 19, 20, 21, 22, 23,
		8,  9,  10, 11, 12, 13, 14, 15,
		0,  1,  2,  3,  4,  5,  6,  7
	};
	
	/* solved here
	 I had an issue when sending note off to 98 (shift key) before others, so midi was stuck
	 */
	
	vector <unsigned int> lateralLeds = {
		71, 70, 69, 68, 67, 66, 65, 64, //baixo
		82, 83, 84, 85, 86, 87, 88, 89, //lateral
		98, //quadrado?
	};
	
//    vector <unsigned int> lateralLeds = {
//        71, 70, 69, 68, 67, 66, 65, 64
//    };
						 

//    unsigned int colors[4] = { 0,1,3,5 };
	unsigned int colors[3] = { 1,3,5 };
	
	ofFbo fbo;
	ofPixels pixels;
	
	void displayFromVals() {
		for (int i=0; i<64; i++) {
//            sendNote(1, apcMiniLeds[i], vals[i]); // 1 green 3 red 5 yellow
			midiControllerOut.sendNoteOn(1, apcMiniLeds[i], vals[i]);
		}
	}
	
	void lateralRandom() {
		for (auto & l  : lateralLeds) {
			sendNote(1, l, ofRandom(0,6)); // 1 green 3 red 5 yellow
		}
	}
	

	
	
	void centerClear() {
		for (auto & l  : apcMiniLeds) {
			sendNote(1, l, 0); // 1 green 3 red 5 yellow
		}
	}
	
	void lateralClear() {
		for (auto & l  : lateralLeds) {
			sendNote(1, l, 0); // 1 green 3 red 5 yellow
		}
	}
	
	void blackout() {
		for (auto & l  : apcMiniLeds) {
			sendNote(1, l, 0); // 1 green 3 red 5 yellow
		}
		for (auto & l  : lateralLeds) {
			sendNote(1, l, 0); // 1 green 3 red 5 yellow
		}
	}
	
	void allRandom() {
		int c=0;
		for (auto & l  : apcMiniLeds) {
			sendNote(1, l, c%6 + 1); // 1 green 3 red 5 yellow
			c++;
		}
		for (auto & l  : lateralLeds) {
			sendNote(1, l, c%2 + 1); // 1 green 3 red 5 yellow
			c++;
		}
	}
	
	map <int, map<int, int> > sentMidi;
	void sendNote(int c, int p, int v) {
//        cout << "sendNote :: " << c << " : " << p << " : " << v << endl;
		sentMidi[c][p] = v;
		midiControllerOut.sendNoteOn(c, p, v);
	}
	
	void restoreLights() {
		for (auto & x : sentMidi) {
			for (auto y : x.second) {
				sendNote(x.first, y.first, y.second);
			}
		}
	}
	
	
	// example
	//    midiController.fbo.begin();
	//    fbo->draw(-fbo->getWidth()*.5, -fbo->getHeight()*.5);
	//    midiController.fbo.end();
	//    midiController.display();
	
	void display() {
		if (!fbo.isAllocated()) {
			fbo.allocate(8, 8, GL_RGB);
			pixels.allocate(8, 8, GL_RGB);
			fbo.begin();
			ofClear(0,255);
			fbo.end();
		} else {
			fbo.getTexture().readToPixels(pixels);
			for (int i=0; i<64; i++) {
				float luma = pixels.getData()[i*3] / 64.0;
				int indexColor = colors[int(luma)];
				int pitch = apcMiniLeds[i];
				sendNote(1, pitch, indexColor); // 1 green 3 red 5 yellow
			}
		}
	}

	
	// todo: some kind of listener able to connect if device is not found at first.
	struct elementListMidiController {
	public:
		string ui;
		string tipo;
		string nome;
		string valor = "";
		int channel;
		int pitch;
	};

	bool connected = false;

	elementListMidiController elementLearn;
	string folder = "";

	ofxMidiIn 	midiControllerIn;
	ofxMidiOut	midiControllerOut;
	ofxMidiMessage midiMessage;
	bool midiKeys[4000];

	//map <int, map <int, map<int, map<int, elementListMidiController> > > > mapaMidiController;

	map <string, elementListMidiController> midiControllerMap;
	vector <elementListMidiController *> elements;

	string lastString;

	map <string,string>			pString;

	// somente para apagar leds nos presets, nada mais
	int lastPresetChannel;
	int lastPresetPitch;

	ofFbo * fboMC = NULL;
	
	int holdPresetNumber = 0;

	ofxMicroUI * _u = NULL;
	void setUI(ofxMicroUI &u) {
		_u = &u;
		ofAddListener(_u->uiEvent,this, &ofxMicroUIMidiController::uiEvent);
		for (auto & uis : _u->uis) {
			ofAddListener(uis.second.uiEvent,this, &ofxMicroUIMidiController::uiEvent);
		}


		
//	this is ios only
//		cout << "ofxMidi::setConnectionListener(this);" << endl;
//		ofxMidi::setConnectionListener(this);
	}

	
//	//--------------------------------------------------------------
//	void midiInputAdded(string name, bool isNetwork) override {
//		cout << "input added" << endl;
//		cout << name << endl;
//	//	stringstream msg;
//	//	msg << "ofxMidi: input added: " << name << " network: " << isNetwork;
//	}
//
//	//--------------------------------------------------------------
//	void midiInputRemoved(string name, bool isNetwork) override {
//		cout << "input removed" << endl;
//		cout << name << endl;
//	//	stringstream msg;
//	//	msg << "ofxMidi: input removed: " << name << " network: " << isNetwork << endl;
//	}


	uint64_t frameAction = 0;
		//--------------------------------------------------------------
	void newMidiMessage(ofxMidiMessage& msg) {
//        cout << "newMidiMessage " << endl;
		frameAction = ofGetFrameNum();
//        cout << frameAction << endl;
//        bool debug = true;
		bool debug = false;
		if (debug) {
			cout << "channel:" +ofToString(msg.channel) << endl;
			cout << "pitch:"   +ofToString(msg.pitch) << endl;
			cout << "control:" +ofToString(msg.control) << endl;
			cout << "status:"  +ofToString(msg.status) << endl;
			cout << "-------" << endl;
		}

		string index = ofToString(msg.channel) + " " + ofToString(msg.pitch) + " " + ofToString(msg.control);
		string index2 = index + " " + ofToString(msg.status);

		if ( midiControllerMap.find(index) != midiControllerMap.end()) {
			// action
			elementListMidiController *te = &midiControllerMap[index];
			ofxMicroUI * _ui;
			_ui = te->ui == "master" ? _u : _ui = &_u->uis[te->ui];
			
			// aqui apenas os controles que somente acontecem no note on
			if (msg.status == 144) {
				if (te->tipo == "bool") {
					if (_ui->getToggle(te->nome) != NULL) {
						_ui->getToggle(te->nome)->flip();
						if (_ui->pBool[te->nome]) {
							sendNote(msg.channel, msg.pitch, 3);
						} else {
							sendNote(msg.channel, msg.pitch, 0);
						}
					}
				}
				
				else if (te->tipo == "preset") {
					sendNote(lastPresetChannel, lastPresetPitch, 0); // 1 green 3 red
					sendNote(msg.channel, msg.pitch, 1); // 1 green 3 red 5 yellow
					_u->willChangePreset = te->nome;
					lastPresetChannel = msg.channel;
					lastPresetPitch = msg.pitch;
				}
				
				else if (te->tipo == "bang") {
					cout << "BANG! " << te->nome << endl;
					ofxMicroUI::booleano * e = _ui->getToggle(te->nome);
					if (e != NULL) {
						e->set(true);
					}
//					ofxMicroUI::bang * e = ((ofxMicroUI::bang*)_ui->getElement(te->nome));
//					if (e != NULL) {
//						e->bang();
////						e->set(true);
//					}
				}
			}

			if (te->tipo == "radio") {
				if (te->valor == "") {
					ofxMicroUI::radio * r = _ui->getRadio(te->nome);
					int nElements = r->elements.size();
					int valor = ofMap(msg.value, 0, 127, 0, nElements);
					r->set(valor);
				}
				else {
					//_ui->futureCommands.push_back(future(te->ui, te->nome, "radioSet", te->valor));
				}
			}
			
			else if (te->tipo == "float" || te->tipo == "int") {
				ofxMicroUI::slider * s = (ofxMicroUI::slider*)_ui->getElement(te->nome);
				if (s != NULL) {
					float valor = ofMap(msg.value, 0, 127, s->min, s->max);
					s->set(valor);
				} else {
					cout << te->nome << endl;
					cout << "NULL" << endl;
				}
			}

			else if (te->tipo == "hold") {
				ofxMicroUI::hold * e = (ofxMicroUI::hold*)_ui->getElement(te->nome);
				if (e != NULL) {
					e->set(msg.status == 144);
					sendNote(msg.channel, msg.pitch, msg.status == 144 ? 1 : 0);
				}
			}

			
			else if (te->tipo == "savePresetNumber") {
				if (_u != NULL) {
					cout << te->tipo << endl;
	//				holdPresetNumber = _u->getPresetNumber();
				}
			}
			
			else if (te->tipo == "restorePresetNumber") {
				if (_u != NULL) {
					cout << te->tipo << endl;
	//				_u->futureCommands.push_back(future("master", "presets", "loadAllPresets", holdPresetNumber));
				}
			}

			// REMOVER?
			else if (te->tipo == "presetHold") {
				if (_u != NULL) {
					sendNote(lastPresetChannel, lastPresetPitch, 0); // 1 green 3 red 5 yellow
	//				_u->futureCommands.push_back(future("master", "presets", "loadPresetHold", ofToInt(te->nome)));
	//				_u->nextPreset.push_back(ofToInt(te->nome));
					sendNote(msg.channel, msg.pitch, 3); // 1 green 3 red 5 yellow
					lastPresetChannel = msg.channel;
					lastPresetPitch = msg.pitch;
					//http://community.akaipro.com/akai_professional/topics/midi-information-for-apc-mini
					//127 = verde
				}
			}
			
			// REMOVER?
			else if (te->tipo == "presetRelease") {
				if (_u != NULL) {
					// TODO XAXA
					// 1 green 3 red 5 yellow
					sendNote(lastPresetChannel, lastPresetPitch, 0);
					
	//				_u->futureCommands.push_back(future("master", "presets", "loadPresetRelease", ofToInt(te->nome)));
					//_u->nextPreset.push_back(ofToInt(te->nome));
					sendNote(msg.channel, msg.pitch, 3); // 1 green 3 red 5 yellow
					lastPresetChannel = msg.channel;
					lastPresetPitch = msg.pitch;
				}
			}
		} else {
			// discard note off ?
			if (msg.status != 128) {
				cout << index << "\t\t" << index2 << endl;
			}
		}
		midiKeys[msg.pitch] = msg.status == 144;
		midiMessage = msg;
	}


	void set(string midiDevice) {
		connected = midiControllerIn.openPort(midiDevice);
		cout << "ofxMicroUIMidiController setup :: " + midiDevice + " :: ";
		cout << (connected ? "connected" : "not found") << endl;
		if (connected) {
			midiControllerOut.openPort(midiDevice); // by number
			midiControllerIn.ignoreTypes(false, false, false);
		//	ofxMidi::setConnectionListener(this);
			midiControllerIn.addListener(this);

			string filename = folder + midiDevice + ".txt";

			if (ofFile::doesFileExist(filename) && midiControllerIn.isOpen()) {
				for (auto & m : ofxMicroUI::textToVector(filename)) {
					if (m != "" && m.substr(0,1) != "#") {
						elementListMidiController te;
						vector <string> cols = ofSplitString(m, "\t");
						te.ui 	= cols[1];
						te.tipo = cols[2];
						te.nome = cols[3];
						if (cols.size() > 4) {
							te.valor = cols[4];
						}
						vector <string> vals = ofSplitString(cols[0], " ");
						int channel = ofToInt(vals[0]);
						int pitch 	= ofToInt(vals[1]);
//						int control = ofToInt(vals[2]);
						int status = 0;
						if (vals.size() > 3) {
							status     = ofToInt(vals[3]);
						}

						te.channel = channel;
						te.pitch = pitch;

						// isso aqui da igual a index = cols[0]
//						string index = ofToString(status) + " " + ofToString(channel) + " " +  ofToString(pitch) + " " +  ofToString(control);
						string index = cols[0];


						midiControllerMap[index] = te;
						elements.push_back(&midiControllerMap[index]);
					}
				}
			}

			ofAddListener(ofEvents().exit, this, &ofxMicroUIMidiController::onExit);
		}
	}


	
	
	void uiEventMidi(vector<string> & strings) {
		elementLearn.nome = strings[0];
		elementLearn.ui = strings[1];
		elementLearn.tipo = "float";
	}


	void onExit(ofEventArgs &data) {
		blackout();
		
		midiControllerOut.closePort();
		midiControllerIn.closePort();
	}
	
	void checkElement(ofxMicroUI::element & e) {
//        cout << "checkElement :: " << e.name << endl;
		if (e.name == "presets" && e._ui->uiName == "master") {
			for (auto & m : midiControllerMap) {
				if (m.second.nome == e._ui->pString["presets"]) {
					if (lastPresetChannel != 0 || lastPresetPitch != 0) {
						sendNote(lastPresetChannel, lastPresetPitch, 0); // 1 green 3 red
					}
					sendNote(m.second.channel, m.second.pitch, 1); // 1 green 3 red
					lastPresetChannel = m.second.channel;
					lastPresetPitch = m.second.pitch;
				}
			}
		}
		
		for (auto & m : midiControllerMap) {
			if (m.second.nome == e.name && m.second.ui == e._ui->uiName) {
				if (m.second.tipo == "bool") {
					if (e._ui->pBool[e.name]) { // *e.b
						sendNote(m.second.channel, m.second.pitch, 5);
					} else {
						sendNote(m.second.channel, m.second.pitch, 0);
					}
				}
			}
		}
	}
	
	void uiEvent(ofxMicroUI::element & e) {
		if (frameAction != ofGetFrameNum()) {
			checkElement(e);
		}
	}
};

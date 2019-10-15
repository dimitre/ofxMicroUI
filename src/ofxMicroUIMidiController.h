/*
ofxMicroUIMidiController
 Created by Dimitre Lima on 09/10/2019.
 Basically ported to ofxDmtrUI3MidiController.
*/

#pragma once
#include "ofMain.h"
#include "ofEvents.h"
#include "ofxMidi.h"

class ofxMicroUIMidiController : public ofBaseApp, public ofxMidiListener {
public:
	
	// todo: some kind of listener able to connect if device is not found at first.
	
	bool connected = false;
	int apcMiniLeds[64] = {
		56, 57, 58, 59, 60, 61, 62, 63,
		48, 49, 50, 51, 52, 53, 54, 55,
		40, 41, 42, 43, 44, 45, 46, 47,
		32, 33, 34, 35, 36, 37, 38, 39,
		24, 25, 26, 27, 28, 29, 30, 31,
		16, 17, 18, 19, 20, 21, 22, 23,
		8,  9,  10, 11, 12, 13, 14, 15,
		0,  1,  2,  3,  4,  5,  6,  7
	};
	
	struct elementListMidiController {
	public:
		string ui;
		string tipo;
		string nome;
		string valor = "";
		int channel;
		int pitch;
	};

	elementListMidiController elementLearn;
	int testeIndex[4] = { 0,1,3,5 };
	string folder = "";

	ofxMidiIn 	midiControllerIn;
	ofxMidiOut	midiControllerOut;
	ofxMidiMessage midiMessage;
	bool midiKeys[4000];

	//map <int, map <int, map<int, map<int, elementListMidiController> > > > mapaMidiController;

	map <string, elementListMidiController> midiControllerMap;
	vector <elementListMidiController *> elements;

	string lastString;

	void teste();

	ofFbo testeFbo;
	ofPixels fboTrailsPixels;

	map <string,string>			pString;

	// somente para apagar leds nos presets, nada mais
	int lastPresetChannel;
	int lastPresetPitch;

	ofFbo * fboMC = NULL;
	
	int holdPresetNumber = 0;

	ofxMicroUI * _u = NULL;
	void setUI(ofxMicroUI &u) {
	    _u = &u;
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



		//--------------------------------------------------------------
	void newMidiMessage(ofxMidiMessage& msg) {
	//	cout << "status:"  +ofToString(msg.status) << endl;
	//	cout << "pitch:"   +ofToString(msg.pitch) << endl;
	//	cout << "channel:" +ofToString(msg.channel) << endl;
	//	cout << "control:" +ofToString(msg.control) << endl;
	//	cout << "-------" << endl;

		string index = ofToString(msg.status) + " " + ofToString(msg.channel) + " " + ofToString(msg.pitch) + " " + ofToString(msg.control);

		if ( midiControllerMap.find(index) != midiControllerMap.end() ) {
			// action
			elementListMidiController *te = &midiControllerMap[index];
			ofxMicroUI * _ui;
			if (te->ui == "master") {
				_ui = _u;
			} else {
				_ui = &_u->uis[te->ui];
			}

			if (te->tipo == "radio") {
				if (te->valor == "") {
					ofxMicroUI::radio * r = _ui->getRadio(te->nome);
					int nElements = r->elements.size();
					int valor = ofMap(msg.value, 0, 127, 0, nElements);
					r->set(valor);
					//_ui->futureCommands.push_back(future(te->ui, te->nome, "radioSetFromIndex", valor));
				}
				else {
					//_ui->futureCommands.push_back(future(te->ui, te->nome, "radioSet", te->valor));
				}
			}
			
			else if (te->tipo == "float" || te->tipo == "int") {
				ofxMicroUI::slider * s = (ofxMicroUI::slider*)_ui->getElement(te->nome);
				if (s != NULL) {
					float valor = ofMap(msg.value, 0, 127, s->min, s->max);
	//				cout << te->nome << endl;
	//				cout << valor << endl;
					s->set(valor);
				} else {
					cout << te->nome << endl;
					cout << "NULL" << endl;
				}
			}

	//		else if (te->tipo == "bang") {
	//			//cout << "YES BANG" << endl;
	//			bang * e = (ofxMicroUI::bang*)_ui->getElement(te->nome);
	//			if (e != NULL) {
	//				e->plau();
	//			}
	//		}
			else if (te->tipo == "boolon") {
				_ui->getElement(te->nome)->set(true);
				midiControllerOut.sendNoteOn(msg.channel, msg.pitch);
			}
			else if (te->tipo == "booloff") {
				_ui->getElement(te->nome)->set(false);
				midiControllerOut.sendNoteOff(msg.channel, msg.pitch);
			}
			else if (te->tipo == "bool") {
				_ui->getElement(te->nome)->set(!_ui->pBool[te->nome]);
				if (_ui->pBool[te->nome]) {
					midiControllerOut.sendNoteOn(msg.channel, msg.pitch);
				} else {
					midiControllerOut.sendNoteOff(msg.channel, msg.pitch);
				}
			}

			else if (te->tipo == "preset") {
				if (_u != NULL) {
					midiControllerOut.sendNoteOn(lastPresetChannel, lastPresetPitch, 0); // 1 green 3 red 5 yellow
					//_u->futureCommands.push_back(future("master", "presets", "loadAllPresets", ofToInt(te->nome)));
					//_u->nextPreset.push_back(ofToInt(te->nome));
					
					midiControllerOut.sendNoteOn(msg.channel, msg.pitch, 3); // 1 green 3 red 5 yellow
					_u->presetElement->set(te->nome);

					lastPresetChannel = msg.channel;
					lastPresetPitch = msg.pitch;
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
					midiControllerOut.sendNoteOn(lastPresetChannel, lastPresetPitch, 0); // 1 green 3 red 5 yellow
	//				_u->futureCommands.push_back(future("master", "presets", "loadPresetHold", ofToInt(te->nome)));
	//				_u->nextPreset.push_back(ofToInt(te->nome));
					midiControllerOut.sendNoteOn(msg.channel, msg.pitch, 3); // 1 green 3 red 5 yellow
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
					midiControllerOut.sendNoteOn(lastPresetChannel, lastPresetPitch, 0); // 1 green 3 red 5 yellow
	//				_u->futureCommands.push_back(future("master", "presets", "loadPresetRelease", ofToInt(te->nome)));
					//_u->nextPreset.push_back(ofToInt(te->nome));
					midiControllerOut.sendNoteOn(msg.channel, msg.pitch, 3); // 1 green 3 red 5 yellow
					lastPresetChannel = msg.channel;
					lastPresetPitch = msg.pitch;
					
					//http://community.akaipro.com/akai_professional/topics/midi-information-for-apc-mini
					//127 = verde
					
				}
			}
		} else {
			// discard note off ?
			if (msg.status != 128) {
				cout << index << endl;
			}
		}
		midiKeys[msg.pitch] = msg.status == 144;
		midiMessage = msg;
	}



	//--------------------------------------------------------------
	void set(string midiDevice) {
		cout << "ofxDmtrUIMidiController setup ::: " + midiDevice << endl;
		connected = midiControllerIn.openPort(midiDevice);
		cout << (connected ? "connected" : "not found") << endl;
		if (connected) {
			midiControllerOut.openPort(midiDevice); // by number
			midiControllerIn.ignoreTypes(false, false, false);
		//	ofxMidi::setConnectionListener(this);
			midiControllerIn.addListener(this);

			string filename = folder + midiDevice + ".txt";

			if (ofFile::doesFileExist(filename) && midiControllerIn.isOpen()) {
				for (auto & m : _u->textToVector(filename)) {
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
						int status 	= ofToInt(vals[0]);
						int channel = ofToInt(vals[1]);
						int pitch 	= ofToInt(vals[2]);
						int control = ofToInt(vals[3]);

						te.channel = channel;
						te.pitch = pitch;

						// isso aqui da igual a index = cols[0]
						string index = ofToString(status) + " " + ofToString(channel) + " " +  ofToString(pitch) + " " +  ofToString(control);


						midiControllerMap[index] = te;
						elements.push_back(&midiControllerMap[index]);
					}
				}
			}

			ofAddListener(ofEvents().exit, this, &ofxMicroUIMidiController::onExit);
		}
		//ofAddListener(_u->uiEvent,this, &ofxMicroUIMidiController::uiEvent);
	}
		
	// END SETUP


	//--------------------------------------------------------------
	void uiEventMidi(vector<string> & strings) {
		elementLearn.nome = strings[0];
		elementLearn.ui = strings[1];
		elementLearn.tipo = "float";
	}


	void onExit(ofEventArgs &data) {  
		midiControllerOut.closePort();
		midiControllerIn.closePort();
	}
	
	void uiEvent(ofxMicroUI::element & e) {
		
	}
};

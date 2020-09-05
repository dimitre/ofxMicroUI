/* 

placeholder 

*/


#pragma once

#if defined( TARGET_OF_IPHONE ) || defined( TARGET_OF_IOS ) || defined( TARGET_ANDROID )
#define MICROUI_TARGET_TOUCH
#endif

#include "ofEvents.h"
#include "ofxMicroUI.h"
#include "ofxOsc.h"

#ifdef MICROUI_TARGET_TOUCH
#include "ofxAccelerometer.h"
#endif

class ofxMicroUIRemote : public ofBaseApp {
public:
	
	ofxMicroUI u;
	ofxMicroUI * _ui = &u;
	ofxMicroUI::inspector * oscInfo = NULL;
	ofxMicroUI::inspector * oscInfoReceive = NULL;

	ofxOscSender 	send;
	ofxOscReceiver 	receive;

	string 	serverAddress = "";
	int 	serverPort = 8000;
	string 	remoteAddress = "";
	int 	remotePort = 9000;
	
	ofxOscBundle bundle;
	
	ofEvent<string> eventMessage;
	map <string, ofxMicroUI *> _nameUIs;
	
	map <string, std::function<void()>> msgFunction;
	
	string reservedAddress = "/reservedAddress/";

	
	ofxMicroUIRemote() {
		ofAddListener(ofEvents().draw, this, &ofxMicroUIRemote::onDraw);
		ofAddListener(ofEvents().update, this, &ofxMicroUIRemote::onUpdate);
	}
	
	~ofxMicroUIRemote() {}
	
	//--------------------------------------------------------------
	map <string, string> loadConfigPairs(string file) {
		map <string, string> configs;
		for (auto & c : ofxMicroUI::textToVector(file)) {
			if (c.substr(0,1) != "#" && c != "") {
				vector <string> cols = ofSplitString(c, "\t");
				if (cols.size() > 1) {
					configs[cols[0]] = cols[1];
				}
			}
		}
		return configs;
	}

	//--------------------------------------------------------------
	void loadConfig(string file) {
		map <string, string> configs = loadConfigPairs(file);
		remotePort = ofToInt(configs["remotePort"]);
		serverPort = ofToInt(configs["serverPort"]);
		remoteAddress = configs["remoteAddress"];
		serverAddress = configs["serverAddress"];
		
	}
	
	void setupServer() {
		receive.setup(serverPort);
		bool serverIsSetup;
		
		try {
			serverIsSetup = send.setup(remoteAddress, remotePort);
		} catch (exception){
			cout << "ofxMicroUIRemote :: &&& no internet &&&" << endl;
		}
		if (serverIsSetup) {
			string message = "ofxMicroUIRemote server \r";
//			message += "ui = " + _ui->UINAME + "\r";
			message += "server = " + serverAddress	 + ":" + ofToString(serverPort) + "\r";
			message += "remote = " + remoteAddress + ":" + ofToString(remotePort) ;
			ofxMicroUI::messageBox(message);
		}
	}
	
	void setupRemote() {
		try {
			receive.setup(remotePort);
			string message = "ofxMicroUIRemote REMOTE \r";
			message += "Port = " + ofToString(remotePort);
			ofxMicroUI::messageBox(message);
		} catch (exception){
			cout << "ofxDmtrUI3Remote :: &&& no internet &&&" << endl;
		}
	}
	
	//--------------------------------------------------------------
	void onDraw(ofEventArgs &data) { draw(); }

	//--------------------------------------------------------------
	void onUpdate(ofEventArgs &data) {
		if (bundle.getMessageCount()) {
			send.sendBundle(bundle);
			bundle.clear();
		}
		
		update();
		while(receive.hasWaitingMessages()){
			ofxOscMessage m;
			receive.getNextMessage(m);
			cout << "receiving message :: " + m.getAddress() << endl;
			string msg = m.getAddress();
			ofNotifyEvent(eventMessage, msg);
			
			
			if (oscInfoReceive != NULL) {
				oscInfoReceive->set(m.getAddress());
			}

			vector <string> addr = ofSplitString(m.getAddress(), "/");
//			cout << addr.size() << endl;
			
			if (m.getAddress() == "/reservedAddress/createFromText") {
				cout << "YOO" << endl;
				cout << _ui->uiName << endl;
				ofBuffer blob = m.getArgAsBlob(0);
				_ui->clear();
				
				_ui->initFlow();
					//alert("createFromText " + fileName);
				if (!_ui->hasListeners) {
					_ui->addListeners();
				}
				
				vector <string> lines;
				for(auto & line: blob.getLines()) {
					lines.push_back(line);
				}
//				_ui->createFromLines(lines);
				string allLines = ofJoinString(lines, "\r");
				_ui->createFromLines(allLines);
				
				cout << ofJoinString(lines, "\r") << endl;
					
				string s = "createFromText";
				ofNotifyEvent(_ui->uiEventMaster, s);

				// NOVIDADE 14 jul 2020
				_ui->redrawUI = true;
				
				cout << "elements size :: " << endl;
				cout << _ui->elements.size() << endl;
				
				
//					_ui->autoFit();
			}
			
			if (addr.size() >= 3) {
				cout << addr.size() << endl;
				string uiName = addr[1];
				string name = addr[2];
				// prova de conceito mas eventualmente nao vai funcionar ainda por causa do propagateevent. refazer isso logo em breve
				
				if ( _nameUIs.find(uiName) != _nameUIs.end() ) {
					
					
					ofxOscArgType k = m.getArgType(0);
					_nameUIs[uiName]->_settings->eventFromOsc = true;
					
					if (k == OFXOSC_TYPE_FLOAT) {
						cout << "FLOAT" << endl;
						_nameUIs[uiName]->set(name, (float) m.getArgAsFloat(0));
					}
					else if (k == OFXOSC_TYPE_INT32 || k == OFXOSC_TYPE_INT64) {
						cout << "INT" << endl;
						_nameUIs[uiName]->set(name, (int) m.getArgAsInt(0));
					}
					else if (k == OFXOSC_TYPE_FALSE) {
						cout << "BOOL FALSE" << endl;
						_nameUIs[uiName]->set(name, (bool) false);
					}
					else if (k == OFXOSC_TYPE_TRUE) {
						cout << "BOOL TRUE" << endl;
						_nameUIs[uiName]->set(name, (bool) true);
					}
					else if (k == OFXOSC_TYPE_STRING) {
						cout << "STRING" << endl;
						_nameUIs[uiName]->set(name, m.getArgAsString(0));
					}
					
					_nameUIs[uiName]->_settings->eventFromOsc = false;
				}
			}
		}
	}
	
	//--------------------------------------------------------------
	void uiEvent(ofxMicroUI::element & e) {
		if (e._settings->eventFromOsc) {
			cout << "EVENT FROM OSC" << endl;
		}
		if (!e._settings->eventFromOsc) {
			string address = "/" + e._ui->uiName + "/" + e.name;
			
			// transformar em bundle aqui
	//		cout << "MSG " << address << endl;
			
			if (oscInfo != NULL) {
				if (!e._settings->presetIsLoading) {
					oscInfo->set(address);
				}
			}
			ofxOscMessage m;
			m.setAddress(address);

			if (ofxMicroUI::slider * els = dynamic_cast<ofxMicroUI::slider*>(&e)) {
				if (els->isInt) {
					m.addIntArg(*e.i);
				} else {
					m.addFloatArg(*e.f);
				}
			}
			
			else if (dynamic_cast<ofxMicroUI::toggle*>(&e)) {
	//			cout << "toggle " << e.name << endl;
				m.addBoolArg(*e.b);
			}

			else if (dynamic_cast<ofxMicroUI::radio*>(&e)) {
				m.addStringArg(*e.s);
			}
			
			else if (dynamic_cast<ofxMicroUI::inspector*>(&e) || dynamic_cast<ofxMicroUI::bar*>(&e)) {
				m.addStringArg(*e.s);
			}
			
			
			if (m.getNumArgs() > 0) {
				if (e._ui->_settings->presetIsLoading) {
					bundle.addMessage(m);
				} else {
					send.sendMessage(m, false);
				}
			}
		}
	}
	
	
	//--------------------------------------------------------------
	void addUI(ofxMicroUI * ui) {
		_nameUIs[ui->uiName] = ui;
		ofAddListener(_nameUIs[ui->uiName]->uiEvent, this, &ofxMicroUIRemote::uiEvent);
		ofAddListener(_nameUIs[ui->uiName]->uiEventMaster, this, &ofxMicroUIRemote::uiEventString);
	}
	
	
	//--------------------------------------------------------------
	void uiEventString(string & e) {
//		cout << "remote event " << e << endl;
		
		// temporario por enquanto. nao sabemos ainda de que UI vem.
		if (e == "createFromText") {
			
			ofxOscMessage m;
			m.setAddress(reservedAddress + "createFromText");

			ofBuffer blob;
//			if (clear) {
//				blob.append("uiClear\n");
//			}

//			if (remoteStyle != "") {
//				blob.append(remoteStyle);
//			}
			
//			if (ofFile::doesFileExist("uiRemote.txt")) {
//				blob.append(ofBufferFromFile("uiRemote.txt"));
//			}

			
			
			
			// this will continue after the event.
			if (2==3) {
				blob.append(_nameUIs["master"]->createdLines);
				cout << "REMOTE OSC createFromText :: " << endl;
				cout << _nameUIs["master"]->createdLines << endl;
				m.addBlobArg(blob);
				send.sendMessage(m, false);
			}

			
			//			bundle.addMessage(m);
		}
	}


};

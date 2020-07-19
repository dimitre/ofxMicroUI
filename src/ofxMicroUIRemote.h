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
	ofxMicroUI::inspector * oscInfo = NULL;
	
	ofxOscSender 	send;
	ofxOscReceiver 	receive;

	string 	serverAddress = "";
	int 	serverPort = 8000;
	string 	remoteAddress = "";
	int 	remotePort = 9000;
	
	ofxOscBundle bundle;
	
	ofEvent<string> eventMessage;

	
//	typedef void (*ScriptFunction)(void);
	map <string, std::function<void()>> msgFunction;
//	map <string, ScriptFunction> msgFunction;
	
//	std::function<void()> func;
	
	ofxMicroUIRemote() {
		ofAddListener(ofEvents().draw, this, &ofxMicroUIRemote::onDraw);
		ofAddListener(ofEvents().update, this, &ofxMicroUIRemote::onUpdate);
	}
	
	~ofxMicroUIRemote() {}
	
	//--------------------------------------------------------------
	map <string, string> loadConfigPairs(string file) {
		map <string, string> configs;
		for (auto & c : u.textToVector(file)) {
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
	
//	vector <ofxMicroUI *> _uis;
	map <string, ofxMicroUI *> _nameUIs;
	//--------------------------------------------------------------
	void addUI(ofxMicroUI * ui) {
//		_uis.push_back(ui);
		_nameUIs[ui->uiName] = ui;
//		cout << "ofxMicroUIRemote addUI :: " << ui->uiName << endl;
//		cout << _nameUIs[ui->uiName]->uiName << endl;
		
//		for (auto & e : _nameUIs[ui->uiName]->elements) {
//			cout << e->name << endl;
//		}

//		ofAddListener(_uis.back()->uiEvent, this, &ofxMicroUIRemote::uiEvent);
//		ofAddListener(_uis.back()->uiEventMaster, this, &ofxMicroUIRemote::uiEventString);
		ofAddListener(_nameUIs[ui->uiName]->uiEvent, this, &ofxMicroUIRemote::uiEvent);
		ofAddListener(_nameUIs[ui->uiName]->uiEventMaster, this, &ofxMicroUIRemote::uiEventString);
	}
	

	//--------------------------------------------------------------
	void onDraw(ofEventArgs &data) { draw(); }

	//--------------------------------------------------------------
	void onUpdate(ofEventArgs &data) {
		
		if (bundle.getMessageCount()) {
//			send.sendMessage(bundle)
			send.sendBundle(bundle);
//			cout << "sending OSC bundle " << bundle.getMessageCount() << endl;
			bundle.clear();
		}
		
		update();
		while(receive.hasWaitingMessages()){
			ofxOscMessage m;
			receive.getNextMessage(m);
			cout << "receiving message :: " + m.getAddress() << endl;
			string msg = m.getAddress();
			ofNotifyEvent(eventMessage, msg);
			
			vector <string> addr = ofSplitString(m.getAddress(), "/");
//			cout << addr.size() << endl;
			
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
	
	//--------------------------------------------------------------
	void uiEventString(string & e) {
		cout << "remote event " << e << endl;
		if (e == "createFromText") {
			
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
				oscInfo->set(address);
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
			
			if (dynamic_cast<ofxMicroUI::toggle*>(&e)) {
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
};

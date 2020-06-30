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
	
	vector <ofxMicroUI *> _uis;
	map <string, ofxMicroUI *> _nameUIs;
	//--------------------------------------------------------------
	void addUI(ofxMicroUI * ui) {
		cout << "ofxMicroUIRemote addUI :: " << ui->uiName << endl;
		_uis.push_back(ui);
		_nameUIs[ui->uiName] = ui;
		ofAddListener(_uis.back()->uiEvent, this, &ofxMicroUIRemote::uiEvent);
		ofAddListener(_uis.back()->uiEventMaster, this, &ofxMicroUIRemote::uiEventString);
	}
	

	//--------------------------------------------------------------
	void onDraw(ofEventArgs &data) { draw(); }

	//--------------------------------------------------------------
	void onUpdate(ofEventArgs &data) {
		
		if (bundle.getMessageCount()) {
//			send.sendMessage(bundle)
			send.sendBundle(bundle);
			cout << "sending bundle " << bundle.getMessageCount() << endl;
			bundle.clear();
		}
		
		
		update();
		while(receive.hasWaitingMessages()){
			ofxOscMessage m;
			receive.getNextMessage(m);

//			lastAdd = m.getAddress();
//			if (debug) {
				cout << "receiving message :: " + m.getAddress() << endl;
//			}
			
			vector <string> addr = ofSplitString(m.getAddress(), "/");
//			cout << addr.size() << endl;
			
			string uiName = addr[1];
			string name = addr[2];
			// prova de conceito mas eventualmente nao vai funcionar ainda por causa do propagateevent. refazer isso logo em breve
			ofxOscArgType k = m.getArgType(0);
			
			if (k == OFXOSC_TYPE_FLOAT) {
				if ( _nameUIs.find(uiName) != _nameUIs.end() ) {
					_nameUIs[uiName]->getSlider(name)->set((float) m.getArgAsFloat(0));
				}
//				_ui->set(name, (float) m.getArgAsFloat(0), propagateEvent);
			}
			else if (k == OFXOSC_TYPE_INT32 || k == OFXOSC_TYPE_INT64) {
				if ( _nameUIs.find(uiName) != _nameUIs.end() ) {
					_nameUIs[uiName]->getSlider(name)->set((int) m.getArgAsInt(0));
				}
//				_ui->set(name, (int) m.getArgAsInt(0), propagateEvent);
			}
			else if (k == OFXOSC_TYPE_FALSE) {
				_nameUIs[uiName]->getSlider(name)->set((int) m.getArgAsInt(0));
				_nameUIs[uiName]->set(name, (bool) false);
//				_ui->set(name, (bool) false, propagateEvent);
			}
			else if (k == OFXOSC_TYPE_TRUE) {
				_nameUIs[uiName]->set(name, (bool) true);
//				_ui->set(name, (bool) true, propagateEvent);
			}
			else if (k == OFXOSC_TYPE_STRING) {
				_nameUIs[uiName]->set(name, m.getArgAsString(0));
//				_ui->set(name, (string) m.getArgAsString(0), propagateEvent);
			}
		}
		
	}
	
	//--------------------------------------------------------------
	void uiEventString(string & e) {
		cout << "remote event" << e << endl;
	}

	//--------------------------------------------------------------
	void uiEvent(ofxMicroUI::element & e) {
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
			m.addBoolArg(*e.b);
		}

		else if (dynamic_cast<ofxMicroUI::radio*>(&e)) {
			m.addStringArg(*e.s);
		}
		
		else if (dynamic_cast<ofxMicroUI::inspector*>(&e) || dynamic_cast<ofxMicroUI::bar*>(&e)) {
			m.addStringArg(*e.s);
		}
		
		if (m.getNumArgs() > 0) {
			bundle.addMessage(m);
//			send.sendMessage(m, false);
		}
	}
		
};

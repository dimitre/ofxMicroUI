#pragma once
#include "ofxMicroUI.h"
#include "ofxOsc.h"
#if defined( TARGET_OF_IPHONE ) || defined( TARGET_OF_IOS ) || defined( TARGET_ANDROID )
#define MICROUI_TARGET_TOUCH
#endif
#ifdef MICROUI_TARGET_TOUCH
#include "ofxAccelerometer.h"
#endif
/*
remote	192.168.1.240:9000
local	8000
useBundle	no
*/
class ofxMicroUIRemote : public ofBaseApp {
public:

	map <ofxOscArgType, string> oscTypeMap = {
		{ OFXOSC_TYPE_FLOAT, "FLOAT" },
		{ OFXOSC_TYPE_INT32, "INT32" },
		{ OFXOSC_TYPE_INT64, "INT64"},
		{ OFXOSC_TYPE_FALSE, "FALSE" },
		{ OFXOSC_TYPE_TRUE, "TRUE"},
		{ OFXOSC_TYPE_STRING, "STRING"},
	};


	// UIMASTER, de onde saem todas as outras. talvez trazer o soft aqui?
	ofxMicroUI * _u = NULL;
	ofxMicroUISoftware * _soft = NULL;
	string name = "";
	string configFile = "";

	ofxOscSender 	send;
	ofxOscReceiver 	receive;
	bool useSend = false;
	bool useReceive = false;
	bool verbose = false;
	
	ofEvent<string> eventMessage;

	ofxOscBundle bundle;
	map <string, ofxMicroUI *> _nameUIs;
	
	bool sendOnLoadPreset = true;

	struct broadOsc {
	public:
		ofxMicroUIRemote * _remote = NULL;
		ofxOscSender send;
		ofxOscReceiver receive;
		ofxOscMessage m;
		string ip = "127.0.0.1";
		int32_t port = 8000;
		
		bool sending = false;
		bool receiving = true;

		float nextJump;
		
		void setup() {
			ofxOscSenderSettings set;
			set.host = "192.168.2.255";
			set.port = 9999;
			set.broadcast = true;
			send.setup(set);
			receive.setup(9999);
			m.setAddress("/my");
			m.addStringArg(ip);
			m.addInt32Arg(port);
		}
		
		void update() {
			if (sending) {
				if (ofGetElapsedTimef() > nextJump) {
					nextJump = ofGetElapsedTimef() + 1;
					send.sendMessage(m, false);
				}
			}
			
			if (receiving) {
				while(receive.hasWaitingMessages()){
//					cout << "XAUUU" << endl;

					ofxOscMessage m;
					receive.getNextMessage(m);
					string ip = m.getArgAsString(0);
					auto port = m.getArgAsInt32(1);
					cout << m.getRemoteHost() << endl;
					_remote->sender(m.getRemoteHost(), port);
				}
			}
		}
	} broad;

	void setup() {
//        cout << "ofxUIRemote Setup" << endl;
		ofAddListener(_u->uiEventMaster, this, &ofxMicroUIRemote::uiEventMaster);
		ofAddListener(ofEvents().update, this, &ofxMicroUIRemote::onUpdate);
//		broad.setup();
//		broad._remote = this;
	}
	
	ofxMicroUIRemote(ofxMicroUI * _ui, string n, string f) : _u(_ui), name(n), configFile(f) {
		setup();
	}

	ofxMicroUIRemote(ofxMicroUISoftware * _s, string n, string f) : _soft(_s), name(n), configFile(f) {
		_u = _soft->_ui;
		setup();
	}
	
	ofxMicroUIRemote() {
		setup();
	}

	enum bundleUsage {
		USEBUNDLE_NO,
		USEBUNDLE_YES,
		USEBUNDLE_ALL,
	};
	
	bundleUsage useBundle = USEBUNDLE_YES;

	ofxMicroUI::inspector * oscInfo = NULL;
	ofxMicroUI::inspector * oscInfoReceive = NULL;
	ofxMicroUI::inspector * oscIP = NULL;

	string message = "";
	
	void receiver(int p) {
		try {
			useReceive = receive.setup(p);
			message += "receiving in local port " + ofToString(p) + "\n";
//		} catch (const exception){
		} catch (const std::exception& ex){
			cout << "ofxMicroUIRemote setupRemote :: ||| NO INTERNET |||" << endl;
		}
	}
	
	void sender(string h, int p) {
		try {
			useSend = send.setup(h, p);
//		} catch (exception){
		} catch (const std::exception& ex){
			message += "ofxMicroUIRemote setupServer :: ||| NO INTERNET |||";
		}

		if (useSend) {
			message += "sending to remote " + h + ":" + ofToString(p) + "\n";
		}
	}

	void uiEventMaster(string & s) {
//		cout << "ofxMicroUIRemote uiEventMaster " << s << endl;
		if (s == "setup") {
			message += "ofxMicroUIRemote " + name + "\n";
			
			for (auto & l : ofxMicroUI::textToVector(configFile)) {
				vector <string> cols = ofSplitString(l, "\t");
				if (cols[0] == "remote") {
					vector <string> vals = ofSplitString(cols[1], ":");
					sender(vals[0], ofToInt(vals[1]));
					if (oscIP != NULL) {
						oscIP->set(name + " " + vals[0]);
						cout << "oscIP = not NULL" << endl;
					} else {
						cout << "oscIP = NULL" << endl;
					}
				}
				
				else if (cols[0] == "local") {
					receiver(ofToInt(cols[1]));
				}

				else if (cols[0] == "useBundle") {
					if (cols[1] == "no") {
						useBundle = USEBUNDLE_NO;
					}
					else if (cols[1] == "yes") {
						useBundle = USEBUNDLE_YES;
					}
					else if (cols[1] == "all") {
						useBundle = USEBUNDLE_ALL;
					}
				}
				else if (cols[0] == "addUI") {
					addUIByNames(cols[1]);
				}
				else if (cols[0] == "addUIByTag") {
					addUIByTag(cols[1]);
				}
				else if (cols[0] == "addAllUIs") {
					addAllUIs();
				}
				else if (cols[0] == "addUIByNames") {
					addUIByNames(cols[1]);
				}
				else if (cols[0] == "verbose") {
					verbose = cols[1] == "no" ? false : true;
				}
			}
			ofxMicroUI::messageBox(message);
		}
	}

	void mirrorMyInterface(ofxMicroUI * _ui) {
//		cout << "mirrorMyInterface " << _ui->uiName << endl;
		ofxOscMessage m;
		m.setAddress("/uiRemoteMirror/" + _ui->uiName + "/createFromText");
		ofBuffer blob;
		blob.append("clear\r");
//		blob.append("uiName\t" + _ui->uiName +"\r");
		blob.append("label\t" + _ui->uiName +"\r");
		blob.append(_ui->createdLines);
//		cout << "REMOTE OSC createFromText :: " << endl;
//		cout << _ui->createdLines << endl;
		m.addBlobArg(blob);
//		send.sendMessage(m, false);
		bundle.addMessage(m);

	}
	
	void mirrorMyInterfaces() {
		string saida = "mirrorMyInterfaces : ";
		for (auto & u : _nameUIs) {
			saida += u.second->uiName + " ";
			mirrorMyInterface(u.second);
		}
		cout << saida << endl;
	}
	
	void uiEventGeneral(ofxMicroUI::event & e) {
//		cout << "uiEventGeneral : " << e._ui->uiName << " : " << e.name << endl;
		// setup, load, createFromText
		if (e.name == "createFromText") {
			mirrorMyInterface(e._ui);
		}
	}

	bool mirror = true;
	
	~ofxMicroUIRemote() {}

	void addUI(ofxMicroUI * ui) {
		alerta("addUI " + ui->uiName);
		_nameUIs[ui->uiName] = ui;
		ofAddListener(_nameUIs[ui->uiName]->uiEvent, this, &ofxMicroUIRemote::uiEvent);
		if (mirror) {
			ofAddListener(_nameUIs[ui->uiName]->uiEventGeneral, this, &ofxMicroUIRemote::uiEventGeneral);
		}
	}
	
	void alerta(string s) {
		if (verbose) {
			cout << "ofxMicroUIRemote " << s << endl;
		}
	}
	
	void addAllUIs() {
		alerta("addALLUIs ");
		addUI(_u);
		for (auto & u : _u->allUIs) {
			addUI(u);
		}
	}

	void addUIByTag(string tags) {
		alerta("addUIByTag " + tags);
		for (auto tag : ofSplitString(tags, ",")) {
			for (auto & u : _u->allUIs) {
				if (u->uiTag == tag) {
					addUI(u);
				}
			}
		}
	}
	
	void addUIByNames(string s) {
		alerta("addUIByNames " + s);
		vector <string> names = ofSplitString(s, ",");
		for (auto & n : names) {
			if (n == "master") {
				addUI(_u);
			} else {
				addUI(&_u->uis[n]);
			}
		}
	}

	void onUpdate(ofEventArgs &data) {
		broad.update();

		if (useSend) {
			parseSend();
		}
		if (useReceive) {
			parseReceive();
		}
	}

	void parseSend() {
		if (bundle.getMessageCount()) {
			send.sendBundle(bundle);
			bundle.clear();
		}
	}

	void parseReceive() {
		while(receive.hasWaitingMessages()){
			ofxOscMessage m;
			receive.getNextMessage(m);
			
			string msg = m.getAddress();
			
			string debugString = msg + " ";
			// debugString += "x";
			if (verbose) {
				// _u->addAlert(m.getAddress());
				// 
			}
			ofNotifyEvent(eventMessage, msg);

			if (oscInfoReceive != NULL) {
				oscInfoReceive->set(msg);
			}
			vector <string> addr = ofSplitString(m.getAddress(), "/");

			if (addr[1] == "uiRemoteMirror") {
				string uiName = addr[2];
				ofBuffer blob = m.getArgAsBlob(0);
				ofxMicroUI * _ui;
				string lines = blob.getText();

				bool exists = _u->uis.count(uiName);
				if (!exists) {
//                    cout << "ui dont exist, creating it" << endl;
					_u->addUI(uiName, false);
				} else {
//                    cout << "ui already exist " << uiName << endl;
				}
				_ui = &_u->uis[uiName];
				_ui->uiName = uiName;

				//                _ui->createFromLine();
				lines = "label  "+uiName+"\r" + lines;
//                cout << lines << endl;
				_ui->createFromLines(lines);
				//teste
				_ui->updateRect();
				_ui->redrawUI = true;
				
				if (!exists) {
					addUI(_ui);
					if (!_ui->hasListeners) {
						_ui->addListeners();
					}
				}
			}
			
//            cout << "----" << endl;
			if (addr[1] == "software") {
				if (addr[2] == "savePreset") {
					if (addr.size() == 3) {
						cout << "SavePreset " << _u->pString["presets"] << endl;
						_u->savePreset(_u->pString["presets"]);
						_u->presetElement->redraw();
					}
					else if (addr.size() == 4) {
						_u->savePreset(addr[3]);
						_u->presetElement->set(addr[3]);
					}
				}
				else if (addr[2] == "loadPreset") {
					if (addr.size() == 4) {
						_u->presetElement->set(addr[3]);
					}
				}
				else if (addr[2] == "presetsFolder") {
					if (addr.size() == 4) {
						_u->getRadio("presetsFolder")->set(addr[3]);
					}
				}
			}


			if (addr.size() >= 3) {
				// cout << addr.size() << endl;
				string uiName = addr[1];
				string name = addr[2];
				// prova de conceito mas eventualmente nao vai funcionar ainda por causa do propagateevent. refazer isso logo em breve
				
				if (verbose) {
					if (m.getNumArgs()) {
						debugString += " " + oscTypeMap[m.getArgType(0)] + " ";
						ofxOscArgType k = m.getArgType(0);
						if (k == OFXOSC_TYPE_FLOAT) {
							debugString += ofToString(m.getArgAsFloat(0));
						}
					}
				}

				if ( _nameUIs.find(uiName) != _nameUIs.end() ) {
//                    cout << "uiName found" << uiName << endl;
//                    if (m.getNumArgs()) {

					ofxOscArgType k = m.getArgType(0);
					_nameUIs[uiName]->_settings->eventFromOsc = true;
					
					if (k == OFXOSC_TYPE_FLOAT) {
						if (verbose) {
							debugString += ofToString(m.getArgAsFloat(0));
						}
						_nameUIs[uiName]->set(name, (float) m.getArgAsFloat(0));
					}
					else if (k == OFXOSC_TYPE_INT32 || k == OFXOSC_TYPE_INT64) {
						if (verbose) {
							debugString += ofToString(m.getArgAsInt(0));
						}
						_nameUIs[uiName]->set(name, (int) m.getArgAsInt(0));
					}
					else if (k == OFXOSC_TYPE_FALSE) {
						_nameUIs[uiName]->set(name, (bool) false);
					}
					else if (k == OFXOSC_TYPE_TRUE) {
						_nameUIs[uiName]->set(name, (bool) true);
					}
					else if (k == OFXOSC_TYPE_STRING) {
						_nameUIs[uiName]->set(name, m.getArgAsString(0));
					}
					_nameUIs[uiName]->_settings->eventFromOsc = false;
						
				}
				if (verbose) {
					_u->addAlert(debugString);
					cout << "receiving :: " << debugString << endl;
				}
			}
		}
	}
	
	// new thing for diogo novas fronteiras
	bool ignoreFromOsc = false;

	void uiEvent(ofxMicroUI::element & e) {

//		cout << sendOnLoadPreset << endl;
//		cout << e._settings->presetIsLoading << endl;
//		cout << "-----" << endl;
	
		if (useSend) {
			if (e._settings->eventFromOsc && ignoreFromOsc) {
				// cout << "EVENT RECEIVED FROM OSC :: ignoring forward " << e.name << endl;
			} else {
				if (sendOnLoadPreset || !e._settings->presetIsLoading) {
					string address = "/" + e._ui->uiName + "/" + e.name;
					// transformar em bundle aqui
			//		cout << "MSG " << address << endl;
					ofxOscMessage m;
					m.setAddress(address);
					
//                    cout << address << endl;
//					if (verbose) {
//						cout << "ofxMicroUIRemote sending " << address << " : " << send.getHost() << ":" << send.getPort() << endl;
//					}

					if (ofxMicroUI::slider * els = dynamic_cast<ofxMicroUI::slider*>(&e)) {
						if (els->isInt) {
							m.addIntArg(*e.i);
						} else {
							m.addFloatArg(*e.f);
						}
					}
					
					else if (dynamic_cast<ofxMicroUI::toggle*>(&e)) {
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
							if (useBundle == USEBUNDLE_NO) {
								send.sendMessage(m, false);
							} else {
								bundle.addMessage(m);
							}
						} else {
							if (useBundle == USEBUNDLE_ALL) {
								bundle.addMessage(m);
							} else {
								send.sendMessage(m, false);
							}
							if (oscInfo != NULL) {
								if (!e._settings->presetIsLoading)
								{
									oscInfo->set(address);
								}
							}
						}
					}
				}
			}
		}
	}

};

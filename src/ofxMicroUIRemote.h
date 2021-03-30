#pragma once
#include "ofxMicroUI.h"
#include "ofxOsc.h"
#if defined( TARGET_OF_IPHONE ) || defined( TARGET_OF_IOS ) || defined( TARGET_ANDROID )
#define MICROUI_TARGET_TOUCH
#endif
#ifdef MICROUI_TARGET_TOUCH
#include "ofxAccelerometer.h"
#endif
/*================================================================
remote	192.168.1.240:9000
local	8000
useBundle	no
#useBundle	yes
#useBundle	all
addUIByTag	unreal,unreal_level,unreal_color,unreal_region
*/
class ofxMicroUIRemote : public ofBaseApp {
public:
	// UIMASTER, de onde saem todas as outras. talvez trazer o soft aqui?
	ofxMicroUI * _u = NULL;
	string name = "";
	string configFile = "";

	ofxOscSender 	send;
	ofxOscReceiver 	receive;
	bool useSend = false;
	bool useReceive = false;
    
    bool verbose = true;

	ofxOscBundle bundle;
	map <string, ofxMicroUI *> _nameUIs;
    
    // apenas se for o empty remote
    ofxMicroUI u;
    
    bool sendOnLoadPreset = true;


	void setup() {
//        cout << "ofxUIRemote Setup" << endl;
		ofAddListener(_u->uiEventMaster, this, &ofxMicroUIRemote::uiEventMaster);
		ofAddListener(ofEvents().update, this, &ofxMicroUIRemote::onUpdate);
	}
	
	ofxMicroUIRemote(ofxMicroUI * _ui, string n, string f) : _u(_ui), name(n), configFile(f) {
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

	void uiEventMaster(string & s) {
		if (s == "setup") {
			string message = "";
            message += "ofxMicroUIRemote " + name + "\r";

			for (auto & l : ofxMicroUI::textToVector(configFile)) {
				vector <string> cols = ofSplitString(l, "\t");
				string name = cols[0];

				if (name == "remote") {
					vector <string> vals = ofSplitString(cols[1], ":");
					try {
						useSend = send.setup(vals[0], ofToInt(vals[1]));
					} catch (exception){
						message += "ofxMicroUIRemote setupServer :: ||| NO INTERNET |||";
					}

					if (useSend) {
                        message += string("sending to remote ") + vals[0] + ":" + vals[1] + "\r";
					}
				}
				else if (name == "local") {
					try {
						useReceive = receive.setup(ofToInt(cols[1]));
						message += "receiving in local port " + cols[1];
					} catch (const exception){
						cout << "ofxMicroUIRemote setupRemote :: ||| NO INTERNET |||" << endl;
					}
				}

				else if (name == "useBundle") {
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

				else if (name == "addUIByTag") {
					addUIByTag(cols[1]);
				}
				else if (name == "addAllUIs") {
					addAllUIs();
				}
				else if (name == "addUIByNames") {
					addUIByNames(cols[1]);
				}
                else if (name == "verbose") {
                    verbose = cols[1] == "no" ? false : true;
                }
			}
			ofxMicroUI::messageBox(message);
		}
	}


	
	~ofxMicroUIRemote() {}

	void addUI(ofxMicroUI * ui) {
		_nameUIs[ui->uiName] = ui;
		ofAddListener(_nameUIs[ui->uiName]->uiEvent, this, &ofxMicroUIRemote::uiEvent);
		// ofAddListener(_nameUIs[ui->uiName]->uiEventMaster, this, &ofxMicroUIRemote::uiEventString);
	}
	
	void addAllUIs() {
		for (auto & u : _u->allUIs) {
			addUI(u);
		}
	}

	void addUIByTag(string tags) {
		for (auto tag : ofSplitString(tags, ",")) {
            if (verbose) {
                cout << "ADDUIBYTAG " << tag << endl;
            }
			for (auto & u : _u->allUIs) {
				if (u->uiTag == tag) {
					addUI(u);
				}
			}
		}
	}
	
	void addUIByNames(string s) {
		vector <string> names = ofSplitString(s, ",");
		for (auto & n : names) {
			addUI(&_u->uis[n]);
		}
	}

	void onUpdate(ofEventArgs &data) {
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
            if (verbose) {
                _u->addAlert(m.getAddress());
                cout << "receiving message :: " + m.getAddress() << endl;
            }

			string msg = m.getAddress();
//			ofNotifyEvent(eventMessage, msg);
			
			if (oscInfoReceive != NULL) {
				oscInfoReceive->set(m.getAddress());
			}

			vector <string> addr = ofSplitString(m.getAddress(), "/");
			
			if (m.getAddress() == "/reservedAddress/createFromText") {
				cout << "YOO" << endl;

				// XAXA todo, ainda fazer 
				string uiName = "";
				ofBuffer blob = m.getArgAsBlob(0);
				ofxMicroUI * _ui;
				if (u.uis.count(uiName)) {
					_ui = &u.uis[uiName];
				} else {
					u.createFromText("addUI" + uiName);
					_ui = &u.uis[uiName];
				}
				_ui->clear();
				_ui->initFlow();
				if (!_ui->hasListeners) {
					_ui->addListeners();
				}
				string lines = blob.getText();
				_ui->createFromLines(lines);
				_ui->uiName = uiName;
				_ui->redrawUI = true;
//					_ui->autoFit();
                cout << "UINAME = " << _ui->uiName << endl;
				cout << "elements size :: " << _ui->elements.size() << endl;
                cout << lines << endl;
			}
			
			if (addr.size() >= 3) {
				// cout << addr.size() << endl;
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
					cout << "-------" << endl;
				}
			}
		}
	}

	void uiEvent(ofxMicroUI::element & e) {
        if (verbose) {
            cout << "ofxMicroUIRemote " << e._ui->uiName << " : " << e.name << endl;
        }
//		cout << sendOnLoadPreset << endl;
//		cout << e._settings->presetIsLoading << endl;
//		cout << "-----" << endl;
	
        if (useSend) {
            if (e._settings->eventFromOsc) {
                // cout << "EVENT RECEIVED FROM OSC :: ignoring forward " << e.name << endl;
            } else {
                if (sendOnLoadPreset || !e._settings->presetIsLoading) {
                    string address = "/" + e._ui->uiName + "/" + e.name;
                    // transformar em bundle aqui
            //		cout << "MSG " << address << endl;
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

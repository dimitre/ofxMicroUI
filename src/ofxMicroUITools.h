	static ofColor stringHexToColor(string corString) {
		//int corInt = ofHexToInt(corString.substr(1));
		
		ofColor cor = ofColor::fromHex(ofHexToInt(corString.substr(1)));
//		if (corString.size() == 7) {
//			cor = ofColor::fromHex(ofHexToInt(corString.substr(1)));
//		}
		if (corString.size() == 9) {
			cor = ofColor::fromHex(ofHexToInt(corString.substr(1, 6)));
			cor.a = ofHexToInt(corString.substr(7,2));
			cout << corString << endl;
			cout << "ALPHa = " << ofHexToInt(corString.substr(7,2)) << endl;
		}
		return cor;
	}
	


	// TOOLS
	static vector <string> textToVector(string file) {
		vector <string> saida;
		ofBuffer buff2 = ofBufferFromFile(file);
		for(auto & line: buff2.getLines()) {
			saida.push_back(line);
		}
		return saida;
	}
	
	static string messageBoxString(string s) {
		vector <string> linhas = ofSplitString(s, "\r");
		unsigned int size = 0;
		for (auto & l : linhas) {
			size = MAX(size, l.size());
		}

		string saida = "";
		saida += "+";
		for (unsigned int a=1; a<size+3; a++) {
			saida += "-" ;
		}
		saida += "+\r";
		
		for (auto & l : linhas) {
			string spaces = "";
			int difSize = (size - l.size());
			if (difSize) {
				for (int a=0; a<difSize; a++) {
					spaces += " ";
				}
			}
			saida += "| " + l + spaces + " |" + "\r";
		}
		
		saida += "+";
		for (unsigned int a=1; a<size+3; a++) {
			saida += "-" ;
		}
		saida += "+";
		saida += "\r";
//		cout << saida << endl;
		return saida;
	}
	
	static void messageBox(string s) {
		cout << messageBoxString(s) << endl;
	}
	
	static void expires(int dataInicial, int dias = 10) {
		time_t rawtime;
		time ( &rawtime );
		//struct tm * timeinfo = localtime ( &rawtime );
		int segundosPorDia = 86400;
		int segundosExpira = segundosPorDia * dias;
		float diasExpira = (segundosExpira - (difftime(rawtime,dataInicial))) / (float)segundosPorDia;
		
		string notice = "Dmtr " + ofToString(rawtime) + " :: ";
		notice +=  "Expires in " + ofToString(diasExpira) + " days";
		messageBox(notice);
		//cout << "-------- Dmtr Expires: " ;
		//cout << rawtime;
		//cout << "expires in " + ofToString(diasExpira) + " days" << endl;
		if (diasExpira < 0 || diasExpira > dias) {
			ofSystemAlertDialog("Dmtr.org Software Expired ~ " + ofToString(dataInicial) + "\rhttp://dmtr.org/");
			std::exit(1);
		}
	}


	// part of old ofxMicroUIRemote. not sure if needed anymore
	//--------------------------------------------------------------
	static map <string, string> loadConfigPairs(string file) {
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


struct alert {
    public:
    string msg;
    int tempo = 300;
    float alpha = 255;
    bool ok = true;
    alert(string m) : msg(m) {
    }

    void draw() {
        if (tempo < 0) {
            ok = false;
        } else {
            tempo -= 4;
            alpha = ofClamp(tempo, 0, 255);
            ofSetColor(40, alpha);
            ofDrawRectangle(0,0,180,20);
            ofSetColor(255, alpha);
            // ofDrawBitmapString(msg + ":" +ofToString(tempo), 20, 18);
            ofDrawBitmapString(msg, 17, 15);
        }
    }
};


vector <alert> alerts;
vector <int> willErase;

void drawAlerts() {
    ofPushMatrix();

    ofSetColor(255);
    ofDrawBitmapString(ofToString(alerts.size()), 20, 18);
    ofTranslate(0, 18);
    for (int a=alerts.size()-1; a>=0; a--) {
//        for (int a=0; a<alerts.size(); a++) {
        if (alerts[a].ok) {
            ofTranslate(0, 18);
            alerts[a].draw();
        } else {
            willErase.push_back(a);
        }
    }
    ofPopMatrix();

    for (auto w : willErase) {
        alerts.erase(alerts.begin() + w);
    }
    willErase.clear();
 }

void addAlert(string s) {
    alerts.emplace_back(s);
}

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

	static string textToString(string file) {
		return ofBufferFromFile(file).getText();
	}

	bool stringToFile(string text, string fileName) {
		ofBuffer dataBuffer;
		ofFile file;

		dataBuffer.set(text.c_str(), text.size());
		cout << "stringToFile :: writing to file " << fileName << endl;
		file.open(fileName, ofFile::WriteOnly);
		bool result = file.writeFromBuffer(dataBuffer);
		file.close();
		return result;
	}
	
	static string messageBoxString(string s) {
//        cout << s << endl;
		s = ofTrim(s);
		string newline = "\n";
		vector <string> linhas = ofSplitString(s, "\n");
		
		// todo: unsigned int to std::size_t
		unsigned int size = 0;
		for (auto & l : linhas) {
			size = MAX(size, (unsigned int)l.size());
		}

		string saida = "";
		saida += "+";
		for (unsigned int a=1; a<size+3; a++) {
			saida += "-" ;
		}
		saida += "+" + newline;
		
		for (auto & l : linhas) {
			string spaces = "";
			unsigned int difSize = (size - (unsigned int)l.size());
			if (difSize) {
				for (unsigned int a=0; a<difSize; a++) {
					spaces += " ";
				}
			}
			saida += "| " + l + spaces + " |" + newline;
		}
		
		saida += "+";
		for (unsigned int a=1; a<size+3; a++) {
			saida += "-" ;
		}
		saida += "+";
		saida += newline;
//		cout << saida << endl;
		return saida;
	}
	
	static void messageBox(string s) {
		cout << messageBoxString(s) << endl;
	}
	
	static void expires(int dataInicial, int dias = 10) {
		time_t rawtime;
		time ( &rawtime );
		int segundosPorDia = 86400;
		int segundosExpira = segundosPorDia * dias;
		float diasExpira = (segundosExpira - (difftime(rawtime,dataInicial))) / (float)segundosPorDia;
		
		string notice = "Dmtr " + ofToString(rawtime) + " :: ";
		notice +=  "Expires in " + ofToString(diasExpira) + " days";
		messageBox(notice);
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


	static void debugPanel(int screenW, int screenH, int w, int h) {
		int pw = screenW / w;
		int ph = screenH / h;
	//    cout << ph << endl;
		int n = 0;
		for (int y=0; y<ph; y++) {
			for (int x=0; x<pw; x++) {
				ofColor cor = ofColor(255.0 * x/(float)pw, 255.0 * y/(float)ph, 127);
				ofSetColor(cor);
				ofPushMatrix();
				ofTranslate(x*w, y*h);
				ofFill();
				ofDrawRectangle(0, 0, w, h);
				ofNoFill();
				ofSetColor(255);
				ofDrawRectangle(0, 0, w, h);
				string s = "x"+ofToString(x) + ":y" + ofToString(y) + "\n" + ofToString(n);
				ofDrawBitmapString(s, 8, 20);
				// glm::vec2 pos = glm::vec2(8,20);
				// _settings->drawLabel(s, pos);
				ofPopMatrix();
				n++;
			}
		}
	}

	void drawString(string s, int x, int y) {
		ofDrawBitmapString(s, x, y);
	}

	struct alert {
		public:
		string msg;
		int tempo = 3200;
		float alpha = 255;
		bool ok = true;
		glm::vec2 pos;
		alert(string m) : msg(m) {
		}

		void draw(int offx = 0, int offy = 0) {
			if (tempo < 0) {
				ok = false;
			} else {
				tempo -= 16;
				alpha = ofClamp(tempo, 0, 255);
				ofPushMatrix();
				ofTranslate(offx, offy);
//				ofSetColor(40, alpha);
//				ofDrawRectangle(0,0,180,20);
				ofSetColor(255, alpha);
				// ofDrawBitmapString(msg + ":" +ofToString(tempo), 20, 18);
//				drawString(msg, 17, 15);
				ofPopMatrix();
			}
		}
	};


	vector <alert> alerts;
	vector <int> willErase;

	void drawAlerts() {
		ofPushMatrix();

		ofSetColor(255);
		drawString(ofToString(alerts.size()), 20, 18);
		
		// ofTranslate(0, 18);
		int offy = 16;
		int offx = 0;
		for (uint16_t a=alerts.size()-1; a>=0; a--) {
	//        for (int a=0; a<alerts.size(); a++) {

			if (alerts[a].ok) {
				// ofTranslate(0, 18);
				offy += 16;
				if (offy > ofGetWindowHeight()) {
					offy = 0;
					offx += 230;
				}
				alerts[a].pos = glm::vec2(offx, offy);
				alerts[a].draw(offx, offy);
				settingsUI.drawLabel(alerts[a].msg, alerts[a].pos);
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

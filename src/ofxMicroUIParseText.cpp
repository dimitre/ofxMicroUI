#include "ofxMicroUI.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;


void ofxMicroUI::createFromLine(std::string l) {
	l = ofTrim(l);
//	cout << l << endl;
	if (l == "") {
		newLine();
		return;
	}

	vector <string> cols { ofSplitString(ofTrim(l), "\t") };
	if (cols.size() == 1) {
		if (l == "rewind") {
			rewind();
		}
		else if (l == "newCol") {
			newCol();
		}
		else if (l == "clear") {
			clear();
		}
		else if (l == "flowVert" || l == "flowHoriz") {
			setFlowVert(l == "flowVert");
		}
		else if (l == "fps" || l == "fpsInfo") {
			elements.push_back(new fps(l, *this));
		}
		else if (l == "ellapsed") {
			elements.push_back(new ellapsed(l, *this));
		}
		return;
	}
	if (cols.size() >= 2) {
		string name { cols.at(1) };

		// START SETTINGS
		if (cols.at(0) == "include") {
			vector <string> linhas = textToVector(cols.at(1));
			createFromLines(linhas, false);
//			updatedRect = false;
		}
//		else if (cols.at(0) == "style") {
//			_settings->styleLines = ofBufferFromFile(cols.at(1)).getText();
//		}
		else if (cols.at(0) == "freeFlow") {
			freeFlow = cols.at(1) == "1";
		}
		else if (cols.at(0) == "uiName") {
			uiName = cols.at(1);
		}
		else if (cols.at(0) == "useFixedLabel") {
			_settings->useFixedLabel = ofToInt(cols.at(1));
		}
		else if (cols.at(0) == "uiMargin") {
			_settings->uiMargin = ofToFloat(cols.at(1));
		}
		else if (cols.at(0) == "alertColor") {
			_settings->alertColor = stringToColor(cols.at(1));
//			redrawUI = true;
		}
		else if (cols.at(0) == "alertColor2") {
			_settings->alertColor2 = stringToColor(cols.at(1));
//			redrawUI = true;
		}
		else if (cols.at(0) == "alertColor3") {
			_settings->alertColor3 = stringToColor(cols.at(1));
//			redrawUI = true;
		}
		else if (cols.at(0) == "uiColorBg") {
			_settings->uiColorBg = stringToColor(cols.at(1));
			uiColorBg = stringToColor(cols.at(1));
		}


		else if (cols.at(0) == "uiOpacity") {
			_settings->uiOpacity = ofToFloat(cols.at(1));
		}
		else if (cols.at(0) == "uiPadding") {
			_settings->uiPadding = ofToFloat(cols.at(1));
			initFlow();
		}
		else if (cols.at(0) == "elementSpacing") {
			_settings->elementSpacing = ofToFloat(cols.at(1));
		}
		else if (cols.at(0) == "elementPadding") {
			_settings->elementPadding = ofToFloat(cols.at(1));
		}
		else if (cols.at(0) == "sliderHeight") {
			_settings->elementRect.height = ofToFloat(cols.at(1));
		}
		else if (cols.at(0) == "sliderWidth") {
			_settings->elementRect.width = ofToFloat(cols.at(1));
		}
		else if (cols.at(0) == "colorBg") {
			_settings->colorBg = stringToColor(cols.at(1));
		}
		else if (cols.at(0) == "colorVal") {
			_settings->colorVal = stringToColor(cols.at(1));
		}
		else if (cols.at(0) == "colorLabel") {
			_settings->colorLabel = stringToColor(cols.at(1));
		}
		else if (cols.at(0) == "colorShadowLabel") {
			_settings->colorShadowLabel = stringToColor(cols.at(1));
		}
		else if (cols.at(0) == "useLabelRainbow") {
			_settings->useLabelRainbow = ofToBool(cols.at(1));
		}
		else if (cols.at(0) == "useBgRainbow") {
			_settings->useBgRainbow = ofToBool(cols.at(1));
		}
		else if (cols.at(0) == "labelPosBaseline") {
			_settings->labelPosBaseline = ofToInt(cols.at(1));
		}
		else if (cols.at(0) == "font") {
//			cout << l << endl;
			if (cols.size() >= 3) {
				_settings->useCustomFont = _settings->font.load(cols.at(1), ofToInt(cols.at(2)));
			}
		}


		else if (cols.at(0) == "presetCols") {
			_settings->presetCols = ofToInt(cols.at(1));
		}
		else if (cols.at(0) == "presetHeight") {
			_settings->presetHeight = ofToInt(cols.at(1));
		}

		else if (cols.at(0) == "saveMode") {
			if (cols.at(1) == "NONE") {
				saveMode = NONE;
			} else if (cols.at(1) == "PRESETSFOLDER") {
				saveMode = PRESETSFOLDER;
			} else if (cols.at(1) == "MASTER") {
				saveMode = MASTER;
			}

//            cout << uiName << " saveMode " << cols.at(1) << endl;
		}

		else if (cols.at(0) == "loadMode") {
			if (cols.at(1) == "NONE") {
				loadMode = NONE;
			} else if (cols.at(1) == "PRESETSFOLDER") {
				loadMode = PRESETSFOLDER;
			} else if (cols.at(1) == "MASTER") {
				loadMode = MASTER;
			}
//            cout << uiName << " loadMode " << cols.at(1) << endl;
		}


		// END SETTINGS

		// Behavior
		else if (cols.at(0) == "saveXml") {
			saveXmlOnNewElement = ofToBool(cols.at(1));
		}
		else if (cols.at(0) == "tag") {
			tagOnNewElement = cols.at(1);
		}
		else if (cols.at(0) == "uiTag") {
			tagOnNewUI = cols.at(1);
		}
		else if (cols.at(0) == "uiColorTop") {
			uiColorTopOnNewUI = stringToColor(cols.at(1));
//			_settings->uiColorBg = stringToColor(cols.at(1));
		}

		// template
		else if (cols.at(0) == "beginTemplate") {
			buildingTemplate = cols.at(1);
//			cout << ">>> beginTemplate " << cols.at(1) << "\t\t" << uiName << endl;
			templateUI[buildingTemplate].clear();
		}

		else if (cols.at(0) == "endTemplate") {
//			cout << "endTemplate :: " << buildingTemplate << endl;
			buildingTemplate = "";
		}

		else if (cols.at(0) == "template") {

			string name = cols.at(1);
			if (cols.size() > 2) {
				buildingTemplateName = cols.at(2);
//				cout << "ofxMicroUI::createFromLine this is the line " << l << endl;
			}

			for (auto s : templateUI[name]) {
				string str = ofJoinString(templateVectorString[name], " ");
				ofStringReplace(s, "{$vectorString}", str);
				if (s == "{$lineString}") {
					createFromLines(templateVectorString[name]);
				}
				if (cols.size() > 3) {
					ofStringReplace(s, "$2", cols.at(3));
				}
				if (cols.size() > 2) {
					ofStringReplace(s, "$", cols.at(2));
				}
				createFromLine(s);
//				cout << "create from template : " << s << endl;
			}
			buildingTemplateName = "";
		}

		else if (cols.at(0) == "intsList" || cols.at(0) == "floatsList") { //
			vector <string> nomes = ofSplitString(name, " ");
			for (auto & n : nomes) {
				string line = "int	" + n + "	" + cols.at(2);
				if ( cols.at(0) == "floatsList" ) {
					line = "float	" + n + "	" + cols.at(2);
				}
				createFromLine(line);
			}
		}

		if (cols.at(0) == "toggleMatrix" || cols.at(0) == "boolMatrix") {
			useLabelOnNewElement = false;

			string valores = cols.at(2);
			if (valores != "") {
				vector <string> lines;
				vector <string> vals = ofSplitString(valores, " ");
				int maxx = ofToInt(vals.at(0));
				int maxy = ofToInt(vals.at(1));
				for (int y=0; y<maxy; y++) {
					lines.push_back("flowHoriz");
					for (int x=0; x<maxx; x++) {
						string nomeElement = name + ofToString(x) + ofToString(y);
						string n = name + "_" + ofToString(x) + "_" +ofToString(y);
//						lines.push_back("toggleNoLabel	" + n + "	0");
						lines.push_back("toggleNoLabel	" + n + "	0");
					}
					lines.push_back("flowVert");
					lines.push_back("");
				}

				for (auto & l : lines) {
					createFromLine(l);
				}
				// isto nao funcionou e parou tudo que havia abaixo dali.
				//createFromLines(lines);
			}
			useLabelOnNewElement = true;
		}


//		else if (tipo == "ints" || tipo == "floats" || tipo == "bools" || tipo == "bangs" ||
//				 tipo == "holds" || tipo == "colors" || tipo == "slider2ds" ||
//				 tipo == "boolsNoLabel" || tipo == "sliderVerts") {

		else if (cols.at(0) == "ints" || cols.at(0) == "floats" || cols.at(0) == "bools") {
			string tipo = cols.at(0);
			string nome = cols.at(1);
			vector <string> nomes { ofSplitString(cols.at(1), "[") };
			string n = nomes.at(0);
			string intervalo = ofSplitString(nomes.at(1), "]").at(0);
			int start = ofToInt(ofSplitString(intervalo, "-").at(0));
			int end = ofToInt(ofSplitString(intervalo, "-").at(1));
			string newTipo = tipo.substr(0, tipo.size()-1);
			if (tipo == "boolsNoLabel") {
				createFromLine("flowHoriz");
				newTipo = "toggleNoLabel";
			}
			for (int a=start; a<=end; a++) {
				createFromLine(newTipo + "	"+n + "_" + ofToString(a) + "	" + cols.at(2));
			}
			if (tipo == "boolsNoLabel") {
				createFromLine("flowVert");
			}
		}

		else if (cols.at(0) == "labelOnNewElement") {
			useLabelOnNewElement = cols.at(1) == "1" ? 1 : 0;
		}
		// 2 parameters
		else if (cols.at(0) == "addUI" || cols.at(0) == "addUIDown") {
			string loadText = "";
			if (cols.size() > 2) {
				loadText = cols.at(2);
			}
			addUI(cols.at(1), cols.at(0) == "addUIDown", loadText);

			// this is to hold a variable to know which template built the UI.
			// Used in template to open shaders and update folder accordingly
		}

		else if (cols.at(0) == "addUILabel") {
			addUI(cols.at(1));
			uis[cols.at(1)].createFromLine( "label	"+cols.at(2) );
			uis[cols.at(1)].updateRect();
		}

		else if (cols.at(0) == "addShortcutUI") {
			if (cols.size() == 2) {
				vector <string> uis = ofSplitString(cols.at(1), " ");
				for (auto & u : uis) {
					addShortcutUI(&_masterUI->uis[u]);
					cout << "addShortcutUI " << uiName << " :: " << u << endl;
				}
			}
			else if (cols.size() == 3) {
				vector <string> uisString = ofSplitString(cols.at(2), " ");
				for (auto & u : uisString) {
					uis[cols.at(1)].addShortcutUI(&uis[u]);
//					addShortcutUI(&_masterUI->uis[u]);
					cout << "addShortcutUI " << uiName << " :: " << u << endl;
				}
			}
		}

		else if (cols.at(0) == "colorHsv" || cols.at(0) == "colorHsvA" || cols.at(0) == "colorHsvRange") {
//			ofColor c = ofColor(255,0,70);
			ofColor c = ofColor(255);
			if (cols.size() > 2) {
				if (cols.at(2) != "") {
//					cout << cols.at(2) << endl;
//				cout << "colorHsv color : " << cols.at(2) << endl;
					c = stringHexToColor(cols.at(2));
				}
			}
			int param = 0;
			if (cols.at(0) == "colorHsvA") {
				param = 1;
			}
			else if (cols.at(0) == "colorHsvRange") {
				param = 2;
			}
			
//			cout << "MUI add Color " << name << endl;
			elements.push_back(new colorHsv(name, *this, c, pColor[name], param));
			// bool useAlpha = cols.at(0) == "colorHsvA";
			// elements.push_back(new colorHsv(name, *this, c, pColor[name], useAlpha));
		}

		else if (cols.at(0) == "colorPalImg") {
//			std::string param = cols.at(2);

			elements.push_back(new colorPalImg(name, *this, pColor[name], cols.at(2)));

		}

		else if (cols.at(0) == "colorHsvTest") {
			// elements.push_back(new slider2d(name, *this, pVec2[name]));
			// ofFbo * _fbo = &((slider2d*)elements.back())->fbo;

//			if (2==3) {
//				_fbo->begin();
//				ofClear(0);
//				ofColor cor;
//				cout << "colorHsvTest" << endl;
//				cout << _fbo->getWidth() << endl;
//				cout << _fbo->getHeight() << endl;
//				cout << "----------" << endl;
//
//				int w = _fbo->getWidth();
//				int h = _fbo->getHeight();
//				for (int b=0; b<h; b++) {
//					for (int a=0; a<w; a++) {
//		//				int este = b*w + a;
//						float hue = (255 * a / (float) w);
//						cor = ofColor::fromHsb(hue, 255, b*255/h, 255);
//						ofFill();
//						ofSetColor(cor);
//						ofDrawRectangle(a,b,1,1);
//					}
//				}
//				_fbo->end();
//			}
		}

		else if (cols.at(0) == "colorPalette") {
//			elements.push_back(new colorPalette(name, *this, c, pColor[name], cols.at(0) == "colorHsvA"));
			elements.push_back(new label(name, *this));
			elements.push_back(new colorPalette(name, *this, pVec2[name], 1));
			((colorPalette*)elements.back())->loadPalettes(cols.at(2));
			((colorPalette*)elements.back())->_colorVal = &pColor[name];
		}

		else if (cols.at(0) == "slider2d") {
			elements.push_back(new slider2d(name, *this, pVec2[name]));
		}

		else if (cols.at(0) == "adsr") {
			elements.push_back(new adsr(name, *this, pVec2[name]));
		}

		else if (cols.at(0) == "label") {
			elements.push_back(new label(name, *this));
		}

		else if (cols.at(0) == "input") {
			elements.push_back(new input(name, *this, pString[name]));
		}

		else if (cols.at(0) == "inputPresetLabel") {
			elements.push_back(new input(name, *this, pString[name]));
			using namespace std::placeholders;
			((input*)elements.back())->invokeString = std::bind(&ofxMicroUI::savePresetLabel, this, _1);
		}

		else if (cols.at(0) == "bar") {
			elements.push_back(new bar(name, *this));
		}

		else if (cols.at(0) == "inspector") {
			elements.push_back(new inspector(name, *this));
		}

		else if (cols.at(0) == "color") {
			//xaxa
			elements.push_back(new color(name, *this, stringToColor(cols.at(1))));
		}


		else if (cols.at(0) == "presets" || cols.at(0) == "presetsNoLabel") {
			if (cols.at(0) == "presetsNoLabel") {
				useLabelOnNewElement = false;
			}
			elements.push_back(new presets(name, *this, ofSplitString(cols.at(2)," "), pString[name]));
			using namespace std::placeholders;
			((presets*)elements.back())->invokeString = std::bind(&ofxMicroUI::saveOrLoadAll, this, _1);
			presetElement = (presets*)elements.back();
			if (cols.at(0) == "presetsNoLabel") {
				useLabelOnNewElement = true;
			}
		}

		else if (cols.at(0) == "presetsLoad") {
			elements.push_back(new radio(name, *this, ofSplitString(cols.at(2)," "), pString[name]));
			using namespace std::placeholders;
			((radio*)elements.back())->invokeString = std::bind(&ofxMicroUI::loadPreset, this, _1);
		}

		else if (cols.at(0) == "presetsSave") {
			elements.push_back(new radio(name, *this, ofSplitString(cols.at(2)," "), pString[name]));
			using namespace std::placeholders;
			((radio*)elements.back())->invokeString = std::bind(&ofxMicroUI::savePreset, this, _1);
		}

		else if (cols.at(0) == "fbo") {
			if (cols.size() == 3) {
				elements.push_back(new fboElement(name, *this, ofToInt(cols.at(2))));
			} else {
				elements.push_back(new fboElement(name, *this));
			}
		}


		// 3 parameters
		else if (cols.at(0) == "image") {
			elements.push_back(new image(name, *this, cols.at(2)));
		}

//		else if (cols.at(0) == "vec3") {
//			elements.push_back(new vec3(name, *this, pVec3[name]));
//		}

//		else if (cols.at(0) == "flipflop") {
//			elements.push_back(new flipflop(name, *this, pInt[name]));
//		}

		else if (cols.at(0) == "float" || cols.at(0) == "int") {
			vector <string> vals { ofSplitString(cols.at(2)," ") };
			glm::vec3 initVals { ofToFloat(vals.at(0)), ofToFloat(vals.at(1)), ofToFloat(vals.at(2)) };
			if (cols.at(0) == "float") {
				elements.push_back(new slider(name, *this, initVals, pFloat[name]));
			} else {
				elements.push_back(new slider(name, *this, initVals, pInt[name]));
			}

			if (cols.size() == 4) {
				if (cols.at(0) == "float") {
					// value minimum able to be zero
					initVals.x = 0;
					initVals.z = 0;
					if (cols.at(3) == "audio") {
						string n = name + "Audio";
						elements.push_back(new slider(n, *this, initVals, pFloat[n]));
					}
					else if (cols.at(3) == "beat") {
						string n = name + "Beat";
						elements.push_back(new slider(n, *this, initVals, pFloat[n]));
					}
					else if (cols.at(3) == "audioBeat") {
						{
							string n = name + "Audio";
							elements.push_back(new slider(n, *this, initVals, pFloat[n]));
						}
						{
							string n = name + "Beat";
							elements.push_back(new slider(n, *this, initVals, pFloat[n]));
						}
					}
				}
			}
		}

		else if (cols.at(0)  == "_float") {
			vector<string> vals = ofSplitString(cols.at(2)," ");
			pFloat[name] = stof(vals.at(2));
		}

		else if (cols.at(0)  == "_int") {
			vector<string> vals = ofSplitString(cols.at(2)," ");
			pInt[name] = stof(vals.at(2));
		}

		else if (cols.at(0)  == "_bool") {
			// cout << "BOOL " << name << " xxx " << stoi(cols.at(2)) << endl;
			pBool[name] = stoi(cols.at(2));
		}


		else if (cols.at(0) == "bool" ||
				 cols.at(0) == "boolNoLabel" ||
				 cols.at(0) == "toggle" ||
				 cols.at(0) == "toggleNoLabel" ||
				 cols.at(0) == "bang"
				 ) {
			bool val = false;
			if (cols.size() > 2) {
				val = ofToBool(cols.at(2));
			}
			pBool[name] = val;
			if (cols.at(0) == "toggleNoLabel" || cols.at(0) == "boolNoLabel") {
				useLabelOnNewElement = false;
			}
			elements.push_back(new toggle (name, *this, val, pBool[name], true));

			if (cols.at(0) == "toggleNoLabel" || cols.at(0) == "boolNoLabel") {
				useLabelOnNewElement = true;
			}

			if (cols.at(0) == "bang") {
				((toggle*)elements.back())->setBang();
			}
		}

		else if (cols.at(0) == "hold") {
			elements.push_back(new hold (name, *this, false, pBool[name], true));
		}

		// bang improvisado aqui. fazer de verdade.
//		else if (cols.at(0) == "bang") {
//			bool val = false;
//			pBool[name] = val;
//			elements.push_back(new toggle (name, *this, val, pBool[name], true));
//		}

		else if (cols.at(0) == "radio" || cols.at(0) == "radioNoLabel") {
			useLabelOnNewElement = !(cols.at(0) == "radioNoLabel"); // false if radioNoLabel == true
			elements.push_back(new radio(name, *this, ofSplitString(cols.at(2)," "), pString[name]));
			useLabelOnNewElement = true;
			if (ofIsStringInString(name, "_shortcut")) {
				elements.back()->saveXml = false;
			}
		}
		else if (cols.at(0) == "radioPipeNoLabel" || cols.at(0) == "radioPipe") {
			// todo : eliminate label. maybe optionally some variable on flow or settings
			//useLabelOnNewElement = false;
			elements.push_back(new radio(name, *this, ofSplitString(cols.at(2),"|"), pString[name]));
			//useLabelOnNewElement = true;
			if (ofIsStringInString(name, "_shortcut")) {
//				cout << name << ":::" << "YESSS" << endl;
				elements.back()->saveXml = false;
			}
		}

		else if (cols.at(0) == "camList") {
			elements.push_back(new camList(name, *this, pString[name], pCam[name]));
		}

		// todo : regexp
		else if (cols.at(0) == "dirList"
				 || cols.at(0) == "scene"
				 || cols.at(0) == "sceneUI"
				 || cols.at(0) == "sceneNoLabel"
				 || cols.at(0) == "imageList"
				 || cols.at(0) == "texList"
				 || cols.at(0) == "videoList"
				 || cols.at(0) == "audioList"
				 || cols.at(0) == "textList"
				 || cols.at(0) == "fontList"
				 ) {
			ofDirectory dir;
			if (
				cols.at(0) == "scene"
				|| cols.at(0) == "sceneNoLabel"
				|| cols.at(0) == "sceneUI"
				) {
				dir.allowExt("txt");
			}

			dir.listDir(cols.at(2));
			dir.sort();
			vector <string> opcoes;
			opcoes.emplace_back("_");
			for (auto & d : dir) {
				if (
					cols.at(0) == "dirListNoExt" ||
					cols.at(0) == "scene" ||
					cols.at(0) == "sceneUI" ||
					cols.at(0) == "sceneNoLabel"
				) {
					// sem extensao
					opcoes.emplace_back(d.getBaseName());
				} else
				{
					// com extensao
					opcoes.emplace_back(d.getFileName());
				}
			}

			if (cols.at(0) == "sceneNoLabel") {
				useLabelOnNewElement = false;
			}

			if (cols.at(0) == "imageList" || cols.at(0) == "texList") {
				elements.push_back(new imageList(name, *this, opcoes, pString[name], pImage[name]));
				if (cols.at(0) == "texList") {
					((imageList*)elements.back())->disableArb = true;
				}

			}

			else if (cols.at(0) == "videoList") {
				elements.push_back(new videoList(name, *this, opcoes, pString[name], pVideo[name]));
			}

			else if (cols.at(0) == "audioList") {
				elements.push_back(new audioList(name, *this, opcoes, pString[name], pAudio[name]));
			}

			else if (cols.at(0) == "textList") {
				elements.push_back(new textList(name, *this, opcoes, pString[name], pText[name]));
			}
			else if (cols.at(0) == "fontList") {
				elements.push_back(new fontList(name, *this, opcoes, pString[name], pFont[name]));
			}

			else {
				elements.push_back(new dirList(name, *this, opcoes, pString[name]));
			}
			((dirList*)elements.back())->filePath = cols.at(2);

			if (cols.at(0) == "sceneNoLabel") {
				useLabelOnNewElement = true;
			}

			if (cols.at(0) == "scene" || cols.at(0) == "sceneNoLabel") {
				if (_masterUI != nullptr) {
					//_masterUI->
//					((dirList*)elements.back())->_ui = &_masterUI->uis[name];
					((dirList*)elements.back())->_uiScene = &_masterUI->uis[name];
				}
			}

			if (cols.at(0) == "sceneUI") {
				if (_masterUI != nullptr) {
					((dirList*)elements.back())->_uiScene = &_masterUI->uis[name + uiName];
				}
			}
		}
	}
}



void ofxMicroUI::updateRect() {
	elementsLookup.clear();

	// todo: substitute all this lookups to one map elementKindLookup[KIND] = x
	slidersLookup.clear();
	togglesLookup.clear();
	radiosLookup.clear();
	inspectorsLookup.clear();

	for (auto & e : elements) {
		if (e != nullptr) {
			if (dynamic_cast<slider*>(e)) {
				slidersLookup[e->name] = (slider*)e;
			}
			if (dynamic_cast<toggle*>(e)) {
				togglesLookup[e->name] = (toggle*)e;
			}
			if (dynamic_cast<inspector*>(e)) {
				inspectorsLookup[e->name] = (inspector*)e;
			}
			if (dynamic_cast<radio*>(e) || dynamic_cast<dirList*>(e)) {
				radiosLookup[e->name] = (radio*)e;
			}

			// todo: avoid label in lookups.
			// novidade 16 dez 2021
			if (!dynamic_cast<label*>(e) || dynamic_cast<inspector*>(e))
			{
				elementsLookup[e->name] = e;
			}

			rect.growToInclude(e->rect);
		}
//		cout << e->name << " : " << e->rect << endl;

	}

	// 31aug2022 - CCRIR Muti, adjust minimum ui width
	rect.width = std::max(rect.width, _settings->uiPadding*2 + _settings->elementRect.width);

	rect.height += _settings->uiPadding;
	//rectPos.setDimensions(rect.getDimensions());
	rectPos.width = rect.width;
	rectPos.height = rect.height;


//	cout << "ofxMicroUI::updateRect() " << rect << " : " << rectPos << endl;

	// cout << "updatedrect: " << endl;
	// cout << rect << endl;

	// cout << "ofxMicroUI updatedrect " << uiName << " : "  << rect << endl;
	fbo.allocate(rect.width, rect.height, GL_RGBA);
//	fbo.begin();
//	ofClear(0,255);
//	fbo.end();
	// Prepared to work with Retina in ofWorks fork.
	fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	updatedRect = true;

	// novidade 15 de outubro de 2019
	adjustUIDown();
//	cout << "updateRect! " << uiName << rect << " : " << elements.size() <<  endl;
}

void ofxMicroUI::createFromLines(const vector<string> & lines, bool complete) {
//	cout << "createFromLines " << lines.size() << endl;

	_settings->presetIsLoading = true;
	if (_settings->useFixedLabel && complete) {
		createFromLine("label	" + ofToUpper(uiName));
	}

	for (auto & l : lines) {
		if (buildingTemplate == "") {
//			cout << l << endl;
			createFromLine(l);
		} else {
			if (ofIsStringInString(l, "endTemplate")) {
				buildingTemplate = "";
			} else {
				templateUI[buildingTemplate].push_back(l);
			}
		}
	}
	if (!updatedRect && complete) {
		updateRect();
	}
	_settings->presetIsLoading = false;
}

void ofxMicroUI::createFromText(const of::filesystem::path & fileName) {
//	alert("xxx createFromText " + fileName.string());
//	if (!of::filesystem::exists(fileName)) {
//		cout << "ofxMicroUI file not found " << fileName << endl;
//		return;
//	}


//	cout << fileName << endl;
//	cout << of::filesystem::absolute(fileName) << endl;
	initFlow();

	loadedTextFile = fileName;

	if (futureLines.size()) {
		createFromLines(futureLines);
		createdLines = ofJoinString(futureLines, "\r");
	}

	string lines { ofBufferFromFile(fileName).getText() };

	if (!empty(templateName)) {
//		cout << ":::::::: " << templateName << endl;
		ofStringReplace(lines, "{templateName}", templateName);
	}

	if (replaces.size()) {
		for (auto & r : replaces) {
//			cout << r.first << " : " << r.second << endl;
			string find = "{" + r.first +"}";
			ofStringReplace(lines, find, r.second);
		}
	}

//	cout << lines << endl;
	createFromLines(lines);

	createdLines += lines;
	notify("createFromText");

	redrawUI = true;
	//cout << futureLines.size() << endl;
	// yes? no?
	//futureLines.clear();
	uiIsCreated = true;
}

void ofxMicroUI::createFromLines(const string & line) {
	vector <string> lines = ofSplitString(line, "\n");
	createFromLines(lines);
}

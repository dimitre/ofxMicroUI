# MICROUI

Proof of concept UI for OpenFrameworks.  
Based on ofxDmtrUI3 but rewritten from scratch  
Made to be a compact user interface for quick testing.
Float Sliders, Toggles, Labels and Radio working. More elements in the next few days.  

## Text Files for interface.
UI Elements (sliders, toggles, etc) are loaded from a text 
file.  
Why? It is easy to copy paste, cut, paste somewhere else, change column, copy interfaces between softwares
and change layout things without recompile anything. things important in prototyping.  
If you have a finished software and don't want external files you can stringify everything to be hard coded.  
Other advantages: In ofxDmtrUI3 I can mirror any interface to another using OSC, sending the elements text as an OSC message and the client assumes the layout and sync.
And I've used this interface texts before in ofxUI, so it was easy to port code to ofxDmtrUI, and later to ofxDmtrUI3 and now to ofxMicroUI with little work.

## ROADMAP
- [x] fbo caching  
- [ ] Settings customization via text (Custom font, colors, margins and spacings for elements and columns)
- [ ] Implement non flow-free mode (mouse dragging affect different elements)
- [ ] Implement capture mouse mode (you can keep using the slider even when the mouse is not on element, if not released)


- [ ] Int Slider  
- [ ] radioNoLabel  
- [ ] Toggle without label (drum machine stepper)  
- [ ] Vertical Slider  
- [ ] Port some elements from ofxDmtrUI3 (Slider2d, fbo inspector, string inspector, progress bar, preset list, radio directory listing, scene element)  

- [ ] Easing between floats when needed  

- [ ] UI Events  
- [ ] OSC Mirroring  
- [ ] Midi Controller support  

## 20190814
- [x] Group element mouse event fix.

## 20190813
- [x] Radio working ok  

## 20190806
- [x] label, toogles and float slider

## 20190521
- [x] ability of saving and loading xml files  

## TODO
Decisions  
boolean / toggle naming, class and xml naming.
varkind?  
events  b


# MICROUI

Proof of concept UI for OpenFrameworks.  
Based on ofxDmtrUI3 but rewritten from scratch  
Made to be a compact user interface for quick testing.

[![IMAGE ALT TEXT HERE](microui.png)]  


## Text Files for interface.
UI Elements (sliders, toggles, etc) are loaded from a text file.  
Why? It is easy to copy paste, cut, paste somewhere else, change column, copy interfaces between softwares
and change layout things without recompile anything. things important in prototyping.  
If you have a finished software and don't want external files you can stringify everything to be hard coded.  
Other advantages: In ofxDmtrUI3 I can mirror any interface to another using OSC, sending the elements text as an OSC message and the client assumes the layout and sync.
And I've used this interface texts before in ofxUI, so it was easy to port code to ofxDmtrUI, and later to ofxDmtrUI3 and now to ofxMicroUI with little work.

## ELEMENTS 
List of implemented elements and very near future ones. most of them are already implemented in ofxDmtrUI
- [x] Float Slider
- [x] Int Slider
- [x] Toggle
- [x] Radio
- [x] Label
- [x] Inspector
- [x] ImageDisplay
- [ ] Radio Pipe variant
- [ ] Vec3
- [ ] ColorRgb / ColorHsv
- [ ] Slider 2d (vec2)
- [ ] Bar / Progress bar
- [ ] Plotter
- [ ] Fbo?
- [ ] Preset / Presets
- [ ] Directory Listing
- [ ] Image List
- [ ] Special slim sliders without labels for vec3 controls


## ROADMAP
- [ ] radioNoLabel
- [ ] Implement non flow-free mode (mouse dragging affect different elements)
- [ ] Implement capture mouse mode (you can keep using the slider even when the mouse is not on element, if not released)
- [ ] Vertical Slider
- [ ] Port some elements from ofxDmtrUI3 (Slider2d, fbo inspector, string inspector, progress bar, preset list, radio directory listing, scene element)
- [ ] Easing between floats when needed
- [ ] UI Events
- [ ] OSC Mirroring
- [ ] Midi Controller support

## IDEAS
- [ ] having an ofFbo for each element to redraw and store, only use texture when not changed.
- [ ] three boolean controls, toggle, hold and bang.
- [ ] event system. bind event to elements.
- [ ] ofParameter?
- [ ] make elementgroup to act as one. in xml, draw, etc.

## 20190814
- [x] toggleNoLabel (drum machine stepper)

## 20190814
- [x] Settings customization via text (Custom font, colors, margins and spacings for elements and columns)
- [x] fbo caching
- [x] Int Slider
- [x] Group element mouse event fix.

## 20190813
- [x] Radio working ok  

## 20190806
- [x] label, toogles and float slider

## 20190521
- [x] ability of saving and loading xml files  

## Decisions  
boolean / toggle naming, class and xml naming.
varkind?  


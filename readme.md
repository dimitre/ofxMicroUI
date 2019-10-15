# MICROUI

UI for OpenFrameworks.  
Based on previous interface projects ofxDmtrUI and ofxDmtrUI3.  
Made to be a compact user interface for software.  

![IMAGE ALT TEXT HERE](microui.png)


## Text Files for interface.
UI Elements (sliders, toggles, etc) are loaded from a text file.  
Why? It is easy to copy paste, cut, paste somewhere else, change column, copy interfaces between softwares
and change layout things without recompile anything. things important in prototyping.  
If you have a finished software and don't want external files you can stringify everything to be hard coded.  
Other advantages: In ofxDmtrUI3 I can mirror any interface to another using OSC, sending the elements text as an OSC message and the client assumes the layout and sync.
And I've used this interface texts before in ofxUI, so it was easy to port code to ofxDmtrUI, and later to ofxDmtrUI3 and now to ofxMicroUI with little work.

## ROADMAP
- [ ] UI Events Advanced
- [ ] Implement non flow-free mode (mouse dragging affect different elements)
- [ ] Implement capture mouse mode (you can keep using the slider even when the mouse is not on element, if not released)
- [ ] ofParameter?
- [ ] OSC Mirroring
- [ ] make elementgroup to act as one. in xml, draw, etc.
- [ ] Easing between floats when needed
- [ ] having an ofFbo for each element to redraw and store, only use texture when not changed.

- [ ]boolean / toggle naming, 

- [x] Software object
- [x] Midi Controller support
- [x] UI Events Basic (element pointer)

- [x] class and xml naming.
- [x] varkind?  

## ELEMENTS 
List of implemented elements and very near future ones. most of them are already implemented in ofxDmtrUI

- [ ] Toggle variants (Bang and Hold)  
- [ ] Bar / Progress bar
- [ ] Plotter (points)
- [ ] Image List
- [ ] Vec3  
- [ ] ColorRgb / ColorHsv
- [ ] Slider 2d (vec2)
- [ ] Special slim sliders without labels for vec3 controls
- [ ] Vertical Slider

- [x] Preset / Presets List
- [x] radioNoLabel  
- [x] Radio Scene Element  
- [x] Float Slider
- [x] Int Slider
- [x] Toggle
- [x] Radio
- [x] Label
- [x] String Inspector
- [x] ImageDisplay
- [x] Radio Directory Listing
- [x] Radio Pipe variant
- [x] Fbo inspector

- [x] toggleNoLabel (drum machine stepper)  20190814

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


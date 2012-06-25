#pragma once

#include "ofParameter.h"
#include "ofxBaseGui.h"

class ofxLabel: public ofxBaseGui {
public:
    ofxLabel(){}
    virtual ~ofxLabel() {}
    ofxLabel * setup(string labelName, string _label, float width = defaultWidth, float height = defaultHeight);

    void draw();

    // Abstract methods we must implement, but have no need for!
    virtual void mouseMoved(ofMouseEventArgs & args){}
    virtual void mousePressed(ofMouseEventArgs & args){}
    virtual void mouseDragged(ofMouseEventArgs & args){}
    virtual void mouseReleased(ofMouseEventArgs & args){}
    virtual void saveToXml(ofxXmlSettings& xml){}
    virtual void loadFromXml(ofxXmlSettings& xml){}
    virtual void setValue(float mx, float my, bool bCheckBounds){}

    string operator=(string v) { label = v; return v; }
	operator string & () { return label; }
    void setLabel(string _label) { label = _label; }
    string getLabel()            { return label; }

    // Wanted to use an ofxParameter here so we could hook label change events,
    // but they don't work with strings.
    string label;
};

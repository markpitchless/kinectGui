#pragma once

#include "ofxBaseGui.h"

class ofxFpsSlider: public ofxBaseGui {
public:
    ofxFpsSlider(){}
    virtual ~ofxFpsSlider(){}

	// Needed for the slider.
    ofxFpsSlider * setup( string fpsName, float _max, float width = defaultWidth, float height = defaultHeight );
    void draw();

    // Abstract methods we must implement
	virtual void mouseMoved(ofMouseEventArgs & args){}
	virtual void mousePressed(ofMouseEventArgs & args){}
	virtual void mouseDragged(ofMouseEventArgs & args){}
	virtual void mouseReleased(ofMouseEventArgs & args){}
	virtual void saveToXml(ofxXmlSettings& xml){}
	virtual void loadFromXml(ofxXmlSettings& xml){}
	virtual void setValue(float mx, float my, bool bCheckBounds){}

protected:
    float min, max;
};

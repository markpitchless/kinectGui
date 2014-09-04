#pragma once

#include "ofMain.h"
#include "ofxBaseGui.h"

class ofxFpsSlider: public ofxBaseGui {
public:
    ofxFpsSlider(){}
    virtual ~ofxFpsSlider(){}

    ofxFpsSlider * setup( string _fpsName, float _max, float width = defaultWidth, float height = defaultHeight );

	ofAbstractParameter & getParameter(){ return fps; }

    void draw();

    // Abstract methods we must implement
	virtual bool mouseMoved(ofMouseEventArgs & args){ return false; }
	virtual bool mousePressed(ofMouseEventArgs & args){ return false; }
	virtual bool mouseDragged(ofMouseEventArgs & args){ return false; }
	virtual bool mouseReleased(ofMouseEventArgs & args){ return false; }
	virtual bool setValue(float mx, float my, bool bCheckBounds){ return false; }
    void render() {}

protected:
    float min, max;
    ofParameter<float> fps;
};

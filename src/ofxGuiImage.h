#pragma once

#include "ofParameter.h"
#include "ofxBaseGui.h"

class ofxGuiImage: public ofxBaseGui {
public:
    ofxGuiImage(){}
    virtual ~ofxGuiImage(){}

    // Height is also set to width to give nice square image instead of
    // of little bar.
    ofxGuiImage * setup(string _name, ofImage* _imgPtr, float width = defaultWidth, float height = defaultWidth );

	virtual void mouseMoved(ofMouseEventArgs & args);
	virtual void mousePressed(ofMouseEventArgs & args);
	virtual void mouseDragged(ofMouseEventArgs & args);
	virtual void mouseReleased(ofMouseEventArgs & args);

	virtual void saveToXml(ofxXmlSettings& xml) ;
	virtual void loadFromXml(ofxXmlSettings& xml);

	void draw();

	//ofImage operator=(ofImage v);
	//operator ofImage & ();

	ofImage* imgPtr;

protected:
	void setValue(float mx, float my, bool bCheck);
};

#pragma once

#include "ofParameter.h"
#include "ofxBaseGui.h"

class ofxGuiImage: public ofxBaseGui {
public:
    ofxGuiImage() : showName(true) {}
    virtual ~ofxGuiImage(){}

    // Set size explicitly
    ofxGuiImage * setup(string _name, ofImage* _imgPtr, bool _showName, float width, float height );
    // Set width scale height based on img
    ofxGuiImage * setup(string _name, ofImage* _imgPtr, bool _showName = true, float width = defaultWidth );

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
	bool showName;
};

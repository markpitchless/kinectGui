#pragma once

#include "ofParameter.h"
#include "ofxBaseGui.h"
#include "ofImage.h"
#include "ofMain.h"

class ofxGuiImage: public ofxBaseGui {
public:
    ofxGuiImage() : showName(true), imgBorder(1) {}
    virtual ~ofxGuiImage(){}

    // Set size explicitly (includes name if showName is true).
    ofxGuiImage * setup(string _name, ofImage* _imgPtr, bool _showName, float width, float height );
    // Set width, scale height based on img and showName to preserve aspect.
    ofxGuiImage * setup(string _name, ofImage* _imgPtr, bool _showName = true, float width = defaultWidth );

	virtual bool mouseMoved(ofMouseEventArgs & args) { return false; }
	virtual bool mousePressed(ofMouseEventArgs & args);
	virtual bool mouseDragged(ofMouseEventArgs & args) { return false; }
	virtual bool mouseReleased(ofMouseEventArgs & args) { return false; }

    template<class ListenerClass>
    void addListener(ListenerClass * listener, void ( ListenerClass::*method )(bool&)){
        value.addListener(listener,method);
    }

    template<class ListenerClass>
    void removeListener(ListenerClass * listener, void ( ListenerClass::*method )(bool&)){
        value.removeListener(listener,method);
    }

    ofParameter<int> getImgBorder() { return imgBorder; }
    void setImgBorder(int v)         { imgBorder = v; }

	bool operator=(bool v) { value = v; return v; }
	operator const bool & ()     { return value; }

	ofAbstractParameter & getParameter(){ return value; }

protected:
	ofImage* imgPtr;
    ofParameter<bool> value;
	bool showName;
	ofParameter<int> imgBorder; // pixels of border around img

	virtual bool setValue(float mx, float my, bool bCheck);
    virtual void render();
};

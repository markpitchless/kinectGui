#include "ofxGuiImage.h"

ofxGuiImage * ofxGuiImage::setup(string _name, ofImage* _imgPtr, bool _showName, float width ) {
	setName(_name);
	showName = _showName;
	b.width  = width;
	b.height = _imgPtr->getHeight()*(width/_imgPtr->getWidth());
	if (showName)
	    b.height += defaultHeight;
	imgPtr   = _imgPtr;
	return this;
}

ofxGuiImage * ofxGuiImage::setup(string _name, ofImage* _imgPtr, bool _showName, float width, float height ) {
	setName(_name);
	showName = _showName;
	b.width  = width;
	b.height = height;
	imgPtr   = _imgPtr;
	return this;
}

bool ofxGuiImage::mousePressed(ofMouseEventArgs & args){
    return setValue(args.x, args.y, true);
}

void ofxGuiImage::render(){
	ofPushStyle();
	ofPushMatrix();

	ofSetColor(backgroundColor);
	ofRect(b);

	ofTranslate(b.x, b.y);
	ofSetColor(ofColor::white);
	float h = b.height-2*imgBorder;
	if (showName)
	    h -= defaultHeight;
	imgPtr->draw(imgBorder, imgBorder, b.width-2*imgBorder, h);

	if (showName) {
        ofSetColor(textColor);
        ofDrawBitmapString(getName(), textPadding, b.height-textPadding);
	}

	ofPopMatrix();
	ofPopStyle();
}

bool ofxGuiImage::setValue(float mx, float my, bool bCheck){
    // TODO - Taken from slider. Do we need out own bGuiActive on the class?
    bool bGuiActive(false);
    if( !isGuiDrawing() ){
    bGuiActive = false;
        return false;
    }
    if( bCheck ){
        if( b.inside(mx, my) ){
            bGuiActive = true;
        }else{
            bGuiActive = false;
        }
    }
    if( bGuiActive ){
        value = !value;
        return true;
    }
    return false;
}

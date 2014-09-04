#include "ofxGuiImage.h"

ofxGuiImage * ofxGuiImage::setup(string _name, ofImage* _imgPtr, bool _showName, float width ) {
	setName(_name);
	showName = _showName;
	b.width  = width;
	b.height = _imgPtr->height*(width/_imgPtr->width);
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

bool ofxGuiImage::mouseMoved(ofMouseEventArgs & args){
    return false;
}

bool ofxGuiImage::mousePressed(ofMouseEventArgs & args){
    return setValue(args.x, args.y, true);
}

bool ofxGuiImage::mouseDragged(ofMouseEventArgs & args){
    return false;
}

bool ofxGuiImage::mouseReleased(ofMouseEventArgs & args){
    return false;
}

void ofxGuiImage::draw(){
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
//    if( bCheck ){
//        if( b.inside(mx, my) ){
//            bGuiActive = true;
//        }else{
//            bGuiActive = false;
//        }
//    }
    if( isGuiDrawing() ){
        value = !value;
    }
    return value;
}

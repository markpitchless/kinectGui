/*
 * ofxGuiImage.cpp
 *
 *  Created on: 23 Jun 2012
 *      Author: mda
 */

#include "ofxGuiImage.h"

ofxGuiImage * ofxGuiImage::setup(string _name, ofImage* _imgPtr, float width, float height ) {
	name = _name;
	b.x = 0;
	b.y = 0;
	b.width = width;
	b.height = height;
	currentFrame = 0;
	bGuiActive = false;
	imgPtr = _imgPtr;

	return this;
}


void ofxGuiImage::mouseMoved(ofMouseEventArgs & args){
}

void ofxGuiImage::mousePressed(ofMouseEventArgs & args){
}

void ofxGuiImage::mouseDragged(ofMouseEventArgs & args){
}

void ofxGuiImage::mouseReleased(ofMouseEventArgs & args){
}

void ofxGuiImage::saveToXml(ofxXmlSettings& xml) {
    // Nothing to save for an image
}

void ofxGuiImage::loadFromXml(ofxXmlSettings& xml) {
    // Nothing to save for an image
}

void ofxGuiImage::draw(){
	currentFrame = ofGetFrameNum();

	ofPushStyle();
	ofPushMatrix();

	ofSetColor(backgroundColor);
	ofRect(b);

	ofTranslate(b.x, b.y);
	ofSetColor(ofColor::white);
	imgPtr->draw(0, 0, b.width, b.height);

	ofSetColor(textColor);
	ofDrawBitmapString(name, textPadding, b.height-textPadding);

	ofPopMatrix();
	ofPopStyle();
}

//ofImage ofxGuiImage::operator=(ofImage v){
//	value = v;
//	return v;
//}
//
//ofxGuiImage::operator ofImage & (){
//	return value;
//}

void ofxGuiImage::setValue(float mx, float my, bool bCheck){
}

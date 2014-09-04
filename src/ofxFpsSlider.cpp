
#include "ofxFpsSlider.h"

ofxFpsSlider* ofxFpsSlider::setup(string _fpsName, float _max, float width, float height) {
    setName(_fpsName);
    fps = 0;
    b.x = 0;
    b.y = 0;
    b.width = width;
    b.height = height;
    min = 0.0;
    max = _max;

    return this;
}

void ofxFpsSlider::draw() {
    fps = ofGetFrameRate();

    ofPushStyle();
    ofPushMatrix();

    ofFill();
    ofSetColor(backgroundColor);
    ofRect(b);

    ofTranslate(b.x, b.y);
    float valAsPct = ofMap( fps, min, max, 0, b.width-2, true );
    ofEnableAlphaBlending();
    ofSetColor(fillColor);
    ofRect(1, 1, valAsPct, b.height-2);

    ofTranslate(0, b.height / 2 + 4);
    ofSetColor(textColor);
    ofDrawBitmapString(getName(), textPadding, 0);
    string valStr = ofToString(fps);
    ofDrawBitmapString(valStr, b.width - textPadding - valStr.length() * 8, 0);

    ofPopMatrix();
    ofPopStyle();
}

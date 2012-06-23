
#include "ofxFpsSlider.h"

ofxFpsSlider* ofxFpsSlider::setup(string fpsName, float _max, float width, float height) {
    name = fpsName;
    b.x = 0;
    b.y = 0;
    b.width = width;
    b.height = height;
    min = 0.0;
    max = _max;

    return this;
}

void ofxFpsSlider::draw() {
    float value = ofGetFrameRate();

    ofPushStyle();
    ofPushMatrix();

    currentFrame = ofGetFrameNum();
    ofFill();
    ofSetColor(backgroundColor);
    ofRect(b);

    ofTranslate(b.x, b.y);
    float valAsPct = ofMap( value, min, max, 0, b.width-2, true );
    ofEnableAlphaBlending();
    ofSetColor(fillColor);
    ofRect(1, 1, valAsPct, b.height-2);

    ofTranslate(0, b.height / 2 + 4);
    ofSetColor(textColor);
    ofDrawBitmapString(name, textPadding, 0);
    string valStr = ofToString(value);
    ofDrawBitmapString(valStr, b.width - textPadding - valStr.length() * 8, 0);

    ofPopMatrix();
    ofPopStyle();
}

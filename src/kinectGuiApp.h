#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

// Custom controls.
#include "ofxFpsSlider.h"
#include "ofxGuiImage.h"

#include "KinectBlobTracker.h"

class kinectGuiApp : public ofBaseApp {

    public:
        KinectBlobTracker kinect;
        ofParameter<ofColor> bgColor1;
        ofParameter<ofColor> bgColor2;

        void setup();
        void update();
        void draw();

        void keyPressed  (int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);
        void exit();

        void setupGui();
        void loadSettings();
        void saveSettings();
        void grabMask();
        void clearMask();
        void drawKinectImages();

        ofxPanel guiApp;
        ofxFpsSlider fpsSlider;
        ofParameter<bool> showGui;
        ofxButton loadButton;
        ofxButton saveButton;
        ofxLabel status;
        ofParameter<bool> showPointCloud;
        ofParameter<bool> showColorImg;
        ofParameter<bool> showDepthImg;
        ofParameter<bool> showMaskImg;
        ofParameter<bool> showStencilImg;
        ofParameter<bool> showGrayImg;
        ofParameter<bool> showBlobs;

        ofxPanel guiKinect;
        ofParameterGroup kinectParams;
        ofParameterGroup blobParams;
        ofxLabel kinectId;
        ofxGuiImage colorImgGui;
        ofxGuiImage depthImgGui;
        ofxGuiImage maskImgGui;
        ofxButton grabMaskButton;
        ofxButton clearMaskButton;
        string maskFilename;

        // used for viewing the point cloud
        ofEasyCam easyCam;

        ofxPanel guiBlobs;
        ofxGuiImage grayImgGui;
        ofxGuiImage stencilImgGui;
};

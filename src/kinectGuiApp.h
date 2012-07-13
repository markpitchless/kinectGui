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
        void loadSettings( bool& pressed) { if (pressed) loadSettings(); } // event hook
        void saveSettings();
        void saveSettings( bool& pressed) { if (pressed) saveSettings(); } // event hook
        void grabMask();
        void grabMask( bool& pressed) { if (pressed) grabMask(); } // event hook
        void clearMask();
        void clearMask( bool& pressed) { if (pressed) clearMask(); } // event hook
        void drawKinectImages();

        KinectBlobTracker kinect;

        ofxPanel guiApp;
        ofxFpsSlider fpsSlider;
        ofxToggle showGui;
        ofxButton loadButton;
        ofxButton saveButton;
        ofxLabel status;

        ofxPanel guiKinect;
        ofxLabel kinectId;
        ofxGuiImage colorImgGui;
        ofxGuiImage depthImgGui;
        ofxGuiImage maskImgGui;
        ofxButton grabMaskButton;
        ofxButton clearMaskButton;
        string maskFilename;
        ofxToggle showPointCloud;
        ofxToggle showColorImg;
        ofxToggle showDepthImg;
        ofxToggle showMaskImg;
        // used for viewing the point cloud
        ofEasyCam easyCam;

        ofxPanel guiBlobs;
        ofxToggle showBlobs;
        ofxGuiImage grayImgGui;
        ofxGuiImage stencilImgGui;
        ofxToggle showStencilImg;
        ofxToggle showGrayImg;
};

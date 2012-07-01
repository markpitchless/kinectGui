#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

// Custom controls.
#include "ofxFpsSlider.h"
#include "ofxGuiImage.h"
#include "ofxLabel.h"

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
        ofxPanel guiApp;
        ofxFpsSlider fpsSlider;
        ofxToggle showGui;
        ofxButton loadButton;
        ofxButton saveButton;

        ofxPanel guiKinect;
        ofxLabel kinectId;
        ofxFloatSlider kinectAngle;
        ofxGuiImage colorImgGui;
        ofxGuiImage depthImgGui;
        ofxGuiImage grayImgGui;
        ofxGuiImage maskImgGui;
        ofxGuiImage stencilImgGui;

        void startKinect();
        void setKinectAngle( float & n_angle );
        void setNearThreshold( int n );
        void setFarThreshold( int n);
        void grabMask();
        void grabMask( bool& pressed) { if (pressed) grabMask(); } // event hook
        void clearMask();
        void clearMask( bool& pressed) { if (pressed) clearMask(); } // event hook
        void drawPointCloud();
        void drawKinectImages();
        ofxKinect kinect;
        ofxCvColorImage colorImg;
        ofxCvGrayscaleImage depthImg;// grayscale depth image
        ofxCvGrayscaleImage grayImg; // grayscale depth image after threshold
        ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
        ofxCvGrayscaleImage grayThreshFar;  // the far thresholded image
        // Remove this img from depth before contour finding. e.g. background.
        ofxCvGrayscaleImage maskImg;
        ofxCvGrayscaleImage stencilImg;
        ofxCvContourFinder contourFinder;
        ofxIntSlider nearThreshold;
        ofxIntSlider farThreshold;
        ofxIntSlider extraMaskDepth;
        ofxToggle kinectFlip;
        ofxToggle bThresholds;
        ofxButton grabMaskButton;
        ofxButton clearMaskButton;
        ofxToggle bMask;
        string maskFilename;
        ofxToggle showPointCloud;
        ofxToggle showColorImg;
        ofxToggle showDepthImg;
        ofxToggle showMaskImg;
        ofxToggle showStencilImg;
        ofxToggle showGrayImg;
        // used for viewing the point cloud
        ofEasyCam easyCam;
};

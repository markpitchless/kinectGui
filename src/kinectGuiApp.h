#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

// Custom controls.
#include "ofxFpsSlider.h"
#include "ofxGuiImage.h"
#include "ofxLabel.h"

// The event for a button gives us a bool&, with a true value.
// You can't directly set a bool& function param default so we us this the
// default, so we can have a doStuff() type function that can also be
// bound to a button.
// http://stackoverflow.com/questions/1059630/default-value-to-a-parameter-while-passing-by-reference-in-c
static bool BOOL_TRUE = true;

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
        void saveSettings();
        void loadButtonPressed(bool&);
        void saveButtonPressed(bool&);
        ofxPanel guiApp;
        ofxFpsSlider fpsSlider;
        ofxToggle showGui;
        ofxButton loadButton;
        ofxButton saveButton;

        ofxPanel guiKinect;
        ofxLabel kinectId;
        ofxFloatSlider kinectAngle;
        ofxGuiImage colorImageGui;
        ofxGuiImage depthImageGui;
        ofxGuiImage grayImageGui;
        ofxGuiImage maskImgGui;

        void startKinect();
        void setKinectAngle( float & n_angle );
        void setNearThreshold( int n );
        void setFarThreshold( int n);
        void grabMask( bool& doit = BOOL_TRUE );
        void drawPointCloud();
        ofxKinect kinect;
        ofxCvColorImage colorImg;
        ofxCvGrayscaleImage depthImage;// grayscale depth image
        ofxCvGrayscaleImage grayImage; // grayscale depth image after threshold
        ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
        ofxCvGrayscaleImage grayThreshFar;  // the far thresholded image
        // Remove this img from depth before contour finding. e.g. background.
        ofxCvGrayscaleImage maskImg;
        ofxCvContourFinder contourFinder;
        ofxIntSlider nearThreshold;
        ofxIntSlider farThreshold;
        ofxToggle kinectFlip;
        ofxButton grabMaskButton;
        // used for viewing the point cloud
        ofEasyCam easyCam;
};

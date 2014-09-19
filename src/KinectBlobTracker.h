#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofParameter.h"

class KinectBlobTracker {
    public:
        KinectBlobTracker();
        virtual ~KinectBlobTracker();

        void setup();
        void update();

        void setCameraTiltAngle(float & n_angle);
        //void setCameraTiltAngle(float & v) { float nv = v; setCameraTiltAngle(nv); };
        void grabMask();
        void clearMask();
        bool loadMask(string filename);
        void saveMask(string filename);
        void findBlobs();

        void drawPointCloud();
        void drawBlobs(float x=0, float y=0, float w=640, float h=480);
        void drawBlobs(const ofRectangle & rect) {
            drawBlobs(rect.x, rect.y, rect.width, rect.height);
        }

        void close();

        ofxKinect kinect;
        ofxCvColorImage colorImg;
        ofxCvGrayscaleImage depthImg;// grayscale depth image
        ofxCvGrayscaleImage grayImg; // grayscale depth image after threshold
        ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
        ofxCvGrayscaleImage grayThreshFar;  // the far thresholded image
        ofxCvGrayscaleImage tempGrayImg;    // Used by some filters
        // Remove this img from depth before contour finding. e.g. background.
        ofxCvGrayscaleImage maskImg;
        ofxCvGrayscaleImage stencilImg;
        ofxCvContourFinder contourFinder;
        vector<ofPolyline> blobs;

        /// True to make video image IR instead of RGB. Default false.
        ofParameter<bool> bInfrared;
        /// Set false to disable video image grabbing.
        ofParameter<bool> bVideo;
        /// Set false to not use internal textures, Default true.
        ofParameter<bool> bTexture;
        /// False to disable depth registration.
        ofParameter<bool> bDepthRegistration;

        /// Angle of kinect, setting thins moves the sensor. e.g.
        ///   kinect.kinectAngle = 23;
        ofParameter<float> kinectAngle;
        ofParameter<bool> bThresholds;
        ofParameter<int> nearThreshold;
        ofParameter<int> farThreshold;
        ofParameter<bool> bMask;
        ofParameter<int> extraMaskDepth;
        ofParameter<int> medianBlur;
        ofParameter<int> gaussianBlur;
        ofParameter<bool> kinectFlip;
        ofParameter<int> minArea, maxArea, maxBlobs;
        ofParameter<bool> bFindHoles, bUseApproximation;
        ofParameter<float> simplify, resampleSpacing;
        ofParameter<int> resampleCount;

        ofParameter<float> lineWidth;
        ofParameter<ofColor> boundingColor;
        ofParameter<ofColor> lineColor;
        ofParameter<bool> bFill, showVerts, showInfo;

    private:
        ofPolyline tmpLine;
};

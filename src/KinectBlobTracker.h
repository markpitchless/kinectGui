#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofParameter.h"

class Blob {
    public:
        Blob() {
            line.setClosed(true);
        }

        ofPolyline line;

        vector<ofPoint> & getVertices() { return line.getVertices(); }

        void addVertices(const vector<ofPoint>& verts) { line.addVertices(verts); }

        void simplify(float tolerance=0.3f) { line.simplify(tolerance); }

        void resampleBySpacing(float spacing) {
            tmpLine = line.getResampledBySpacing(spacing);
            line = tmpLine;
        }

        void resampleByCount(float resampleCount) {
            tmpLine = line.getResampledByCount(resampleCount);
            line = tmpLine;
        }

        ofRectangle getBoundingBox() const { return line.getBoundingBox(); }
        float getArea() const { return line.getArea(); }
        size_t size() const { return line.size(); }

        void draw() { line.draw(); }

    private:
        ofPolyline tmpLine;

};

typedef vector<Blob> BlobVector;

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
        BlobVector blobs;
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
};


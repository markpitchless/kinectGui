#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofParameter.h"

/// A single blob (or hole) found by the tracker.
class Blob {
    public:
        Blob() {
            line.setClosed(true);
        }

        /// The points of the blob as an ofPolyline
        ofPolyline line;

        /// True if this blob is a hole
        bool bHole;

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

        void drawLine() { line.draw(); }

        void drawBlob() {
            ofBeginShape();
                for (size_t i=0; i<line.getVertices().size(); ++i) {
                    ofVertex(line[i].x, line[i].y);
                }
            ofEndShape(true);
        }

        void drawVertices() {
            vector<ofPoint> pts = line.getVertices();
            for (size_t j=0; j<pts.size(); ++j) {
                ofSetColor(255,0,255,100);
                ofFill();
                ofCircle(pts[j].x, pts[j].y, 10);
                ofSetColor(0,0,0);
                ofDrawBitmapString(ofToString(j), pts[j].x-6, pts[j].y+3);
                ofSetColor(255,255,255);
                ofNoFill();
                ofCircle(pts[j].x, pts[j].y, 10);
            }
        }

        void drawBounding(bool bInfo = false) {
            ofRectangle box = getBoundingBox();
            ofRect(box);
            if (bInfo) {
                stringstream info;
                info << "Hole:" << bHole
                     << " Size:" << size()
                     << " Area:" << getArea()
                     << " Perimeter:" << line.getPerimeter();
                ofDrawBitmapString(info.str(), box.x, box.y);
            }
        }

    private:
        ofPolyline tmpLine;

};

typedef vector<Blob> BlobVector;

/// A frames worth of data from the tracker, includes kinect images
/// (depth and color) as well as the blobs found.
class BlobFrame {
    public:
        ofxCvColorImage colorImg;
        ofxCvGrayscaleImage depthImg;// grayscale depth image
        ofxCvGrayscaleImage grayImg; // grayscale depth image after processing
        BlobVector blobs;
};

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


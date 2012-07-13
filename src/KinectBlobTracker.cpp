#include "KinectBlobTracker.h"

KinectBlobTracker::KinectBlobTracker() {
    kinectAngle.set("Angle", 0.0, -30.0, 30.0);
    bThresholds.set("Apply Threshold", false);
    nearThreshold.set("Near Threshold", 255, 0, 255);
    farThreshold.set("Far Threshold", 0, 0, 255);
    kinectFlip.set("H Flip", false);
    bMask.set("Apply Mask", false);
    extraMaskDepth.set("Extra Mask Depth", 0, 0, 100);
    medianBlur.set("Median Blur", 0, 0, 99);
    gaussianBlur.set("Gaussian Blur", 0, 0, 23);
    int kinectArea = kinect.width*kinect.height;
    minArea.set("Min Blob Area", 10, 0, (kinectArea/2) );
    maxArea.set("Max Blob Area", (kinectArea/2), 0, kinectArea );
    maxBlobs.set("Max Blobs", 10, 0, 200);
    bFindHoles.set("Find Holes", false);
    bUseApproximation.set("Use Approximation", false);
}

KinectBlobTracker::~KinectBlobTracker() {
    kinectAngle.removeListener(this, &KinectBlobTracker::setCameraTiltAngle);
}

void KinectBlobTracker::setup() {
    colorImg.allocate(kinect.width, kinect.height);
    depthImg.allocate(kinect.width, kinect.height);
    grayImg.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    maskImg.allocate(kinect.width, kinect.height);
    stencilImg.allocate(kinect.width, kinect.height);
    tempGrayImg.allocate(kinect.width, kinect.height);

    kinectAngle.addListener(this, &KinectBlobTracker::setCameraTiltAngle);

    // enable depth->video image calibration
    kinect.setRegistration(true);

    kinect.init();
    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)

    kinect.open();		// opens first available kinect
    //kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
    //kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #

    kinect.setCameraTiltAngle(kinectAngle);
}

void KinectBlobTracker::update() {
    kinect.update();
    if(kinect.isFrameNew()) {
        // load the rgb image
        colorImg.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
        if (kinectFlip)
            colorImg.mirror(false,true);

        // load grayscale depth image from the kinect source
        depthImg.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        if (kinectFlip)
            depthImg.mirror(false,true);

        grayImg = depthImg;

        // Apply the thresholds, setting pix to black when past thresh.
        if (bThresholds) {
            cvThreshold(grayImg.getCvImage(), grayImg.getCvImage(), farThreshold, 0, CV_THRESH_TOZERO);
            cvThreshold(grayImg.getCvImage(), grayImg.getCvImage(), nearThreshold, 0, CV_THRESH_TOZERO_INV);
        }

        if (bMask) {
            // Make a stencil. fooImg is 255 for keep and 0 for remove.
            cvCmp(depthImg.getCvImage(), maskImg.getCvImage(), stencilImg.getCvImage(), CV_CMP_GT);
            // Apply the stencil to the depth image.
            cvAnd(grayImg.getCvImage(), stencilImg.getCvImage(), grayImg.getCvImage());
        }

        if (medianBlur > 0) {
            if (medianBlur % 2 == 0) medianBlur++; // must be odd
            tempGrayImg = grayImg;
            cvSmooth(tempGrayImg.getCvImage(), grayImg.getCvImage(), CV_MEDIAN, medianBlur);
        }

        if (gaussianBlur > 0) {
            if (gaussianBlur % 2 == 0) gaussianBlur++; // must be odd
            grayImg.blurGaussian(gaussianBlur);
        }

        // update the cv images
        grayImg.flagImageChanged();
        stencilImg.flagImageChanged();

        // find contours
        contourFinder.findContours(grayImg, minArea, maxArea, maxBlobs, bFindHoles, bUseApproximation);
    }
}

void KinectBlobTracker::setCameraTiltAngle(float & n_angle) {
    //if (n_angle>30)  n_angle=30;
    //if (n_angle<-30) n_angle=-30;
    kinect.setCameraTiltAngle(n_angle);
}

void KinectBlobTracker::grabMask() {
    maskImg = depthImg;

    // Add a bit to each value to pull the mask forward a bit, help deal with
    // noise in the kinect data.
    // TODO - Do this with cvAdd, could be faster?
    unsigned char* pix = maskImg.getPixels();
    int numPixels = maskImg.getWidth() * maskImg.getHeight();
    for ( int i=0; i<numPixels; ++i ) {
        pix[i] = ofClamp(pix[i]+extraMaskDepth, 0, 255);
    }
}

void KinectBlobTracker::clearMask() {
    maskImg.set(0);
}

bool KinectBlobTracker::loadMask(string filename) {
    ofFile maskfile(filename);
    if (!maskfile.exists())
        return false;

    ofImage loadImg;
    if (!loadImg.loadImage(filename))
        return false;
    ofxCvColorImage cvColorImg;
    cvColorImg.allocate(maskImg.width, maskImg.height);
    cvColorImg.setFromPixels(loadImg.getPixels(), maskImg.width, maskImg.height);
    maskImg = cvColorImg;
    return true;
}

void KinectBlobTracker::saveMask(string filename) {
    // Save the mask. CV images have no save so we convert to ofImage
    // Can't seem to do that for a cvGrayScale image so convert to color first.
    ofxCvColorImage cvColorImg;
    cvColorImg.allocate(maskImg.width, maskImg.height);
    cvColorImg = maskImg;
    ofImage saveImg;
    saveImg.setFromPixels(cvColorImg.getPixels(),
            cvColorImg.width, cvColorImg.height, OF_IMAGE_COLOR);
    saveImg.saveImage(filename);
}

void KinectBlobTracker::drawPointCloud() {
    int w = 640;
    int h = 480;
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_POINTS);
    int step = 2;
    for(int y = 0; y < h; y += step) {
        for(int x = 0; x < w; x += step) {
            if(kinect.getDistanceAt(x, y) > 0) {
                mesh.addColor(kinect.getColorAt(x,y));
                mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
            }
        }
    }
    glPointSize(3);
    ofPushMatrix();
    // the projected points are 'upside down' and 'backwards'
    ofScale(1, -1, -1);
    ofTranslate(0, 0, -1000); // center the points a bit
    glEnable(GL_DEPTH_TEST);
    mesh.drawVertices();
    glDisable(GL_DEPTH_TEST);
    ofPopMatrix();
}

void KinectBlobTracker::close() {
    kinect.close();
}

#include "kinectGuiApp.h"

//--------------------------------------------------------------
void kinectGuiApp::setup(){
    ofSetWindowTitle("kinectGui");
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofEnableSmoothing();

    colorImg.allocate(kinect.width, kinect.height);
    depthImg.allocate(kinect.width, kinect.height);
    grayImg.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    maskImg.allocate(kinect.width, kinect.height);
    stencilImg.allocate(kinect.width, kinect.height);
    tempGrayImg.allocate(kinect.width, kinect.height);

    maskFilename = "mask.png";

    // Starting the kinect after the gui seems to break loading xml settings
    // in setup, which breaks any future load and save. If you don't load xml
    // in setup you don't see the bug at all. Very strange.
    setupGui();
    loadSettings();
    startKinect();
}


void kinectGuiApp::setupGui() {
    // Note: The panels will get repositioned in windowResized
    guiApp.setup("KinectGui");
    guiApp.add( fpsSlider.setup("FPS", 60.0 + 10.0) );
    guiApp.add( showGui.setup("Show Gui", true) );
    guiApp.add( loadButton.setup("Load") );
    guiApp.add( saveButton.setup("Save") );
    loadButton.addListener(this, &kinectGuiApp::loadSettings);
    saveButton.addListener(this, &kinectGuiApp::saveSettings);

    guiKinect.setup("Kinect");
    guiKinect.add( kinectId.setup("kinectId", "Connecting...") );
    guiKinect.add( kinectAngle.setup("Angle", 0.0, -30.0, 30.0) );
    kinectAngle.addListener(this, &kinectGuiApp::setKinectAngle);
    guiKinect.add( kinectFlip.setup("H Flip Image", false) );
    guiKinect.add( nearThreshold.setup("Near", 255, 0, 255) );
    guiKinect.add( farThreshold.setup("Far", 0, 0, 255) );
    guiKinect.add( bThresholds.setup("Apply Threshold", true) );
    guiKinect.add( grabMaskButton.setup("Grab Mask") );
    grabMaskButton.addListener(this, &kinectGuiApp::grabMask);
    guiKinect.add( clearMaskButton.setup("Clear Mask") );
    clearMaskButton.addListener(this, &kinectGuiApp::clearMask);
    guiKinect.add( extraMaskDepth.setup("Extra Mask Depth", 0, 0, 100) );
    guiKinect.add( bMask.setup("Apply Mask", false) );
    guiKinect.add( medianBlur.setup("Median Blur", 0, 0, 100) );
    guiKinect.add( showPointCloud.setup("Point Cloud", true) );
    // Images
    // Hide the names and use toggles as labels on the images.
    guiKinect.add( colorImgGui.setup("Color", (ofImage*)&colorImg,false) );
    guiKinect.add( showColorImg.setup("RGB", false) );
    guiKinect.add( depthImgGui.setup("Depth", (ofImage*)&depthImg, false) );
    guiKinect.add( showDepthImg.setup("Depth", false) );
    guiKinect.add( maskImgGui.setup("Mask", (ofImage*)&maskImg, false) );
    guiKinect.add( showMaskImg.setup("Mask", false) );
    guiKinect.add( stencilImgGui.setup("Stencil", (ofImage*)&stencilImg, false) );
    guiKinect.add( showStencilImg.setup("Stencil", false) );
    guiKinect.add( grayImgGui.setup("Gray", (ofImage*)&grayImg, false) );
    guiKinect.add( showGrayImg.setup("Gray", false) );
}

void kinectGuiApp::startKinect() {
    // enable depth->video image calibration
    kinect.setRegistration(true);

    kinect.init();
    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)

    kinect.open();		// opens first available kinect
    //kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
    //kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #

    kinect.setCameraTiltAngle(kinectAngle);

    kinectId = "ID: " + ofToString(kinect.getDeviceId()) + " " + kinect.getSerial();;

    // Some time to settle the kinect.
    ofSleepMillis(1000);
}

//--------------------------------------------------------------
void kinectGuiApp::loadSettings() {
    guiApp.loadFromFile("settings.xml");
    guiKinect.loadFromFile("kinect.xml");

    // Load the mask, if it is there.
    ofFile maskfile(maskFilename);
    if (maskfile.exists()) {
        ofImage loadImg;
        loadImg.loadImage(maskFilename);
        ofxCvColorImage cvColorImg;
        cvColorImg.allocate(maskImg.width, maskImg.height);
        cvColorImg.setFromPixels(loadImg.getPixels(), maskImg.width, maskImg.height);
        maskImg = cvColorImg;
    }

}

void kinectGuiApp::saveSettings() {
    guiApp.saveToFile("settings.xml");
    guiKinect.saveToFile("kinect.xml");

    // Save the mask. CV images have no save so we convert to ofImage
    // Can't seem to do that for a cvGrayScale image so convert to color first.
    ofxCvColorImage cvColorImg;
    cvColorImg.allocate(maskImg.width, maskImg.height);
    cvColorImg = maskImg;
    ofImage saveImg;
    saveImg.setFromPixels(cvColorImg.getPixels(),
            cvColorImg.width, cvColorImg.height, OF_IMAGE_COLOR);
    saveImg.saveImage(maskFilename);
}

void kinectGuiApp::setKinectAngle(float & n_angle) {
    if (n_angle>30)  n_angle=30;
    if (n_angle<-30) n_angle=-30;
    kinect.setCameraTiltAngle(n_angle);
}

void kinectGuiApp::setNearThreshold(int n) {
    if (n>255) n=255;
    if (n<0)   n=0;
    nearThreshold = n;
}

void kinectGuiApp::setFarThreshold(int n) {
    if (n>255) n=255;
    if (n<0)   n=0;
    farThreshold = n;
}

void kinectGuiApp::grabMask() {
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

void kinectGuiApp::clearMask() {
    maskImg.set(0);
    ofFile maskfile(maskFilename);
    if (maskfile.exists())
        maskfile.remove();
}

//--------------------------------------------------------------
void kinectGuiApp::update(){
    // there is a new frame and we are connected
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
            if (medianBlur % 2 == 0)
                medianBlur++; // must be odd
            tempGrayImg = grayImg;
            cvSmooth(tempGrayImg.getCvImage(), grayImg.getCvImage(), CV_MEDIAN, medianBlur);
        }

        // update the cv images
        grayImg.flagImageChanged();
        stencilImg.flagImageChanged();

        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayImg, 10, (kinect.width*kinect.height)/2, 20, false);
    }
}

//--------------------------------------------------------------
void kinectGuiApp::draw(){
    ofBackgroundGradient(ofColor::white, ofColor::gray);

    drawKinectImages();

    if (showPointCloud) {
        easyCam.begin();
        drawPointCloud();
        easyCam.end();
    }

    if (showGui) {
        guiApp.draw();
        guiKinect.draw();
    }
}

void kinectGuiApp::drawPointCloud() {
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

void kinectGuiApp::drawKinectImages() {
    vector<ofxCvImage*> images;
    if (showColorImg)   images.push_back(&colorImg);
    if (showDepthImg)   images.push_back(&depthImg);
    if (showMaskImg)    images.push_back(&maskImg);
    if (showStencilImg) images.push_back(&stencilImg);
    if (showGrayImg)    images.push_back(&grayImg);
    int numImg = images.size();

    int ww = ofGetWidth();
    int wh = ofGetHeight();
    ofRectangle rects[4];
    if (numImg == 1) {
        rects[0] = ofRectangle(0,0,ww,wh);
    }
    else if (numImg < 5) { // 2 - 4
        int w = ww/2;
        int h = wh/2;
        rects[0] = ofRectangle(0,0,w,h);
        rects[1] = ofRectangle(w,0,w,h);
        rects[2] = ofRectangle(0,h,w,h);
        rects[3] = ofRectangle(w,h,w,h);
    }
    else {
        // 4 max!
        numImg = 4;
    }

    for (int i = 0; i < numImg; ++i)
        images[i]->draw(rects[i]);
}

//--------------------------------------------------------------
void kinectGuiApp::keyPressed(int key){
    if( key == 'h' ){
        showGui = !showGui;
	}
	if(key == 's') {
	    saveSettings();
	}
	if(key == 'l') {
	    loadSettings();
	}
	if(key == 'g') {
	    grabMask();
	}
}

//--------------------------------------------------------------
void kinectGuiApp::keyReleased(int key){

}

//--------------------------------------------------------------
void kinectGuiApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void kinectGuiApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void kinectGuiApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void kinectGuiApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void kinectGuiApp::windowResized(int w, int h){
    guiApp.setPosition(ofGetWidth()-guiApp.getShape().width-10, 10);
    guiKinect.setPosition(10,10);
}

//--------------------------------------------------------------
void kinectGuiApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void kinectGuiApp::dragEvent(ofDragInfo dragInfo){

}

//--------------------------------------------------------------
void kinectGuiApp::exit() {
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();

    loadButton.removeListener(this, &kinectGuiApp::loadSettings);
    saveButton.removeListener(this, &kinectGuiApp::saveSettings);
    kinectAngle.removeListener(this, &kinectGuiApp::setKinectAngle);
    grabMaskButton.removeListener(this, &kinectGuiApp::grabMask);
}



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

    // Starting the kinect after the gui seems to break loading xml settings
    // in setup, which breaks any future load and save. If you don't load xml
    // in setup you don't see the bug at all. Very strange.
    setupGui();
    loadSettings();
    startKinect();
}


void kinectGuiApp::setupGui() {
    guiApp.setup("KinectGui");
    guiApp.add( fpsSlider.setup("FPS", 60.0 + 10.0) );
    guiApp.add( showGui.setup("Show Gui", true) );
    guiApp.add( loadButton.setup("Load") );
    guiApp.add( saveButton.setup("Save") );
    loadButton.addListener(this, &kinectGuiApp::loadSettings);
    saveButton.addListener(this, &kinectGuiApp::saveSettings);

    guiKinect.setup("Kinect");
    guiKinect.setPosition(guiApp.getShape().width+guiApp.getPosition().x+10.0, 10.0);
    guiKinect.add( kinectId.setup("kinectId", "Connecting...") );
    guiKinect.add( kinectAngle.setup("Angle", 0.0, -30.0, 30.0) );
    kinectAngle.addListener(this, &kinectGuiApp::setKinectAngle);
    guiKinect.add( kinectFlip.setup("H Flip Image", false) );
    guiKinect.add( nearThreshold.setup("Near", 255, 0, 255) );
    guiKinect.add( farThreshold.setup("Far", 0, 0, 255) );
    guiKinect.add( bThresholds.setup("Apply Threshold", true) );
    guiKinect.add( grabMaskButton.setup("Grab Mask") );
    grabMaskButton.addListener(this, &kinectGuiApp::grabMask);
    guiKinect.add( extraMaskDepth.setup("Extra Mask Depth", 0, 0, 100) );
    guiKinect.add( bMask.setup("Apply Mask", false) );
    // Images
    guiKinect.add( colorImgGui.setup("Color Image", (ofImage*)&colorImg) );
    guiKinect.add( depthImgGui.setup("Depth Image", (ofImage*)&depthImg) );
    guiKinect.add( maskImgGui.setup("Mask", (ofImage*)&maskImg) );
    guiKinect.add( stencilImgGui.setup("Stencil", (ofImage*)&stencilImg) );
    guiKinect.add( grayImgGui.setup("Gray Image", (ofImage*)&grayImg) );
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
}

void kinectGuiApp::saveSettings() {
    guiApp.saveToFile("settings.xml");
    guiKinect.saveToFile("kinect.xml");
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
    easyCam.begin();
    drawPointCloud();
    easyCam.end();

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



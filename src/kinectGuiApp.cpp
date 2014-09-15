#include "kinectGuiApp.h"

//--------------------------------------------------------------
void kinectGuiApp::setup(){
    ofSetWindowTitle("kinectGui");
    ofSetLogLevel(OF_LOG_VERBOSE);
    //ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofEnableSmoothing();

    maskFilename = "mask.png";
    bgColor1.set("bgColor1", ofColor(200,200,200),ofColor(0,0),ofColor(255,255));
    bgColor2.set("bgColor2", ofColor(23,23,23),ofColor(0,0),ofColor(255,255));

    // Starting the kinect after the gui seems to break loading xml settings
    // in setup, which breaks any future load and save. If you don't load xml
    // in setup you don't see the bug at all. Very strange.
    kinect.setup();
    setupGui();
    loadSettings();
    // XXX - Access to serials when multi kinect.
    kinectId = ofToString(kinect.kinect.getDeviceId()) + " " + kinect.kinect.getSerial();;
    // Some time to settle the kinect.
    ofSleepMillis(1000);
}


void kinectGuiApp::setupGui() {
    ofxGuiSetHeaderColor( ofColor(100) );    // param group headers
    ofxGuiSetBackgroundColor( ofColor(60) ); // bg of params (sliders and stuff but not buttons)
    ofxGuiSetBorderColor( ofColor(200) );     // bg of param groups (but not panels)
    ofxGuiSetFillColor( ofColor(175,145,0) ); // Fill on slider bars etc
    ofxGuiSetTextColor( ofColor::white );
    ofxGuiSetFont("verdana.ttf", 10);

    // Note: The panels will get repositioned in windowResized
    guiApp.setup("KinectGui");
    guiApp.add( fpsSlider.setup("FPS", 60) );
    guiApp.add( loadButton.setup("Load") );
    guiApp.add( saveButton.setup("Save") );
    guiApp.add( grabMaskButton.setup("Grab Mask") );
    guiApp.add( clearMaskButton.setup("Clear Mask") );
    appParams.setName("Display");
    appParams.add( showGui.set("Show Gui", true) );
    appParams.add( showPointCloud.set("Show Point Cloud", true) );
    appParams.add( showColorImg.set("RGB", false) );
    appParams.add( showDepthImg.set("Depth", false) );
    appParams.add( showMaskImg.set("Mask", false) );
    appParams.add( showStencilImg.set("Stencil", false) );
    appParams.add( showGrayImg.set("Gray", false) );
    appParams.add( showBlobs.set("Show Blobs", false) );
    appParams.add( bgColor1 );
    appParams.add( bgColor2 );
    guiApp.add( appParams );
    guiApp.add( status.setup("Status","") );
    loadButton.addListener(this, &kinectGuiApp::loadSettings);
    saveButton.addListener(this, &kinectGuiApp::saveSettings);
    grabMaskButton.addListener(this, &kinectGuiApp::grabMask);
    clearMaskButton.addListener(this, &kinectGuiApp::clearMask);

    guiKinect.setup("Kinect");
    guiKinect.add( kinectId.setup("ID", "Connecting...") );
    kinectParams.setName("Settings");
    kinectParams.add( kinect.kinectAngle );
    kinectParams.add( kinect.kinectFlip );
    kinectParams.add( kinect.nearThreshold );
    kinectParams.add( kinect.farThreshold );
    kinectParams.add( kinect.bThresholds );
    kinectParams.add( kinect.extraMaskDepth );
    kinectParams.add( kinect.bMask );
    guiKinect.add( kinectParams );

    guiBlobs.setup("Blobs");
    blobParams.add( kinect.medianBlur );
    blobParams.add( kinect.gaussianBlur );
    blobParams.add( kinect.minArea );
    blobParams.add( kinect.maxArea );
    blobParams.add( kinect.maxBlobs );
    blobParams.add( kinect.bFindHoles );
    blobParams.add( kinect.bUseApproximation );
    blobParams.add( kinect.simplify );
    blobParams.add( kinect.resampleSpacing );
    blobParams.add( kinect.resampleCount );
    blobParams.add( kinect.showInfo );
    blobParams.add( kinect.showVerts );
    blobParams.add( kinect.lineWidth );
    blobParams.add( kinect.lineColor );
    blobParams.add( kinect.bFill );
    guiBlobs.add( blobParams );

    // Images
    // Hide the names and use toggles as labels on the images.
    guiImages.setup("Images");
    //guiImages.setSize(100,100);
    ofxGuiGroup * guiImgGroup = new ofxGuiGroup();
    guiImgGroup->setup("");
    //guiImgGroup->setSize(100,18);
    int imgWidth = 200;
    guiImgGroup->add( colorImgGui.setup("Color", (ofImage*)&kinect.colorImg, false, imgWidth) );
    guiImgGroup->add( depthImgGui.setup("Depth", (ofImage*)&kinect.depthImg, false) );
    guiImgGroup->add( maskImgGui.setup("Mask", (ofImage*)&kinect.maskImg, false) );
    guiImgGroup->add( stencilImgGui.setup("Stencil", (ofImage*)&kinect.stencilImg, false) );
    guiImgGroup->add( grayImgGui.setup("Gray", (ofImage*)&kinect.grayImg, false) );
    guiImages.add( guiImgGroup );
}

//--------------------------------------------------------------
void kinectGuiApp::loadSettings() {
    guiApp.loadFromFile("settings.xml");
    guiKinect.loadFromFile("kinect.xml");
    guiBlobs.loadFromFile("blobs.xml");
    kinect.loadMask(maskFilename);
}

void kinectGuiApp::saveSettings() {
    guiApp.saveToFile("settings.xml");
    guiKinect.saveToFile("kinect.xml");
    guiBlobs.saveToFile("blobs.xml");
    kinect.saveMask(maskFilename);
}

void kinectGuiApp::grabMask() {
    kinect.grabMask();
}

void kinectGuiApp::clearMask() {
    kinect.clearMask();
    ofFile maskfile(maskFilename);
    if (maskfile.exists())
        maskfile.remove();
}

//--------------------------------------------------------------
void kinectGuiApp::update(){
    kinect.update();
}

//--------------------------------------------------------------
void kinectGuiApp::draw(){
    ofBackgroundGradient(bgColor1, bgColor2);

    drawKinectImages();

    if (showPointCloud) {
        easyCam.begin();
        kinect.drawPointCloud();
        easyCam.end();
    }

    if (showBlobs) {
        kinect.drawBlobs(0,0,ofGetWidth(),ofGetWindowHeight());
    }

    if (showGui) {
        guiApp.draw();
        guiKinect.draw();
        guiBlobs.draw();
        guiImages.draw();
    }
}

void kinectGuiApp::drawKinectImages() {
    vector<ofxCvImage*> images;
    if (showColorImg)   images.push_back(&kinect.colorImg);
    if (showDepthImg)   images.push_back(&kinect.depthImg);
    if (showMaskImg)    images.push_back(&kinect.maskImg);
    if (showStencilImg) images.push_back(&kinect.stencilImg);
    if (showGrayImg)    images.push_back(&kinect.grayImg);
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
    if( key == 'f' ){
        ofToggleFullscreen();
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
    status = ofToString(w) + "x" + ofToString(h);
    guiApp.setPosition(ofGetWidth()-guiApp.getShape().width-10, 10);
    guiImages.setPosition(10,10);
    guiKinect.setPosition(10+guiImages.getShape().width+10,10);
    guiBlobs.setPosition(10+guiKinect.getPosition().x+guiKinect.getShape().width+10,10);

}

//--------------------------------------------------------------
void kinectGuiApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void kinectGuiApp::dragEvent(ofDragInfo dragInfo){

}

//--------------------------------------------------------------
void kinectGuiApp::exit() {
//    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();

    loadButton.removeListener(this, &kinectGuiApp::loadSettings);
    saveButton.removeListener(this, &kinectGuiApp::saveSettings);
    grabMaskButton.removeListener(this, &kinectGuiApp::grabMask);
}



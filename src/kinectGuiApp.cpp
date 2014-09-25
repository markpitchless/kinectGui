#include "kinectGuiApp.h"

//--------------------------------------------------------------
void kinectGuiApp::setup(){
    ofSetWindowTitle("kinectGui");
    //ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofEnableSmoothing();

    maskFilename = "mask.png";
    bgColor1.set("bgColor1", ofColor(200,200,200),ofColor(0,0),ofColor(255,255));
    bgColor2.set("bgColor2", ofColor(23,23,23),ofColor(0,0),ofColor(255,255));

    // Video
    iCurVideo = 0;
    showVideo.set("Show Video", true);
    loadVideoDir("video");
    //playVideo();
    //pauseVideo();

    showMain.set("Show Main", true);
    mainAlpha.set("Main Alpha", 100, 0, 255);
    mainHue.set("Main Hue", 255, 0, 255);
    mainSaturation.set("Main Saturation", 255, 0, 255);
    imgMain.allocate(kinect.kinect.width, kinect.kinect.height, OF_IMAGE_COLOR_ALPHA);

    // Midi
    // print ports to console
	midiIn.listPorts(); // via instance
	//ofxMidiIn::listPorts(); // via static as well

	// open port by number (you may need to change this)
	midiIn.openPort(1);
	//midiIn.openPort("IAC Pure Data In");	// by name
	//midiIn.openVirtualPort("ofxMidiIn Input");	// open a virtual port

	// don't ignore sysex, timing, & active sense messages,
	// these are ignored by default
	midiIn.ignoreTypes(false, false, false);

	// add app as a listener
	midiIn.addListener(this);

	// print received messages to the console
	midiIn.setVerbose(true);

    joyAxisLeftX  = 0.0;
    joyAxisLeftY  = 0.0;
    joyAxisRightX = 0.0;
    joyAxisRightY = 0.0;
    showJoystick.set("Show Joystick", false);
    joyDeadzone.set("Joystick Deadzone", 0.1, 0.0, 1.0);
	ofxGamepadHandler::get()->enableHotplug();
	//CHECK IF THERE EVEN IS A GAMEPAD CONNECTED
	if(ofxGamepadHandler::get()->getNumPads()>0){
        ofxGamepad* pad = ofxGamepadHandler::get()->getGamepad(0);
        ofAddListener(pad->onAxisChanged, this, &kinectGuiApp::axisChanged);
        ofAddListener(pad->onButtonPressed, this, &kinectGuiApp::buttonPressed);
        ofAddListener(pad->onButtonReleased, this, &kinectGuiApp::buttonReleased);
	}

    kinect.setup();
    setupGui();
    loadSettings();
    kinect.connect();

    cueVideo(0);
}

//--------------------------------------------------------------
void kinectGuiApp::newMidiMessage(ofxMidiMessage& msg) {
	// make a copy of the latest message
	midiMessage = msg;

    stringstream text;
    // draw the last recieved message contents to the screen
	text << "Received: " << ofxMidiMessage::getStatusString(midiMessage.status);
	text << " channel: " << midiMessage.channel;
	text << " pitch: " << midiMessage.pitch;
	text << " velocity: " << midiMessage.velocity;
	text << " control: " << midiMessage.control;
	text << " value: " << midiMessage.value;

//	if(midiMessage.status == MIDI_PITCH_BEND) {
//		ofRect(20, 202, ofMap(midiMessage.value, 0, MIDI_MAX_BEND, 0, ofGetWidth()-40), 20);
//	}
//	else {
//		ofRect(20, 202, ofMap(midiMessage.value, 0, 127, 0, ofGetWidth()-40), 20);
//	}

	text << "delta: " << midiMessage.deltatime;
    ofLogNotice() << "MIDI: " << text.str();

    if (msg.channel == 10 && msg.velocity > 0 && msg.pitch == 60) {
        cueNextVideo();
    }
    if (msg.channel == 10 && msg.velocity > 0 && msg.pitch == 44) {
        togglePlayVideo();
    }
}

void kinectGuiApp::loadVideoDir(string dirname) {
    ofLogNotice() << "Loading video from: " << dirname;
    iCurVideo = 0;
    ofDirectory dir(dirname);
    dir.listDir();
    if (dir.numFiles() == 0) {
        videos.resize(1); // at least one blank video
        ofLogNotice() << "No video found, added default blank video.";
        return;
    }
    //videos.resize(dir.numFiles());
    vector<string> names;
    for (size_t i=0; i < dir.numFiles(); i++) {
        names.push_back(dir.getPath(i));
    }
    sort(names.begin(), names.end());

    int num_loaded = 0;
    for (size_t i=0; i < names.size(); i++) {
        if ( addVideo(names[i]) ) { num_loaded++; }
    }
    ofLogNotice() << "Loaded " << num_loaded << " video(s) in: " << dirname;
}

ofVideoPlayer& kinectGuiApp::getCurVideo(){
    return videos[iCurVideo];
}

bool kinectGuiApp::addVideo(string filename) {
    ofVideoPlayer vid;
    ofLogNotice() << "Loading movie: " << filename;
    //videoPlayer.setPixelFormat(OF_PIXELS_RGBA);
    if (!vid.loadMovie(filename)) {
        ofLogError() << "Failed to load movie: " << filename;
        return false;
    }
    vid.setLoopState(OF_LOOP_NONE);
    videos.push_back(vid);
    return true;
}

void kinectGuiApp::playVideo() {
    getCurVideo().play();
    ofLogNotice() << "Playing video: " << getCurVideo().getMoviePath();
}

void kinectGuiApp::togglePlayVideo() {
    ofVideoPlayer vid = getCurVideo();
    if ( vid.isPaused() ) { playVideo(); }
    else { pauseVideo(); }
}

void kinectGuiApp::pauseVideo() {
    getCurVideo().setPaused(true);
    ofLogNotice() << "Pause video: " << getCurVideo().getMoviePath();
}

void kinectGuiApp::cueNextVideo() {
    //getCurVideo().stop();
    //iCurVideo++;
    int num = iCurVideo + 1;
    //if ( iCurVideo > videos.size()-1 ) { iCurVideo = 0; }
    if ( num > videos.size()-1 ) { num = 0; }
    //videos[iCurVideo].play();
    //videos[iCurVideo].setPaused(true);
    //videos[iCurVideo].firstFrame();
    //ofLogNotice() << "Cue video: " << getCurVideo().getMoviePath();
    cueVideo(num);
}

void kinectGuiApp::cueVideo(int num) {
    if ( num < 0 || num > videos.size()-1 ) {
        ofLogWarning() << "Attempt to cue unknown video: " << num;
        return;
    }
    getCurVideo().stop();
    iCurVideo = num;
    videos[iCurVideo].play();
    videos[iCurVideo].setPaused(true);
    videos[iCurVideo].firstFrame();
    ofLogNotice() << "Cue video: " << getCurVideo().getMoviePath();
    showBlobs = false;
    showMain = false;
    if (num == 0) {
        kinect.lineColor.set(ofColor(0,0,10,32));
    }
    else if (num == 1) {
        kinect.lineColor.set(ofColor(0,201,0,50));
    }
    else if (num == 2) {
        kinect.lineColor.set(ofColor(0,201,0,50));
    }
    else if (num == 4) {
        kinect.lineColor.set(ofColor(10,0,23,23));
    }
    else {
        kinect.lineColor.set(ofColor(255,255,255,23));
    }
}

void kinectGuiApp::playNextVideo(){
    cueNextVideo();
    playVideo();
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
    guiApp.add( reConnectButton.setup("Connect") );
    guiApp.add( loadButton.setup("Load") );
    guiApp.add( saveButton.setup("Save") );
    guiApp.add( grabMaskButton.setup("Grab Mask") );
    guiApp.add( clearMaskButton.setup("Clear Mask") );
    guiApp.add( playVideoButton.setup("Play Video") );
    guiApp.add( pauseVideoButton.setup("Pause Video") );
    guiApp.add( cueNextVideoButton.setup("Cue Next Video") );
    guiApp.add( nextVideoButton.setup("Play Next Video") );
    appParams.setName("Display");
    appParams.add( showGui.set("Show Gui", true) );
    appParams.add( showJoystick );
    appParams.add( joyDeadzone );
    appParams.add( showPointCloud.set("Show Point Cloud", true) );
    appParams.add( showColorImg.set("RGB", false) );
    appParams.add( showDepthImg.set("Depth", false) );
    appParams.add( showMaskImg.set("Mask", false) );
    appParams.add( showStencilImg.set("Stencil", false) );
    appParams.add( showGrayImg.set("Gray", false) );
    appParams.add( showBlobs.set("Show Blobs", false) );
    appParams.add( showVideo );
    appParams.add( showMain );
    appParams.add( mainAlpha );
    appParams.add( mainHue );
    appParams.add( mainSaturation );
    appParams.add( bgColor1 );
    appParams.add( bgColor2 );
    guiApp.add( appParams );
    guiApp.add( status.setup("Status","") );
    reConnectButton.addListener(this, &kinectGuiApp::connect);
    loadButton.addListener(this, &kinectGuiApp::loadSettings);
    saveButton.addListener(this, &kinectGuiApp::saveSettings);
    grabMaskButton.addListener(this, &kinectGuiApp::grabMask);
    clearMaskButton.addListener(this, &kinectGuiApp::clearMask);
    playVideoButton.addListener(this, &kinectGuiApp::playVideo);
    pauseVideoButton.addListener(this, &kinectGuiApp::pauseVideo);
    cueNextVideoButton.addListener(this, &kinectGuiApp::cueNextVideo);
    nextVideoButton.addListener(this, &kinectGuiApp::playNextVideo);

    guiKinect.setup("Kinect");
    ofxGuiGroup * guiKinectGroup = new ofxGuiGroup();
    guiKinectGroup->setup("");
    // Open settings
    connectionParams.setName("Connection");
    connectionParams.add( kinect.deviceId );
    connectionParams.add( kinect.serial );
    connectionParams.add( kinect.bDepthRegistration );
    connectionParams.add( kinect.bVideo );
    connectionParams.add( kinect.bInfrared );
    connectionParams.add( kinect.bTexture );
    guiKinectGroup->add( connectionParams );

    // Run time settings
    kinectParams.setName("Settings");
    kinectParams.add( kinect.kinectAngle );
    kinectParams.add( kinect.kinectFlip );
    kinectParams.add( kinect.nearThreshold );
    kinectParams.add( kinect.farThreshold );
    kinectParams.add( kinect.bThresholds );
    kinectParams.add( kinect.nearClip );
    kinectParams.add( kinect.farClip );
    kinectParams.add( kinect.extraMaskDepth );
    kinectParams.add( kinect.bMask );
    guiKinectGroup->add( kinectParams );

    blobParams.setName("Blobs");
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
    guiKinectGroup->add(blobParams);

    guiKinect.add( guiKinectGroup );

    // Images
    // Hide the names and use toggles as labels on the images.
    guiImages.setup("Images");
    //guiImages.setSize(100,100);
    ofxGuiGroup * guiImgGroup = new ofxGuiGroup();
    guiImgGroup->setup("");
    //guiImgGroup->setSize(100,18);
    int imgWidth = 200;
    guiImgGroup->add( colorImgGui.setup("Color", (ofImage*)&kinect.colorImg, true, imgWidth) );
    guiImgGroup->add( depthImgGui.setup("Depth", (ofImage*)&kinect.depthImg, true) );
    guiImgGroup->add( maskImgGui.setup("Mask", (ofImage*)&kinect.maskImg, true) );
    guiImgGroup->add( stencilImgGui.setup("Stencil", (ofImage*)&kinect.stencilImg, true) );
    guiImgGroup->add( grayImgGui.setup("Gray", (ofImage*)&kinect.grayImg, true) );
    guiImages.add( guiImgGroup );
}

void kinectGuiApp::connect() { kinect.reConnect(); }

//--------------------------------------------------------------
void kinectGuiApp::loadSettings() {
    guiApp.loadFromFile("settings.xml");
    guiKinect.loadFromFile("kinect.xml");
    kinect.loadMask(maskFilename);
}

void kinectGuiApp::saveSettings() {
    guiApp.saveToFile("settings.xml");
    guiKinect.saveToFile("kinect.xml");
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
    getCurVideo().update();
    kinect.update();

    if (joyAxisLeftY != 0) {
        ofColor c = kinect.lineColor.get();
        int foo = -4*joyAxisLeftY;
        //ofLogNotice() << "joy: " << joyAxisLeftY << " foo: " << foo;
        //c[3] += int(-2*joyAxisLeftY); // -1 reverse negative is up on stick
        c[3] = ofClamp(c[3] + foo, 0, 255);
        //c.clamp();
        kinect.lineColor.set(c);

        mainAlpha = ofClamp(mainAlpha+foo, 0, 255);
    }
    //if (joyAxisRightX != 0) {
    //    mainHue = ofClamp(mainHue+(-4*joyAxisRightX), 0, 255);
    //}
    //if (joyAxisRightY != 0) {
    //    mainSaturation = ofClamp(mainSaturation+(-4*joyAxisRightY), 0, 255);
    //}

    // Copy the kinect grey image into our video layer
    unsigned char* newPix = imgMain.getPixels();
    unsigned char* pix = kinect.grayImg.getPixels();
    int numPix = kinect.grayImg.getWidth() * kinect.grayImg.getHeight();
    ofColor col(0,0,0,0);
    ofColor blank(0,0,0,0);
    for ( int i=0; i<numPix; i++ ) {
        if ( pix[i] < 1 ) {
            newPix[i*4]   = 0;
            newPix[i*4+1] = 0;
            newPix[i*4+2] = 0;
            newPix[i*4+3] = 0;
        }
        else {
            int val = pix[i];
            //ofColor newcol = ofColor(val, mainAlpha);
            ofColor newcol = ofColor::fromHsb(mainHue, mainSaturation, pix[i], mainAlpha);
            //col.r = pix[i];
            //col.g = pix[i];
            //col.b = pix[i];
            //col[4] = mainAlpha;
            //imgMain.setColor(i,col);
            //newPix[i*4]   = pix[i];
            //newPix[i*4+1] = pix[i];
            //newPix[i*4+2] = pix[i];
            //newPix[i*4+3] = mainAlpha
            //newPix[i*4]   = col.r;
            //newPix[i*4+1] = col.g;
            //newPix[i*4+2] = col.b;
            //newPix[i*4+3] = col[4];
            newPix[i*4]   = newcol.r;
            newPix[i*4+1] = newcol.g;
            newPix[i*4+2] = newcol.b;
            newPix[i*4+3] = newcol[3];
            //newPix[i*4+3] = mainAlpha;
        }
    }
    imgMain.update();
}

//--------------------------------------------------------------
void kinectGuiApp::draw(){
    float w = ofGetWidth();
    float h = ofGetWindowHeight();

    ofBackgroundGradient(bgColor1, bgColor2);

    if (showVideo)
        getCurVideo().draw(0,0,w,h);

    drawKinectImages();

    if (showPointCloud) {
        easyCam.begin();
        kinect.drawPointCloud();
        easyCam.end();
    }

    if (showMain)
        imgMain.draw(0,0,w,h);

    if (showBlobs) {
        kinect.drawBlobs(0,0,w,h);
    }

    if (showJoystick)
        ofxGamepadHandler::get()->draw(42,80);

    if (showGui) {
        guiApp.draw();
        guiKinect.draw();
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
    if (key == 'H') { showGui = !showGui; }
    if (key == 'F') { ofToggleFullscreen(); }
    if (key == 'S') { saveSettings(); }
    if (key == 'L') { loadSettings(); }
    if (key == 'G') { grabMask(); }
    if (key == 'p') { playVideo(); }
    if (key == 'P') { pauseVideo(); }
    //if (key == ' ') { togglePlayVideo(); }
    if (key == ' ') { showBlobs = false; showMain = false; }
    if (key == 'C') { cueNextVideo(); }
    if (key == 'N') { playNextVideo(); }
    if (key == '1') { cueVideo(0); }
    if (key == '2') { cueVideo(1); }
    if (key == '3') { cueVideo(2); }
    if (key == '4') { cueVideo(3); }
    if (key == '5') { cueVideo(4); }
    if (key == '6') { cueVideo(5); }
    if (key == '7') { cueVideo(6); }
    if (key == '8') { cueVideo(7); }
    if (key == '9') { cueVideo(8); }
    if (key == '0') { cueVideo(9); }
    else if (key == 'g') { kinect.lineColor.set(ofColor(0,230,0,32)); }
    else if (key == 'b') { kinect.lineColor.set(ofColor(0,0,200,32)); }
    else if (key == 'y') { kinect.lineColor.set(ofColor(200,200,0,32)); }
    else if (key == 'm') { showBlobs = false; showMain = true; }
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
// ofxGamepad events

void kinectGuiApp::axisChanged(ofxGamepadAxisEvent& e) {
	//ofLogNotice() << "AXIS " << e.axis << " VALUE " << ofToString(e.value) << endl;
    float val = e.value;
    if ( !(val > joyDeadzone || val < -joyDeadzone) ) {
        val = 0.0;
    }
	if ( e.axis == 0 ) { joyAxisLeftX  = val; }
    if ( e.axis == 1 ) { joyAxisLeftY  = val; }
    if ( e.axis == 3 ) { joyAxisRightX = val; }
    if ( e.axis == 4 ) { joyAxisRightY = val; }
}

void kinectGuiApp::buttonPressed(ofxGamepadButtonEvent& e) {
	//ofLogNotice() << "BUTTON " << e.button << " PRESSED" << endl;
	if (e.button == 1) { showBlobs = !showBlobs; } // B
	if (e.button == 3) {
        kinect.bFill = !kinect.bFill;
        if (kinect.bFill && kinect.lineColor.get()[3] > 100) {
            ofColor c = kinect.lineColor.get();
            c[3] = 100;
            kinect.lineColor.set(c);
        }
    }
    else if (e.button == 4) { kinect.lineColor.set(ofColor(255,255,255,32)); } // left shoulder
    else if (e.button == 5) { kinect.lineColor.set(ofColor(0,0,0,32)); } // left shoulder
}

void kinectGuiApp::buttonReleased(ofxGamepadButtonEvent& e) {
	//ofLogNotice() << "BUTTON " << e.button << " RELEASED" << endl;
}

//--------------------------------------------------------------
void kinectGuiApp::windowResized(int w, int h){
    status = ofToString(w) + "x" + ofToString(h);
    guiApp.setPosition(ofGetWidth()-guiApp.getShape().width-10, 10);
    guiImages.setPosition(10,10);
    guiKinect.setPosition(10+guiImages.getShape().width+10,10);
    guiImages.minimizeAll();
    guiKinect.minimizeAll();

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

    // clean up midi
	midiIn.closePort();
	midiIn.removeListener(this);

}



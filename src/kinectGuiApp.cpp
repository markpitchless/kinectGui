#include "kinectGuiApp.h"

//--------------------------------------------------------------
void kinectGuiApp::setup(){
    ofSetWindowTitle("kinectGui");
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(60);
	ofEnableAlphaBlending();
	ofEnableSmoothing();

    setupKinect();
    setupGui();
}


void kinectGuiApp::setupKinect() {
	// enable depth->video image calibration
	kinect.setRegistration(true);

	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)

	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #

	colorImg.allocate(kinect.width, kinect.height);
	depthImage.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);

	nearThreshold = 230;
	farThreshold = 70;
	bThreshWithOpenCV = true;

	// zero the tilt on startup
	kinect.setCameraTiltAngle(kinectAngle);

	// start from the front
	bDrawPointCloud = false;
}

void kinectGuiApp::setupGui() {
    guiApp.setup("KinectGui");
    guiApp.add( fpsSlider.setup("FPS", 60.0 + 10.0) );
    guiApp.add( showGui.setup("Show Gui", true) );
    guiApp.loadFromFile("settings.xml");
    guiKinect.setup("Kinect");
    guiKinect.setPosition(guiApp.getShape().width+guiApp.getPosition().x+10.0, 10.0);
    guiKinect.add( kinectAngle.setup("Angle", 0.0, -30.0, 30.0) );
    kinectAngle.addListener(this, &kinectGuiApp::setKinectAngle);
    guiKinect.add( kinectFlip.setup("H Flip Image", false) );
    guiKinect.add( colorImageGui.setup("Color Image", (ofImage*)&colorImg) );
    guiKinect.add( depthImageGui.setup("Depth Image", (ofImage*)&depthImage) );
    guiKinect.add( grayImageGui.setup("Gray Image", (ofImage*)&grayImage) );
    guiKinect.loadFromFile("settings.xml");
}

//--------------------------------------------------------------
void kinectGuiApp::setKinectAngle(float & n_angle) {
    if (n_angle>30)  n_angle=30;
    if (n_angle<-30) n_angle=-30;
    kinect.setCameraTiltAngle(n_angle);
}

void kinectGuiApp::setNearThreshold(int n) {
    cout << "near: " << n << endl;
    if (n>255) n=255;
    if (n<0)   n=0;
    nearThreshold = n;
}

void kinectGuiApp::setFarThreshold(int n) {
    cout << "far: " << n << endl;
    if (n>255) n=255;
    if (n<0)   n=0;
    farThreshold = n;
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
        depthImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        if (kinectFlip)
            depthImage.mirror(false,true);
        grayImage = depthImage;

        // we do two thresholds - one for the far plane and one for the near plane
        // we then do a cvAnd to get the pixels which are a union of the two thresholds
        if(bThreshWithOpenCV) {
            grayThreshNear = grayImage;
            grayThreshFar = grayImage;
            grayThreshNear.threshold(nearThreshold, true);
            grayThreshFar.threshold(farThreshold);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        } else {

            // or we do it ourselves - show people how they can work with the pixels
            unsigned char * pix = grayImage.getPixels();

            int numPixels = grayImage.getWidth() * grayImage.getHeight();
            for(int i = 0; i < numPixels; i++) {
                if(pix[i] < nearThreshold && pix[i] > farThreshold) {
                    pix[i] = 255;
                } else {
                    pix[i] = 0;
                }
            }
        }

        // update the cv images
        grayImage.flagImageChanged();

        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
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
		guiKinect.saveToFile("settings.xml");
	}
	if(key == 'l') {
		guiKinect.loadFromFile("settings.xml");
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
}



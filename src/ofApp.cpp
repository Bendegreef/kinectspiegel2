#include "ofApp.h"
using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup() {
	kinect.open();
	kinect.initColorSource();
	kinect.initBodySource();
	kinect.initDepthSource();
	ofEnableSmoothing();
	ofSetVerticalSync(true);
	//offset = -0.24;
	usePreview = false;

	previewCamera.setDistance(3.0f);
	previewCamera.setNearClip(0.01f);
	previewCamera.setFarClip(500.0f);
	previewCamera.setPosition(0.4f, 0.2f, 0.8f);
	previewCamera.lookAt(ofVec3f(0.0f, 0.0f, 0.0f));

	headTrackedCamera.setNearClip(0.01f);
	headTrackedCamera.setFarClip(1000.0f);

	windowWidth = 0.3f;
	windowHeight = 0.2f;

	windowTopLeft = ofVec3f(-windowWidth / 2.0f,
		+windowHeight / 2.0f,
		0.0f);
	windowBottomLeft = ofVec3f(-windowWidth / 2.0f,
		-windowHeight / 2.0f,
		0.0f);
	windowBottomRight = ofVec3f(+windowWidth / 2.0f,
		-windowHeight / 2.0f,
		0.0f);

	gui.setup();
	gui.add(yOffsetSlider.setup("yOffset", 0.315, -0.5, 0.5));
	gui.add(xOffsetSlider.setup("xOffset", 0, -0.5, 0.5));
	gui.add(scalar.setup("scale", 0.5175, 0, 1.5));
	gui.add(jurkScaleSlider.setup("Jurk Scale", 0.0005375, 0.0005, 0.002));

	jurk.loadModel("hemd2.dae");
	
	jurk.setRotation(0, 90, 0, 1, 0);
	jurk.setRotation(1, 180, 0, 0, 1);

	debug = true;
	scherm = 0;
	nu = true;
	startTimer = 0;
	ofSetFrameRate(24);

	udpConnection.Create();
	udpConnection.Bind(11999);
	udpConnection.SetNonBlocking(true);
	
	basisURL = "http://localhost/kinectspiegel2/index.php";
	resp = ofLoadURL(basisURL);
	vector<string> moviesAndModels = ofSplitString(resp.data.getText(), "pp");
	//cout << moviesAndModels[1];
	movieLocations = ofSplitString(moviesAndModels[0], " ");
	modelLocations = ofSplitString(moviesAndModels[1], " ");
}

//--------------------------------------------------------------
void ofApp::update() {
	kinect.update();

	ofVec3f headPosCamera = getHeadPos();
	headPosCamera.y -= yOffsetSlider;
	headPosCamera.x -= xOffsetSlider;
	headPosRefl = calcRefl(getHeadPos());
	headPosScreen = calcScreen(headPos, headPosRefl);
	headPositionHistory.push_back(headPosCamera);

	lShPos = getLeftShoulder();
	lShPosRefl = calcRefl(lShPos);
	lShPosScreen = calcScreen(headPos, lShPosRefl);
	lShPosScreenScale = scaleOnScreen(headPosScreen, lShPosScreen);

	rShPos = getRightShoulder();
	rShPosRefl = calcRefl(rShPos);
	rShPosScreen = calcScreen(headPos, rShPosRefl);
	rShPosScreenScale = scaleOnScreen(headPosScreen, rShPosScreen);

	jurk.setScale(scalar / 1000, scalar / 1000, scalar / 1000);
	ofVec2f linker2D(lShPos.x, lShPos.y);
	ofVec2f rechter2D(rShPos.x, rShPos.y);

	headTrackedCamera.setPosition(headPosCamera);
	headTrackedCamera.setupOffAxisViewPortal(windowTopLeft, windowBottomLeft, windowBottomRight);
	
	//UDP OSC
	char udpMessage[100000];
	udpConnection.Receive(udpMessage, 100000);
	string message = udpMessage;
	vector<string> result = ofSplitString(message, "f:");
	cout << message << endl;

}

void ofApp::drawScene(bool isPreview) {
	ofEnableDepthTest();

	if (isPreview) {
		ofPushStyle();
		ofSetColor(150, 100, 100);
		ofDrawGrid(1.0f, 5.0f, true);

		ofSetColor(255);

		//--
		//draw camera preview
		//
		headTrackedCamera.transformGL();

		ofPushMatrix();
		ofScale(0.002f, 0.002f, 0.002f);
		ofNode().draw();
		ofPopMatrix();

		ofMultMatrix(headTrackedCamera.getProjectionMatrix().getInverse());

		ofNoFill();
		ofDrawBox(2.0f);

		headTrackedCamera.restoreTransformGL();
		//
		//--

		//--
		//draw window preview
		//
		window.clear();
		window.addVertex(windowTopLeft);
		window.addVertex(windowBottomLeft);
		window.addVertex(windowBottomRight);
		window.setMode(OF_PRIMITIVE_LINE_STRIP);
		window.draw();
		glPointSize(3.0f);
		ofPopStyle();
	}

	ofPushStyle();

	if (debug) {
		drawCoordinates(headTrackedCamera.getPosition());
		ofSphere(headPosRefl, 0.01);
		drawCoordinates(headPosRefl);
		ofSphere(headPosScreen, 0.01);
		drawCoordinates(headPosScreen);


		ofSetColor(255, 0, 0);
		ofSphere(lShPosScreenScale, 0.01);
		ofSetColor(0, 255, 0);
		ofSphere(rShPosScreenScale, 0.01);
		ofNoFill();
		ofColor col(200, 100, 100);
	}
	ofEnableSmoothing();
	ofSetColor(255);
	ofSetLineWidth(5.0f);

	jurk.setPosition(lShPosScreenScale.x, lShPosScreenScale.y, 0);
	
	//Rotation around Z-axis

	ofPushMatrix();
	ofVec2f lSh2(lShPosScreenScale.x, lShPosScreenScale.y);
	ofVec2f rSh2(rShPosScreenScale.x,rShPosScreenScale.y);
	ofVec2f dirr = rSh2 - lSh2;
	dirr.normalize();
	float angle = atan2(dirr.y, dirr.x);
	ofTranslate(lSh2);
	ofRotateZ(ofRadToDeg(angle));
	jurk.setPosition(0,0,0);
	jurk.setScale(jurkScaleSlider, jurkScaleSlider, jurkScaleSlider);
	jurk.drawFaces();
	ofPopMatrix();	
	ofPopStyle();
	ofDisableDepthTest(); 
	
}
//--------------------------------------------------------------
void ofApp::draw() {

	switch (scherm) {
	case 0:
		ofBackground(0, 0, 0);
		ofDrawBitmapString("Stand on the red circle and push the button", ofVec2f(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
		break;
	case 1:
		ofBackground(0, 0, 0);
		ofDrawBitmapString("Analyzing", ofVec2f(ofGetWindowWidth() / 2 + (ofGetElapsedTimef() * 10), ofGetWindowHeight() / 2));
		if (timer(5)) {
			scherm += 1;
		}
		break;
	case 2:
		ofBackgroundGradient(ofColor(50), ofColor(0));
		//------
		//draw the scene
		//
		if (usePreview) {
			previewCamera.begin();
		}
		else {
			headTrackedCamera.begin();
		}

		drawScene(usePreview);

		if (usePreview) {
			previewCamera.end();
		}
		else {
			headTrackedCamera.end();
		}


		stringstream message;
		message << "[SPACE] = User preview camera [" << (usePreview ? 'x' : ' ') << "]";

		ofDrawBitmapString(message.str(), kinect.getColorSource()->getWidth() + 10, 20);

		if (usePreview) {
			ofRectangle bottomLeft(0, ofGetHeight() - 200.0f, 300.0f, 200.0f);

			ofPushStyle();
			ofSetColor(0);
			ofDrawRectangle(bottomLeft);
			ofPopStyle();

			headTrackedCamera.begin(bottomLeft);
			drawScene(false);
			headTrackedCamera.end();
		}
		gui.draw();
		break;
	}
}

ofVec3f ofApp::getHeadPos() {
	kinect.update();
	ofVec3f headPos;
	{
		auto bodies = kinect.getBodySource()->getBodies();
		for (auto body : bodies) {
			for (auto joint : body.joints) {
			}
		}
	}
	{
		auto bodies = kinect.getBodySource()->getBodies();
		auto boneAtles = ofxKinectForWindows2::Data::Body::getBonesAtlas();

		for (auto body : bodies) {
			if (body.tracked) {
				//https://forum.openframeworks.cc/t/joint-position-and-orientation-ofxkinectforwindows2/20140/4
				headPos = body.joints[JointType_Head].getPosition();
			}
		}
	}
	return headPos;
}

ofVec3f ofApp::getHeadPosDepthMap() {
	kinect.update();
	ofVec3f headPos;
	{
		auto bodies = kinect.getBodySource()->getBodies();
		for (auto body : bodies) {
			for (auto joint : body.joints) {
			}
		}
	}
	{
		auto bodies = kinect.getBodySource()->getBodies();
		auto boneAtles = ofxKinectForWindows2::Data::Body::getBonesAtlas();

		for (auto body : bodies) {
			if (body.tracked) {
				//https://forum.openframeworks.cc/t/joint-position-and-orientation-ofxkinectforwindows2/20140/4
				headPos = body.joints[JointType_Head].getPositionInDepthMap();
			}
		}
	}
	return headPos;
}

ofVec3f ofApp::getLeftShoulder() {
	kinect.update();
	ofVec3f lShoulderPos;
	{
		auto bodies = kinect.getBodySource()->getBodies();
		for (auto body : bodies) {
			for (auto joint : body.joints) {
			}
		}
	}
	{
		auto bodies = kinect.getBodySource()->getBodies();
		auto boneAtles = ofxKinectForWindows2::Data::Body::getBonesAtlas();

		for (auto body : bodies) {
			if (body.tracked) {
				//https://forum.openframeworks.cc/t/joint-position-and-orientation-ofxkinectforwindows2/20140/4
				lShoulderPos = body.joints[JointType_ShoulderLeft].getPosition();
			}
		}
	}
	return lShoulderPos;
}

ofVec3f ofApp::getRightShoulder() {
	kinect.update();
	ofVec3f rShoulderPos;
	{
		auto bodies = kinect.getBodySource()->getBodies();
		for (auto body : bodies) {
			for (auto joint : body.joints) {
			}
		}
	}
	{
		auto bodies = kinect.getBodySource()->getBodies();
		auto boneAtles = ofxKinectForWindows2::Data::Body::getBonesAtlas();

		for (auto body : bodies) {
			if (body.tracked) {
				//https://forum.openframeworks.cc/t/joint-position-and-orientation-ofxkinectforwindows2/20140/4
				rShoulderPos = body.joints[JointType_ShoulderRight].getPosition();
			}
		}
	}
	return rShoulderPos;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	/*if (key = 'u') {
		usePreview = !usePreview;
	}*/
	if (key == 'd') {
		debug = !debug;
	}
	if (key == 's') {
		scherm += 1;
	}
	if (key == 'r') {
		scherm = 0;
		nu = true;
	}
}

void ofApp::drawCoordinates(ofVec3f point) {
	ofDrawBitmapString("x: " + ofToString(roundf(point.x * 100) / 100) + " y: " + ofToString(roundf(point.y * 100) / 100) + " z: " + ofToString(roundf(point.z * 100) / 100), point - ofVec3f(0.02, 0.02, 0.02));
}

ofVec3f ofApp::calcRefl(ofVec3f input) {
	input.z = -input.z;
	input.y -= yOffsetSlider;
	input.x -= xOffsetSlider;
	return input;
}

ofVec3f ofApp::calcScreen(ofVec3f eyes, ofVec3f refl) {
	ofVec3f output((eyes.x + refl.x) / 2, (eyes.y + refl.y) / 2, 0);
	return output;
}

ofVec3f ofApp::scaleOnScreen(ofVec3f eyes, ofVec3f bodypoint) {
	auto p = bodypoint - eyes;
	float length = p.length();
	length *= scalar;
	p.normalize();
	p *= length;
	p += eyes;
	return p;
}

bool ofApp::timer(float sec) {
	
	if(nu){
		startTimer = ofGetElapsedTimef();
		nu = false;
	}
	if (ofGetElapsedTimef() - startTimer >= sec) {
		return true;
	}
	else {
		return false;
	}
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
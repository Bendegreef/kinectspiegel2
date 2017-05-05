#include "ofApp.h"

using namespace ofxCv;
GLfloat lightOnePosition[] = { 40, 40, 0, 0.0 };
GLfloat lightOneColor[] = { 0.99, 0.99, 0.99, 0.4 };

GLfloat lightTwoPosition[] = { -40.0, 40, -25, 0.0 };
GLfloat lightTwoColor[] = { 0.99, 0.99, 0.99, 0.4 };

GLfloat lightThreePosition[] = { -40.0, 40, 50, 0.0 };
GLfloat lightThreeColor[] = { 0.99, 0.99, 0.99, 0.4 };
//--------------------------------------------------------------
void ofApp::setup(){
	kinect.open();
	kinect.initColorSource();
	kinect.initBodySource();
	kinect.initDepthSource();
	ofEnableSmoothing();
	ofSetVerticalSync(true);

	//calibration
	ofSetVerticalSync(false);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	/* initialize lighting */
	glLightfv(GL_LIGHT0, GL_POSITION, lightOnePosition);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightOneColor);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT1, GL_POSITION, lightTwoPosition);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightTwoColor);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT2, GL_POSITION, lightThreePosition);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightThreeColor);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHTING);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	bdrawCalib = false;
	calibDone = false;
	calibStep = 0;
	screenNormal.set(0, 0, 1);
	licht.setDiffuseColor(ofColor(30.f, 255.f, 100.f));
	licht.setPosition(50, 30, 50);
}

//--------------------------------------------------------------
void ofApp::update(){
	/*ofVec3f headPos = getHeadPos();
	headPos.x = -headPos.x;
	headPos.y = -headPos.y;
	headPositionHistory.push_back(headPos);
	while (headPositionHistory.size() > 50.0f) {
		headPositionHistory.pop_front();
	}

	headTrackedCamera.setPosition(headPos);
	headTrackedCamera.setupOffAxisViewPortal(windowTopLeft, windowBottomLeft, windowBottomRight);*/
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
		window.drawVertices();
		//
		//--
		ofPopStyle();
	}

	ofPushStyle();
	ofNoFill();
	ofColor col(200, 100, 100);
	for (float z = 0.0f; z > -40.0f; z -= 0.1f) {
		col.setHue(int(-z * 100.0f + ofGetElapsedTimef() * 10.0f) % 360);
		ofSetColor(col);
		ofDrawRectangle(-windowWidth / 2.0f, -windowHeight / 2.0f, z, windowWidth, windowHeight);
	}
	ofPopStyle();

	ofPushStyle();
	ofEnableSmoothing();
	ofSetColor(255);
	ofSetLineWidth(5.0f);
	ofBeginShape();
	for (unsigned int i = 0; i<headPositionHistory.size(); i++) {
		ofPoint vertex(headPositionHistory[i].x, headPositionHistory[i].y, -float(headPositionHistory.size() - i) * 0.05f);
		ofCurveVertex(vertex);
	}
	ofEndShape(false);
	ofPopStyle();

	ofDisableDepthTest();
}
//--------------------------------------------------------------
void ofApp::draw(){
	/*ofBackgroundGradient(ofColor(50), ofColor(0));
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

	//ofDrawBitmapString(message.str(), kinect.getColorSource()->getWidth() + 10, 20);

	if (usePreview) {
		ofRectangle bottomLeft(0, ofGetHeight() - 200.0f, 300.0f, 200.0f);

		ofPushStyle();
		ofSetColor(0);
		ofDrawRectangle(bottomLeft);
		ofPopStyle();

		headTrackedCamera.begin(bottomLeft);
		drawScene(false);
		headTrackedCamera.end();
	}*/

	//calibration
	if (needsCalib()) {
		/*
		* When you add GL attributes like in this program, you need to make
		* sure to disable them for the drawing of the calibration
		*/

		glPushAttrib(GL_DEPTH_TEST);
		glDisable(GL_DEPTH_TEST);
		drawCalib();
		glPopAttrib();
	}
	else {
		portalCamBegin();
		ofBackground(0, 0, 0);

		// Place the nalgene bottle
		ofPushMatrix();
		ofTranslate(-ofGetWindowWidth() / 2 + 200, -ofGetWindowHeight() / 2, -800);
		ofRotateX(-90);
		ofRotateX(3);
		ofRotateZ(-120);
		glDisable(GL_LIGHT2);
		glEnable(GL_LIGHT2);
		ofPopMatrix();

		// Place stacks of paper
		ofPushMatrix();
		ofPushStyle();
		ofTranslate(-ofGetWindowWidth() / 2 + 110, -ofGetWindowHeight() / 2 - 75, -1130);
		ofRotateX(2);
		ofSetColor(255, 255, 255);
		// White stack of paper
		ofPushMatrix();
		/*
		The neato thing about this project is that you can define things in
		terms of real world units.  My original set of code had this:

		ofScale(8.5, 2, 11);  // ie: a stack of paper -- 8.5" by 11" by 2" tall
		ofBox(0, 0, 0, 95);   // ie: 95 pixels per inch

		However I had to ditch that because ofScale was also scaling out the
		location of the GL lights.  I just bumped things around by a factor
		of ten to accomodate.
		*/
		ofScale(.85, .2, 1.1);
		ofBox(0, 0, 0, 950);
		ofPopMatrix();  /* end white stack */

		// Blue stack of paper
		ofSetColor(14, 118, 214);
		ofTranslate(0, -2 * 95, 0);
		ofPushMatrix();
		ofScale(.85, .2, 1.1);
		ofBox(0, 0, 0, 950);
		ofPopMatrix(); /* end blue stack */

		ofPopStyle();
		ofPopMatrix(); /* end paper stacks */

					   // Place desk block
		ofPushMatrix();
		ofPushStyle();
		ofSetColor(74, 48, 25);
		ofTranslate(50, -ofGetWindowHeight() / 2 - (4 * 12 * 95 / 2) - 4 * 95, -5 * 95);
		ofBox(0, 0, 0, (4 * 12 * 95));
		ofPopStyle();
		ofPopMatrix();
		licht.enable();
		ofVec3f headPos = getHeadPos();
		ofSphere(headPos.x, headPos.y, headPos.z, 20);
		// Sphere for hand -- note, kinect defines the body backwards... "LeftHand" refers to the users right : P
		/*ofVec3f rHandPos = screenify(getHandPos());
		ofSphere(rHandPos, 100);*/
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
				headPos = body.joints[JointType_Head].getPositionInWorld();
			}
		}
	}
	return headPos;
}

ofVec3f ofApp::getHandPos() {
	kinect.update();
	ofVec3f handPos;
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
				handPos = body.joints[JointType_HandLeft].getPositionInWorld();
			}
		}
	}
	return handPos;
}

//calibration

bool ofApp::needsCalib() {
	return !calibDone;
}

void ofApp::resetCalib() {
	calibStep = 0;
	calibDone = false;
}

void ofApp::drawCalib() {
	kinect.update();
	ofPushStyle();
	ofSetColor(255, 255, 255);
	ofFill();
		int rad = 30;
		int circleX = 0;
		if ((calibStep % 3) == 1) {
			circleX = ofGetWindowWidth();
		}
		int circleY = ofGetWindowHeight() * (calibStep % 3) / 2;
		ofPushStyle();
		ofSetColor(255, 0, 0);
		ofCircle(circleX, circleY, rad);
		ofPopStyle();
	ofPopStyle();
}

void ofApp::createCalibRay() {
	if (!calibDone) {
		ofVec3f headPos = getHeadPos();
		ofVec3f handPos = getHandPos();
		calibRays[calibStep] = ofRay(headPos, handPos - headPos);
		calibStep = calibStep + 1;
		if (calibStep == CALIBRATION_STEPS) {
			calcCalib();
		}
	}
}

void ofApp::calcCalib() {
	ofVec3f planeDefinition[3];

	for (int i = 0; i < 3; i++) {
		ofVec3f averagePoint;
		const int numOfIntersections = CALIBRATION_STEPS / 3; // num of intersections per plane definition point
		ofVec3f individualPoints[numOfIntersections];
		for (int j = 0; j < numOfIntersections; j++) {
			int thisInd = (i + j * 3);
			int nextInd = (i + (j + 1) * 3) % CALIBRATION_STEPS;
			ofRay intersectionSegment;
			intersectionSegment = calibRays[thisInd].intersect(calibRays[nextInd]);
			ofVec3f intersectionMidpoint;
			intersectionMidpoint.set(intersectionSegment.getMidpoint());
			individualPoints[j] = intersectionMidpoint;
		}
		averagePoint.average(individualPoints, numOfIntersections);
		planeDefinition[i] = averagePoint;
	}

	// next, i need to find out how to transform kinect space into screen space.

	// find the scalar difference between planeDef[0]-planeDef[2] & window height
	float kinectSpaceWindowHeight = planeDefinition[0].distance(planeDefinition[2]);
	scaleFactor = ofGetWindowHeight() / kinectSpaceWindowHeight;
	for (int i = 0; i < 3; i++) {
		planeDefinition[i] = planeDefinition[i] * scaleFactor;
	}

	// find the vector difference between planeDef[0] and (- window width / 2, -window height / 2, 0)
	displaceFactor = planeDefinition[0];

	// find the rotation required to get planeDef[1] & planeDef[2] in place
	ofVec3f rotation1Init = planeDefinition[2] - planeDefinition[0];
	ofVec3f rotation1End;
	rotation1End.set(0, ofGetWindowHeight(), 0);

	rotation1Perp = rotation1Init.getPerpendicular(rotation1End);
	rotation1 = rotation1Init.angle(rotation1End);

	ofVec3f	rotation2PreInit, rotation2Init, rotation2InitDebugYSquash, rotation2End, rotation2EndSquash;
	rotation2PreInit = planeDefinition[1] - planeDefinition[0];
	rotation2Init = rotation2PreInit;
	rotation2Init.rotate(rotation1, rotation1Perp);
	rotation2InitDebugYSquash = rotation2Init;
	rotation2InitDebugYSquash.y = 0; // Force this whole thing into a 2d rotation
	rotation2End;
	rotation2End.set(ofGetWindowWidth(), ofGetWindowHeight() / 2, 0);
	rotation2EndSquash = rotation2End;
	rotation2EndSquash.y = 0;
	rotation2Perp = rotation1End;
	rotation2 = rotation2InitDebugYSquash.angle(rotation2EndSquash);
	float rotation2CandidateA = rotation2InitDebugYSquash.angle(rotation2EndSquash);
	float rotation2CandidateB = -rotation2CandidateA;
	ofVec3f rotation2CandidateAPoint = rotation2Init.rotate(rotation2CandidateA, rotation2Perp);
	ofVec3f rotation2CandidateBPoint = rotation2Init.rotate(rotation2CandidateB, rotation2Perp);
	if ((rotation2CandidateAPoint - rotation2End).length() < (rotation2CandidateBPoint - rotation2End).length()) {
		rotation2 = rotation2CandidateA;
	}
	else {
		rotation2 = rotation2CandidateB;
	}
	calibDone = true;
}

void ofApp::tweakOrientation() {
	if (calibDone) {
		tweakAngle = 0;
		ofVec3f screenHead = screenify(getHeadPos());
		tweakPerp = screenHead.getPerpendicular(screenNormal);
		tweakAngle = screenHead.angle(screenNormal);
	}
}

ofVec3f ofApp::screenify(ofVec3f kinectPoint) {
	ofVec3f newPoint;
	newPoint = kinectPoint;
	newPoint = newPoint * scaleFactor;
	newPoint = newPoint - displaceFactor;
	newPoint.rotate(rotation1, rotation1Perp);
	newPoint.rotate(rotation2, rotation2Perp);
	newPoint.x = newPoint.x - ofGetWindowWidth() / 2;
	newPoint.x = -newPoint.x;
	newPoint.y = newPoint.y - ofGetWindowHeight() / 2;
	newPoint.rotate(180, screenNormal);
	newPoint.rotate(tweakAngle, tweakPerp);
	return newPoint;
}

void ofApp::portalCamBegin() {
	kinect.update();
	ofVec3f screenHead = screenify(getHeadPos());
	screenHead.x = -screenHead.x;
	screenHead.y = -screenHead.y;
	myOfCamera.setPosition(screenHead);
	ofVec3f topLeft, bottomLeft, bottomRight;
	topLeft.set(-ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, 0);
	bottomLeft.set(-ofGetWindowWidth() / 2, -ofGetWindowHeight() / 2, 0);
	bottomRight.set(ofGetWindowWidth() / 2, -ofGetWindowHeight() / 2, 0);
	myOfCamera.setupOffAxisViewPortal(topLeft, bottomLeft, bottomRight);
	myOfCamera.begin();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == 'r') {
		resetCalib();
	}
	else if (key == 't') {
		tweakOrientation();
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	createCalibRay();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

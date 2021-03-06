#include "ofApp.h"

using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup(){
	kinect.open();
	kinect.initColorSource();
	kinect.initBodySource();
	kinect.initDepthSource();
	ofEnableSmoothing();
	ofSetVerticalSync(true);

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

	windowTopLeft = ofVec3f(-windowWidth /2.0f,
		+windowHeight / 2.0f,
		0.0f);
	windowBottomLeft = ofVec3f(-windowWidth / 2.0f,
		-windowHeight / 2.0f,
		0.0f);
	windowBottomRight = ofVec3f(+windowWidth / 2.0f,
		-windowHeight / 2.0f,
		0.0f);

	
}

//--------------------------------------------------------------
void ofApp::update(){
	kinect.update();
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
	headPos.x = -headPos.x;
	headPos.y = -headPos.y;
	headPositionHistory.push_back(headPos);
	while (headPositionHistory.size() > 50.0f) {
		headPositionHistory.pop_front();
	}

	headTrackedCamera.setPosition(headPos);
	headTrackedCamera.setupOffAxisViewPortal(windowTopLeft, windowBottomLeft, windowBottomRight);
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

	kinect.getColorSource()->draw(0, 0, 320, 240);

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
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

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

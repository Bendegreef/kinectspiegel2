#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void drawScene(bool isPreview);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofxKFW2::Device kinect;
		ofVec3f headPos, windowTopLeft, windowBottomLeft, windowBottomRight;
		ofCamera headTrackedCamera;
		ofEasyCam previewCamera;

		bool usePreview;
		float windowWidth;
		float windowHeight;
		float viewerDistance;
		float offset;

		deque<ofPoint> headPositionHistory;

		ofVboMesh window;
		ofVec3f getHeadPos();
		ofVec3f getHeadPosDepthMap();
		ofVec3f getLeftShoulder();
		ofVec3f getRightShoulder();
		ofVec3f headPosScreen;
		ofVec3f headPosRefl;
		ofVec3f headPosCamera;
		ofVec3f lShPosScreenScale;
		ofVec3f lShPosScreen;
		ofVec3f lShPosRefl;
		ofVec3f lShPos;
		ofVec3f rShPosScreenScale;
		ofVec3f rShPosScreen;
		ofVec3f rShPosRefl;
		ofVec3f rShPos;

		void drawCoordinates(ofVec3f point);
		ofVec3f calcScreen(ofVec3f eyes, ofVec3f reflection);
		ofVec3f calcRefl(ofVec3f point);
		ofVec3f scaleOnScreen(ofVec3f eyes, ofVec3f bodyPoint);

		ofxFloatSlider yOffsetSlider;
		ofxFloatSlider xOffsetSlider;
		ofxFloatSlider scalar;
		ofxFloatSlider jurkScaleSlider;
		ofxPanel gui;

		float degX;
		float degY;
		float degZ;

		ofxAssimpModelLoader jurk;
		
};

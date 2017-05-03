#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxRay.h"

#define CALIBRATION_STEPS 12

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

		ofVec3f getHeadPos();

		ofxKFW2::Device kinect;
		ofVec3f windowTopLeft, windowBottomLeft, windowBottomRight;
		ofCamera headTrackedCamera;
		ofEasyCam previewCamera;

		bool usePreview;
		float windowWidth;
		float windowHeight;
		float viewerDistance;

		deque<ofPoint> headPositionHistory;

		ofVboMesh window;

		//calibration

		//void portalCamBegin();
		//void portalCamEnd();

		void drawCalib();
		//void createCalibRay();

		bool needsCalib();
		//ofVec3f screenify(ofVec3f kinectPoint);

		//void tweakOrientation();

		bool bdrawCalib;
		void resetCalib();
		bool calibDone;
		int calibStep;
		//void calcCalib();
		ofRay calibRays[CALIBRATION_STEPS];

		ofVec3f displaceFactor;
		float scaleFactor;
		ofVec3f rotation1Perp;
		float rotation1;
		ofVec3f rotation2Perp;
		float rotation2;
		ofVec3f tweakPerp;
		float tweakAngle;

		ofVec3f screenNormal;
		
};

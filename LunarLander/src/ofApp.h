#pragma once

#include "ofMain.h"
#include "Surface.h"
#include "Lander.h"
#include "ofxBox2d.h"

class ofApp : public ofBaseApp{

	Surface* surf;
	SurfaceGenerationParams surfGenerationParams;

	Lander* lander;
	LanderParams landerParams;

	ofxBox2d world;
	ofxBox2dRender physicsDebug;

	vector   <shared_ptr<ofxBox2dCircle> > circles; // default box2d circles
	vector   <shared_ptr<ofxBox2dRect> >   boxes;   // defalut box2d rects

	std::map<int, bool> keyDownMap;

	bool drawDebug = false;

	enum GameState{ Flying, Landing, Landed };
	GameState gameState;

	float LandingTimer;

	public:
		void setup();
		void update();
		void draw();

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
		
		bool isKeyDown(int key);
		void HandleControls();


		bool CheckWin();
		void StartLanding();
		void EndLanding();
};

#include "ofApp.h"
#include "ContactListeners.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60);
	ofBackground(0);
	
	world.init();
	world.setGravity(0, 1);
	world.createBounds();
	world.setFPS(60);
	world.getWorld()->SetDebugDraw(&world.debugRender);
	
	world.getWorld()->SetContactListener(LunarLanderConatactManager::Get());

	surf = new Surface(&world);
	surfGenerationParams = { 
		.5f,	//minHeight
		.95f,	//maxHeight
		200,	//numPoints
		.05f, 	//maxHeightDiff
		4, 		//minPlateauSize
		8, 		//maxPlateauSize
		3		//plateauCount
	};
	surf->SetScreenSize(ofGetWindowWidth(), ofGetWindowHeight());
	surf->GenerateSurface(surfGenerationParams);

	landerParams = { 
		5.f,					//angularDamping
		.01f,					//linearDamping
		1.f, 					//density
		.1f, 					//friction
		.1f,					//bounce
		ofVec2f(200.f, 100.f),	//startingPos
		3.f						//startVelocity
	};
	lander = new Lander(&world, landerParams, ofVec2f(.65f, .6f), ofVec2f(.8f, .4f), "lander");
	lander->SetScale(20);

	LandingSpotContactListener* landingListener = new LandingSpotContactListener(this, lander->GetBody());
	landingListener->SetBodyFilter(surf->GetLandingSpotBody());
	LunarLanderConatactManager::Get()->SetCallback(landingListener, ContactCallbackFlag::BeginContact | ContactCallbackFlag::EndContact);
	
	lander->Sleep();
	gameState = GameState::Landed;
}

//--------------------------------------------------------------
void ofApp::update(){
	
	if(CheckWin())
	{
		ofLogNotice() << "CHICKEN DINNER";
		lander->Sleep();
		gameState = GameState::Landed;
	}
	if(gameState == GameState::Flying || gameState == GameState::Landing)
	{
		HandleControls();
		lander->Update();
	}
	
	world.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
	ofSetColor(255);
	//ofSetLineWidth(3);
	surf->Draw();
	lander->Draw();

	float time = ofGetElapsedTimef();

	for (int i = 0; i < circles.size(); i++) {
		ofFill();
		ofSetHexColor(0xf6c738);
		circles[i].get()->draw();
	}

	for (int i = 0; i < boxes.size(); i++) {
		ofFill();
		ofSetHexColor(0xBF2545);
		boxes[i].get()->draw();
	}

	if(drawDebug)
		world.getWorld()->DrawDebugData();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	float r = 0, h = 0, w = 0;
	switch (key)
	{
	case 'r':
		surf->GenerateSurface(surfGenerationParams);
		lander->Reset();
		break;
	case 'c':
		r = ofRandom(4, 20);
		circles.push_back(shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle));
		circles.back().get()->setPhysics(3.0, 0.53, 0.1);
		circles.back().get()->setup(world.getWorld(), mouseX, mouseY, r);
		break;
	case 'b':
		w = ofRandom(4, 20);
		h = ofRandom(4, 20);
		boxes.push_back(shared_ptr<ofxBox2dRect>(new ofxBox2dRect));
		boxes.back().get()->setPhysics(3.0, 0.53, 0.1);
		boxes.back().get()->setup(world.getWorld(), mouseX, mouseY, w, h);
		break;
	case 'd':
		drawDebug = !drawDebug;
		break;
	case 'p':
		if(gameState == GameState::Landed)
		{
			surf->GenerateSurface(surfGenerationParams);
			lander->Start(landerParams);
			gameState = GameState::Flying;
		}
		break;
	default:
		break;
	}
	keyDownMap[key] = true;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	keyDownMap[key] = false;
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
	surf->SetScreenSize(w, h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

bool ofApp::isKeyDown(int key)
{
	auto keyDown = keyDownMap.find(key);
	if (keyDown == keyDownMap.end())
	{
		return false;
	}
	return keyDown->second;
}

void ofApp::HandleControls()
{
	if (isKeyDown(OF_KEY_UP))
	{
		lander->AddThrusterStrength(0.005f);
	}
	if (isKeyDown(OF_KEY_DOWN))
	{
		lander->AddThrusterStrength(-.005f);
	}
	bool right = isKeyDown(OF_KEY_RIGHT);
	bool left = isKeyDown(OF_KEY_LEFT);
	lander->SetRotationRate(0.f);
	if (left) {
		lander->SetRotationRate(-.05f);
		if (right)
		{
			lander->SetRotationRate(0.f);
		}
	}
	else if (right)
	{
		lander->SetRotationRate(.05f);
	}
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

bool ofApp::CheckWin()
{
	if(gameState == GameState::Landing)
	{
		if(lander->IsStationary())
		{
			if((ofGetSeconds() - LandingTimer) > 3.f)
				return true;
		}
		else
		{
			LandingTimer = ofGetSeconds();
		}
	}
	return false;
}

void ofApp::StartLanding()
{
	gameState = GameState::Landing;
	ofLogNotice("Landing") << "Started";
}

void ofApp::EndLanding()
{
	gameState = GameState::Flying;
	ofLogNotice("Landing") << "Ended";
}
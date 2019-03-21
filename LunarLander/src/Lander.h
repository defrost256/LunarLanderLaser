#pragma once

#include <string>
#include "ofxSvg.h"
#include "ofUtils.h"
#include "ofxBox2d.h"
#include "ContactListeners.h"

struct LanderParams {
	float angularDamping;
	float linearDamping;
	float density;
	float friction;
	float bounce;
	ofVec2f startingPos = ofVec2f(200.f, 100.f);
	float startVelocity = 10.f;
};

class Lander {

	const std::string svgSourceFolder = "res/";

	LanderParams params;
	std::string landerSvg;
	ofVec2f topBoxSize = ofVec2f(.65f, .6f);
	ofVec2f bottomBoxSize = ofVec2f(.8f, .4f);

	ofVec2f currentPosition = ofVec2f(0.f, 0.f);
	float currentRotationRad = 0.f;
	float currentScale = 1.f;

	float currentRotationRate = 0.f;
	float currentThrusterStrength = 0.f;

	ofPath graphics;

	LanderCrashContactListener* crashListener;

	ofxBox2d* world;
	b2Body* physicsBody;
	b2BodyDef physicsBodyDef;
	b2Fixture* topFixture;
	b2Fixture* bottomFixture;
	b2FixtureDef topFixtureDef;
	b2FixtureDef bottomFixtureDef;

	bool isActive = false;

public:

	Lander(ofxBox2d* world, LanderParams params, ofVec2f topBoxSize, ofVec2f bottomBoxSize, std::string svgFileName);
	void Draw();
	void Update();
	void SetScale(float scale);

	void Start(LanderParams params);
	void Sleep();
	
	void SetThrusterStrength(float strength);
	void AddThrusterStrength(float strength);
	void SetRotationRate(float rotation);
	void Reset();

	ofVec2f GetPosition();
	float GetRotationRad();
	float GetRotationDeg();
	bool IsStationary(float tolerance = .5f);

	b2Body* GetBody();

};
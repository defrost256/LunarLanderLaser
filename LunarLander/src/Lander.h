#pragma once

#include <string>
#include "ofxSvg.h"
#include "ofUtils.h"
#include "ofxBox2d.h"

struct LanderParams {
	std::string landerSvg;
	float angularDamping;
	float linearDamping;
	float density;
	float friction;
	float bounce;
	ofVec2f topBoxSize = ofVec2f(.65f, .6f);
	ofVec2f bottomBoxSize = ofVec2f(.8f, .4f);
	ofVec2f startingPos = ofVec2f(200.f, 100.f);
};

class Lander {

	const std::string svgSourceFolder = "res/";

	LanderParams params;

	ofVec2f currentPosition = ofVec2f(0.f, 0.f);
	float currentRotationRad = 0.f;
	float currentScale = 1.f;

	float currentRotationRate = 0.f;
	float currentThrusterStrength = 0.f;

	ofPath graphics;

	ofxBox2d* world;
	b2PolygonShape* topShape;
	b2PolygonShape* bottomShape;
	b2Body* physicsBody;
	b2BodyDef physicsBodyDef;
	b2Fixture* topFixture;
	b2Fixture* bottomFixture;
	b2FixtureDef topFixtureDef;
	b2FixtureDef bottomFixtureDef;

public:

	Lander(ofxBox2d* world, LanderParams params);
	void Draw();
	void Update();
	void SetScale(float scale);

	void SetThrusterStrength(float strength);
	void AddThrusterStrength(float strength);
	void SetRotationRate(float rotation);

	ofVec2f GetPosition();
	float GetRotationRad();
	float GetRotationDeg();

};
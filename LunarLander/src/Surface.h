#pragma once

#include "ofPolyline.h"
#include "ofxBox2d.h"

struct SurfaceGenerationParams {
	float minHeight;
	float maxHeight;
	int numPoints;
	float maxHeightDiff;
	int minPlateauSize;
	int maxPlateauSize;
	int plateauCount;
};

class Surface
{
private:

	ofxBox2d* world;
	b2ChainShape* physics;
	b2Body* physicsBody;
	b2BodyDef physicsBodyDef;
	b2Fixture* fixture;
	b2FixtureDef fixtureDef;

	b2BodyDef goalTriggerBodyDef;
	b2Body* goalTriggerBody;
	std::vector<b2Fixture*> goalTriggerFixtures;

	ofPolyline graphics;
	int ScreenWidth, ScreenHeight;

	float friction = .5f;
	float bounce = .5f;

public:

	Surface(ofxBox2d* world);
	void GenerateSurface(SurfaceGenerationParams params);
	void SetScreenSize(int screenWidth, int screenHeight);
	void SetPhysicalParams(float friction, float bounce);
	void Draw();
	~Surface();

private:

	b2Fixture* CreateTriggerBox(ofVec2f size, ofVec2f center);
};


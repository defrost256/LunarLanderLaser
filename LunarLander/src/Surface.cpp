#include "Surface.h"

#include "ofMath.h"
#include "ContactListeners.h"

Surface::Surface(ofxBox2d* world)
{
	this->world = world;

	physicsBodyDef.type = b2BodyType::b2_staticBody;
	physicsBodyDef.active = true;
	physicsBody = world->getWorld()->CreateBody(&physicsBodyDef);

	goalTriggerBodyDef.type = b2BodyType::b2_staticBody;
	goalTriggerBodyDef.active = true;
	goalTriggerBody = world->getWorld()->CreateBody(&goalTriggerBodyDef);
}

void Surface::GenerateSurface(SurfaceGenerationParams params)
{

	//Clear bprevious surface
	if (graphics.size() > 0)
	{
		graphics.clear();
		physics->Clear();
		physicsBody->DestroyFixture(fixture);
		for (auto f : goalTriggerFixtures)
		{
			goalTriggerBody->DestroyFixture(f);
		}
		goalTriggerFixtures.clear();
	}

	b2Vec2* physVerts = new b2Vec2 [params.numPoints];

	//Calculate initial params for surface creation
	float pointSeparation = 1.f / params.numPoints * ScreenWidth; //The distance between two points on the x axis
	float lastHeight = ofRandom(params.minHeight, params.maxHeight); //The height of the last vertex (for plateau and even terrain)
	graphics.setClosed(false);	//our polyline should stay open
	float currentHeight;	//The y axis of the current vertex

	//Calculate initial params for plateau generation
	bool generatePlateau = false;	//flag true if we are generating a plateau
	int plateauSpacing = params.numPoints / params.plateauCount;	//The maximum even spacing of plateaus
	int nextPlateauIdx = 0;	//The index of the next (and current) plateau we are doing
	int nextPlateauStartIdx = rand() % plateauSpacing; //The index of the vertex where the next plateau will start (or the current plateau started)
	int plateauSegmentRemain = (rand() % (params.maxPlateauSize - params.minPlateauSize)) + params.minPlateauSize; //The amount of vertexes the next plateau will have (or the current plateau has left)

	//Main generation loop
	for (int i = 0; i < params.numPoints; i++)
	{
		//plateau generation
		if (generatePlateau)
		{
			plateauSegmentRemain--;
			//If plateau is finished
			if (plateauSegmentRemain <= 0)
			{
				generatePlateau = false;
				//calculate params for the next plateau
				nextPlateauIdx++;
				nextPlateauStartIdx = (rand() % ((nextPlateauIdx + 1) * plateauSpacing - i)) + i;
				plateauSegmentRemain = (rand() % (params.maxPlateauSize - params.minPlateauSize)) + params.minPlateauSize;
			}
			currentHeight = lastHeight;
		}
		//mountain generation
		else 
		{
			currentHeight = ofRandom(std::max(lastHeight - params.maxHeightDiff, params.minHeight), std::min(lastHeight + params.maxHeightDiff, params.maxHeight));
			if (nextPlateauIdx < params.plateauCount && i >= nextPlateauStartIdx)
			{
				generatePlateau = true;
				b2Fixture* triggerBox = CreateTriggerBox(
					ofVec2f(pointSeparation * plateauSegmentRemain, params.maxHeightDiff * 200.f), 
					ofVec2f(pointSeparation * (plateauSegmentRemain / 2.f + i), currentHeight * ScreenHeight - params.maxHeightDiff * 100.f)
				);
				goalTriggerFixtures.push_back(triggerBox);
			}
		}

		graphics.addVertex(pointSeparation * i, currentHeight * ScreenHeight, 0.f);
		physVerts[i] = screenPtToWorldPt(ofVec2f(pointSeparation * i, currentHeight * ScreenHeight));
		lastHeight = currentHeight;
	}

	//Create physical body
	physics = new b2ChainShape();
	physics->CreateChain(physVerts, params.numPoints);
	fixtureDef.isSensor = false;
	fixtureDef.shape = physics;
	fixtureDef.friction = friction;
	fixtureDef.restitution = bounce;

	fixture = physicsBody->CreateFixture(&fixtureDef);

	delete[] physVerts;
}

void Surface::SetScreenSize(int screenWidth, int screenHeight)
{
	if (graphics.size() > 0)
	{
		float xRatio = screenWidth / (float)ScreenWidth;
		float yRatio = screenHeight / (float)ScreenHeight;
		for (ofDefaultVertexType& vert : graphics)
		{
			//Scale polyline points
			vert.x *= xRatio;
			vert.y *= yRatio;
		}
	}

	ScreenWidth = screenWidth;
	ScreenHeight = screenHeight;
}

void Surface::SetPhysicalParams(float friction, float bounce)
{
	this->friction = friction;
	this->bounce = bounce;
}


void Surface::Draw()
{
	graphics.draw();
}

Surface::~Surface()
{
}

b2Fixture* Surface::CreateTriggerBox(ofVec2f size, ofVec2f center)
{
	b2PolygonShape triggerBox;
	b2Vec2 b2size = screenPtToWorldPt(size / 2.f);//b2Vec2(size.x / 2.f, size.y / 2.f);
	b2Vec2 b2center = screenPtToWorldPt(center);
	triggerBox.SetAsBox(b2size.x, b2size.y, b2center, 0.f);
	b2FixtureDef def;
	def.isSensor = true;
	def.shape = &triggerBox;
	b2Fixture* fixture = goalTriggerBody->CreateFixture(&def);
	ofLogNotice("Surface") << "Creating trigger box \n\tfrom\tS:" << ofToString(size) << "\tC:" << ofToString(center) << "\n\tto:\tS:(" << ofToString(b2size.x) << "," << ofToString(b2size.y) << ")\tC:(" << ofToString(b2center.x) << "," << ofToString(b2center.y) << ")\n";
	return fixture;
}

b2Body* Surface::GetBody()
{
	return physicsBody;
}

b2Body* Surface::GetLandingSpotBody()
{
	return goalTriggerBody;
}